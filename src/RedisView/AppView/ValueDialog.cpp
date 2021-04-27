/**
* @file      ValueDialog.cpp
* @brief     键值查看
* @author    王长春
* @date      2021-04-21
* @version   001
* @copyright Copyright (c) 2021
*/
#include "AppView/ValueDialog.h"
#include "ui_valuedialog.h"

ValueDialog::ValueDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ValueDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("查看"));
    ui->_radioButtonJson->setChecked(false);
    ui->_radioButtonText->setChecked(true);
    ui->_radioButtonXml->setChecked(false);
}

ValueDialog::~ValueDialog()
{
    delete ui;
}

QString ValueDialog::getText() {
    return ui->_valueTextEdit->document()->toPlainText();
}

void ValueDialog::on__pushButtonY_clicked()
{
    accept();
}

void ValueDialog::clear() {
    ui->_valueTextEdit->clear();
}

void ValueDialog::on__pushButtonN_clicked()
{
    reject();
}

void ValueDialog::setEnableEdit(bool enableEdit)
{
    _enableEdit = enableEdit;
    ui->_valueTextEdit->setReadOnly(!_enableEdit);
    ui->_pushButtonY->setEnabled(_enableEdit);
}

bool ValueDialog::getEnableEdit() const
{
    return _enableEdit;
}

void ValueDialog::setText(const QString &text)
{
    if(ui->_radioButtonJson->isChecked()) {
        QJsonDocument jsonDocument = QJsonDocument::fromJson(text.toLocal8Bit().data());
        if(jsonDocument.isNull()) {
            ui->_radioButtonText->setChecked(true);
            ui->_valueTextEdit->setPlainText(text);
        } else {
            ui->_valueTextEdit->setPlainText(jsonDocument.toJson(QJsonDocument::JsonFormat::Indented));
        }
    } else if(ui->_radioButtonXml->isChecked()) {
        QDomDocument domDocument;
        if(domDocument.setContent(text)) {
            ui->_valueTextEdit->setPlainText(domDocument.toString(4));
        } else {
            ui->_radioButtonText->setChecked(true);
            ui->_valueTextEdit->setPlainText(text);
        }
    } else if(ui->_radioButtonHtml->isChecked()) {
        ui->_valueTextEdit->setHtml(text);
    } else {
        ui->_valueTextEdit->setPlainText(text);
    }
}

void ValueDialog::on__radioButtonXml_toggled(bool checked)
{
    if(!checked)
        return;
    QDomDocument domDocument;
    if(domDocument.setContent(getText())) {
        ui->_valueTextEdit->setPlainText(domDocument.toString(4));
    } else {
        ui->_radioButtonText->setChecked(true);
        QMessageBox::critical(this, tr("错误"), tr("非xml格式"));
    }
}

void ValueDialog::on__radioButtonJson_toggled(bool checked)
{
    if(!checked)
        return;

    QJsonDocument jsonDocument = QJsonDocument::fromJson(getText().toLocal8Bit().data());
    if(jsonDocument.isNull()) {
        ui->_radioButtonText->setChecked(true);
        QMessageBox::critical(this, tr("错误"), tr("非json格式"));
    } else {
        ui->_valueTextEdit->setPlainText(jsonDocument.toJson(QJsonDocument::JsonFormat::Indented));
    }
}

void ValueDialog::on__radioButtonText_toggled(bool checked)
{
    if(!checked)
        return;
}

void ValueDialog::on__radioButtonHtml_toggled(bool checked)
{
    if(!checked)
        return;
    ui->_valueTextEdit->setHtml(getText());
}
