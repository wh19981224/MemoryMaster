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
    bool m_isLeftPressed;       // �������Ƿ���
    QPoint m_pressPoint;        // ��갴�µĳ�ʼλ��
    int m_curPos;               // ��갴�µķ�Χ������Ϊ9��
    QPoint m_lastPressPoint;    // ����ϴΰ��µ�����

    bool m_isMaximize;
};
