/**
* @file      MainWindow.cpp
* @brief     主窗口
* @author    王长春
* @date      2018-11-24
* @version   001
* @copyright Copyright (c) 2018
*/
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
    //beginMenu->addSeparator();

    _runAct = beginMenu->addAction(tr("运行命令"), this, &MainWindow::run);
    _runAct->setStatusTip(tr("运行命令，F8快捷键..."));
    //beginMenu->addSeparator();

    QAction *msgAct = beginMenu->addAction(tr("订阅发布"), this, &MainWindow::subscribe);
    msgAct->setStatusTip(tr("订阅与发布模式..."));
    beginMenu->addSeparator();

    QAction *connectAct = beginMenu->addAction(tr("连接信息"), this, SLOT(connectInfo()));
    connectAct->setStatusTip(tr("查看客户端连接信息..."));
    //beginMenu->addSeparator();

    QAction *refreshConnAct = beginMenu->addAction(tr("刷新连接"), this, SLOT(refreshConnInfo()));
    refreshConnAct->setStatusTip(tr("刷新连接信息..."));
    //beginMenu->addSeparator();

    QAction *conectHostAct = beginMenu->addAction(tr("连接主机"), this, SLOT(connectHost()));
    conectHostAct->setStatusTip(tr("连接主机..."));
    beginMenu->addSeparator();

    QAction *saveAct = beginMenu->addAction(tr("保存窗口"), this, SLOT(writeSettings()));
    saveAct->setStatusTip(tr("保存窗口大小位置信息..."));
    //beginMenu->addSeparator();

    QAction *exitAct = beginMenu->addAction(tr("退出系统"), this, SLOT(exit()));
    exitAct->setStatusTip(tr("退出系统..."));
    beginMenu->addSeparator();

    // 维护菜单
    QMenu *maintainMenu = menuBar()->addMenu(tr("维护"));

    QAction *batchOperateAct = maintainMenu->addAction(tr("批量操作"), this, &MainWindow::batchOprate);
    batchOperateAct->setStatusTip(tr("批量操作键值..."));
    //maintainMenu->addSeparator();

    QAction *redisInfoAct = maintainMenu->addAction(tr("实例信息"), this, &MainWindow::redisInfo);
    redisInfoAct->setStatusTip(tr("查看Redis实例信息..."));
    maintainMenu->addSeparator();

    // 设置菜单
    QMenu *setMenu = menuBar()->addMenu(tr("设置"));


    QMenu* themeMenu = setMenu->addMenu(tr("设置主题"));

    _nothemeAct = themeMenu->addAction(tr("默认主题"), this, &MainWindow::Notheme);
    _nothemeAct->setStatusTip(tr("默认不设置主题信息..."));

    _deepdarkthemeAct = themeMenu->addAction(tr("深黑主题"), this, &MainWindow::DeepDarktheme);
    _deepdarkthemeAct->setStatusTip(tr("设置深黑主题..."));

    _darkthemeAct = themeMenu->addAction(tr("黑色主题"), this, &MainWindow::Darktheme);
    _darkthemeAct->setStatusTip(tr("设置黑色主题..."));

    _graythemeAct = themeMenu->addAction(tr("灰色主题"), this, &MainWindow::Graytheme);
    _graythemeAct->setStatusTip(tr("设置灰色主题..."));

    _pinkthemeAct = themeMenu->addAction(tr("粉色主题"), this, &MainWindow::Pinktheme);
    _pinkthemeAct->setStatusTip(tr("设置粉色主题..."));


    QMenu* langMenu = setMenu->addMenu(tr("设置语言"));

    QAction *lanCnAct = langMenu->addAction(tr("中文"), this, &MainWindow::langCnAction);
    lanCnAct->setStatusTip(tr("设置中文语言..."));
    //langMenu->addSeparator();

    QAction *lanEnAct = langMenu->addAction(tr("英文"), this, &MainWindow::langEnAction);
    lanEnAct->setStatusTip(tr("设置英文语言..."));
    langMenu->addSeparator();

    //setMenu->addSeparator();

    QMenu* encodeMenu = setMenu->addMenu(tr("设置编码"));

    _big5Act = encodeMenu->addAction(tr("Big5"), this, &MainWindow::big5Action);
    _big5Act->setStatusTip(tr("设置Big5编码..."));

    _big5HkscsAct = encodeMenu->addAction(tr("Big5-HKSCS"), this, &MainWindow::big5HkscsAction);
    _big5HkscsAct->setStatusTip(tr("设置Big5-HKSCS编码..."));

    _cp949Act = encodeMenu->addAction(tr("CP949"), this, &MainWindow::cp949Action);
    _cp949Act->setStatusTip(tr("设置CP949编码..."));

    _eucjpAct = encodeMenu->addAction(tr("EUC-JP"), this, &MainWindow::eucjpAction);
    _eucjpAct->setStatusTip(tr("设置EUC-JP编码..."));

    _euckrAct = encodeMenu->addAction(tr("EUC-KR"), this, &MainWindow::euckrAction);
    _euckrAct->setStatusTip(tr("设置EUC-KR编码..."));

    _gb18030Act = encodeMenu->addAction(tr("GB18030"), this, &MainWindow::gb18030Action);
    _gb18030Act->setStatusTip(tr("设置GB18030编码..."));

    _hproman8Act = encodeMenu->addAction(tr("HP-ROMAN8"), this, &MainWindow::hproman8Action);
    _hproman8Act->setStatusTip(tr("设置HP-ROMAN8编码..."));

    _ibm850Act = encodeMenu->addAction(tr("IBM 850"), this, &MainWindow::ibm850Action);
    _ibm850Act->setStatusTip(tr("设置IBM 850编码..."));

    _ibm866Act = encodeMenu->addAction(tr("IBM 866"), this, &MainWindow::ibm866Action);
    _ibm866Act->setStatusTip(tr("设置IBM 866编码..."));

    _ibm874Act = encodeMenu->addAction(tr("IBM 874"), this, &MainWindow::ibm874Action);
    _ibm874Act->setStatusTip(tr("设置IBM 874编码..."));

    _iso2022jpAct = encodeMenu->addAction(tr("ISO 2022-JP"), this, &MainWindow::iso2022jpAction);
    _iso2022jpAct->setStatusTip(tr("设置ISO 2022-JP编码..."));

    _iso88591Act = encodeMenu->addAction(tr("ISO 8859-1"), this, &MainWindow::iso88591Action);
    _iso88591Act->setStatusTip(tr("设置ISO 8859-1编码..."));

    _iso88592Act = encodeMenu->addAction(tr("ISO 8859-2"), this, &MainWindow::iso88592Action);
    _iso88592Act->setStatusTip(tr("设置ISO 8859-2编码..."));

    _iso88593Act = encodeMenu->addAction(tr("ISO 8859-3"), this, &MainWindow::iso88593Action);
    _iso88593Act->setStatusTip(tr("设置ISO 8859-3编码..."));

    _iso88594Act = encodeMenu->addAction(tr("ISO 8859-4"), this, &MainWindow::iso88594Action);
    _iso88594Act->setStatusTip(tr("设置ISO 8859-4编码..."));

    _iso88595Act = encodeMenu->addAction(tr("ISO 8859-5"), this, &MainWindow::iso88595Action);
    _iso88595Act->setStatusTip(tr("设置ISO 8859-5编码..."));

    _iso88596Act = encodeMenu->addAction(tr("ISO 8859-6"), this, &MainWindow::iso88596Action);
    _iso88596Act->setStatusTip(tr("设置ISO 8859-6编码..."));

    _iso88597Act = encodeMenu->addAction(tr("ISO 8859-7"), this, &MainWindow::iso88597Action);
    _iso88597Act->setStatusTip(tr("设置ISO 8859-7编码..."));

    _iso88598Act = encodeMenu->addAction(tr("ISO 8859-8"), this, &MainWindow::iso88598Action);
    _iso88598Act->setStatusTip(tr("设置ISO 8859-8编码..."));

    _iso88599Act = encodeMenu->addAction(tr("ISO 8859-9"), this, &MainWindow::iso88599Action);
    _iso88599Act->setStatusTip(tr("设置ISO 8859-9编码..."));

    _iso885910Act = encodeMenu->addAction(tr("ISO 8859-10"), this, &MainWindow::iso885910Action);
    _iso885910Act->setStatusTip(tr("设置ISO 8859-10编码..."));

    _iso885913Act = encodeMenu->addAction(tr("ISO 8859-13"), this, &MainWindow::iso885913Action);
    _iso885913Act->setStatusTip(tr("设置ISO 8859-13编码..."));

    _iso885914Act = encodeMenu->addAction(tr("ISO 8859-14"), this, &MainWindow::iso885914Action);
    _iso885914Act->setStatusTip(tr("设置ISO 8859-14编码..."));

    _iso885915Act = encodeMenu->addAction(tr("ISO 8859-15"), this, &MainWindow::iso885915Action);
    _iso885915Act->setStatusTip(tr("设置ISO 8859-15编码..."));

    _iso885916Act = encodeMenu->addAction(tr("ISO 8859-16"), this, &MainWindow::iso885916Action);
    _iso885916Act->setStatusTip(tr("设置ISO 8859-16编码..."));

    _isciibngAct = encodeMenu->addAction(tr("Iscii-Bng"), this, &MainWindow::isciibngAction);
    _isciibngAct->setStatusTip(tr("设置Iscii-Bng编码..."));

    _isciidevAct = encodeMenu->addAction(tr("Iscii-Dev"), this, &MainWindow::isciidevAction);
    _isciidevAct->setStatusTip(tr("设置Iscii-Dev编码..."));

    _isciigjrAct = encodeMenu->addAction(tr("Iscii-Gjr"), this, &MainWindow::isciigjrAction);
    _isciigjrAct->setStatusTip(tr("设置Iscii-Gjr编码..."));

    _isciikndAct = encodeMenu->addAction(tr("Iscii-Knd"), this, &MainWindow::isciikndAction);
    _isciikndAct->setStatusTip(tr("设置Iscii-Knd编码..."));

    _isciimlmAct = encodeMenu->addAction(tr("Iscii-Mlm"), this, &MainWindow::isciimlmAction);
    _isciimlmAct->setStatusTip(tr("设置Iscii-Mlm编码..."));

    _isciioriAct = encodeMenu->addAction(tr("Iscii-Ori"), this, &MainWindow::isciioriAction);
    _isciioriAct->setStatusTip(tr("设置Iscii-Ori编码..."));

    _isciipnjAct = encodeMenu->addAction(tr("Iscii-Pnj"), this, &MainWindow::isciipnjAction);
    _isciipnjAct->setStatusTip(tr("设置Iscii-Pnj编码..."));

    _isciitlgAct = encodeMenu->addAction(tr("Iscii-Tlg"), this, &MainWindow::isciitlgAction);
    _isciitlgAct->setStatusTip(tr("设置Iscii-Tlg编码..."));

    _isciitmlAct = encodeMenu->addAction(tr("Iscii-Tml"), this, &MainWindow::isciitmlAction);
    _isciitmlAct->setStatusTip(tr("设置Iscii-Tml编码..."));

    _koi8rAct = encodeMenu->addAction(tr("KOI8-R"), this, &MainWindow::koi8rAction);
    _koi8rAct->setStatusTip(tr("设置KOI8-R编码..."));

    _koi8uAct = encodeMenu->addAction(tr("KOI8-U"), this, &MainWindow::koi8uAction);
    _koi8uAct->setStatusTip(tr("设置KOI8-U编码..."));

    _macintoshAcy = encodeMenu->addAction(tr("Macintosh"), this, &MainWindow::macintoshAction);
    _macintoshAcy->setStatusTip(tr("设置Macintosh编码..."));

    _shiftjisAct = encodeMenu->addAction(tr("Shift-JIS"), this, &MainWindow::shiftjisAction);
    _shiftjisAct->setStatusTip(tr("设置Shift-JIS编码..."));

    _tis620Act = encodeMenu->addAction(tr("TIS-620"), this, &MainWindow::tis620Action);
    _tis620Act->setStatusTip(tr("设置TIS-620编码..."));

    _tsciiAct = encodeMenu->addAction(tr("TSCII"), this, &MainWindow::tsciiAction);
    _tsciiAct->setStatusTip(tr("设置TSCII编码..."));

    _utf8Act = encodeMenu->addAction(tr("UTF-8"), this, &MainWindow::utf8Action);
    _utf8Act->setStatusTip(tr("设置UTF-8编码..."));

    _utf16Act = encodeMenu->addAction(tr("UTF-16"), this, &MainWindow::utf16Action);
    _utf16Act->setStatusTip(tr("设置UTF-16编码..."));

    _utf16beAct = encodeMenu->addAction(tr("UTF-16BE"), this, &MainWindow::utf16beAction);
    _utf16beAct->setStatusTip(tr("设置UTF-16BE编码..."));

    _utf16leAct = encodeMenu->addAction(tr("UTF-16LE"), this, &MainWindow::utf16leAction);
    _utf16leAct->setStatusTip(tr("设置UTF-16LE编码..."));

    _utf32Act = encodeMenu->addAction(tr("UTF-32"), this, &MainWindow::utf32Action);
    _utf32Act->setStatusTip(tr("设置UTF-32编码..."));

    _utf32beAct = encodeMenu->addAction(tr("UTF-32BE"), this, &MainWindow::utf32beAction);
    _utf32beAct->setStatusTip(tr("设置UTF-32BE编码..."));

    _utf32leAct = encodeMenu->addAction(tr("UTF-32LE"), this, &MainWindow::utf32leAction);
    _utf32leAct->setStatusTip(tr("设置UTF-32LE编码..."));

    _windows1250Act = encodeMenu->addAction(tr("Windows-1250"), this, &MainWindow::windows1250Action);
    _windows1250Act->setStatusTip(tr("设置Windows-1250编码..."));

    _windows1251Act = encodeMenu->addAction(tr("Windows-1251"), this, &MainWindow::windows1251Action);
    _windows1251Act->setStatusTip(tr("设置Windows-1251编码..."));

    _windows1252Act = encodeMenu->addAction(tr("Windows-1252"), this, &MainWindow::windows1252Action);
    _windows1252Act->setStatusTip(tr("设置Windows-1252编码..."));

    _windows1253Act = encodeMenu->addAction(tr("Windows-1253"), this, &MainWindow::windows1253Action);
    _windows1253Act->setStatusTip(tr("设置Windows-1253编码..."));

    _windows1254Act = encodeMenu->addAction(tr("Windows-1254"), this, &MainWindow::windows1254Action);
    _windows1254Act->setStatusTip(tr("设置Windows-1254编码..."));

    _windows1255Act = encodeMenu->addAction(tr("Windows-1255"), this, &MainWindow::windows1255Action);
    _windows1255Act->setStatusTip(tr("设置Windows-1255编码..."));

    _windows1256Act = encodeMenu->addAction(tr("Windows-1256"), this, &MainWindow::windows1256Action);
    _windows1256Act->setStatusTip(tr("设置Windows-1256编码..."));

    _windows1257Act = encodeMenu->addAction(tr("Windows-1257"), this, &MainWindow::windows1257Action);
    _windows1257Act->setStatusTip(tr("设置Windows-1257编码..."));

    _windows1258Act = encodeMenu->addAction(tr("Windows-1258"), this, &MainWindow::windows1258Action);
    _windows1258Act->setStatusTip(tr("设置Windows-1258编码..."));
    //setMenu->addSeparator();

    //QAction *fontAct = setMenu->addAction(tr("设置字体"), this, &MainWindow::setFontAction);
    //fontAct->setStatusTip(tr("设置程序字体..."));
    //setMenu->addSeparator();


    // 帮助菜单
    QMenu *helpMenu = menuBar()->addMenu(tr("帮助"));

    QAction *instructionAct = helpMenu->addAction(tr("使用说明"), this, &MainWindow::instruction);
    instructionAct->setStatusTip(tr("使用说明..."));
    //helpMenu->addSeparator();

    QAction *historyAct = helpMenu->addAction(tr("版本历史"), this, &MainWindow::history);
    historyAct->setStatusTip(tr("显示版本历史记录信息..."));
    helpMenu->addSeparator();

    QAction *donateAct = helpMenu->addAction(tr("捐赠作者"), this, &MainWindow::donate);
    donateAct->setStatusTip(tr("捐赠作者..."));

    QAction *contributorAct = helpMenu->addAction(tr("特别鸣谢"), this, &MainWindow::contribute);
    contributorAct->setStatusTip(tr("鸣谢贡献者信息..."));
    helpMenu->addSeparator();

    QAction *feedbackAct = helpMenu->addAction(tr("我要反馈"), this, &MainWindow::feedback);
    feedbackAct->setStatusTip(tr("反馈BUG或提交建议..."));
    //helpMenu->addSeparator();

    QAction *updateAct = helpMenu->addAction(tr("检查更新"), this, &MainWindow::updatesys);
    updateAct->setStatusTip(tr("检查是否有新版本..."));
    //helpMenu->addSeparator();

    QAction *aboutAct = helpMenu->addAction(tr("关于系统"), this, &MainWindow::about);
    aboutAct->setStatusTip(tr("显示系统信息..."));
    helpMenu->addSeparator();


    /* 设置工具条 */
    QToolBar *toolBar = addToolBar("MyToolBar");
    //toolBar->addSeparator();
    toolBar->addAction(_runAct);
    //toolBar->addSeparator();
    toolBar->addAction(viewAct);
    //toolBar->addSeparator();
    toolBar->addAction(msgAct);
    //toolBar->addSeparator();
    toolBar->addAction(connectAct);
    //toolBar->addSeparator();
    toolBar->addAction(refreshConnAct);
    //toolBar->addSeparator();
    toolBar->addAction(batchOperateAct);
    //toolBar->addSeparator();
    toolBar->addAction(redisInfoAct);
    //toolBar->addSeparator();
    toolBar->addAction(donateAct);
    //toolBar->addSeparator();
    toolBar->addAction(contributorAct);
    //toolBar->addSeparator();
    toolBar->addAction(aboutAct);
    //toolBar->addSeparator();
    toolBar->addAction(exitAct);
    //toolBar->addSeparator();

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
    contributorAct->setIcon(QIcon(ICON_CONTRIBUTOR));
    themeMenu->setIcon(QIcon(ICON_CONTRIBUTOR));
    batchOperateAct->setIcon(QIcon(ICON_BATCHOP));
    redisInfoAct->setIcon(QIcon(ICON_REDISINFO));
    langMenu->setIcon(QIcon(ICON_LANGUAGE));
    encodeMenu->setIcon(QIcon(ICON_ENCODE));
    lanCnAct->setIcon(QIcon(ICON_CN));
    lanEnAct->setIcon(QIcon(ICON_EN));
    instructionAct->setIcon(QIcon(ICON_INSTRUCTION));
    conectHostAct->setIcon(QIcon(ICON_LONGIN));
    feedbackAct->setIcon(QIcon(ICON_FEEDBACK));
    updateAct->setIcon(QIcon(ICON_UPDATE));

    // 托盘菜单
    QMenu *exitMenu = new QMenu(this);
    exitMenu->addAction(QIcon(ICON_DISPLAY), tr("显示窗口"), this, SLOT(trayShowWindow()));
    exitMenu->addAction(QIcon(ICON_HIDE), tr("隐藏窗口"), this, SLOT(trayHideWindow()));
    exitMenu->addAction(QIcon(ICON_EXIT), tr("退出系统"), this, SLOT(trayExit()));

    _sysTrayIcon = new QSystemTrayIcon(this);
    _sysTrayIcon->setIcon(QIcon(ICON_TRAY));
    _sysTrayIcon->setToolTip(WindowTitle);
    _sysTrayIcon->show();
    _sysTrayIcon->setContextMenu(exitMenu);
    connect(_sysTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(slotActivated(QSystemTrayIcon::ActivationReason)));
    // 关闭标志
    _exitFlag = false;
    setEncodeIcon();
    setThemeIcon();
}

