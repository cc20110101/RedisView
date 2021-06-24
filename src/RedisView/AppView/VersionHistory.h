/**
* @file      VersionHistory.h
* @brief     说明信息
* @author    王长春
* @date      2018-11-24
* @version   001
* @copyright Copyright (c) 2018
*/
#ifndef VERSIONHISTORY_H
#define VERSIONHISTORY_H

#include <QDialog>

namespace Ui {
class VersionHistory;
}

class VersionHistory : public QDialog
{
    Q_OBJECT

public:
    explicit VersionHistory(int width, int height, QWidget *parent = nullptr);
    ~VersionHistory();

private slots:
    void on_pushButton_clicked();

private:
    Ui::VersionHistory *ui;
};

#endif // VERSIONHISTORY_H
