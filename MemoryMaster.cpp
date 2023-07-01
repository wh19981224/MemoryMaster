#pragma execution_character_set("utf-8")
#include "MemoryMaster.h"

#include <QTableWidgetItem>
#include <QDebug>

#define MARGIN 10//�ĸ��ǵĳ���  

MemoryMaster::MemoryMaster(QWidget *parent) :
    QMainWindow(parent),
    m_tableModel(new QStandardItemModel),
    curPos(0),
    m_areaMovable(geometry()),
    isLeftPressed(false),
    m_isMaximize(false)
{
    ui.setupUi(this);

    ui.memoryMasterIconLabel->setPixmap(QPixmap(":/MemoryMaster/res/icons/MemoryMaster.png").scaled(QSize(20, 20), Qt::KeepAspectRatio));

    ui.closeBtn->setIcon(QIcon(":/MemoryMaster/res/icons/close.png"));
    ui.resizeBtn->setIcon(QIcon(":/MemoryMaster/res/icons/maximize.png"));
    ui.minimizeBtn->setIcon(QIcon(":/MemoryMaster/res/icons/backstage.png"));

    ui.closeBtn->setStyleSheet("QPushButton {background-color:red}");

    ui.memoryTableView->setModel(m_tableModel);
    ui.memoryTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui.memoryTableView->horizontalHeader()->setStretchLastSection(true);

    m_tableModel->setHorizontalHeaderLabels({ "�ļ���", "�ļ�����", "�ļ���С" });

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
    qDebug() << "��ʼ�����ļ��б�";
    qDebug() << "�ļ����У�" << fileNames;
    qDebug() << "�ļ������У�" << fileTypes;
    qDebug() << "�ļ���С�У�" << fileSizes;
    // �����б�ǰ������б�����
    if (m_tableModel->removeRows(0, m_tableModel->rowCount()) == false)
    {
        qDebug() << "�б����ʧ��";
    }
    
    // �����б�����
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

    ui.stateHitLabel->setText("���������");
}

void MemoryMaster::setStateHitLabelStart()
{
    ui.stateHitLabel->setText("���ڼ����С�����");
    update();
}

void MemoryMaster::setStateHitLabelUnvalidPath()
{
    ui.stateHitLabel->setText("·�������ã�");
}

void MemoryMaster::setWindowMaiximize()
{
    if (m_isMaximize)
    {
        ui.resizeBtn->setIcon(QIcon(":/MemoryMaster/res/icons/maximize.png"));
        m_isMaximize = false;
        this->showNormal();
    }
    else
    {
        ui.resizeBtn->setIcon(QIcon(":/MemoryMaster/res/icons/minimize.png"));
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

void MemoryMaster::mousePressEvent(QMouseEvent* event)
{
    //������
    if (event->button() == Qt::LeftButton)
    {
        m_ptPress = event->pos();
        pLast = event->globalPos();
        qDebug() << pos() << event->pos() << m_ptPress;
        isLeftPressed = true;
        curPos = countFlag(event->pos(), countRow(event->pos()));
    }
}

void MemoryMaster::mouseMoveEvent(QMouseEvent* event)
{
    int poss = countFlag(event->pos(), countRow(event->pos()));
    setCursorType(poss);
    if (isLeftPressed)
    {
        QPoint ptemp = event->globalPos();
        ptemp = ptemp - pLast;

        if (curPos == 22)//�ƶ�����  
        {
            ptemp = ptemp + pos();
            move(ptemp);
        }
        else
        {
            QRect wid = geometry();

            switch (curPos)//�ı䴰�ڵĴ�С  
            {
            case 11:wid.setTopLeft(wid.topLeft() + ptemp); break;//���Ͻ�  
            case 13:wid.setTopRight(wid.topRight() + ptemp); break;//���Ͻ�  
            case 31:wid.setBottomLeft(wid.bottomLeft() + ptemp); break;//���½�  
            case 33:wid.setBottomRight(wid.bottomRight() + ptemp); break;//���½�  
            case 12:wid.setTop(wid.top() + ptemp.y()); break;//���Ͻ�  
            case 21:wid.setLeft(wid.left() + ptemp.x()); break;//�����  
            case 23:wid.setRight(wid.right() + ptemp.x()); break;//���ҽ�  
            case 32:wid.setBottom(wid.bottom() + ptemp.y()); break;//���½�  
            }
            setGeometry(wid);
        }

        pLast = event->globalPos();//����λ��  
    }
}

void MemoryMaster::mouseReleaseEvent(QMouseEvent*)
{
    isLeftPressed = false;
}

int MemoryMaster::countFlag(QPoint p, int row)//�����������һ�к���һ��  
{
    if (p.y() < MARGIN)
        return 10 + row;
    else if (p.y() > this->height() - MARGIN)
        return 30 + row;
    else
        return 20 + row;
}

void MemoryMaster::setCursorType(int flag)//�����������λ�øı����ָ����״  
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
        // QApplication::restoreOverrideCursor();//�ָ����ָ����״  
        break;
    }
    setCursor(cursor);
}

int MemoryMaster::countRow(QPoint p)//��������һ��  
{
    return (p.x() < MARGIN) ? 1 : (p.x() > (this->width() - MARGIN) ? 3 : 2);
}