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

    QDateTime nowDateTime = QDateTime::currentDateTime();
    QDateTime endDateTime(QDate(2020,1,1));
    int iday = nowDateTime.daysTo(endDateTime);
    if(iday <= 0) {
        QMessageBox box(QMessageBox::Critical,"提示","软件版本生命周期结束，请下载新版本");
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