void MainWindow::slotActivated(QSystemTrayIcon::ActivationReason reason) {
    if(reason == QSystemTrayIcon::Trigger) {
        if(this->isHidden())
            this->show();
        else
            this->hide();
    }
}

void MainWindow::closeEvent(QCloseEvent *e) {

    if(_exitFlag == false && !this->isHidden()) {
        e->ignore();
        this->hide();
    } else
        e->accept();
}

void MainWindow::exit() {
    int ret = QMessageBox::question(this, tr("确认"), tr("是否要退出系统？"));
    if(ret == QMessageBox::Yes) {
        _exitFlag = true;
        this->close();
    } else {
        _exitFlag = false;
    }
}

void MainWindow::trayExit() {
    _exitFlag = true;
    this->close();
}

void MainWindow::trayShowWindow() {
    if(this->isHidden())
        this->show();
    if(this->isMinimized()) {
        this->showNormal();
    }
}

void  MainWindow::trayHideWindow() {
    if(!this->isHidden())
        this->hide();
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

void MainWindow::updatesys() {
    UpdateView *updateView = new UpdateView();
    updateView->show();
}

void MainWindow::donate() {
    Donation donation(this);
    donation.exec();
}

void MainWindow::contribute() {
    ContributorDialog contributor(this);
    contributor.exec();
}

void MainWindow::Darktheme() {
    clearThemeIcon();
    setTheme(DARK_THEME);
    setThemeIcon();
}

void MainWindow::DeepDarktheme() {
    clearThemeIcon();
    setTheme(DEEPDARK_THEME);
    setThemeIcon();
}

void MainWindow::Notheme() {
    clearThemeIcon();
    setTheme(NO_THEME);
    setThemeIcon();
}

void MainWindow::Graytheme() {
    clearThemeIcon();
    setTheme(GRAY_THEME);
    setThemeIcon();
}

void MainWindow::Pinktheme() {
    clearThemeIcon();
    setTheme(PINK_THEME);
    setThemeIcon();
}

void MainWindow::setTheme(QString theme) {
    PubLib::setConfig("Theme",theme);
    Global::gTheme = theme;
    QMessageBox::about(this, tr("提示"), tr("主题设置成功，重启生效!"));
}

void MainWindow::setThemeIcon() {
    if(Global::gTheme == DARK_THEME) {
        _darkthemeAct->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gTheme == GRAY_THEME) {
        _graythemeAct->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gTheme == PINK_THEME) {
        _pinkthemeAct->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gTheme == DEEPDARK_THEME) {
        _deepdarkthemeAct->setIcon(QIcon(ICON_SELECT));
    } else {
        _nothemeAct->setIcon(QIcon(ICON_SELECT));
    }
}

void MainWindow::clearThemeIcon() {
    if(Global::gTheme == DARK_THEME) {
        _darkthemeAct->setIcon(QIcon());
    } else if(Global::gTheme == GRAY_THEME) {
        _graythemeAct->setIcon(QIcon());
    } else if(Global::gTheme == PINK_THEME) {
        _pinkthemeAct->setIcon(QIcon());
    } else if(Global::gTheme == DEEPDARK_THEME) {
        _deepdarkthemeAct->setIcon(QIcon());
    } else {
        _nothemeAct->setIcon(QIcon());
    }
}

void MainWindow::createSlot() {
    connect(_mainWidget, SIGNAL(runEnd(bool)) , this, SLOT(runEnd(bool)));
    connect(_mainWidget, &MainWidget::runStart , this, &MainWindow::runStart);
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

void MainWindow::setFontAction() {
    bool isok;
    QFont qf = QFontDialog::getFont(
                &isok, QFont("Courier New", 10), this, tr("选择字体"));
    if(isok)
        qApp->setFont(qf);
}

void MainWindow::instruction() {
    Instructions instructions(this);
    instructions.exec();
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
        if(_redisClient->getCustomMode()) {
            if(_redisClient->openClient()) {
                _mainWidget->reOpenClient();
            } else {
                QMessageBox::information(this,tr("错误"),_redisClient->getErrorInfo());
            }
        } else {
            if(_redisClient->openCluster()) {
                _mainWidget->reOpenClient();
            } else {
                QMessageBox::information(this,tr("错误"),_redisClient->getErrorInfo());
            }
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

void MainWindow::connectHost() {

    if(_mainWidget->getTaskSize() > 0) {
        QMessageBox::information(this,tr("错误"),tr("后台任务未完成,请稍后连接主机!"));
        return;
    }

    LoginDialog loginDialog;
    if(loginDialog.exec() != QDialog::Accepted) {
        return;
    }

    // 设置编码
    clearEncodeIcon();
    Global::gConnectName =loginDialog.getLableName();
    Global::gEncode = loginDialog.getEncode();
    Global::gEncode = Global::gEncode.isEmpty() ? "GB18030" : Global::gEncode;
    QTextCodec::setCodecForLocale(QTextCodec::codecForName(Global::gEncode.toLatin1()));
    setEncodeIcon();

    if(_redisClient) {
        _redisClient->close();
        delete _redisClient;
        _redisClient = nullptr;
    }
    _redisClient = loginDialog.getClient();
    _mainWidget = new MainWidget(_redisClient);
    setCentralWidget(_mainWidget);
    createSlot();
}

void MainWindow::redisInfo() {
    if(!_redisClient) {
        QMessageBox::information(this,tr("错误"),tr("客户端连接为空!"));
        return;
    }
    RedisInfoDialog redisInfoDialog(_redisClient);
    redisInfoDialog.exec();
}

void MainWindow::batchOprate() {
    if(!_redisClient) {
        QMessageBox::information(this,tr("错误"),tr("客户端连接为空!"));
        return;
    }
    BatchOperateDialog batchOperateDialog(_redisClient);
    batchOperateDialog.exec();
}

void MainWindow::feedback() {
    FeedBack feedback;
    feedback.exec();
}

void MainWindow::langCnAction() {
    PubLib::setConfig("language", "cn");
    QMessageBox::about(this, tr("提示"), tr("语言设置成功，重启生效!"));
}

void MainWindow::langEnAction() {
    PubLib::setConfig("language", "en");
    QMessageBox::about(this, tr("提示"), tr("语言设置成功，重启生效!"));
}

void MainWindow::readSettings() {
    QString sPath = QCoreApplication::applicationDirPath() + "/" + IniFileName;
    //settings.setPath(QSettings::IniFormat, QSettings::SystemScope, sPath);
    QSettings settings(sPath, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    if(geometry.isEmpty()) {
        this->showMaximized();
        settings.setValue("geometry", saveGeometry());
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

void MainWindow::setEncode(QString encode) {
    QList<ClientInfoDialog> vClientInfo;
    ClientInfoDialog clientInfo;
    QString sPath = QCoreApplication::applicationDirPath() + "/" + IniFileName;
    QSettings settings(sPath, QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    int size = settings.beginReadArray("logins");
    for(int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        clientInfo._name = settings.value("name").toString().trimmed();
        clientInfo._encodeAddr = settings.value("addr").toByteArray();
        clientInfo._encodePasswd = settings.value("passwd").toByteArray();
        clientInfo._encode = settings.value("encode","GB18030").toString().trimmed();
        clientInfo._keyPattern = settings.value("keypattern","").toString();
        clientInfo._valuePattern = settings.value("valuepattern","").toString();
        vClientInfo << clientInfo;
    }
    settings.endArray();
    settings.remove("logins");
    settings.beginWriteArray("logins");
    for(int j =0; j < vClientInfo.size(); ++j) {
        settings.setArrayIndex(j);
        settings.setValue("name", vClientInfo[j]._name);
        settings.setValue("addr", vClientInfo[j]._encodeAddr);
        settings.setValue("passwd", vClientInfo[j]._encodePasswd);
        if(_redisClient->getConnectName() == vClientInfo[j]._name)
            settings.setValue("encode", encode);
        else
            settings.setValue("encode", vClientInfo[j]._encode);
        settings.setValue("keypattern", vClientInfo[j]._keyPattern);
        settings.setValue("valuepattern", vClientInfo[j]._valuePattern);
    }
    settings.endArray();
    QTextCodec::setCodecForLocale(QTextCodec::codecForName(encode.toLatin1()));
    Global::gEncode = encode;
}

void MainWindow::setEncodeIcon() {
    if(Global::gEncode == "Big5") {
        _big5Act->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "Big5-HKSCS") {
        _big5HkscsAct->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "CP949") {
        _cp949Act->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "EUC-JP") {
        _eucjpAct->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "EUC-KR") {
        _euckrAct->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "GB18030") {
        _gb18030Act->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "HP-ROMAN8") {
        _hproman8Act->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "IBM 850") {
        _hproman8Act->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "IBM 866") {
        _ibm866Act->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "IBM 874") {
        _ibm874Act->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "ISO 2022-JP") {
        _iso2022jpAct->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "ISO 8859-1") {
        _iso88591Act->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "ISO 8859-2") {
        _iso88592Act->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "ISO 8859-3") {
        _iso88593Act->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "ISO 8859-4") {
        _iso88594Act->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "ISO 8859-5") {
        _iso88595Act->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "ISO 8859-6") {
        _iso88596Act->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "ISO 8859-7") {
        _iso88597Act->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "ISO 8859-8") {
        _iso88598Act->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "ISO 8859-9") {
        _iso88599Act->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "ISO 8859-10") {
        _iso885910Act->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "ISO 8859-13") {
        _iso885913Act->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "ISO 8859-14") {
        _iso885914Act->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "ISO 8859-15") {
        _iso885915Act->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "ISO 8859-16") {
        _iso885916Act->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "Iscii-Bng") {
        _isciibngAct->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "Iscii-Dev") {
        _isciidevAct->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "Iscii-Gjr") {
        _isciigjrAct->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "Iscii-Knd") {
        _isciikndAct->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "Iscii-Mlm") {
        _isciimlmAct->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "Iscii-Ori") {
        _isciioriAct->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "Iscii-Pnj") {
        _isciipnjAct->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "Iscii-Tlg") {
        _isciitlgAct->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "Iscii-Tml") {
        _isciitmlAct->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "KOI8-R") {
        _koi8rAct->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "KOI8-U") {
        _koi8uAct->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "Macintosh") {
        _macintoshAcy->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "Shift-JIS") {
        _shiftjisAct->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "TIS-620") {
        _tis620Act->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "TSCII") {
        _tsciiAct->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "UTF-8") {
        _utf8Act->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "UTF-16") {
        _utf16Act->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "UTF-16BE") {
        _utf16beAct->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "UTF-16LE") {
        _utf16leAct->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "UTF-32") {
        _utf32Act->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "UTF-32BE") {
        _utf32beAct->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "UTF-32LE") {
        _utf32leAct->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "Windows-1250") {
        _windows1250Act->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "Windows-1251") {
        _windows1251Act->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "Windows-1252") {
        _windows1252Act->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "Windows-1253") {
        _windows1253Act->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "Windows-1254") {
        _windows1254Act->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "Windows-1255") {
        _windows1255Act->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "Windows-1256") {
        _windows1256Act->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "Windows-1257") {
        _windows1257Act->setIcon(QIcon(ICON_SELECT));
    } else if(Global::gEncode == "Windows-1258") {
        _windows1258Act->setIcon(QIcon(ICON_SELECT));
    }
}

void MainWindow::clearEncodeIcon() {
    if(Global::gEncode == "Big5") {
        _big5Act->setIcon(QIcon());
    } else if(Global::gEncode == "Big5-HKSCS") {
        _big5HkscsAct->setIcon(QIcon());
    } else if(Global::gEncode == "CP949") {
        _cp949Act->setIcon(QIcon());
    } else if(Global::gEncode == "EUC-JP") {
        _eucjpAct->setIcon(QIcon());
    } else if(Global::gEncode == "EUC-KR") {
        _euckrAct->setIcon(QIcon());
    } else if(Global::gEncode == "GB18030") {
        _gb18030Act->setIcon(QIcon());
    } else if(Global::gEncode == "HP-ROMAN8") {
        _hproman8Act->setIcon(QIcon());
    } else if(Global::gEncode == "IBM 850") {
        _hproman8Act->setIcon(QIcon());
    } else if(Global::gEncode == "IBM 866") {
        _ibm866Act->setIcon(QIcon());
    } else if(Global::gEncode == "IBM 874") {
        _ibm874Act->setIcon(QIcon());
    } else if(Global::gEncode == "ISO 2022-JP") {
        _iso2022jpAct->setIcon(QIcon());
    } else if(Global::gEncode == "ISO 8859-1") {
        _iso88591Act->setIcon(QIcon());
    } else if(Global::gEncode == "ISO 8859-2") {
        _iso88592Act->setIcon(QIcon());
    } else if(Global::gEncode == "ISO 8859-3") {
        _iso88593Act->setIcon(QIcon());
    } else if(Global::gEncode == "ISO 8859-4") {
        _iso88594Act->setIcon(QIcon());
    } else if(Global::gEncode == "ISO 8859-5") {
        _iso88595Act->setIcon(QIcon());
    } else if(Global::gEncode == "ISO 8859-6") {
        _iso88596Act->setIcon(QIcon());
    } else if(Global::gEncode == "ISO 8859-7") {
        _iso88597Act->setIcon(QIcon());
    } else if(Global::gEncode == "ISO 8859-8") {
        _iso88598Act->setIcon(QIcon());
    } else if(Global::gEncode == "ISO 8859-9") {
        _iso88599Act->setIcon(QIcon());
    } else if(Global::gEncode == "ISO 8859-10") {
        _iso885910Act->setIcon(QIcon());
    } else if(Global::gEncode == "ISO 8859-13") {
        _iso885913Act->setIcon(QIcon());
    } else if(Global::gEncode == "ISO 8859-14") {
        _iso885914Act->setIcon(QIcon());
    } else if(Global::gEncode == "ISO 8859-15") {
        _iso885915Act->setIcon(QIcon());
    } else if(Global::gEncode == "ISO 8859-16") {
        _iso885916Act->setIcon(QIcon());
    } else if(Global::gEncode == "Iscii-Bng") {
        _isciibngAct->setIcon(QIcon());
    } else if(Global::gEncode == "Iscii-Dev") {
        _isciidevAct->setIcon(QIcon());
    } else if(Global::gEncode == "Iscii-Gjr") {
        _isciigjrAct->setIcon(QIcon());
    } else if(Global::gEncode == "Iscii-Knd") {
        _isciikndAct->setIcon(QIcon());
    } else if(Global::gEncode == "Iscii-Mlm") {
        _isciimlmAct->setIcon(QIcon());
    } else if(Global::gEncode == "Iscii-Ori") {
        _isciioriAct->setIcon(QIcon());
    } else if(Global::gEncode == "Iscii-Pnj") {
        _isciipnjAct->setIcon(QIcon());
    } else if(Global::gEncode == "Iscii-Tlg") {
        _isciitlgAct->setIcon(QIcon());
    } else if(Global::gEncode == "Iscii-Tml") {
        _isciitmlAct->setIcon(QIcon());
    } else if(Global::gEncode == "KOI8-R") {
        _koi8rAct->setIcon(QIcon());
    } else if(Global::gEncode == "KOI8-U") {
        _koi8uAct->setIcon(QIcon());
    } else if(Global::gEncode == "Macintosh") {
        _macintoshAcy->setIcon(QIcon());
    } else if(Global::gEncode == "Shift-JIS") {
        _shiftjisAct->setIcon(QIcon());
    } else if(Global::gEncode == "TIS-620") {
        _tis620Act->setIcon(QIcon());
    } else if(Global::gEncode == "TSCII") {
        _tsciiAct->setIcon(QIcon());
    } else if(Global::gEncode == "UTF-8") {
        _utf8Act->setIcon(QIcon());
    } else if(Global::gEncode == "UTF-16") {
        _utf16Act->setIcon(QIcon());
    } else if(Global::gEncode == "UTF-16BE") {
        _utf16beAct->setIcon(QIcon());
    } else if(Global::gEncode == "UTF-16LE") {
        _utf16leAct->setIcon(QIcon());
    } else if(Global::gEncode == "UTF-32") {
        _utf32Act->setIcon(QIcon());
    } else if(Global::gEncode == "UTF-32BE") {
        _utf32beAct->setIcon(QIcon());
    } else if(Global::gEncode == "UTF-32LE") {
        _utf32leAct->setIcon(QIcon());
    } else if(Global::gEncode == "Windows-1250") {
        _windows1250Act->setIcon(QIcon());
    } else if(Global::gEncode == "Windows-1251") {
        _windows1251Act->setIcon(QIcon());
    } else if(Global::gEncode == "Windows-1252") {
        _windows1252Act->setIcon(QIcon());
    } else if(Global::gEncode == "Windows-1253") {
        _windows1253Act->setIcon(QIcon());
    } else if(Global::gEncode == "Windows-1254") {
        _windows1254Act->setIcon(QIcon());
    } else if(Global::gEncode == "Windows-1255") {
        _windows1255Act->setIcon(QIcon());
    } else if(Global::gEncode == "Windows-1256") {
        _windows1256Act->setIcon(QIcon());
    } else if(Global::gEncode == "Windows-1257") {
        _windows1257Act->setIcon(QIcon());
    } else if(Global::gEncode == "Windows-1258") {
        _windows1258Act->setIcon(QIcon());
    }
}

void MainWindow::big5Action() {
    clearEncodeIcon();
    setEncode("Big5");
    setEncodeIcon();
}

void MainWindow::big5HkscsAction() {
    clearEncodeIcon();
    setEncode("Big5-HKSCS");
    setEncodeIcon();
}

void MainWindow::cp949Action() {
    clearEncodeIcon();
    setEncode("CP949");
    setEncodeIcon();
}

void MainWindow::eucjpAction() {
    clearEncodeIcon();
    setEncode("EUC-JP");
    setEncodeIcon();
}

void MainWindow::euckrAction() {
    clearEncodeIcon();
    setEncode("EUC-KR");
    setEncodeIcon();
}

void MainWindow::gb18030Action() {
    clearEncodeIcon();
    setEncode("GB18030");
    setEncodeIcon();
}

void MainWindow::hproman8Action() {
    clearEncodeIcon();
    setEncode("HP-ROMAN8");
    setEncodeIcon();
}

void MainWindow::ibm850Action() {
    clearEncodeIcon();
    setEncode("IBM 850");
    setEncodeIcon();
}

void MainWindow::ibm866Action() {
    clearEncodeIcon();
    setEncode("IBM 866");
    setEncodeIcon();
}

void MainWindow::ibm874Action() {
    clearEncodeIcon();
    setEncode("IBM 874");
    setEncodeIcon();
}

void MainWindow::iso2022jpAction() {
    clearEncodeIcon();
    setEncode("ISO 2022-JP");
    setEncodeIcon();
}

void MainWindow::iso88591Action() {
    clearEncodeIcon();
    setEncode("ISO 8859-1");
    setEncodeIcon();
}

void MainWindow::iso88592Action() {
    clearEncodeIcon();
    setEncode("ISO 8859-2");
    setEncodeIcon();
}

void MainWindow::iso88593Action() {
    clearEncodeIcon();
    setEncode("ISO 8859-3");
    setEncodeIcon();
}

void MainWindow::iso88594Action() {
    clearEncodeIcon();
    setEncode("ISO 8859-4");
    setEncodeIcon();
}

void MainWindow::iso88595Action() {
    clearEncodeIcon();
    setEncode("ISO 8859-5");
    setEncodeIcon();
}

void MainWindow::iso88596Action() {
    clearEncodeIcon();
    setEncode("ISO 8859-6");
    setEncodeIcon();
}

void MainWindow::iso88597Action() {
    clearEncodeIcon();
    setEncode("ISO 8859-7");
    setEncodeIcon();
}

void MainWindow::iso88598Action() {
    clearEncodeIcon();
    setEncode("ISO 8859-8");
    setEncodeIcon();
}

void MainWindow::iso88599Action() {
    clearEncodeIcon();
    setEncode("ISO 8859-9");
    setEncodeIcon();
}

void MainWindow::iso885910Action() {
    clearEncodeIcon();
    setEncode("ISO 8859-10");
    setEncodeIcon();
}

void MainWindow::iso885913Action() {
    clearEncodeIcon();
    setEncode("ISO 8859-13");
    setEncodeIcon();
}

void MainWindow::iso885914Action() {
    clearEncodeIcon();
    setEncode("ISO 8859-14");
    setEncodeIcon();
}

void MainWindow::iso885915Action() {
    clearEncodeIcon();
    setEncode("ISO 8859-15");
    setEncodeIcon();
}

void MainWindow::iso885916Action() {
    clearEncodeIcon();
    setEncode("ISO 8859-16");
    setEncodeIcon();
}

void MainWindow::isciibngAction() {
    clearEncodeIcon();
    setEncode("Iscii-Bng");
    setEncodeIcon();
}

void MainWindow::isciidevAction() {
    clearEncodeIcon();
    setEncode("Iscii-Dev");
    setEncodeIcon();
}

void MainWindow::isciigjrAction() {
    clearEncodeIcon();
    setEncode("Iscii-Gjr");
    setEncodeIcon();
}

void MainWindow::isciikndAction() {
    clearEncodeIcon();
    setEncode("Iscii-Knd");
    setEncodeIcon();
}

void MainWindow::isciimlmAction() {
    clearEncodeIcon();
    setEncode("Iscii-Mlm");
    setEncodeIcon();
}

void MainWindow::isciioriAction() {
    clearEncodeIcon();
    setEncode("Iscii-Ori");
    setEncodeIcon();
}

void MainWindow::isciipnjAction() {
    clearEncodeIcon();
    setEncode("Iscii-Pnj");
    setEncodeIcon();
}

void MainWindow::isciitlgAction() {
    clearEncodeIcon();
    setEncode("Iscii-Tlg");
    setEncodeIcon();
}

void MainWindow::isciitmlAction() {
    clearEncodeIcon();
    setEncode("Iscii-Tml");
    setEncodeIcon();
}

void MainWindow::koi8rAction() {
    clearEncodeIcon();
    setEncode("KOI8-R");
    setEncodeIcon();
}

void MainWindow::koi8uAction() {
    clearEncodeIcon();
    setEncode("KOI8-U");
    setEncodeIcon();
}

void MainWindow::macintoshAction() {
    clearEncodeIcon();
    setEncode("Macintosh");
    setEncodeIcon();
}

void MainWindow::shiftjisAction() {
    clearEncodeIcon();
    setEncode("Shift-JIS");
    setEncodeIcon();
}

void MainWindow::tis620Action() {
    clearEncodeIcon();
    setEncode("TIS-620");
    setEncodeIcon();
}

void MainWindow::tsciiAction() {
    clearEncodeIcon();
    setEncode("TSCII");
    setEncodeIcon();
}

void MainWindow::utf8Action() {
    clearEncodeIcon();
    setEncode("UTF-8");
    setEncodeIcon();
}

void MainWindow::utf16Action() {
    clearEncodeIcon();
    setEncode("UTF-16");
    setEncodeIcon();
}

void MainWindow::utf16beAction() {
    clearEncodeIcon();
    setEncode("UTF-16BE");
    setEncodeIcon();
}

void MainWindow::utf16leAction() {
    clearEncodeIcon();
    setEncode("UTF-16LE");
    setEncodeIcon();
}

void MainWindow::utf32Action() {
    clearEncodeIcon();
    setEncode("UTF-32");
    setEncodeIcon();
}

void MainWindow::utf32beAction() {
    clearEncodeIcon();
    setEncode("UTF-32BE");
    setEncodeIcon();
}

void MainWindow::utf32leAction() {
    clearEncodeIcon();
    setEncode("UTF-32LE");
    setEncodeIcon();
}

void MainWindow::windows1250Action() {
    clearEncodeIcon();
    setEncode("Windows-1250");
    setEncodeIcon();
}

void MainWindow::windows1251Action() {
    clearEncodeIcon();
    setEncode("Windows-1251");
    setEncodeIcon();
}

void MainWindow::windows1252Action() {
    clearEncodeIcon();
    setEncode("Windows-1252");
    setEncodeIcon();
}

void MainWindow::windows1253Action() {
    clearEncodeIcon();
    setEncode("Windows-1253");
    setEncodeIcon();
}

void MainWindow::windows1254Action() {
    clearEncodeIcon();
    setEncode("Windows-1254");
    setEncodeIcon();
}

void MainWindow::windows1255Action() {
    clearEncodeIcon();
    setEncode("Windows-1255");
    setEncodeIcon();
}

void MainWindow::windows1256Action() {
    clearEncodeIcon();
    setEncode("Windows-1256");
    setEncodeIcon();
}

void MainWindow::windows1257Action() {
    clearEncodeIcon();
    setEncode("Windows-1257");
    setEncodeIcon();
}

void MainWindow::windows1258Action() {
    clearEncodeIcon();
    setEncode("Windows-1258");
    setEncodeIcon();
}

void MainWindow::about() {
    QMessageBox::about(this, tr("关于"),
                       tr(
                           "<br>"
                           "<b>RedisView</b><br><br>"
                           "作者 ：王长春<br>"
                           "版本 ：Community v1.7.2<br>"
                           "邮箱 ：cc20110101@126.com<br>"
                           "地址 ：<a href='http://www.cc123.net.cn/'>RedisView</a> <a href='https://github.com/cc20110101/RedisView'>GitHub</a> <a href='https://sourceforge.net/projects/redisview/'>SourceForge</a><br>"
                           "版权 ：Copyright 2019 Powered By CC<br>"
                           )
                       );
}

void MainWindow::history() {
    QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
    VersionHistory versionHistory(availableGeometry.width(), availableGeometry.height(), this);
    versionHistory.exec();
}
