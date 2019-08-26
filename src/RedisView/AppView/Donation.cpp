/**
* @file      Donation.cpp
* @brief     捐赠类
* @author    王长春
* @date      2018-11-24
* @version   001
* @copyright Copyright (c) 2018
*/
#include "AppView/Donation.h"
#include "ui_donation.h"

Donation::Donation(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Donation)
{
    ui->setupUi(this);
    setWindowTitle(tr("捐赠"));
    setFixedSize(this->size());
}

Donation::~Donation()
{
    delete ui;
}

void Donation::on__donationButton_clicked()
{
    accept();
}
