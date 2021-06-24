/**
* @file      VersionHistory.cpp
* @brief     说明信息
* @author    王长春
* @date      2021-04-21
* @version   001
* @copyright Copyright (c) 2021
*/
#include "AppView/VersionHistory.h"
#include "ui_versionhistory.h"

VersionHistory::VersionHistory(int width, int height, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VersionHistory)
{
    ui->setupUi(this);

    setWindowTitle(tr("版本历史"));
    setFixedSize(width * 1 / 2, height * 1 / 2);
    ui->_textBrowser->setOpenExternalLinks(true);
    ui->_textBrowser->setText(tr(
                                  "<br>2021/06/21&nbsp;&nbsp;Version 1.7.2&nbsp;&nbsp;适配浪潮云redis集群环境.<br>"
                                  "<br>2021/04/21&nbsp;&nbsp;Version 1.7.1&nbsp;&nbsp;新增键值查看功能.<br>"
                                  "<br>2019/06/26&nbsp;&nbsp;Version 1.7.0&nbsp;&nbsp;新增主题设置和数据库导入导出.<br>"
                                  "<br>2019/06/01&nbsp;&nbsp;Version 1.6.7&nbsp;&nbsp;密码AES加密存储.<br>"
                                  "<br>2019/05/30&nbsp;&nbsp;Version 1.6.6&nbsp;&nbsp;新增集群信息查看分析功能.<br>"
                                  "<br>2019/05/08&nbsp;&nbsp;Version 1.6.5&nbsp;&nbsp;修复设置含空格值失败Bug.<br>"
                                  "<br>2019/04/08&nbsp;&nbsp;Version 1.6.4&nbsp;&nbsp;修复值初始化模式Bug.<br>"
                                  "<br>2019/04/05&nbsp;&nbsp;Version 1.6.3&nbsp;&nbsp;修复键值过长显示不全Bug.<br>"
                                  "<br>2019/03/24&nbsp;&nbsp;Version 1.6.2&nbsp;&nbsp;增加操作进度提示,修复中文乱码Bug.<br>"
                                  "<br>2019/03/15&nbsp;&nbsp;Version 1.6.1&nbsp;&nbsp;支持编码选择.<br>"
                                  "<br>2019/02/21&nbsp;&nbsp;Version 1.6.0&nbsp;&nbsp;支持复制集模式.<br>"
                                  "<br>2019/01/20&nbsp;&nbsp;Version 1.5.0&nbsp;&nbsp;增加批量删除键功能.<br>"
                                  "<br>2019/01/19&nbsp;&nbsp;Version 1.4.0&nbsp;&nbsp;增加订阅发布模式.<br>"
                                  "<br>2019/01/10&nbsp;&nbsp;Version 1.3.0&nbsp;&nbsp;界面调整,增加键值初始化扫描模式.<br>"
                                  "<br>2019/01/07&nbsp;&nbsp;Version 1.2.0&nbsp;&nbsp;自定义值模型,内存优化;新增Mac、Linux版本.<br>"
                                  "<br>2018/12/30&nbsp;&nbsp;Version 1.1.0&nbsp;&nbsp;自定义键模型,内存优化.<br>"
                                  "<br>2018/12/25&nbsp;&nbsp;Version 1.0.0&nbsp;&nbsp;Windows初版发布.<br>"
                                  )
                              );
    ui->_textBrowser->setEnabled(true);
    ui->_textBrowser->setReadOnly(true);
}

VersionHistory::~VersionHistory()
{
    delete ui;
}

void VersionHistory::on_pushButton_clicked()
{
    accept();
}
