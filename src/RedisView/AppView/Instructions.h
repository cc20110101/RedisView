/**
* @file      Instructions.h
* @brief     说明信息
* @author    王长春
* @date      2018-11-24
* @version   001
* @copyright Copyright (c) 2018
*/
#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <QDialog>

namespace Ui {
class Instructions;
}

class Instructions : public QDialog
{
    Q_OBJECT

public:
    explicit Instructions(QWidget *parent = nullptr);
    ~Instructions();

private slots:
    void on_pushButton_clicked();

private:
    Ui::Instructions *ui;
};

#endif // INSTRUCTIONS_H
