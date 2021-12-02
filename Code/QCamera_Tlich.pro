#-------------------------------------------------
#
# Project created by QtCreator 2021-03-24T00:41:35
#
#-------------------------------------------------

QT       += core gui
QT       += widgets
QT       += multimedia
QT       += multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QCamera_Tlich
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        camera_widget.cpp \
    v4l2cam.cpp

HEADERS += \
        camera_widget.h \
    common.h \
    v4l2cam.h

FORMS += \
        camera_widget.ui

message('运行ARM版本')
#arm opencv3 lib
INCLUDEPATH += /usr/opencv_arm/include

LIBS += /usr/opencv_arm/lib/libopencv_core.so\
        /usr/opencv_arm/lib/libopencv_highgui.so\
        /usr/opencv_arm/lib/libopencv_imgproc.so\
        /usr/opencv_arm/lib/libopencv_videoio.so\
        /usr/opencv_arm/lib/libopencv_imgcodecs.so\
        /usr/opencv_arm/lib/libopencv_objdetect.so\
         -lpthread

#message('运行X86版本')
##x86 opencv3 lib
#INCLUDEPATH += /usr/local/include          \
#               /usr/local/include/opencv   \
#               /usr/local/include/opencv2
#
#LIBS += /usr/local/lib/libopencv_core.so \
#/usr/local/lib/libopencv_highgui.so \
#/usr/local/lib/libopencv_imgproc.so \
#/usr/local/lib/libopencv_videoio.so \
#/usr/local/lib/libopencv_imgcodecs.so\
#/usr/local/lib/libopencv_objdetect.so\
#-lpthread

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
