QT       += core gui multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    cameraopencv.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    cameraopencv.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

INCLUDEPATH += \
    /usr/include/opencv4 \

LIBS += \
    $(shell pkg-config opencv4 --libs) \
    -lavutil -lavformat -lavcodec -lswscale


DESTDIR         = $$PWD/bin
MOC_DIR         = $$PWD/build
OBJECTS_DIR     = $$PWD/build

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
