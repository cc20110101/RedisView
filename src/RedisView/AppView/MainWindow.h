#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "AppView/Instructions.h"
#include "AppView/Donation.h"
#include "AppView/MainWidget.h"
#include "AppView/ClientDialog.h"
#include "AppView/BatchOperateDialog.h"

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
    // 删除键值
    void batchOprate();
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
};

#endif // MAINWINDOW_H
