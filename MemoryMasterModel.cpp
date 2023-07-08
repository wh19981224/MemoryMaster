#pragma execution_character_set("utf-8")
#include "MemoryMasterModel.h"

#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QMap>
#include <QtConcurrent/qtconcurrentrun.h>

const QMap<QChar, int> SizeUintMap = { {'K', 0}, {'M', 1}, {'G', 2}, {'T', 3}};

SizeRelationship fileSizeSort(QString, QString);
SizeRelationship fileNameSort(QString, QString);

qint64 MemoryMasterModel::computeAllFilesAndDirs(QString path)
{
	QFileInfo info(path);

	qulonglong ret = 0;

	if (info.isFile())
	{
		ret = info.size();
	}
	else if (info.isDir())
	{
		QDir dir(path);

		QFileInfoList list = dir.entryInfoList();

		for (int i = 0; i < list.length(); i++)
		{
			if (list[i].fileName() == "." || list[i].fileName() == "..") continue;
			ret = ret + computeAllFilesAndDirs(list[i].filePath());
		}
	}

	return ret;
}

QString MemoryMasterModel::computeSize(qint64 size)
{
	int idx = 0;

	while ((idx < m_sizeUnit.size() - 1) && (size > 1024))
	{
		if (size > size / 1024 * 1024)
		{
			size = size / 1024 + 1;
		}
		else
		{
			size /= 1024;
		}
		++idx;
	}
	qDebug() << "计算所得大小为：" << QString::number(size) + m_sizeUnit[idx];
	return QString::number(size) + m_sizeUnit[idx];
}

void MemoryMasterModel::receiveOnePath(QString path)
{
	qDebug() << "获取到一个路径：" << path;
	m_path = path;
}
// test path = D:\C++project\MemoryMaster\MemoryMaster.vcxproj
void MemoryMasterModel::startCompute()
{
	QFileInfo info(m_path);
	if (info.exists() == false)
	{
		qDebug() << m_path << "不存在";
		emit unvalidPath();
		return;
	}

	m_fileNames.clear();
	m_fileSizes.clear();
	m_fileTpyes.clear();
	emit sendPrograssBarValue(0);

	QtConcurrent::run([=] {
		if (info.isFile())
		{
			m_fileNames.append(info.fileName());
			m_fileSizes.append(computeSize(info.size()));
			m_fileTpyes.append(info.suffix());
			emit finishCompute(m_fileNames, m_fileSizes, m_fileTpyes);
			emit sendPrograssBarValue(100);
			return;
		}

		QDir dir(m_path);
		QFileInfoList list = dir.entryInfoList();
		for (int i = 0; i < list.length(); i++)
		{
			qDebug() << list[i].filePath();
			if (list[i].fileName() == "." || list[i].fileName() == "..") continue;
			m_fileNames.append(list[i].fileName());
			m_fileSizes.append(computeSize(computeAllFilesAndDirs(list[i].filePath())));
			if (list[i].isDir())
			{
				m_fileTpyes.append("文件夹");
			}
			else
			{
				m_fileTpyes.append(list[i].suffix());
			}
			if (i < list.length() - 1)
			{
				emit sendPrograssBarValue(static_cast<int>(1.0 * i / (list.length() - 2) * 100));
			}
			else
			{
				emit sendPrograssBarValue(100);
			}
		}
		qDebug() << "文件名有：" << m_fileNames;
		qDebug() << "文件类型有：" << m_fileTpyes;
		qDebug() << "文件大小有：" << m_fileSizes;
		fileSort(0);
		emit finishCompute(m_fileNames, m_fileSizes, m_fileTpyes);
	});
}

void MemoryMasterModel::fileSort(int modeNum)
{
	SortMode sortMode = static_cast<SortMode>(modeNum);
	switch (sortMode)
	{
	case SortMode::FILESIZE:
		quick_sort(m_fileSizes, 0, m_fileSizes.size() - 1, fileSizeSort);
		break;
	case SortMode::FILENAME:
		quick_sort(m_fileNames, 0, m_fileNames.size() - 1, fileSizeSort);
		break;
	default:
		quick_sort(m_fileSizes, 0, m_fileSizes.size() - 1, fileSizeSort);
		break;
	}
}

SizeRelationship fileSizeSort(QString filesize1, QString filesize2)
{
	filesize1.chop(1);
	filesize2.chop(1);

	if (filesize1[filesize1.size() - 1].isDigit() && filesize2[filesize2.size() - 1].isLetter())
	{
		return SizeRelationship::SMALLER;
	}
	if (filesize1[filesize1.size() - 1].isLetter() && filesize2[filesize2.size() - 1].isDigit())
	{
		return SizeRelationship::BIGGER;
	}
	if (filesize1[filesize1.size() - 1].isDigit() && filesize2[filesize2.size() - 1].isDigit())
	{
		int size1 = filesize1.toInt();
		int size2 = filesize2.toInt();
		if (size1 < size2)
		{
			return SizeRelationship::SMALLER;
		}
		else if (size1 > size2)
		{
			return SizeRelationship::BIGGER;
		}
		else
		{
			return SizeRelationship::EQUAL;
		}
	}

	if (filesize1[filesize1.size() - 1] == filesize2[filesize2.size() - 1])
	{
		filesize1.chop(1);
		filesize2.chop(1);
		
		int size1 = filesize1.toInt();
		int size2 = filesize2.toInt();
		if (size1 < size2)
		{
			return SizeRelationship::SMALLER;
		}
		else if (size1 > size2)
		{
			return SizeRelationship::BIGGER;
		}
		else
		{
			return SizeRelationship::EQUAL;
		}
	}
	
	if (SizeUintMap[filesize1[filesize1.size() - 1]] < SizeUintMap[filesize2[filesize2.size() - 1]])
	{
		return SizeRelationship::SMALLER;
	}
	return SizeRelationship::BIGGER;
}

SizeRelationship fileNameSort(QString filename1, QString filename2)
{
	if (filename1 < filename2)
	{
		return SizeRelationship::SMALLER;
	}
	else if (filename1 > filename2)
	{
		return SizeRelationship::BIGGER;
	}
	return SizeRelationship::EQUAL;
}


void MemoryMasterModel::quick_sort(QStringList q, int l, int r, SizeRelationship(*sortFunction)(QString, QString))
{
	if (l >= r) return;
	int i = l - 1, j = r + 1;
	QString x = q[(l + r) / 2];
	
	while (i < j)
	{
		do
		{
			i++;
		}while ((*sortFunction)(q[i], x) == SizeRelationship::BIGGER);
		do
		{
			j--;
		} while ((*sortFunction)(q[j], x) == SizeRelationship::SMALLER);
		
		if (i < j)
		{
			swapAll(i, j);

			QString tempStr = q[i];
			q[i] = q[j];
			q[j] = tempStr;
		}
	}
	//下面对左右区间继续递归
	quick_sort(q, j + 1, r, sortFunction);
	quick_sort(q, l, j, sortFunction);
}

void MemoryMasterModel::swapAll(int i, int j)
{
	QString tempStr = m_fileNames[i];
	m_fileNames[i] = m_fileNames[j];
	m_fileNames[j] = tempStr;

	tempStr = m_fileSizes[i];
	m_fileSizes[i] = m_fileSizes[j];
	m_fileSizes[j] = tempStr;

	tempStr = m_fileTpyes[i];
	m_fileTpyes[i] = m_fileTpyes[j];
	m_fileTpyes[j] = tempStr;
}