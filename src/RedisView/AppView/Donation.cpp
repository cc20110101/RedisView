#include "AppView/Donation.h"
#include "ui_donation.h"

Donation::Donation(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Donation)
{
    ui->setupUi(this);
    setWindowTitle(tr("支付宝捐赠"));
    setFixedSize(this->size());
}

Donation::~Donation()
{
    delete ui;
}

void Donation::on_pushButton_clicked()
{
    accept();
}
