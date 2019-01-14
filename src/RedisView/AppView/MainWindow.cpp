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

    _runAct = beginMenu->addAction(tr("运行命令"), this, &MainWindow::run);
    _runAct->setStatusTip(tr("运行命令，F8快捷键..."));
    beginMenu->addSeparator();

    QAction *saveAct = beginMenu->addAction(tr("保存窗口"), this, SLOT(writeSettings()));
    saveAct->setStatusTip(tr("保存窗口大小位置信息..."));
    beginMenu->addSeparator();

    QAction *exitAct = beginMenu->addAction(tr("退出系统"), this, SLOT(exit()));
    exitAct->setStatusTip(tr("退出系统..."));
    beginMenu->addSeparator();

    // 维护菜单
    QMenu *maintainMenu = menuBar()->addMenu(tr("维护"));

    QAction *nextAct = maintainMenu->addAction(tr("信息提示"), this, &MainWindow::nextVersion);
    nextAct->setStatusTip(tr("显示提示信息..."));
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
    toolBar->addAction(exitAct);

    //设置图标
    _runAct->setIcon(QIcon(ICON_RUN));
    saveAct->setIcon(QIcon(ICON_SAVE));
    aboutAct->setIcon(QIcon(ICON_ABOUT));
    historyAct->setIcon(QIcon(ICON_HISTORY));
    exitAct->setIcon(QIcon(ICON_EXIT));
    donateAct->setIcon(QIcon(ICON_DONATE));
    nextAct->setIcon(QIcon(ICON_DISPLAY));
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
                           "版本 ：Version 1.3.0<br>"
                           "邮箱 ：cc20110101@126.com<br>"
                           "地址 ：<a href='https://sourceforge.net/projects/redisview/'>sourceforge</a> <a href='https://github.com/cc20110101/RedisView'>github</a><br>"
                           "版权 ：Copyright 2018 Powered By CC<br>"
                           )
                       );
}

void MainWindow::history() {
    QMessageBox::about(this, tr("版本历史"),
                       tr(
                           "<br>2019/01/10&nbsp;&nbsp;Version 1.3.0&nbsp;&nbsp;界面调整,增加搜索过滤功能.<br>"
                           "<br>2019/01/07&nbsp;&nbsp;Version 1.2.0&nbsp;&nbsp;自定义值模型,内存优化;新增Mac、Linux版本.<br>"
                           "<br>2018/12/30&nbsp;&nbsp;Version 1.1.0&nbsp;&nbsp;自定义键模型,内存优化.<br>"
                           "<br>2018/12/25&nbsp;&nbsp;Version 1.0.0&nbsp;&nbsp;Windows初版发布.<br>"
                           )
                       );
}

void MainWindow::nextVersion() {
    QMessageBox::information(this,tr("提示"),tr("功能暂未实现，敬请期待下一版本..."));
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
