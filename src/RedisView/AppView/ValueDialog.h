/**
* @file      ValueDialog.h
* @brief     键值查看
* @author    王长春
* @date      2021-04-21
* @version   001
* @copyright Copyright (c) 2021
*/
#ifndef VALUEDIALOG_H
#define VALUEDIALOG_H

#include "Public/Publib.h"

namespace Ui {
class ValueDialog;
}

class ValueDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ValueDialog(QWidget *parent = nullptr);
    ~ValueDialog();

    void clear();
    QString getText();
    void setText(const QString &text);

    bool getEnableEdit() const;
    void setEnableEdit(bool enableEdit);

private slots:
    void on__pushButtonY_clicked();
    void on__pushButtonN_clicked();
    void on__radioButtonXml_toggled(bool checked);
    void on__radioButtonJson_toggled(bool checked);
    void on__radioButtonText_toggled(bool checked);
    void on__radioButtonHtml_toggled(bool checked);

private:
    Ui::ValueDialog *ui;
    QString _text;
    bool _enableEdit;
};

#endif // VALUEDIALOG_H
