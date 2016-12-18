#-------------------------------------------------
#
# Project created by QtCreator 2016-06-03T13:43:38
#
#-------------------------------------------------

QT       -= core gui

TARGET = cjCMS
TEMPLATE = app
CONFIG += c++14 console
CONFIG -= app_bundle

HEADERS += \
    ../src/builderModule.h \
    ../src/cjCMS.h \
    ../src/connectionPool.h \
    ../src/modal.h \
    ../src/siteManager.h \
    ../src/taskModule.h \
    ../src/userModule.h \
    ../src/webModule.h \
    ../src/webPage.h \
    ../src/webSite.h \
    ../src/webstudio.h \
    ../src/webTemplate.h \
    ../src/widget.h

SOURCES += \
    ../src/builderModule.cpp \
    ../src/cjCMS.cpp \
    ../src/connectionPool.cpp \
    ../src/modal.cpp \
    ../src/siteManager.cpp \
    ../src/userModule.cpp \
    ../src/webModule.cpp \
    ../src/webPage.cpp \
    ../src/webSite.cpp \
    ../src/webstudio.cpp \
    ../src/webTemplate.cpp \
    ../src/widget.cpp

LIBS += \
    -L../../cjCore/Release/ -lcjCore \
    -L../../cjMySQL/Release/ -lcjMySQL \
    -L../../cjNetwork/Release/ -lcjNetwork \
    -L../lib/ -lmysqlclient
