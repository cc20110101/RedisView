#include "AppView/MainWindow.h"
#include "AppView/LoginDialog.h"

// PLEASE USE UTF-8 CODE
int main(int argc, char *argv[]) {
    // 程序框架
    QApplication app(argc, argv);
    // 初始化资源
    Q_INIT_RESOURCE(icon);

    // 设置编码
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));

    Global::trans = new QTranslator();
    QString sPath = QCoreApplication::applicationDirPath() + "/" + IniFileName;
    QSettings settings(sPath, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    QString sLanguage = settings.value("language", "en").toString();
    if(sLanguage == "en") {
        Global::trans->load(":/Resources/en.qm");
    } else if(sLanguage == "cn") {
        Global::trans->load(":/Resources/cn.qm");
    } else {
        Global::trans->load(":/Resources/en.qm");
    }
    app.installTranslator(Global::trans);

    LoginDialog loginDialog;
    if(loginDialog.exec() != QDialog::Accepted) {
        exit(0);
    }

    // 主窗口
    MainWindow w(loginDialog.getClient());
    w.show();

    return app.exec();
}
