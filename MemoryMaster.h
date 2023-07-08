#pragma once
#include "ui_MemoryMaster.h"

#include <QtWidgets/QMainWindow>
#include <QStandardItemModel>
#include <QMouseEvent>

class MemoryMaster : public QMainWindow
{
    Q_OBJECT

public:
    MemoryMaster(QWidget *parent = nullptr);
    ~MemoryMaster();

    int countFlag(QPoint p, int row);
    void setCursorType(int flag);
    int countRow(QPoint p);

public slots:
    void updateListWidget(QStringList, QStringList, QStringList);
    void setStateHitLabelStart();
    void setStateHitLabelUnvalidPath();
    void setWindowMaiximize();
    void setWindowHide();
    void setWindowClose();
    void setPrograssBarValue(int);

signals:
    void pathChanged(QString);
    void startCompute();

protected:
    void mousePressEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);

private:
    Ui::MemoryMasterClass ui;

    QStandardItemModel* m_tableModel;
    QRect m_areaMovable;//���ƶ����ڵ��������ֻ���ڸ������²����ƶ�����
    bool isLeftPressed;//��갴�±�־���������Ҽ���
    QPoint m_ptPress;//��갴�µĳ�ʼλ��
    int curPos;
    QPoint pLast;

    bool m_isMaximize;
};
