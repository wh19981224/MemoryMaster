#pragma execution_character_set("utf-8")
#include "MemoryMaster.h"

#include <QTableWidgetItem>
#include <QDebug>

#define MARGIN 10//四个角的长度  

MemoryMaster::MemoryMaster(QWidget *parent) :
    m_tableModel(new QStandardItemModel),
    m_isLeftPressed(false),
    m_isMaximize(false),
    QMainWindow(parent),
    m_curPos(0)
{
    ui.setupUi(this);

    ui.memoryMasterIconLabel->setPixmap(QPixmap(":/MemoryMaster/res/icons/MemoryMaster.png").scaled(QSize(20, 20), Qt::KeepAspectRatio));

    ui.closeBtn->setIcon(QIcon(":/MemoryMaster/res/icons/window_close_focus@2x.png"));
    ui.resizeBtn->setIcon(QIcon(":/MemoryMaster/res/icons/checkbox_unchecked_focus@2x.png"));
    ui.minimizeBtn->setIcon(QIcon(":/MemoryMaster/res/icons/window_minimize_focus@2x.png"));

    ui.closeBtn->setStyleSheet("QPushButton {background-color:red}");

    ui.memoryTableView->setModel(m_tableModel);
    ui.memoryTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui.memoryTableView->horizontalHeader()->setStretchLastSection(true);

    m_tableModel->setHorizontalHeaderLabels({ "文件名", "文件类型", "文件大小" });

    this->setMouseTracking(true);
    ui.centralWidget->setMouseTracking(true);
    ui.widget->setMouseTracking(true);
    ui.memoryTableView->setMouseTracking(true);
    
    connect(ui.pathLineEdit, SIGNAL(textEdited(QString)), this, SIGNAL(pathChanged(QString)));
    connect(ui.startBtn, SIGNAL(clicked()), this, SLOT(setStateHitLabelStart()));
    connect(ui.startBtn, SIGNAL(clicked()), this, SIGNAL(startCompute()));
    connect(ui.closeBtn, SIGNAL(clicked()), this, SLOT(setWindowClose()));
    connect(ui.resizeBtn, SIGNAL(clicked()), this, SLOT(setWindowMaiximize()));
    connect(ui.minimizeBtn, SIGNAL(clicked()), this, SLOT(setWindowHide()));
}

MemoryMaster::~MemoryMaster()
{

}

void MemoryMaster::updateListWidget(QStringList fileNames, QStringList fileSizes, QStringList fileTypes)
{
    qDebug() << "开始更新文件列表";
    qDebug() << "文件名有：" << fileNames;
    qDebug() << "文件类型有：" << fileTypes;
    qDebug() << "文件大小有：" << fileSizes;
    // 更新列表前先清除列表内容
    if (m_tableModel->removeRows(0, m_tableModel->rowCount()) == false)
    {
        qDebug() << "列表清除失败";
    }
    
    // 更新列表内容
    int sz = fileNames.size();
    for (int i = 0; i < sz; ++i)
    {
        QStandardItem* fileNameItem = new QStandardItem(fileNames[i]);
        QStandardItem* fileTypeItem = new QStandardItem(fileTypes[i]);
        QStandardItem* fileSizeItem = new QStandardItem(fileSizes[i]);

        m_tableModel->setItem(i, 0, fileNameItem);
        m_tableModel->setItem(i, 1, fileTypeItem);
        m_tableModel->setItem(i, 2, fileSizeItem);
    }

    ui.stateHitLabel->setText("计算结束！");
}

void MemoryMaster::setStateHitLabelStart()
{
    ui.stateHitLabel->setText("正在计算中···");
    update();
}

void MemoryMaster::setStateHitLabelUnvalidPath()
{
    ui.stateHitLabel->setText("路径不可用！");
}

void MemoryMaster::setWindowMaiximize()
{
    if (m_isMaximize)
    {
        ui.resizeBtn->setIcon(QIcon(":/MemoryMaster/res/icons/checkbox_unchecked_focus@2x.png"));
        m_isMaximize = false;
        this->showNormal();
    }
    else
    {
        ui.resizeBtn->setIcon(QIcon(":/MemoryMaster/res/icons/window_undock_focus@2x.png"));
        m_isMaximize = true;
        this->showMaximized();
    }
}

