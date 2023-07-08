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
    QRect m_areaMovable;//可移动窗口的区域，鼠标只有在该区域按下才能移动窗口
    bool isLeftPressed;//鼠标按下标志（不分左右键）
    QPoint m_ptPress;//鼠标按下的初始位置
    int curPos;
    QPoint pLast;

    bool m_isMaximize;
};
