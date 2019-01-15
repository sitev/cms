TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += ../../core/src
INCLUDEPATH += ../../logger/src
INCLUDEPATH += ../../application/src
INCLUDEPATH += ../../network/src
INCLUDEPATH += ../../mysql/src
INCLUDEPATH += ../../lib/mysqlconn/include
INCLUDEPATH += ../../webserver/src


# core lib
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../core/qt/release/ -lcore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../core/qt/debug/ -lcore
else:unix: LIBS += -L$$PWD/../../core/qt/ -lcore

INCLUDEPATH += $$PWD/../../core/src
DEPENDPATH += $$PWD/../../core/src

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../core/qt/release/libcore.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../core/qt/debug/libcore.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../core/qt/release/core.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../core/qt/debug/core.lib
else:unix: PRE_TARGETDEPS += $$PWD/../../core/qt/libcore.a

# logger lib

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../logger/qt/release/ -llogger
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../logger/qt/debug/ -llogger
else:unix:!macx: LIBS += -L$$PWD/../../logger/qt/ -llogger

INCLUDEPATH += $$PWD/../../logger/src
DEPENDPATH += $$PWD/../../logger/src

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../logger/qt/release/liblogger.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../logger/qt/debug/liblogger.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../logger/qt/release/logger.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../logger/qt/debug/logger.lib
else:unix:!macx: PRE_TARGETDEPS += $$PWD/../../logger/qt/liblogger.a

# application lib

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../application/qt/application/release/ -lapplication
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../application/qt/application/debug/ -lapplication
else:unix:!macx: LIBS += -L$$PWD/../../application/qt/application/ -lapplication

INCLUDEPATH += $$PWD/../../application/src
DEPENDPATH += $$PWD/../../application/src

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../application/qt/application/release/libapplication.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../application/qt/application/debug/libapplication.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../application/qt/application/release/application.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../application/qt/application/debug/application.lib
else:unix:!macx: PRE_TARGETDEPS += $$PWD/../../application/qt/application/libapplication.a


# network lib

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../network/qt/release/ -lnetwork
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../network/qt/debug/ -lnetwork
else:unix: LIBS += -L$$PWD/../../network/qt/ -lnetwork

INCLUDEPATH += $$PWD/../../network/src
DEPENDPATH += $$PWD/../../network/src

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../network/qt/release/libnetwork.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../network/qt/debug/libnetwork.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../network/qt/release/network.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../network/qt/debug/network.lib
else:unix: PRE_TARGETDEPS += $$PWD/../../network/qt/libnetwork.a

# mysql lib
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../mysql/qt/release/ -lmysql
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../mysql/qt/debug/ -lmysql
else:unix: LIBS += -L$$PWD/../../mysql/qt/ -lmysql

INCLUDEPATH += $$PWD/../../mysql/src
DEPENDPATH += $$PWD/../../mysql/src
INCLUDEPATH += ../../../lib/mysqlconn/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../mysql/qt/release/libmysql.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../mysql/qt/debug/libmysql.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../mysql/qt/release/mysql.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../mysql/qt/debug/mysql.lib
else:unix: PRE_TARGETDEPS += $$PWD/../../mysql/qt/libmysql.a

# mysql-connector-c lib


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../lib/mysqlconn/lib/ -llibmysql
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../lib/mysqlconn/lib/ -llibmysql
else:unix:!macx: LIBS += -L$$PWD/../../../lib/mysqlconn/lib/ -llibmysql

INCLUDEPATH += $$PWD/../../../lib/mysqlconn/include
DEPENDPATH += $$PWD/../../../lib/mysqlconn/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../lib/mysqlconn/lib/liblibmysql.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../lib/mysqlconn/lib/liblibmysqld.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../lib/mysqlconn/lib/libmysql.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../lib/mysqlconn/lib/libmysql.lib
else:unix:!macx: PRE_TARGETDEPS += $$PWD/../../../lib/mysqlconn/lib/liblibmysql.a


# webserver lib
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../webserver/qt/release/ -lwebserver
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../webserver/qt/debug/ -lwebserver
else:unix: LIBS += -L$$PWD/../../webserver/qt/ -lwebserver

INCLUDEPATH += $$PWD/../../webserver/src
DEPENDPATH += $$PWD/../../webserver/src

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../webserver/qt/release/libwebserver.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../webserver/qt/debug/libwebserver.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../webserver/qt/release/webserver.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../webserver/qt/debug/webserver.lib
else:unix: PRE_TARGETDEPS += $$PWD/../../webserver/qt/libwebserver.a

# other

unix: LIBS += -lpthread -luuid



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
    ../src/widget.cpp \
    ../src/shop.cpp

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
    ../src/widget.h \
    ../src/shop.h


