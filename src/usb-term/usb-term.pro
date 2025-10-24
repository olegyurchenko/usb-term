QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../usbcon.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui


INCLUDEPATH += \
../

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

LIBS += -lusb-1.0
win32 {
    DEFINES += WIN32
    LIBS += -lws2_32
    #LIBS += -liso7816 -L../../lib/win32

}
!win32 {
    DEFINES += UNIX \
        LINUX
    LIBS += -ldl -lpthread
    #LIBS += -liso7816 -L../../lib/linux.$$QT_ARCH
}
