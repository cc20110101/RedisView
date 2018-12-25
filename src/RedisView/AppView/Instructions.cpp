#include "Instructions.h"
#include "ui_instructions.h"

Instructions::Instructions(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Instructions)
{
    ui->setupUi(this);
    setWindowTitle("使用说明");

    _display = "    RedisView工具为查看Redis数据库视图工具，提供简单的视图对Redis进行增、删、改、查，"
               "同时支持基本命令操作。工具支持集群模式，集群模式时部分命令无法使用。\n"
               "    RedisView1.0.0版本支持少数据量数据库，大数据量数据库暂不支持，下一版本在进行内"
               "存优化。工具使用scan命令扫描键值，不阻塞服务端。\n";

    ui->_textBrowser->setText(_display);
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
