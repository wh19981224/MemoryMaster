#include "MemoryMasterController.h"
#include <QtWidgets/QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/MemoryMaster/res/icons/MemoryMaster.png"));

    // 初始化MVC结构
    MemoryMaster view;
    MemoryMasterModel model;
    MemoryMasterController controller(view, model);

    // 创建样式
    QFile file(":/MemoryMaster/res/qss/darkstyle.qss");
    file.open(QFile::ReadOnly);
    view.setStyleSheet(file.readAll());

    // 设置为无边框样式
    view.setWindowFlags(Qt::FramelessWindowHint);

    // 设置窗口icon
    view.setWindowIcon(QIcon(":/MemoryMaster/res/icons/MemoryMaster.png"));

    // 打开软件
    view.show();

    return a.exec();
}
