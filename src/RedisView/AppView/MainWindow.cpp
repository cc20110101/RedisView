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
    langMenu->addSeparator();

    QAction *lanEnAct = langMenu->addAction(tr("英文"), this, &MainWindow::langEnAction);
    lanEnAct->setStatusTip(tr("设置英文语言..."));
    langMenu->addSeparator();
    setMenu->addSeparator();

    QMenu* encodeMenu = setMenu->addMenu(tr("设置编码"));

    _big5Act = encodeMenu->addAction(tr("Big5"), this, &MainWindow::big5Action);
    _big5Act->setStatusTip(tr("设置Big5编码..."));

    _big5HkscsAct = encodeMenu->addAction(tr("Big5-HKSCS"), this, &MainWindow::big5HkscsAction);
    _big5HkscsAct->setStatusTip(tr("设置Big5-HKSCS编码..."));

    _cp949Act = encodeMenu->addAction(tr("CP949"), this, &MainWindow::cp949Action);
    _cp949Act->setStatusTip(tr("设置CP949编码..."));

    _eucjpAct = encodeMenu->addAction(tr("EUC-JP"), this, &MainWindow::eucjpAction);
    _eucjpAct->setStatusTip(tr("设置UTF-8编码..."));

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
    encodeMenu->setIcon(QIcon(ICON_ENCODE));
    lanCnAct->setIcon(QIcon(ICON_CN));
    lanEnAct->setIcon(QIcon(ICON_EN));
    instructionAct->setIcon(QIcon(ICON_INSTRUCTION));
    setEncodeIcon();
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
        clientInfo._addr = settings.value("addr").toString().trimmed();
        clientInfo._passwd = settings.value("passwd").toString().trimmed();
        clientInfo._encode = settings.value("encode","GB18030").toString().trimmed();
        vClientInfo << clientInfo;
    }
    settings.endArray();
    settings.remove("logins");
    settings.beginWriteArray("logins");
    for(int j =0; j < vClientInfo.size(); ++j) {
        settings.setArrayIndex(j);
        settings.setValue("name", vClientInfo[j]._name);
        settings.setValue("addr", vClientInfo[j]._addr);
        settings.setValue("passwd", vClientInfo[j]._passwd);
        if(_redisClient->getConnectName() == vClientInfo[j]._name)
            settings.setValue("encode", encode);
        else
            settings.setValue("encode", vClientInfo[j]._encode);
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
