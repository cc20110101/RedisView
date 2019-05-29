/**
* @file      MainWindow.h
* @brief     主窗口
* @author    王长春
* @date      2018-11-24
* @version   001
* @copyright Copyright (c) 2018
*/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "AppView/Instructions.h"
#include "AppView/Donation.h"
#include "AppView/MainWidget.h"
#include "AppView/ClientDialog.h"
#include "AppView/BatchOperateDialog.h"
#include "AppView/RedisInfoDialog.h"
#include "AppView/LoginDialog.h"
#include "AppView/ContributorDialog.h"

// 主窗口
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(RedisCluster *redisClient = nullptr,
                        QWidget *parent = nullptr);

    virtual void keyPressEvent(QKeyEvent *event);

signals:

private:

    MainWidget * _mainWidget;
    QAction *_runAct;
    RedisCluster *_redisClient;

    QAction *_big5Act;
    QAction *_big5HkscsAct;
    QAction *_cp949Act;
    QAction *_eucjpAct;
    QAction *_euckrAct;
    QAction *_gb18030Act;
    QAction *_hproman8Act;
    QAction *_ibm850Act;
    QAction *_ibm866Act;
    QAction *_ibm874Act;
    QAction *_iso2022jpAct;
    QAction *_iso88591Act;
    QAction *_iso88592Act;
    QAction *_iso88593Act;
    QAction *_iso88594Act;
    QAction *_iso88595Act;
    QAction *_iso88596Act;
    QAction *_iso88597Act;
    QAction *_iso88598Act;
    QAction *_iso88599Act;
    QAction *_iso885910Act;
    QAction *_iso885913Act;
    QAction *_iso885914Act;
    QAction *_iso885915Act;
    QAction *_iso885916Act;
    QAction *_isciibngAct;
    QAction *_isciidevAct;
    QAction *_isciigjrAct;
    QAction *_isciikndAct;
    QAction *_isciimlmAct;
    QAction *_isciioriAct;
    QAction *_isciipnjAct;
    QAction *_isciitlgAct;
    QAction *_isciitmlAct;
    QAction *_koi8rAct;
    QAction *_koi8uAct;
    QAction *_macintoshAcy;
    QAction *_shiftjisAct;
    QAction *_tis620Act;
    QAction *_tsciiAct;
    QAction *_utf8Act;
    QAction *_utf16Act;
    QAction *_utf16beAct;
    QAction *_utf16leAct;
    QAction *_utf32Act;
    QAction *_utf32beAct;
    QAction *_utf32leAct;
    QAction *_windows1250Act;
    QAction *_windows1251Act;
    QAction *_windows1252Act;
    QAction *_windows1253Act;
    QAction *_windows1254Act;
    QAction *_windows1255Act;
    QAction *_windows1256Act;
    QAction *_windows1257Act;
    QAction *_windows1258Act;

private:
    // 读取窗口位置大小信息
    void readSettings();
    // 创建菜单和工具条
    void createMenu();
    // 状态条
    void createStatusBar();
    // 进入主界面
    void mainWidget();
    // 创建槽连接
    void createSlot();
    // 初始化
    void init(RedisCluster *redisClient);
    // 设置编码
    void setEncode(QString encode);
    // 设置编码图标
    void setEncodeIcon();
    // 清理编码图标
    void clearEncodeIcon();

private slots:

    // 使用说明
    void instruction();
    // 关于系统
    void about();
    // 版本历史
    void history();
    // 保存窗口大小位置信息
    void writeSettings();
    // 退出系统
    void exit();
    // 命令运行结束
    void runEnd(bool bEnd = false);
    // 运行命令
    void run();
    // 运行开始
    void runStart();
    // 捐赠
    void donate();
    // 贡献者
    void contribute();
    // 删除键值
    void batchOprate();
    // Redis实例信息
    void redisInfo();
    // 中文
    void langCnAction();
    // 英文
    void langEnAction();
    // 连接信息
    void connectInfo();
    // 刷新连接
    void refreshConnInfo();
    // 查看键值
    void keyView();
    // 订阅发布
    void subscribe();
    // 连接主机
    void connectHost();
    // 修改编码函数
    void big5Action();
    void big5HkscsAction();
    void cp949Action();
    void eucjpAction();
    void euckrAction();
    void gb18030Action();
    void hproman8Action();
    void ibm850Action();
    void ibm866Action();
    void ibm874Action();
    void iso2022jpAction();
    void iso88591Action();
    void iso88592Action();
    void iso88593Action();
    void iso88594Action();
    void iso88595Action();
    void iso88596Action();
    void iso88597Action();
    void iso88598Action();
    void iso88599Action();
    void iso885910Action();
    void iso885913Action();
    void iso885914Action();
    void iso885915Action();
    void iso885916Action();
    void isciibngAction();
    void isciidevAction();
    void isciigjrAction();
    void isciikndAction();
    void isciimlmAction();
    void isciioriAction();
    void isciipnjAction();
    void isciitlgAction();
    void isciitmlAction();
    void koi8rAction();
    void koi8uAction();
    void macintoshAction();
    void shiftjisAction();
    void tis620Action();
    void tsciiAction();
    void utf8Action();
    void utf16Action();
    void utf16beAction();
    void utf16leAction();
    void utf32Action();
    void utf32beAction();
    void utf32leAction();
    void windows1250Action();
    void windows1251Action();
    void windows1252Action();
    void windows1253Action();
    void windows1254Action();
    void windows1255Action();
    void windows1256Action();
    void windows1257Action();
    void windows1258Action();

};

#endif // MAINWINDOW_H
