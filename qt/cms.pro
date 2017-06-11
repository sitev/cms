TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += ../../core/src
INCLUDEPATH += ../../network/src
INCLUDEPATH += ../../mysql/src
INCLUDEPATH += ../../libraries/mysqlconn/include
INCLUDEPATH += ../../webserver/src

SOURCES += \
    ../src/builderModule.cpp \
    ../src/cms.cpp \
    ../src/connectionPool.cpp \
    ../src/modal.cpp \
    ../src/siteManager.cpp \
    ../src/taskModule.cpp \
    ../src/userModule.cpp \
    ../src/webModule.cpp \
    ../src/webPage.cpp \
    ../src/webSite.cpp \
    ../src/webstudio.cpp \
    ../src/websurf.cpp \
    ../src/webTemplate.cpp \
    ../src/widget.cpp

HEADERS += \
    ../src/builderModule.h \
    ../src/cms.h \
    ../src/connectionPool.h \
    ../src/modal.h \
    ../src/siteManager.h \
    ../src/taskModule.h \
    ../src/userModule.h \
    ../src/webModule.h \
    ../src/webPage.h \
    ../src/webSite.h \
    ../src/webstudio.h \
    ../src/websurf.h \
    ../src/webTemplate.h \
    ../src/widget.h

LIBS += \
    -L../../core/qt/debug \
    -L../../network/qt/debug \
    -L../../libraries/mysqlconn/lib \
    -L../../mysql/qt/debug \
    -L../../webserver/qt/debug
