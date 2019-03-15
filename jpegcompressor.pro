#-------------------------------------------------
#
# Project created by QtCreator 2019-02-07T14:34:59
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CMPT365A2
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    utils.cpp

HEADERS  += mainwindow.h \
    utils.h

FORMS    += mainwindow.ui
DEFINES += QT_NO_VERSION_TAGGING
QMAKE_CXXFLAGS += -D WITH_TIFF=ON -D BUILD_TIFF=ON
QMAKE_CXXFLAGS_DEBUG+= -D WITH_TIFF=ON -D BUILD_TIFF=ON

INCLUDEPATH += /usr/local/include/opencv4
LIBS += -L/opt/opencv/lib -L/usr/local/lib -lopencv_core -lopencv_imgcodecs -lopencv_highgui -lopencv_imgproc -L/usr/lib/x86_64-linux-gnu/ -ltiff
