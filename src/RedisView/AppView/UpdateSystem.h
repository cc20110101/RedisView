/**
* @file      UpdateSystem.h
* @brief     更新系统
* @author    王长春
* @date      2019-05-31
* @version   001
* @copyright Copyright (c) 2018
*/
#ifndef UPDATESYSTEM_H
#define UPDATESYSTEM_H

#include "Public/Define.h"

class Updater {
public:
    Updater();

public:
    void paint(QPainter *painter, QPaintEvent *event, int elapsed);

private:
    QBrush _background;
    QBrush _circleBrush;
    QFont _textFont;
    QPen _circlePen;
    QPen _textPen;
};

class UpdateWidget : public QWidget {
    Q_OBJECT

public: UpdateWidget(Updater *updater, QWidget *parent);

public slots:
    void animate();

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:
    Updater *_updater;
    int _elapsed;
};

class UpdateView : public QWidget {
    Q_OBJECT

public:
    UpdateView();

private slots:
    // 检查更新接口，菜单项函数
    void noupdate();

private:
    Updater _updater;
    QLabel *_nativeLabel;
    QTimer *_timer;
};

#endif // UPDATESYSTEM_H
