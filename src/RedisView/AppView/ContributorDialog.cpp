/**
* @file      ContributorDialog.cpp
* @brief     贡献者信息
* @author    王长春
* @date      2019-05-15
* @version   001
* @copyright Copyright (c) 2018
*/
#include "ContributorDialog.h"
#include "ui_contributordialog.h"

ContributorDialog::ContributorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ContributorDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("鸣谢"));
    setFixedSize(this->size());
    ui->_textBrowser->setEnabled(false);
    ui->_textBrowser->setText("<br><b>Sponsor:</b><br><br>"
                              "&nbsp;&nbsp;*才"
                              "&nbsp;&nbsp;*帅"
                              "&nbsp;&nbsp;*惜"
                              "&nbsp;&nbsp;列"
                              );
}

ContributorDialog::~ContributorDialog()
{
    delete ui;
}

void ContributorDialog::on__pushButton_clicked()
{
    accept();
}
