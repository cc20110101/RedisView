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
    QMenu *beginMenu = menuBar()->addMenu("开始");

    _runAct = beginMenu->addAction("运行命令", this, &MainWindow::run);
    _runAct->setStatusTip("运行命令，F8快捷键...");
    beginMenu->addSeparator();

    QAction *saveAct = beginMenu->addAction("保存窗口", this, SLOT(writeSettings()));
    saveAct->setStatusTip("保存窗口大小位置信息...");
    beginMenu->addSeparator();

    QAction *exitAct = beginMenu->addAction("退出系统", this, SLOT(exit()));
    exitAct->setStatusTip("退出系统...");
    beginMenu->addSeparator();

    // 维护菜单
    QMenu *maintainMenu = menuBar()->addMenu("维护");

    QAction *nextAct = maintainMenu->addAction("信息提示", this, &MainWindow::nextVersion);
    nextAct->setStatusTip("显示提示信息...");
    maintainMenu->addSeparator();

    // 设置菜单
    QMenu *setMenu = menuBar()->addMenu("设置");

    QAction *nextAct1 = setMenu->addAction("信息提示", this, &MainWindow::nextVersion);
    nextAct1->setStatusTip("显示提示信息...");
    setMenu->addSeparator();

    // 帮助菜单
    QMenu *helpMenu = menuBar()->addMenu("帮助");

    QAction *instructionAct = helpMenu->addAction("使用说明", this, &MainWindow::instruction);
    instructionAct->setStatusTip("使用说明...");
    helpMenu->addSeparator();

    QAction *aboutAct = helpMenu->addAction("关于系统", this, &MainWindow::about);
    aboutAct->setStatusTip("显示帮助信息...");
    helpMenu->addSeparator();

    QAction *donateAct = helpMenu->addAction("捐赠作者", this, &MainWindow::donate);
    donateAct->setStatusTip("捐赠作者...");
    helpMenu->addSeparator();


    /* 设置工具条 */
    QToolBar *toolBar = addToolBar("MyToolBar");
    toolBar->addSeparator();
    toolBar->addAction(_runAct);
    toolBar->addSeparator();
    toolBar->addAction(exitAct);

    //设置图标
    _runAct->setIcon(QIcon(ICON_RUN));
    saveAct->setIcon(QIcon(ICON_SAVE));
    aboutAct->setIcon(QIcon(ICON_ABOUT));
    exitAct->setIcon(QIcon(ICON_EXIT));
    donateAct->setIcon(QIcon(ICON_DONATE));
    nextAct->setIcon(QIcon(ICON_DISPLAY));
    nextAct1->setIcon(QIcon(ICON_DISPLAY));
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
    int ret = QMessageBox::question(this, "确认", "是否要退出系统？");
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
    QMessageBox::about(this, "关于",
                       "<br>"
                       "<b>RedisView</b><br><br>"
                       "作者 ：菜鸟人<br>"
                       "版本 ：Version 1.1812.30<br>"
                       "邮箱 ：cc20110101@126.com<br>"
                       "版权 ：Copyright 2018 Powered By CC<br>"
                       );
}

void MainWindow::nextVersion() {
    QMessageBox::information(this,"提示","功能暂未实现，敬请期待下一版本...");
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
    QMessageBox::about(this, "保存信息", "成功保存窗口大小与位置信息！");
}
