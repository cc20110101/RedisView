#include "ClientDialog.h"
#include "ui_clientdialog.h"

ClientDialog::ClientDialog(RedisCluster *redisClient, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ClientDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("客户端信息"));

    QRect availableGeometry = QApplication::desktop()->availableGeometry( this);
    resize(availableGeometry.width() * 6 / 7, availableGeometry.height() * 5 / 7);
    move((availableGeometry.width() - width()) / 2,
         (availableGeometry.height() - height()) / 2);

    QStringList header;
    header<<"Node"<<"Ip"<<"Port"<<"Passwd"<<"Role"<<"Start Slot"<<"End Slot"<<"Slot Num";
    if(!redisClient) {
        QMessageBox::critical(this, tr("提示"), tr("客户端连接为空!"));
        return;
    }

    QList<ClusterClient> clientInfo = redisClient->getClients(false);
    ui->_tableWidget->setColumnCount(8);
    ui->_tableWidget->setRowCount(clientInfo.size());
    ui->_tableWidget->setHorizontalHeaderLabels(header);
    ui->_tableWidget->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->_tableWidget->verticalHeader()->setVisible(false);
    ui->_tableWidget->horizontalHeader()->setVisible(true);
    ui->_tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->_tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->_tableWidget->setFrameShape(QFrame::NoFrame);

    for(int i = 0; i < clientInfo.size(); ++i) {
        ui->_tableWidget->setItem(i,0,new QTableWidgetItem(clientInfo[i]._nodeId.isEmpty() ? "Empty" : clientInfo[i]._nodeId));
        ui->_tableWidget->setItem(i,1,new QTableWidgetItem(clientInfo[i]._host));
        ui->_tableWidget->setItem(i,2,new QTableWidgetItem(QString::number(clientInfo[i]._port)));
        ui->_tableWidget->setItem(i,3,new QTableWidgetItem(clientInfo[i]._passwd));
        ui->_tableWidget->setItem(i,4,new QTableWidgetItem(clientInfo[i]._master ? "Master" : "Slave"));
        ui->_tableWidget->setItem(i,5,new QTableWidgetItem(QString::number(clientInfo[i]._startSlot)));
        ui->_tableWidget->setItem(i,6,new QTableWidgetItem(QString::number(clientInfo[i]._endSlot)));
        ui->_tableWidget->setItem(i,7,new QTableWidgetItem(QString::number(clientInfo[i]._slotNum)));
    }
}

ClientDialog::~ClientDialog()
{
    delete ui;
}

void ClientDialog::on__pushButton_clicked()
{
    accept();
}
