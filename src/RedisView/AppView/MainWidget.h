/**
* @file      MainWidget.h
* @brief     主控件视图
* @author    王长春
* @date      2018-11-24
* @version   001
* @copyright Copyright (c) 2018
*/
#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "Public/WorkThread.h"
#include "AppView/DataView.h"
#include "AppView/KeyDialog.h"
#include "AppView/AddIndexDialog.h"
#include "Model/KeyTreeModel.h"
#include "AppView/PubsubDialog.h"
#include "ui_mainwidget.h"

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(RedisCluster *redisClient = nullptr,
                        QWidget *parent = nullptr);
    ~MainWidget();
    void openView();
    void openMsg();
    int getTaskSize();
    void reOpenClient();
    void resizeEvent(QResizeEvent *event);

protected:
    bool eventFilter(QObject *target, QEvent *event);

private:

    void initKeyView();
    void initView();
    void initSlot();
    void initSet(RedisCluster *redisClient);
    void initKeyListData(int dbIndex = -1);
    void initValueListData(const InitValueMsg &initValueMsg);
    void commitValue(QList<CmdMsg> &cmd);
    void runWait(bool isRun = false);
    void setKeyPattern(QString keyPattern);
    QString getKeyPattern();

signals:
    void runStart();
    void runEnd(bool bEnd = true);

public slots:
    void run();
    void on__pushButtonClear_clicked();
    void on_CmdClear();

private slots:
    void closeView();
    void closeCmd();
    void closeMsg();
    void count();
    void keySort();
    void addDatabase();
    void flush();
    void del();
    void alter();
    void add();
    void runError(const int taskid, const QString & error);
    void recvData(const TaskMsg taskMsg);
    void finishWork(const int taskid);
    void showTabRightMenu(const QPoint &pos);
    void showTreeRightMenu(const QPoint &pos);
    void treeClicked(const QModelIndex &index);
    void on__pushButtonRun_clicked();
    void autoScroll();
    void on__checkBoxFomat_stateChanged(int arg1);
    void on__checkBoxSplit_stateChanged(int arg1);
    void on__radioButtonResp_toggled(bool checked);
    void on__radioButtonJson_toggled(bool checked);
    void on__radioButtonText_toggled(bool checked);
    void on__refreshButton_clicked();
    void on__publishButton_clicked();
    void on__subscribeButton_clicked();
    void on__unSubcribeButton_clicked();
    void recvMessage(const RespType &msg);
    void on__msgClearButton_clicked();
    void on__msgInfoButton_clicked();
    void on__enterRadioButton_toggled(bool checked);

private:
    bool _isEnterSend;
    bool _isOpen;
    bool _isFormat;
    bool _isJson;
    bool _isResp;
    bool _isText;
    bool _isCmdSplit;
    bool _isClusterMode;
    bool _isReplicationMode;
    bool _isCustomMode;
    bool _haveError;
    bool _displayViewTab;
    bool _displayCmdTab;
    bool _displayMsgTab;
    int _tabIndex;
    int _idbNums;
    int _idbIndex;
    int _iRet;
    int _iScanKeySeq;
    int _iScanValueSeq;
    qlonglong _qLongLong;
    QString _strCmd;
    QString _strCmdSplit;
    QString _strConnectName;
    QString _keyPattern;
    QByteArray _byteArray;
    QJsonArray _jsonArray;
    QJsonDocument _jsonDocument;
    RedisCluster *_redisClient;
    RedisClient * _redisSendClient;
    RedisClient * _redisRecvClient;
    DataView *_dataView;
    KeyDialog * _keyDialog;
    AddIndexDialog *_addDbDialog;
    Ui::MainWidget *ui;
    QAction* _mKeySort;
    QAction* _mAddDb;
    QAction* _mCount;
    QAction* _mRefresh;
    QAction* _mCreated;
    QAction* _mDelete;
    QAction* _mAlter;
    QAction* _closeView;
    QAction* _closeCmd;
    QAction* _closeMsg;
    QMenu *_treeMenu;
    QMenu *_tabMenu;
    QTabBar *_tabBar;
    QLabel *_waitLabel;
    QMovie *_movie;
    KeyTreeModel* _itemKeyModel;
    KeyTreeItem *_rootKeyTreeItem;
    KeyTreeItem *_subKeyTreeItem;
    QList<KeyTreeItem *> _vTreeItemKey;
    QThreadPool *_threadPool;
    WorkThread *_workThread;
    TaskMsg *_taskMsg;
    QList<ClusterClient> _vMasterClients;
    QList<ClusterClient> _vClients;
    QList<QByteArray> _cmdRsult;
    QList<int> _vTaskId;
    CmdMsg _cmdMsg;
    QList<CmdMsg> _vCmdMsg;
    QMap<int, Qt::SortOrder> _vSortMap;
    QVector<QWidget *> _tabPage;
    QReadWriteLock _SCAN_KEY_LOCK;
    QReadWriteLock _SCAN_VALUE_LOCK;
};

#endif // MAINWIDGET_H
