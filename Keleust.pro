QT += widgets

CONFIG += c++17

VERSION = 0.0.1
DEFINES += PRG_VERSION=\\\"$$VERSION\\\"

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    windowmanager.cpp

HEADERS += \
    mainwindow.h \
    windowmanager.h

FORMS += \
    mainwindow.ui

win32:  LIBS += -luser32
linux:  LIBS += -lX11

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target