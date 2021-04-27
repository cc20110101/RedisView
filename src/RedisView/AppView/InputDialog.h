/**
* @file      InputDialog.h
* @brief     键值设置输入
* @author    王长春
* @date      2018-11-24
* @version   001
* @copyright Copyright (c) 2018
*/
#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H

#include "Public/Publib.h"

namespace Ui {
class InputDialog;
}

class InputDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InputDialog(QWidget *parent = nullptr);
    ~InputDialog();

    void setTip(const QString & info);
    void setType(const int &type);
    void clear();
    QString getText();
    QList<QString> getTextList();

private slots:
    void on__pushButtonY_clicked();

    void on__pushButtonN_clicked();

private:
    Ui::InputDialog *ui;
    int _type;
    QString _text;
    QList<QString> _textList;
};

#endif // INPUTDIALOG_H
