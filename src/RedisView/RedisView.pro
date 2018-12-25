SOURCES += \
    AppView/AppMain.cpp \
    AppView/MainWindow.cpp \
    Public/Define.cpp \
    RedisLib/RedisClient.cpp \
    RedisLib/RedisRespParser.cpp \
    RedisLib/RedisRoute.cpp \
    RedisLib/RedisTransMgr.cpp \
    RedisLib/RedisCluster.cpp \
    AppView/MainWidget.cpp \
    AppView/LoginDialog.cpp \
    AppView/LoginSet.cpp \
    Model/KeyTreeItemModel.cpp \
    Public/WorkThread.cpp \
    AppView/DataView.cpp \
    AppView/Donation.cpp \
    AppView/InputDialog.cpp \
    Public/Publib.cpp \
    AppView/KeyDialog.cpp \
    AppView/Instructions.cpp


HEADERS += \
    AppView/MainWindow.h \
    Public/Define.h \
    RedisLib/RedisClient.h \
    RedisLib/RedisRespParser.h \
    RedisLib/RedisRoute.h \
    RedisLib/RedisTransMgr.h \
    RedisLib/RedisCluster.h \
    RedisLib/RedisDefine.h \
    AppView/MainWidget.h \
    AppView/LoginDialog.h \
    AppView/LoginSet.h \
    Model/KeyTreeItemModel.h \
    Public/WorkThread.h \
    AppView/DataView.h \
    AppView/Donation.h \
    AppView/InputDialog.h \
    Public/Publib.h \
    AppView/KeyDialog.h \
    AppView/Instructions.h


TARGET = RedisView

RESOURCES += icon.qrc

QT += gui widgets core network

RC_ICONS = Resources/tray.ico

CONFIG += C++11

FORMS += \
    AppView/mainwidget.ui \
    AppView/dataview.ui \
    AppView/donation.ui \
    AppView/inputdialog.ui \
    AppView/instructions.ui

DISTFILES +=
