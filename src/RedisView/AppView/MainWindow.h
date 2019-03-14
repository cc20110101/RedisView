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
};

#endif // MAINWINDOW_H
