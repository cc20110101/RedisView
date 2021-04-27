/**
* @file      ContributorDialog.h
* @brief     贡献者信息
* @author    王长春
* @date      2019-05-15
* @version   001
* @copyright Copyright (c) 2018
*/
#ifndef CONTRIBUTORDIALOG_H
#define CONTRIBUTORDIALOG_H

#include "Public/Define.h"

namespace Ui {
class ContributorDialog;
}

class ContributorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ContributorDialog(QWidget *parent = nullptr);
    ~ContributorDialog();

private slots:
    void on__pushButton_clicked();

private:
    Ui::ContributorDialog *ui;
};

#endif // CONTRIBUTORDIALOG_H
