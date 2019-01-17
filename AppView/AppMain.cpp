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

    QDateTime nowDateTime = QDateTime::currentDateTime();
    QDateTime endDateTime(QDate(2021,1,1));
    int iday = nowDateTime.daysTo(endDateTime);
    if(iday <= 0) {
        QMessageBox box(QMessageBox::Critical,"错误","软件运行异常");
        box.setStandardButtons(QMessageBox::Yes);
        box.setDefaultButton(QMessageBox::Yes);
        box.exec();
        exit(0);
    }
    Global::gLeftDay = QString::number(iday);

    LoginDialog loginDialog;
    if(loginDialog.exec() != QDialog::Accepted) {
        exit(0);
    }

    // 主窗口
    MainWindow w(loginDialog.getClient());
    w.show();

    return app.exec();
}