void MemoryMaster::setWindowHide()
{
    this->showMinimized();
}

void MemoryMaster::setWindowClose()
{
    this->close();
}

void MemoryMaster::setPrograssBarValue(int value)
{
    ui.progressBar->setValue(value);
}

void MemoryMaster::mousePressEvent(QMouseEvent* event)
{
    //鼠标左键
    if (event->button() == Qt::LeftButton)
    {
        m_pressPoint = event->pos();
        m_lastPressPoint = event->globalPos();
        m_isLeftPressed = true;
        m_curPos = countFlag(event->pos(), countRow(event->pos()));
    }
}

void MemoryMaster::mouseMoveEvent(QMouseEvent* event)
{
    int poss = countFlag(event->pos(), countRow(event->pos()));
    setCursorType(poss);
    if (m_isLeftPressed)
    {
        QPoint ptemp = event->globalPos();
        ptemp = ptemp - m_lastPressPoint;

        if (m_curPos == 22)//移动窗口  
        {
            ptemp = ptemp + pos();
            move(ptemp);
        }
        else
        {
            QRect wid = geometry();

            switch (m_curPos)//改变窗口的大小  
            {
            case 11:wid.setTopLeft(wid.topLeft() + ptemp); break;//左上角  
            case 13:wid.setTopRight(wid.topRight() + ptemp); break;//右上角  
            case 31:wid.setBottomLeft(wid.bottomLeft() + ptemp); break;//左下角  
            case 33:wid.setBottomRight(wid.bottomRight() + ptemp); break;//右下角  
            case 12:wid.setTop(wid.top() + ptemp.y()); break;//中上角  
            case 21:wid.setLeft(wid.left() + ptemp.x()); break;//中左角  
            case 23:wid.setRight(wid.right() + ptemp.x()); break;//中右角  
            case 32:wid.setBottom(wid.bottom() + ptemp.y()); break;//中下角  
            }
            setGeometry(wid);
        }

        m_lastPressPoint = event->globalPos();//更新位置  
    }
}

void MemoryMaster::mouseReleaseEvent(QMouseEvent*)
{
    m_isLeftPressed = false;
}

/*
窗口各个位置的flag值
以下是一个窗口的示意

|------------------------|
| 11 |      12      | 13 |
|------------------------|
|    |              |    |
| 21 |      22      | 23 |
|    |              |    |
|------------------------|
| 31 |      32      | 33 |
|------------------------|

*/

void MemoryMaster::setCursorType(int flag)//根据鼠标所在位置改变鼠标指针形状  
{
    Qt::CursorShape cursor = Qt::ArrowCursor;
    switch (flag)
    {
    case 11:
    case 33:
        cursor = Qt::SizeFDiagCursor; break;
    case 13:
    case 31:
        cursor = Qt::SizeBDiagCursor; break;
    case 21:
    case 23:
        cursor = Qt::SizeHorCursor; break;
    case 12:
    case 32:
        cursor = Qt::SizeVerCursor; break;
    case 22:
        // cursor = Qt::OpenHandCursor; break;
    default:
        // QApplication::restoreOverrideCursor();//恢复鼠标指针性状  
        break;
    }
    setCursor(cursor);
}

/*
 以下两个函数，将窗口划分为 3 * 3 大小的部分，具体划分见 setCursorType 解释
 每一个区域可以用 列数 * 10 + 行数 来表示
 于是以下两个函数分别计算列数和行数，再代入公式计算区域数

 MARGIN表示每个区域的内边界距离外边框多远
*/

int MemoryMaster::countRow(QPoint p) // 计算在哪一行  
{
    return (p.x() < MARGIN) ? 1 : (p.x() > (this->width() - MARGIN) ? 3 : 2);
}

int MemoryMaster::countFlag(QPoint p, int row) // 计算鼠标在哪一列，并和行数一起计算区域数
{
    if (p.y() < MARGIN)
        return 10 + row;
    else if (p.y() > this->height() - MARGIN)
        return 30 + row;
    else
        return 20 + row;
}