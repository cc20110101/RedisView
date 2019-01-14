#include "AppView/Instructions.h"
#include "ui_instructions.h"

Instructions::Instructions(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Instructions)
{
    ui->setupUi(this);
    setWindowTitle(tr("使用说明"));

    ui->_textBrowser->setText(tr("    RedisView工具为查看Redis数据库视图工具，提供简单的视图对Redis进行增、删、改、查，"
                                 "同时支持基本命令操作。工具支持集群模式，但是集群模式时部分命令无法使用。\n"
                                 "    工具使用scan命令扫描键值，不阻塞服务端。软件启动时没进行扫描键，请手工右键点击左边键"
                                 "标签进行刷新，目的为了防止在超大数据量时内存耗尽而没法使用命令方式操作。"));
    ui->_textBrowser->setEnabled(false);
}

Instructions::~Instructions()
{
    delete ui;
}

void Instructions::on_pushButton_clicked()
{
    accept();
}
