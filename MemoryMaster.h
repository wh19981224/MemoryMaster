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
    bool m_isLeftPressed;       // 鼠标左键是否按下
    QPoint m_pressPoint;        // 鼠标按下的初始位置
    int m_curPos;               // 鼠标按下的范围，共分为9块
    QPoint m_lastPressPoint;    // 鼠标上次按下的坐标

    bool m_isMaximize;
};
