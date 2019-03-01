#include "AppView/MainWindow.h"

MainWindow::MainWindow(RedisCluster *redisClient,
                       QWidget *parent) : QMainWindow(parent) {
    init(redisClient);
}

void MainWindow::init(RedisCluster *redisClient)
{
    _redisClient = redisClient;
    setWindowTitle(WindowTitle);
    setWindowIcon(QIcon(ICON_TRAY));
    readSettings();
    createMenu();
    createStatusBar();
    mainWidget();
    createSlot();
}

void MainWindow::mainWidget() {
    _mainWidget = new MainWidget(_redisClient);
    setCentralWidget(_mainWidget);
}

void MainWindow::createStatusBar() {
    /* 状态条*/
    statusBar()->showMessage("Ready");
}

void MainWindow::createMenu() {

    // 开始菜单
    QMenu *beginMenu = menuBar()->addMenu(tr("开始"));

    QAction *viewAct = beginMenu->addAction(tr("查看键值"), this, &MainWindow::keyView);
    viewAct->setStatusTip(tr("视图查看键值..."));
    beginMenu->addSeparator();

    _runAct = beginMenu->addAction(tr("运行命令"), this, &MainWindow::run);
    _runAct->setStatusTip(tr("运行命令，F8快捷键..."));
    beginMenu->addSeparator();

    QAction *msgAct = beginMenu->addAction(tr("订阅发布"), this, &MainWindow::subscribe);
    msgAct->setStatusTip(tr("订阅与发布模式..."));
    beginMenu->addSeparator();

    QAction *connectAct = beginMenu->addAction(tr("连接信息"), this, SLOT(connectInfo()));
    connectAct->setStatusTip(tr("查看客户端连接信息..."));
    beginMenu->addSeparator();

    QAction *refreshConnAct = beginMenu->addAction(tr("刷新连接"), this, SLOT(refreshConnInfo()));
    refreshConnAct->setStatusTip(tr("刷新连接信息..."));
    beginMenu->addSeparator();

    QAction *saveAct = beginMenu->addAction(tr("保存窗口"), this, SLOT(writeSettings()));
    saveAct->setStatusTip(tr("保存窗口大小位置信息..."));
    beginMenu->addSeparator();

    QAction *exitAct = beginMenu->addAction(tr("退出系统"), this, SLOT(exit()));
    exitAct->setStatusTip(tr("退出系统..."));
    beginMenu->addSeparator();

    // 维护菜单
    QMenu *maintainMenu = menuBar()->addMenu(tr("维护"));

    QAction *batchOperateAct = maintainMenu->addAction(tr("批量操作"), this, &MainWindow::batchOprate);
    batchOperateAct->setStatusTip(tr("批量操作键值..."));
    maintainMenu->addSeparator();

    // 设置菜单
    QMenu *setMenu = menuBar()->addMenu(tr("设置"));

    QMenu* langMenu = setMenu->addMenu(tr("设置语言"));

    QAction *lanCnAct = langMenu->addAction(tr("中文"), this, &MainWindow::langCnAction);
    lanCnAct->setStatusTip(tr("设置中文语言..."));
    setMenu->addSeparator();

    QAction *lanEnAct = langMenu->addAction(tr("英文"), this, &MainWindow::langEnAction);
    lanEnAct->setStatusTip(tr("设置英文语言..."));
    setMenu->addSeparator();

    // 帮助菜单
    QMenu *helpMenu = menuBar()->addMenu(tr("帮助"));

    QAction *instructionAct = helpMenu->addAction(tr("使用说明"), this, &MainWindow::instruction);
    instructionAct->setStatusTip(tr("使用说明..."));
    helpMenu->addSeparator();

    QAction *aboutAct = helpMenu->addAction(tr("关于系统"), this, &MainWindow::about);
    aboutAct->setStatusTip(tr("显示帮助信息..."));
    helpMenu->addSeparator();

    QAction *historyAct = helpMenu->addAction(tr("版本历史"), this, &MainWindow::history);
    historyAct->setStatusTip(tr("显示版本历史记录信息..."));
    helpMenu->addSeparator();

    QAction *donateAct = helpMenu->addAction(tr("捐赠作者"), this, &MainWindow::donate);
    donateAct->setStatusTip(tr("捐赠作者..."));
    helpMenu->addSeparator();


    /* 设置工具条 */
    QToolBar *toolBar = addToolBar("MyToolBar");
    toolBar->addSeparator();
    toolBar->addAction(_runAct);
    toolBar->addSeparator();
    toolBar->addAction(viewAct);
    toolBar->addSeparator();
    toolBar->addAction(msgAct);
    toolBar->addSeparator();
    toolBar->addAction(connectAct);
    toolBar->addSeparator();
    toolBar->addAction(refreshConnAct);
    toolBar->addSeparator();
    toolBar->addAction(batchOperateAct);
    toolBar->addSeparator();
    toolBar->addAction(aboutAct);
    toolBar->addSeparator();
    toolBar->addAction(exitAct);
    toolBar->addSeparator();

    //设置图标
    _runAct->setIcon(QIcon(ICON_RUN));
    viewAct->setIcon(QIcon(ICON_VIEW));
    msgAct->setIcon(QIcon(ICON_SUBSCRIBE));
    saveAct->setIcon(QIcon(ICON_SAVE));
    connectAct->setIcon(QIcon(ICON_INFO));
    refreshConnAct->setIcon(QIcon(ICON_RECONNECT));
    aboutAct->setIcon(QIcon(ICON_ABOUT));
    historyAct->setIcon(QIcon(ICON_HISTORY));
    exitAct->setIcon(QIcon(ICON_EXIT));
    donateAct->setIcon(QIcon(ICON_DONATE));
    batchOperateAct->setIcon(QIcon(ICON_BATCHOP));
    langMenu->setIcon(QIcon(ICON_LANGUAGE));
    lanCnAct->setIcon(QIcon(ICON_CN));
    lanEnAct->setIcon(QIcon(ICON_EN));
    instructionAct->setIcon(QIcon(ICON_INSTRUCTION));
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_F8) {
        run();
    } else if(event->key() == Qt::Key_F9){
        _mainWidget->on__pushButtonClear_clicked();
    } else if (event->key() == Qt::Key_F7) {
        _mainWidget->on_CmdClear();
    }
    QMainWindow::keyPressEvent(event);
}

