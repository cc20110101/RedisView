#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include "AppView/LoginSet.h"

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *event);
    RedisCluster * getClient();
    QString getEncode(QString lableName);
    QString getEncode();
    QString getLableName() const;

private:

    QLabel *_labelCreate;
    QLabel *_labelEdit;
    QLabel *_labelDelte;
    QGridLayout *_gBox;

    QPushButton *_btConnect;
    QPushButton *_btCancel;

    QTreeWidget *_treeWidget;
    QTreeWidgetItem *_treeWidgetItem;

    QMenu* _menu;
    QAction* _mConnect;
    QAction* _mCreated;
    QAction* _mEdit;
    QAction* _mDelete;

    ClientInfoDialog _clientInfo;
    QList<ClientInfoDialog> _vClientInfo;
    RedisCluster *_redisClient;
    QString _lableName;

private:

    void saveSet(QList<ClientInfoDialog> &vClientInfo);
    void initConnect(int index = 0);

signals:

public slots:

    void onOK();
    void onExit();
    void onConnect();
    void onCreate();
    void onEdit();
    void onDelete();
    void onLink(const QString &url);
    void showTreeRightMenu(const QPoint &pos);
    void treeItemDoubleClicked(QTreeWidgetItem* treeWidgetItem,int column);

};

#endif // LOGINDIALOG_H
