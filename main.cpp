#include "MemoryMasterController.h"
#include <QtWidgets/QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/MemoryMaster/res/icons/MemoryMaster.png"));

    // ��ʼ��MVC�ṹ
    MemoryMaster view;
    MemoryMasterModel model;
    MemoryMasterController controller(view, model);

    // ������ʽ
    QFile file(":/MemoryMaster/res/qss/darkstyle.qss");
    file.open(QFile::ReadOnly);
    view.setStyleSheet(file.readAll());

    // ����Ϊ�ޱ߿���ʽ
    view.setWindowFlags(Qt::FramelessWindowHint);

    // ���ô���icon
    view.setWindowIcon(QIcon(":/MemoryMaster/res/icons/MemoryMaster.png"));

    // �����
    view.show();

    return a.exec();
}
