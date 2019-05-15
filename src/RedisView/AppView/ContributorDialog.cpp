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
    ui->label->setText("<br><b>Developer:</b><br><br>"
                       "&nbsp;&nbsp;cc20110101<br>"
                       "<br><br>"
                       "<br><b>Sponsor:</b><br><br>"
                       "&nbsp;&nbsp;*才<br>"
                       );
}

ContributorDialog::~ContributorDialog()
{
    delete ui;
}

void ContributorDialog::on_pushButton_clicked()
{
    accept();
}
