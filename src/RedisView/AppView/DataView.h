#ifndef DATAVIEW_H
#define DATAVIEW_H

#include <Public/Define.h>
#include "AppView/InputDialog.h"

namespace Ui {
class DataView;
}

class DataView : public QWidget
{
    Q_OBJECT

public:
    explicit DataView(QWidget *parent = 0);
    ~DataView();

    void clearData();
    void initValueListData();
    void appendValue(const QList<QByteArray> & vList, const QByteArray flag);
    void setDbIndex(const int &index);
    void setIndex(const int &index);
    void setKey(const QString &key);
    void setValue(const QString &value);
    void setType(const QByteArray &type);
    void setTimeMs(const qlonglong &timeMs);
    void setEncode(const QByteArray &encode);
    void setRefcount(const qlonglong &count);
    void setIdleTimeS(const qlonglong &times);

private:
    int _clientIndex;
    int _dbIndex;
    QString _key;
    QString _value;
    QByteArray _type;
    qlonglong _time;
    qlonglong _listIndex;
    qlonglong _longLong;
    QByteArray _byteArray;
    CmdMsg _cmdMsg;
    QList<CmdMsg> _vCmdMsg;
    QAction *_resetAc;
    QAction *_addAc;
    QAction *_delAc;
    QAction *_commitAc;
    QAction *_inheadAc;
    QAction *_intailAc;
    QAction *_delheadAc;
    QAction *_deltailAc;
    Ui::DataView *ui;
    QHBoxLayout *_headHLayout;
    QVBoxLayout *_rightVlayout;
    QVBoxLayout *_leftVlayout;
    QHBoxLayout *_bodyHLayout;
    QVBoxLayout *_mainHLayout;
    QLineEdit *_keyLineEdit;
    QLineEdit *_timeLineEdit;
    QLineEdit *_typeLineEdit;
    QLineEdit *_refcountLineEdit;
    QLineEdit *_encodLineEdit;
    QLineEdit *_idletimeLineEdit;
    QPushButton *_bodyOkButton;
    QPushButton *_bodyAddButton;
    QPushButton *_bodyDelButton;
    QPushButton *_bodyRefreshButton;
    QPushButton *_bodyAddHeadButton;
    QPushButton *_bodyAddTailButton;
    QPushButton *_bodyDelHeadButton;
    QPushButton *_bodyDelTailButton;
    QStandardItemModel* _itemValueModel;
    QTableView *_tableView;
    InputDialog *_inputDialog;
    QStandardItem *_standardItemValue;
    QStandardItem *_standardSubItem;
    QList<QStandardItem *> _vItemData;

signals:
    //注意！要使用信号，采用QObejct 和 QRunnable多继承，记得QObject要放在前面
    void getData(const QString &key, const QByteArray &type, const int &clientIndex, const int &dbindex);
    void commitData(QList<CmdMsg> &cmd);

public slots:
    void valueChanged(QStandardItem *item);
    void commit();
    void del();
    void add();
    void addHead();
    void addTail();
    void delHead();
    void delTail();
    void flush();
};

#endif // DATAVIEW_H
