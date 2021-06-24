/**
* @file      AddIndexDialog.h
* @brief     添加数据库索引数对话框
* @author    王长春
* @date      2021-06-21
* @version   001
* @copyright Copyright (c) 2021
*/
#ifndef AddINDEXDIALOG_H
#define AddINDEXDIALOG_H

#include "Public/Publib.h"

class AddIndexDialog : public QDialog {
    Q_OBJECT
public:
    explicit AddIndexDialog(QWidget *parent = nullptr);
    void init();
    int dbNum() const;

private:

    int _dbNum;
    QLineEdit *_editDbNum;
    QLabel *_labelDbNum;
    QPushButton *_buttonOk;
    QPushButton *_buttonNo;
    QGridLayout *_grid;

signals:

public slots:
    void onOK();
    void onExit();

};

#endif // AddINDEXDIALOG_H
