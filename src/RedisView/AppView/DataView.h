#ifndef DATAVIEW_H
#define DATAVIEW_H

#include "AppView/InputDialog.h"
#include "Model/ValueTableModel.h"
#include "Model/ValueTableProxyModel.h"

namespace Ui {
class DataView;
}

class DataView : public QWidget
{
    Q_OBJECT

public:
    explicit DataView(QWidget *parent = nullptr);
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
    QString _valuePattern;
    QString _key;
    QString _value;
    QByteArray _type;
    qlonglong _time;
    qlonglong _listIndex;
    qlonglong _longLong;
    QByteArray _byteArray;
    InitValueMsg _initValueMsg;
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
    QAction *_countAc;
    Ui::DataView *ui;
    QHBoxLayout *_headHLayout;
    QVBoxLayout *_rightVlayout;
    QVBoxLayout *_leftVlayout;
    QHBoxLayout *_bodyHLayout;
    QHBoxLayout *_searchHLayout;
    QVBoxLayout *_mainHLayout;
    QLineEdit *_valueScanPattern;
    QLineEdit *_keyLineEdit;
    QLineEdit *_timeLineEdit;
    QLineEdit *_typeLineEdit;
    QLineEdit *_refcountLineEdit;
    QLineEdit *_encodLineEdit;
    QLineEdit *_idletimeLineEdit;
    QLineEdit *_searchLineEdit;
    QComboBox *_syntaxComb;
    QComboBox *_columnComb;
    QCheckBox *_filterCaseSensitivityCheckBox;
    QPushButton *_searchButton;
    QPushButton *_bodyOkButton;
    QPushButton *_bodyAddButton;
    QPushButton *_bodyDelButton;
    QPushButton *_bodyRefreshButton;
    QPushButton *_bodyAddHeadButton;
    QPushButton *_bodyAddTailButton;
    QPushButton *_bodyDelHeadButton;
    QPushButton *_bodyDelTailButton;
    ValueTableModel* _itemTableModel;
    ValueTableProxyModel *_valueProxyModel;
    QTableView *_tableView;
    InputDialog *_inputDialog;
    ValueTableItem *_tableItemValue;
    ValueTableItem *_tableSubItem;
    QVector<QVariant> vRowData;

signals:
    //注意！要使用信号，采用QObejct 和 QRunnable多继承，记得QObject要放在前面
    void getData(const InitValueMsg &initValueMsg);
    void commitData(QList<CmdMsg> &cmd);

public slots:
    void valueChanged(ValueTableItem *item, int column);
    void commit();
    void del();
    void add();
    void addHead();
    void addTail();
    void delHead();
    void delTail();
    void flush();
    void count();
    void search();
    void filterRegExpChanged();
    void filterColumnChanged();
};

#endif // DATAVIEW_H
