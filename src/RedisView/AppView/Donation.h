/**
* @file      Donation.h
* @brief     捐赠类
* @author    王长春
* @date      2018-11-24
* @version   001
* @copyright Copyright (c) 2018
*/
#ifndef DONATION_H
#define DONATION_H

#include <QDialog>

namespace Ui {
class Donation;
}

class Donation : public QDialog
{
    Q_OBJECT

public:
    explicit Donation(QWidget *parent = nullptr);
    ~Donation();

private slots:
    void on__donationButton_clicked();

private:
    Ui::Donation *ui;
};

#endif // DONATION_H
