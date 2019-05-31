/**
* @file      FeedBack.cpp
* @brief     反馈信息对话框
* @author    王长春
* @date      2019-05-31
* @version   001
* @copyright Copyright (c) 2018
*/
#include "FeedBack.h"

FeedBack::FeedBack(QWidget *parent) :
    QDialog(parent) {

    setWindowTitle(tr("反馈信息"));
    QHBoxLayout *hBox_LB = new QHBoxLayout;
    QLabel *lb_say = new QLabel(tr("请输入你要提交的内容..."));
    hBox_LB->addWidget(lb_say);

    QHBoxLayout *hBox_ED = new QHBoxLayout;
    QTextEdit *te_infomation = new QTextEdit;
    te_infomation->setPlaceholderText(tr("1、这个问题的详细描述，有何现象？\n2、是如何产生的，造成了什么影响？"));
    hBox_ED->addWidget(te_infomation);

    QHBoxLayout *hBox_LE = new QHBoxLayout;
    QLabel *lb_contact = new QLabel(tr("联系方式: "));
    QLineEdit *le_contact = new QLineEdit;
    le_contact->setPlaceholderText(tr("留下您的QQ或手机号"));
    hBox_LE->addWidget(lb_contact);
    hBox_LE->addWidget(le_contact);

    QHBoxLayout *hBox_BT = new QHBoxLayout;
    QPushButton *OK = new QPushButton(tr("确定"));
    QPushButton *Exit = new QPushButton(tr("取消"));
    hBox_BT->addWidget(OK);
    hBox_BT->addWidget(Exit);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addLayout(hBox_LB, 0, 1);
    mainLayout->addLayout(hBox_ED, 1, 1);
    mainLayout->addLayout(hBox_LE, 2, 1);
    mainLayout->addLayout(hBox_BT, 4, 1);
    mainLayout->setRowStretch(3, 1);
    mainLayout->setColumnStretch(0, 1);
    mainLayout->setColumnStretch(2, 1);

    setLayout(mainLayout);

    connect(OK, SIGNAL(clicked()), this, SLOT(onOK()));
    connect(Exit, SIGNAL(clicked()), this, SLOT(onExit()));

    te_infomation->setText(tr("感谢反馈，但是鉴于服务端成本压力，此功能暂时不实现，请到下载网站进行反馈，谢谢"));
}

void FeedBack::onOK() {
    //发送信息，服务端很容易，但开源软件成本问题此处不实现
    accept();
}

void FeedBack::onExit() {
    reject();
}
