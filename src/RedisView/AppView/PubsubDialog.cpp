/**
* @file      PubsubDialog.cpp
* @brief     订阅发布模式
* @author    王长春
* @date      2018-11-24
* @version   001
* @copyright Copyright (c) 2018
*/
#include "PubsubDialog.h"
#include "ui_pubsubdialog.h"

PubsubDialog::PubsubDialog(RedisClient *redisClient, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PubsubDialog)
{
    ui->setupUi(this);
    _redisClient = redisClient;
    setWindowTitle(tr("订阅发布信息"));
    setFixedSize(this->size());
    ui->_comboBox->addItem("CHANNELS");
    ui->_comboBox->addItem("NUMSUB");
    ui->_comboBox->addItem("NUMPAT");
    emit ui->_comboBox->currentIndexChanged(ui->_comboBox->currentText());
}

PubsubDialog::~PubsubDialog()
{
    delete ui;
}

void PubsubDialog::on__cancelButton_clicked()
{
    accept();
}

void PubsubDialog::on__queryButton_clicked()
{
    if(!_redisClient) {
        QMessageBox::about(this, tr("错误"), tr("客户端连接为空!"));
        return;
    }

    if(ui->_comboBox->currentText() == "NUMSUB") {
        if(ui->_lineEdit->text().trimmed().isEmpty()) {
            QMessageBox::about(this, tr("错误"), tr("命令为NUMSUB时,频道参数不可为空!"));
            return;
        }
    }

    ui->_textBrowser->clear();
    RespType respValue;
    if(_redisClient->pubsub(ui->_comboBox->currentText(),
                            ui->_lineEdit->text().trimmed(),respValue)) {
        if(respValue._formatType == '*' && respValue._arrayLength == 0) {
            ui->_textBrowser->append("The query result is empty");
            return;
        }

        int iRet = 0;
        QByteArray byteArray;
        _redisClient->formatToText(respValue, byteArray, iRet);
        ui->_textBrowser->append(QTextCodec::codecForLocale()->toUnicode(byteArray));
    } else {
        QMessageBox::about(this, tr("错误"), _redisClient->getErrorInfo());
        return;
    }
}

void PubsubDialog::on__comboBox_currentIndexChanged(const QString &arg1)
{
    if(arg1 == "CHANNELS") {
        ui->_lineEdit->setEnabled(true);
        ui->_lineEdit->setPlaceholderText("pattern");
    } else if(arg1 == "NUMSUB") {
        ui->_lineEdit->setEnabled(true);
        ui->_lineEdit->setPlaceholderText("channe");
    } else if(arg1 == "NUMPAT") {
        ui->_lineEdit->setEnabled(false);
        ui->_lineEdit->setPlaceholderText("");
        ui->_lineEdit->clear();
    }
}
