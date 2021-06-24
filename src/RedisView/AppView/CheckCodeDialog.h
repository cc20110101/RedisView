#ifndef CHECKCODEDIALOG_H
#define CHECKCODEDIALOG_H

#include "Public/Publib.h"

namespace Ui {
class CheckCodeDialog;
}

class CheckCodeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CheckCodeDialog(QWidget *parent = nullptr);
    ~CheckCodeDialog();

private slots:

    void on__okButton_clicked();

    void on__cancelButton_clicked();

private:
    Ui::CheckCodeDialog *ui;
};

#endif // CHECKCODEDIALOG_H
