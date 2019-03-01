#ifndef LOGINSET_H
#define LOGINSET_H

#include "Public/Define.h"

class LoginSet : public QDialog {
    Q_OBJECT
public:
    explicit LoginSet(QWidget *parent = nullptr);

    void getClientInfo(ClientInfoDialog & clientInfo);
    void setClientInfo(ClientInfoDialog & clientInfo);

signals:

private:
    QLabel *_lb_name;
    QLineEdit *_le_name;
    QLabel *_lb_addr;
    QLineEdit *_le_addr;
    QLabel *_lb_passwd;
    QLineEdit *_le_passwd;
    ClientInfoDialog _clientInfo;

public slots:
    void onOK();
    void onExit();
};

#endif // LoginSet_H