void MainWindow::donate() {
    Donation donation(this);
    donation.exec();
}

void MainWindow::createSlot() {
    connect(_mainWidget, SIGNAL(runEnd(bool)) , this, SLOT(runEnd(bool)));
    connect(_mainWidget, &MainWidget::runStart , this, &MainWindow::runStart);
}

void MainWindow::exit() {
    int ret = QMessageBox::question(this, tr("确认"), tr("是否要退出系统？"));
    if(ret == QMessageBox::Yes) {
        this->close();
    }
}

void MainWindow::runEnd(bool bEnd)
{
    _runAct->setEnabled(bEnd);
}

void MainWindow::run()
{
    runEnd(false);
    _mainWidget->run();
}

void MainWindow::runStart()
{
    runEnd(false);
}

void MainWindow::instruction() {
    Instructions instructions(this);
    instructions.exec();
}

void MainWindow::about() {
    QMessageBox::about(this, tr("关于"),
                       tr(
                           "<br>"
                           "<b>RedisView</b><br><br>"
                           "作者 ：菜鸟小白<br>"
                           "版本 ：Community v1.6.0<br>"
                           "邮箱 ：cc20110101@126.com<br>"
                           "地址 ：<a href='https://sourceforge.net/projects/redisview/'>sourceforge</a> <a href='https://github.com/cc20110101/RedisView'>github</a><br>"
                           "版权 ：Copyright 2018 Powered By CC<br>"
                           )
                       );
}

