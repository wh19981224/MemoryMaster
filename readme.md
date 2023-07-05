# 磁盘计算大师

## 项目背景
在清除磁盘的时候总是不清楚具体是那些文件夹占用的磁盘空间较大，于是决定开发一款软件用来计算一个目录下面所有文件和文件夹的磁盘空间占用情况。

## 如何使用
打开软件后，在输入框输入一个本地文件夹或者本地文件的地址，如果是文件，就能计算这个文件的大小；如果是一个文件夹，就能计算这个文件夹下面所有文件夹和文件的大小。  
![image](https://github.com/wh19981224/MemoryMaster/tree/master/res/images/MemoryMasterShortcut.png)

## 如何协作开发
本项目使用C++11和Qt 5.15.2在visual studio 2022社区版进行开发，需要将Qt的路径配置环境变量QT_ROOT才能编译通过本项目。  
  
如果想要为本项目贡献代码，需要自己拉一个处理问题的分支，然后提交一个到master的pull request才能够成功将自己的代码同步到正式版本中。