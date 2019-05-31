/**
* @file      FeedBack.h
* @brief     反馈信息对话框
* @author    王长春
* @date      2019-05-31
* @version   001
* @copyright Copyright (c) 2018
*/
#ifndef FEEDBACK_H
#define FEEDBACK_H

#include "Public/Define.h"

class FeedBack : public QDialog {
    Q_OBJECT
public:
    explicit FeedBack(QWidget *parent = nullptr);

signals:

public slots:
    void onOK();
    void onExit();
};

#endif // FEEDBACK_H
