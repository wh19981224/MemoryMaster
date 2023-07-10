#pragma once
#include <QObject>
#include <QMutex>
#include <QQueue>
#include <QThread>

enum class SortMode
{
	FILESIZE,
	FILENAME
};

enum class SizeRelationship
{
	BIGGER,
	SMALLER,
	EQUAL
};

class ComputeDirSize;

class MemoryMasterModel : public QObject
{
	Q_OBJECT
public:
	MemoryMasterModel();
	qint64 computeAllFilesAndDirs(QString);
	QString computeSize(qint64);
	
	void quick_sort(QStringList q, int l, int r, SizeRelationship(* sortFunction)(QString, QString));
	void swapAll(int i, int j);
	void completeCompute();

public slots:
	void receiveOnePath(QString);
	void startCompute();
	void receiveDirSize(QString, qint64);
	void fileSort(int);

signals:
	void unvalidPath();
	void finishCompute(QStringList, QStringList, QStringList);
	void sendPrograssBarValue(int);

private:
	QStringList m_fileNames;
	QStringList m_fileSizes;
	QStringList m_fileTpyes;

	QString m_path;

	QStringList m_sizeUnit;

	QMutex m_fileInfoMutex;
	QMutex m_taskQueueMutex;
	QQueue<QString> m_taskQueue;
	QVector<ComputeDirSize*> m_threadVector;

	int m_subNum;
	int m_completeNum;
};

/************************* 计算文件夹大小的线程类 ***************************/

class ComputeDirSize : public QThread
{
	Q_OBJECT
public:
	ComputeDirSize(QString path);
	qint64 computeAllFilesAndDirs(QString);
	void setExit();

signals:
	void sendSizeValue(QString, qint64);

protected:
	void run();

private:
	QString m_dirPath;
	bool m_exit;
};