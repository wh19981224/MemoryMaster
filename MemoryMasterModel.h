#pragma once
#include <QObject>
#include <QMutex>
#include <QQueue>

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

class MemoryMasterModel : public QObject
{
	Q_OBJECT
public:
	qint64 computeAllFilesAndDirs(QString);
	QString computeSize(qint64);
	
	void quick_sort(QStringList q, int l, int r, SizeRelationship(* sortFunction)(QString, QString));
	void swapAll(int i, int j);

public slots:
	void receiveOnePath(QString);
	void startCompute();
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

	QStringList m_sizeUnit = {"B", "KB", "MB", "GB", "TB" };

	QMutex m_fileInfoMutex;
	QMutex m_taskQueueMutex;
	QQueue<QString> m_taskQueue;
};