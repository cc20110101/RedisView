#ifndef KEYDIALOG_H
#define KEYDIALOG_H

#include "Public/Publib.h"

class KeyDialog : public QDialog {
    Q_OBJECT
public:
    explicit KeyDialog(QWidget *parent = nullptr);

    void init();
    void setFlag(int flag = 0);
    void setTtl(const QString &strTtl);
    void setType(const QString &strType);
    void setKey(const QString &strKey);
    QString getType() const;
    QString getKey() const;
    QString getTtl() const;
    QString getValue() const;
    QList<QByteArray> getTextList() const;

private:

    int _flag;
    QString _strType;
    QString _strKey;
    QString _strTtl;
    QString _strValue;
    QByteArray _strbTtl;
    QList<QByteArray> _textList;
    QLineEdit *_editKey;
    QLineEdit *_editTtl;
    QComboBox *_combType;
    QLabel *_labelKey;
    QLabel *_labelTtl;
    QLabel *_labelType;
    QTextEdit *_textEdit;
    QPushButton *_buttonOk;
    QPushButton *_buttonNo;
    QGridLayout *_grid;

signals:

public slots:
    void onOK();
    void onExit();

};

#endif // KEYDIALOG_H
