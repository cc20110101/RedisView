#ifndef DONATION_H
#define DONATION_H

#include <QDialog>

namespace Ui {
class Donation;
}

class Donation : public QDialog
{
    Q_OBJECT

public:
    explicit Donation(QWidget *parent = 0);
    ~Donation();

private slots:
    void on_pushButton_clicked();

private:
    Ui::Donation *ui;
};

#endif // DONATION_H
