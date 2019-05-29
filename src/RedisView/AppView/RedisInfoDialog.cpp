#include "RedisInfoDialog.h"
#include "ui_redisinfodialog.h"

RedisInfoDialog::RedisInfoDialog(RedisCluster *redisClient,
                                 QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RedisInfoDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("实例信息"));
    _redisClient = redisClient;
    setFixedSize(this->size());

    //    QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
    //    resize(availableGeometry.width() * 5 / 7, availableGeometry.height() * 5 / 7);
    //    move((availableGeometry.width() - width()) / 2,
    //         (availableGeometry.height() - height()) / 2);

    if(_redisClient) {
        QString clientInfo;
        _vClients.clear();
        _vClients = _redisClient->getClients(false);
        _isClusterMode = _redisClient->getClusterMode();
        _isReplicationMode = _redisClient->getReplicationMode();
        if(_isClusterMode) {
            for(int j = 0; j < _vClients.size(); ++j) {
                _vClients[j]._client = nullptr;
                clientInfo = QString("%1:%2:%3")
                        .arg(_vClients[j]._host)
                        .arg(_vClients[j]._port)
                        .arg(_vClients[j]._master ? "Master" : "Slave");
                ui->_hostComboBox->addItem(clientInfo);
            }
        } else if(_isReplicationMode) {
            for(int j = 0; j < _vClients.size(); ++j) {
                _vClients[j]._client = nullptr;
                clientInfo = QString("%1:%2:%3")
                        .arg(_vClients[j]._host)
                        .arg(_vClients[j]._port)
                        .arg(_vClients[j]._master ? "Master" : "Slave");
                ui->_hostComboBox->addItem(clientInfo);
            }
        } else {
            for(int j = 0; j < _vClients.size(); ++j) {
                _vClients[j]._client = nullptr;
                clientInfo = QString("%1:%2:%3")
                        .arg(_vClients[j]._host)
                        .arg(_vClients[j]._port)
                        .arg(_vClients[j]._master ? "Master" : "Slave");
                ui->_hostComboBox->addItem(clientInfo);
            }
        }

        ui->_itemComboBox->addItem("All");
        ui->_itemComboBox->addItem("Dbsize");
        ui->_itemComboBox->addItem("Server");
        ui->_itemComboBox->addItem("Clients");
        ui->_itemComboBox->addItem("Memory");
        ui->_itemComboBox->addItem("Persistence");
        ui->_itemComboBox->addItem("Stats");
        ui->_itemComboBox->addItem("Replication");
        ui->_itemComboBox->addItem("CPU");
        ui->_itemComboBox->addItem("Cluster");
        ui->_itemComboBox->addItem("Keyspace");

        //on__queryPushButton_clicked();
    }
}

RedisInfoDialog::~RedisInfoDialog()
{
    delete ui;
}

void RedisInfoDialog::on__queryPushButton_clicked()
{
    if(!_redisClient) {
        QMessageBox::information(this, tr("错误"), tr("客户端连接为空!"));
        return;
    }

    if(ui->_hostComboBox->count() <= 0 ||
            ui->_itemComboBox->count() <= 0)
        return;

    int iRet = 0;
    qlonglong llRet = 0;
    _cmdRsult.clear();
    _byteArray.clear();
    ui->_textBrowser->clear();
    _strCmd = ui->_itemComboBox->currentText();
    if(_strCmd == "All") {
        _strCmd = "info";
    } else if(_strCmd == "Dbsize") {
        if(!_redisClient->dbsize(llRet))
            llRet = -1;
        _appendInfo = QString("# Dbsize\r\nmaster_total_key:%1\r\n").arg(llRet);
        ui->_textBrowser->insertPlainText(_appendInfo);
        return;
    } else {
        _strCmd = QString("info %1").arg(_strCmd);
    }
    _cmdRsult = _redisClient->command(_strCmd, "", ui->_hostComboBox->currentIndex());
    for(int i = 0; i < _cmdRsult.size(); ++i) {
        _redisClient->formatToText(_cmdRsult[i], _byteArray , iRet);
        ui->_textBrowser->append(QTextCodec::codecForLocale()->toUnicode(_byteArray));
    }

    if(ui->_itemComboBox->currentText() == "All") {
        if(!_redisClient->dbsize(llRet))
            llRet = -1;
        _appendInfo = QString("# Dbsize\r\nmaster_total_key:%1\r\n").arg(llRet);
        ui->_textBrowser->insertPlainText(_appendInfo);
    }
}

void RedisInfoDialog::on__exitPushButton_clicked()
{
    accept();
}

void RedisInfoDialog::on__hostComboBox_currentTextChanged(const QString &arg1)
{
    on__queryPushButton_clicked();
    Q_UNUSED(arg1);
}

void RedisInfoDialog::on__itemComboBox_currentTextChanged(const QString &arg1)
{
    on__queryPushButton_clicked();
    Q_UNUSED(arg1);
}

void RedisInfoDialog::on__textBrowser_textChanged()
{
    ui->_textBrowser->moveCursor(QTextCursor::End);
}
