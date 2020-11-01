
QT       += core gui #项目中加入core gui模块。core gui是Qt用于GUI设计的类库模块，如果创建的是控制台（Console）应用程序，就不需要添加core gui
QT       += serialport #串口通讯
#QT      += sql #如果项目中使用到了涉及数据库操作的类就需要用到sql模块，在pro文件中需要增加如下一�
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets #条件执行语句，表示当 Qt 主版本大�4 时，才加�widgets 模块

TARGET = demo02  #表示生成的目标可执行文件的名称，即编译后生成的可执行文件是demo03.exe
TEMPLATE = app  #表示项目使用的模板是 app，是一般的应用程序

win32 {
INCLUDEPATH += D:\OpenCV\build-Qt\install\include
LIBS += D:\OpenCV\build-Qt\install\x64\mingw\bin\libopencv_*.dll
}

#SOURCES、HEADERS、FORMS 记录了项目中包含的源程序文件、头文件和窗体文件（.ui 文件）的名称。这些文件列表是Qt Creator自动添加到项目管理文件里面的，用户不需要手动修�
SOURCES += main.cpp\
        mainwindow.cpp \
    titlebar/maintitlebar.cpp \
    titlebar/titlebar.cpp \
    systemtray/systemtray.cpp \
    images_change/images_change.cpp \
    measure/getlen.cpp \
    measure/measure.cpp \
    measure/solution.cpp \
    measure/standard.cpp


HEADERS  += mainwindow.h \
    globaldef.hpp \
    titlebar/maintitlebar.h \
    titlebar/titlebar.h \
    systemtray/systemtray.h \
    images_change/images_change.h \
    measure/getlen.h \
    measure/measure.h \
    measure/param.h \
    measure/solution.h \
    measure/standard.h \
    camera/cameracrl.h \
    camera/CameraParams.h \
    camera/MvCameraControl.h \
    camera/MvDeviceBase.h \
    camera/MvErrorDefine.h \
    camera/MvGigEDevice.h \
    camera/MvInclude.h \
    camera/MvUsb3VDevice.h \
    camera/PixelType.h \
    camera/TlFactory.h


FORMS    += mainwindow.ui \
    titlebar/maintitlebar.ui

RESOURCES += \
    res.qrc

DISTFILES += \
    image_save/1.jpg \
    image_save/2.jpg \
    image_save/3.jpg \
    image_save/4.jpg \
    image_save/5.jpg \
    image_save/6.jpg \
    data.txt \
    day1.txt \
    measure/CMakeLists.txt \
    README.md \
    img/27/test/1.bmp \
    img/27/test/2.bmp \
    img/27/test/3.bmp \
    img/27/test/4.bmp \
    img/27/test/5.bmp \
    img/27/test/6.bmp \
    img/27/test/7.bmp \
    img/27/test/8.bmp \
    img/27/test/s1.bmp \
    img/27/test/s2.bmp \
    img/34/test/1.bmp \
    img/34/test/2.bmp \
    img/34/test/3.bmp \
    img/34/test/4.bmp \
    img/34/test/5.bmp \
    img/34/test/6.bmp \
    img/34/test/7.bmp \
    img/34/test/8.bmp \
    img/res/341.jpg \
    img/res/342.jpg \
    img/res/343.jpg \
    img/res/344.jpg \
    img/res/345.jpg \
    img/res/346.jpg \
    img/res/347.jpg \
    img/res/348.jpg \
    img/res/sample341.jpg \
    img/res/sample342.jpg \
    img/res/sample343.jpg \
    img/res/sample344.jpg \
    img/res/sample345.jpg \
    img/res/sample346.jpg \
    img/res/sample347.jpg \
    img/res/sample348.jpg \
    img/res/detect34.txt \
    .git/refs/heads/master \
    .git/refs/remotes/origin/HEAD \
    .git/refs/remotes/origin/master \
    .gitignore