void MainWindow::history() {
    QMessageBox::about(this, tr("版本历史"),
                       tr(
                           "<br>2019/02/21&nbsp;&nbsp;Version 1.6.0&nbsp;&nbsp;支持复制集模式.<br>"
                           "<br>2019/01/20&nbsp;&nbsp;Version 1.5.0&nbsp;&nbsp;增加批量删除键功能.<br>"
                           "<br>2019/01/19&nbsp;&nbsp;Version 1.4.0&nbsp;&nbsp;增加订阅发布模式.<br>"
                           "<br>2019/01/10&nbsp;&nbsp;Version 1.3.0&nbsp;&nbsp;界面调整,增加键值初始化扫描模式.<br>"
                           "<br>2019/01/07&nbsp;&nbsp;Version 1.2.0&nbsp;&nbsp;自定义值模型,内存优化;新增mac、linux版本.<br>"
                           "<br>2018/12/30&nbsp;&nbsp;Version 1.1.0&nbsp;&nbsp;自定义键模型,内存优化.<br>"
                           "<br>2018/12/25&nbsp;&nbsp;Version 1.0.0&nbsp;&nbsp;Windows初版发布.<br>"
                           )
                       );
}

void MainWindow::connectInfo() {
    ClientDialog clientDialog(_redisClient);
    clientDialog.exec();
}

void MainWindow::refreshConnInfo() {
    if(_mainWidget->getTaskSize() > 0) {
        QMessageBox::information(this,tr("错误"),tr("后台任务未完成,请稍后刷新连接!"));
        return;
    }

    if(_redisClient) {
        if(_redisClient->openCluster()) {
            _mainWidget->reOpenClient();
        } else {
            QMessageBox::information(this,tr("错误"),_redisClient->getErrorInfo());
        }
    } else {
        QMessageBox::information(this,tr("错误"),tr("客户端连接为空!"));
    }
}

void MainWindow::keyView() {
    _mainWidget->openView();
}

void MainWindow::subscribe() {
    _mainWidget->openMsg();
}

void MainWindow::batchOprate() {
    if(!_redisClient) {
        QMessageBox::information(this,tr("错误"),tr("客户端连接为空!"));
        return;
    }
    BatchOperateDialog batchOperateDialog(_redisClient);
    batchOperateDialog.exec();
}

void MainWindow::langCnAction() {
    QString sPath = QCoreApplication::applicationDirPath() + "/" + IniFileName;
    //settings.setPath(QSettings::IniFormat, QSettings::SystemScope, sPath);
    QSettings settings(sPath, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    settings.setValue("language", "cn");
    QMessageBox::about(this, tr("提示"), tr("语言设置成功，重启生效!"));
}

void MainWindow::langEnAction() {
    QString sPath = QCoreApplication::applicationDirPath() + "/" + IniFileName;
    //settings.setPath(QSettings::IniFormat, QSettings::SystemScope, sPath);
    QSettings settings(sPath, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    settings.setValue("language", "en");
    QMessageBox::about(this, tr("提示"), tr("语言设置成功，重启生效!"));
}

void MainWindow::readSettings() {
    QString sPath = QCoreApplication::applicationDirPath() + "/" + IniFileName;
    //settings.setPath(QSettings::IniFormat, QSettings::SystemScope, sPath);
    QSettings settings(sPath, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    const QByteArray geometry = settings.value("geometry",
                                               QByteArray()).toByteArray();
    if(geometry.isEmpty()) {
        this->showMaximized();
    } else {
        restoreGeometry(geometry);
    }
}

void MainWindow::writeSettings() {
    QString sPath = QCoreApplication::applicationDirPath() + "/" + IniFileName;
    //settings.setPath(QSettings::IniFormat, QSettings::SystemScope, sPath);
    QSettings settings(sPath, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    settings.setValue("geometry", saveGeometry());
    QMessageBox::about(this, tr("保存信息"), tr("成功保存窗口大小与位置信息！"));
}
