#include "AppView/MainWindow.h"

// PLEASE USE UTF-8 CODE
int main(int argc, char *argv[]) {
    // 程序框架
    QApplication app(argc, argv);
    // 初始化资源
    Q_INIT_RESOURCE(icon);

    Global::gTrans = new QTranslator();
    QString sPath = QCoreApplication::applicationDirPath() + "/" + IniFileName;
    QSettings settings(sPath, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    QString sLanguage = settings.value("language", "en").toString();
    if(sLanguage == "cn") {
        Global::gTrans->load(":/Resources/cn.qm");
    } else {
        Global::gTrans->load(":/Resources/en.qm");
    }
    app.installTranslator(Global::gTrans);

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
