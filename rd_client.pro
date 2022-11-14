QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network x11extras
CONFIG += c++17
LIBS += -lX11
LIBS += -lXtst
LIBS += -lXfixes

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    key_mapper.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    common.h \
    key_mapper.h \
    mainwindow.h \
    xlib_utils.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
