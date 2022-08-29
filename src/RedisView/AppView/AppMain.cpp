/**
* @file      AppMain.cpp
* @brief     程序主函数入口
* @author    王长春
* @date      2018-11-24
* @version   001
* @copyright Copyright (c) 2018
*/
#include "AppView/MainWindow.h"
#include "AppView/CheckCodeDialog.h"
#include "Public/Log.h"

// PLEASE USE UTF-8 CODE BROWSE CODE
int main(int argc, char *argv[]) {
    // 程序框架
    QApplication app(argc, argv);

    // 初始化资源
    Q_INIT_RESOURCE(icon);
    QCoreApplication::setOrganizationName(OrganizationName);
    QCoreApplication::setApplicationName(ApplicationName);
    QCoreApplication::setApplicationVersion(ApplicationVersion);

    // 安装语言包
    Global::gTrans = new QTranslator();
    QString sLanguage = PubLib::getConfig("language", "cn");
    if(sLanguage == "cn") {
        Global::gTrans->load(":/Resources/cn.qm");
    } else {
        Global::gTrans->load(":/Resources/en.qm");
    }
    app.installTranslator(Global::gTrans);

    // 获取主题
    QString sTheme = PubLib::getConfig("Theme", PINK_THEME);
    Global::gTheme = sTheme;
    if(sTheme == DARK_THEME) {
        sTheme = DARK_THEME_FILE;
    } else if(sTheme == GRAY_THEME) {
        sTheme = GRAY_THEME_FILE;
    } else if(sTheme == PINK_THEME) {
        sTheme = PINK_THEME_FILE;
    } else if(sTheme == DEEPDARK_THEME) {
        sTheme = DEEPDARK_THEME_FILE;
    }

    // 安装样式表
    if(sTheme != NO_THEME) {
        QFile styleSheet(sTheme);
        if (styleSheet.open(QIODevice::ReadOnly)) {
            app.setStyleSheet(QLatin1String(styleSheet.readAll()));
            styleSheet.close();
        } else {
            Log::Error("install style sheet failed");
        }
    }

    // 登入验证
    LoginDialog loginDialog;
    if(loginDialog.exec() != QDialog::Accepted) {
        exit(0);
    }

    // 设置编码
    Global::gEncode = loginDialog.getEncode();
    Global::gConnectName = loginDialog.getLableName();
    Global::gEncode = Global::gEncode.isEmpty() ? "GB18030" : Global::gEncode;
    QTextCodec::setCodecForLocale(QTextCodec::codecForName(Global::gEncode.toLatin1()));

    // 主窗口
    MainWindow w(loginDialog.getClient());
    w.show();

    return app.exec();
}
