/**
* @file      CheckCodeDialog.h
* @brief     模拟注册功能
* @author    王长春
* @date      2019-6-23
* @version   001
* @copyright Copyright (c) 2018
*/
#ifndef CHECKCODEDIALOG_H
#define CHECKCODEDIALOG_H

#include "Public/Publib.h"

namespace Ui {
class CheckCodeDialog;
}

class CheckCodeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CheckCodeDialog(QWidget *parent = nullptr);
    ~CheckCodeDialog();

private slots:

    void on__okButton_clicked();

    void on__cancelButton_clicked();

private:
    Ui::CheckCodeDialog *ui;
};

#endif // CHECKCODEDIALOG_H
