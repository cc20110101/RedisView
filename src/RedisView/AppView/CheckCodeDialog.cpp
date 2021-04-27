#include "CheckCodeDialog.h"
#include "ui_checkcodedialog.h"

CheckCodeDialog::CheckCodeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CheckCodeDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("注册"));
    ui->_lineEdit_1->setText(QString("%1：%2").arg(tr("软件码")).arg(PubLib::getSoftCode()));
    ui->_lineEdit_1->setReadOnly(true);
    ui->_lineEdit->setPlaceholderText(tr("请输入注册码"));
    ui->_lineEdit->setFocus();
}

CheckCodeDialog::~CheckCodeDialog()
{
    delete ui;
}

void CheckCodeDialog::on__okButton_clicked()
{
    QString md5 = ui->_lineEdit->text().trimmed();
    //    if(md5 == PubLib::getCode().trimmed()) {
    //        PubLib::saveCode(md5);
    //        accept();
    //    } else {
    //        QMessageBox::critical(this, tr("注册失败"), "无效注册码，请重新输入！");
    //    }
    if(md5 == PubLib::getCode().trimmed()) {
        PubLib::setConfig("_sys_c", md5);
        accept();
    } else {
        QMessageBox::critical(this, tr("注册失败"), "无效注册码，请重新输入！");
    }
}

void CheckCodeDialog::on__cancelButton_clicked()
{
    reject();
}
