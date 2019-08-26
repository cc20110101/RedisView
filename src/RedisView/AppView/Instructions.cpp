/**
* @file      Instructions.cpp
* @brief     说明信息
* @author    王长春
* @date      2018-11-24
* @version   001
* @copyright Copyright (c) 2018
*/
#include "AppView/Instructions.h"
#include "ui_instructions.h"

Instructions::Instructions(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Instructions)
{
    ui->setupUi(this);
    setWindowTitle(tr("使用说明"));

    ui->_textBrowser->setOpenExternalLinks(true);
    ui->_textBrowser->setText(tr("&nbsp;&nbsp;RedisView工具为查看Redis数据库视图工具，提供简单的视图对Redis进行增、删、改、查，"
                                 "同时支持基本命令操作。工具支持集群模式，但是集群模式时部分命令无法使用。<br>"
                                 "&nbsp;&nbsp;工具使用scan命令扫描键值，不阻塞服务端。软件启动时没进行扫描键，请手工右键点击左边键"
                                 "标签进行刷新，目的为了防止在超大数据量时内存耗尽而没法使用命令方式操作。<br>"
                                 )
                              );
    ui->_textBrowser->append(tr("&nbsp;&nbsp;<a href=\"https://blog.csdn.net/cc20110101/article/details/87301562\">更多信息</a>"));
    ui->_textBrowser->setEnabled(true);
}

Instructions::~Instructions()
{
    delete ui;
}

void Instructions::on_pushButton_clicked()
{
    accept();
}
