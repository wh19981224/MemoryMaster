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

MemoryMasterModel::MemoryMasterModel() : 
	m_sizeUnit({ "B", "KB", "MB", "GB", "TB" }),
	m_path(""),
	m_subNum(0),
	m_completeNum(0)
{

}

qint64 MemoryMasterModel::computeAllFilesAndDirs(QString path)
{
	if (m_taskQueue.size() > 1) return 0;
	
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
	
	return QString::number(size) + m_sizeUnit[idx];
}

void MemoryMasterModel::receiveOnePath(QString path)
{
	m_path = path;
}

void MemoryMasterModel::startCompute()
{
	QFileInfo info(m_path);
	if (info.exists() == false)
	{
		qDebug() << m_path << "不存在";
		emit unvalidPath();
		return;
	}
	qDebug() << "开始进入线程" << ",主线程id = " << QThread::currentThreadId();
	QtConcurrent::run([=] {
		m_taskQueueMutex.lock();
		m_taskQueue.push_back(m_path);
		qDebug() << "压入队列：" << m_path << ",thread id = " << QThread::currentThreadId();
		m_taskQueueMutex.unlock();

		if (info.isFile())
		{
			m_fileInfoMutex.lock();
			m_fileNames.clear();
			m_fileSizes.clear();
			m_fileTpyes.clear();

			m_fileNames.append(info.fileName());
			m_fileSizes.append(computeSize(info.size()));
			m_fileTpyes.append(info.suffix());
			emit finishCompute(m_fileNames, m_fileSizes, m_fileTpyes);

			m_fileInfoMutex.unlock();

			m_taskQueueMutex.lock();
			m_taskQueue.pop_front();
			m_taskQueueMutex.unlock();
			emit sendPrograssBarValue(100);
			return;
		}

		QDir dir(m_path);
		QFileInfoList list = dir.entryInfoList();

		m_fileInfoMutex.lock();
		emit sendPrograssBarValue(0);
		m_fileNames.clear();
		m_fileSizes.clear();
		m_fileTpyes.clear();
		qDebug() << "list.count() = " << list.count() << ",list.size() = " << list.size() << ",list.length() = " << list.length();
		m_completeNum = 0;
		m_subNum = list.length();
		for (int i = 0; i < m_threadVector.size(); ++i)
		{
			m_threadVector[i]->setExit();
			m_threadVector[i]->wait();
			delete m_threadVector[i];
		}
		m_threadVector.clear();
		m_fileInfoMutex.unlock();

		for (int i = 0, len = list.length(); i < len; i++)
		{
			if (m_taskQueue.size() > 1)
			{
				m_taskQueueMutex.lock();
				qDebug() << "中断——推出队列：" << m_taskQueue.front() << ",thread id = " << QThread::currentThreadId();
				m_taskQueue.pop_front();
				m_taskQueueMutex.unlock();

				return;
			}
			
			if (list[i].fileName() == "." || list[i].fileName() == "..")
			{
				m_fileInfoMutex.lock();
				m_subNum--;
				m_fileInfoMutex.unlock();

				continue;
			}
			
			if (list[i].isDir())
			{
				ComputeDirSize *computeDirSize = new ComputeDirSize(list[i].filePath());
				connect(computeDirSize, SIGNAL(sendSizeValue(QString, qint64)),
					    this, SLOT(receiveDirSize(QString, qint64)));
				m_fileInfoMutex.lock();
				m_threadVector.append(computeDirSize);
				m_fileInfoMutex.unlock();
				computeDirSize->start();
				continue;
			}

			m_fileInfoMutex.lock();
			m_fileNames.append(list[i].fileName());
			m_fileSizes.append(computeSize(computeAllFilesAndDirs(list[i].filePath())));
			m_fileTpyes.append(list[i].suffix());
			m_fileInfoMutex.unlock();

			m_completeNum++;
			if (m_completeNum < m_subNum)
			{
				emit sendPrograssBarValue(static_cast<int>(1.0 * m_completeNum / m_subNum * 100));
				qDebug() << list[i].fileName() << "计算完成, m_completeNum = " << m_completeNum << ",m_subNum = " << m_subNum;
			}
			else
			{
				completeCompute();
				emit sendPrograssBarValue(100);
				qDebug() << list[i].fileName() << "计算完成, m_completeNum = " << m_completeNum << ",m_subNum = " << m_subNum;
			}
		}

		m_taskQueueMutex.lock();
		qDebug() << "循环结束——推出队列：" << m_taskQueue.front() << ",thread id = " << QThread::currentThreadId();
		m_taskQueue.pop_front();
		m_taskQueueMutex.unlock();
	});
}

void MemoryMasterModel::receiveDirSize(QString dirPath, qint64 dirSize)
{
	m_fileInfoMutex.lock();
	QFileInfo info(dirPath);
	m_fileNames.append(info.fileName());
	m_fileSizes.append(computeSize(dirSize));
	m_fileTpyes.append("文件夹");
	m_fileInfoMutex.unlock();

	m_completeNum++;
	if (m_completeNum < m_subNum)
	{
		emit sendPrograssBarValue(static_cast<int>(1.0 * m_completeNum / m_subNum * 100));
		qDebug() << dirPath << "计算完成, m_completeNum = " << m_completeNum << ",m_subNum = " << m_subNum;
	}
	else
	{
		completeCompute();
		emit sendPrograssBarValue(100);
		qDebug() << dirPath << "计算完成, m_completeNum = " << m_completeNum << ",m_subNum = " << m_subNum;
	}
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

void MemoryMasterModel::completeCompute()
{
	m_fileInfoMutex.lock();
	qDebug() << "结束计算线程id = " << QThread::currentThreadId();
	for (int i = 0; i < m_threadVector.size(); ++i)
	{
		// m_threadVector[i]->setExit();
		// qDebug() << "清除第" << i + 1 << "个线程";
		m_threadVector[i]->quit();
		m_threadVector[i]->wait();
		delete m_threadVector[i];
	}
	m_threadVector.clear();
	m_fileInfoMutex.unlock();
	
	qDebug() << "文件名有：" << m_fileNames;
	qDebug() << "文件类型有：" << m_fileTpyes;
	qDebug() << "文件大小有：" << m_fileSizes;
	fileSort(0);
	emit finishCompute(m_fileNames, m_fileSizes, m_fileTpyes);
}

/************************* 计算文件夹大小的线程类 ***************************/

ComputeDirSize::ComputeDirSize(QString path) :
	m_dirPath(path),
	m_exit(false)
{

}

qint64 ComputeDirSize::computeAllFilesAndDirs(QString path)
{
	if (m_exit) return 0;

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

void ComputeDirSize::setExit()
{
	m_exit = true;
}

void ComputeDirSize::run()
{
	qDebug() << "开始多线程计算文件夹: " << m_dirPath << "的大小， 线程id = " << QThread::currentThreadId();
	qint64 size = computeAllFilesAndDirs(m_dirPath);
	if (m_exit == false)
	{
		emit sendSizeValue(m_dirPath, size);
	}
	qDebug() << "多线程计算文件夹: " << m_dirPath << "的大小完成";
}