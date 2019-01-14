#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H

#include "Public/Publib.h"

namespace Ui {
class InputDialog;
}

class InputDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InputDialog(QWidget *parent = nullptr);
    ~InputDialog();

    void setTip(const QString & info);
    void setType(const QByteArray & type);
    void setFlag(int flag);
    void clear();
    QString getText();
    QList<QByteArray> getTextList();

private slots:
    void on__pushButtonY_clicked();

    void on__pushButtonN_clicked();

private:
    int _flag;
    Ui::InputDialog *ui;
    QByteArray _type;
    QString _text;
    QList<QByteArray> _textList;
};

#endif // INPUTDIALOG_H
