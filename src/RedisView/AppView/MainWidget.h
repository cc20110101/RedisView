#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "Public/WorkThread.h"
#include "AppView/DataView.h"
#include "AppView/KeyDialog.h"
#include "Model/TreeModel.h"
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

private:

    void initKeyView();
    void initView();
    void initSet(RedisCluster *redisClient);
    void initKeyListData(int dbIndex = -1);
    void initValueListData(const QString &key, const QByteArray &type, const int &clientIndex, const int &dbindex);
    void commitValue(QList<CmdMsg> &cmd);

signals:
    void runStart();
    void runEnd(bool bEnd = true);

public slots:
    void run();
    void on__pushButtonClear_clicked();
    void on_CmdClear();

private slots:
    void count();
    void flush();
    void del();
    void alter();
    void add();
    void runError(const int taskid, const QString & error);
    void recvData(const TaskMsg taskMsg);
    void finishWork(const int taskid);
    void showTreeRightMenu(const QPoint &pos);
    void treeClicked(const QModelIndex &index);
    void on__pushButtonRun_clicked();
    void autoScroll();
    void on__checkBoxFomat_stateChanged(int arg1);
    void on__checkBoxSplit_stateChanged(int arg1);
    void on__radioButtonResp_toggled(bool checked);
    void on__radioButtonJson_toggled(bool checked);
    void on__radioButtonText_toggled(bool checked);

private:
    bool _isOpen;
    bool _isFormat;
    bool _isJson;
    bool _isResp;
    bool _isText;
    bool _isCmdSplit;
    bool _isClusterMode;
    int _idbNums;
    int _idbIndex;
    int _iRet;
    int _iScanKeySeq;
    int _iScanValueSeq;
    qlonglong _qLongLong;
    QString _strCmd;
    QString _strCmdSplit;
    QString _strConnectName;
    QByteArray _byteArray;
    QJsonArray _jsonArray;
    QJsonDocument _jsonDocument;
    RedisCluster *_redisClient;
    QTextBrowser *_textBrowser;
    DataView *_dataView;
    KeyDialog * _keyDialog;
    Ui::MainWidget *ui;
    QAction* _mCount;
    QAction* _mRefresh;
    QAction* _mCreated;
    QAction* _mDelete;
    QAction* _mAlter;
    QMenu *_menu;
    TreeModel* _itemKeyModel;
    TreeItem *_treeItemKey;
    TreeItem *_subTreeItem;
    QList<TreeItem *> _vTreeItemKey;
    QThreadPool *_threadPool;
    WorkThread *_workThread;
    TaskMsg *_taskMsg;
    QList<ClusterClient> vClients;
    QList<QByteArray> _cmdRsult;
    QList<int> _vTaskId;
    QReadWriteLock _SCAN_KEY_LOCK;
    QReadWriteLock _SCAN_VALUE_LOCK;
};

#endif // MAINWIDGET_H
