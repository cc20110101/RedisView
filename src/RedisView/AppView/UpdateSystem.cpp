/**
* @file      UpdateSystem.cpp
* @brief     更新系统
* @author    王长春
* @date      2019-05-31
* @version   001
* @copyright Copyright (c) 2018
*/
#include "UpdateSystem.h"

UpdateView::UpdateView() {
    setWindowTitle(tr("检查更新"));
    setWindowFlags(windowFlags() &
                   ~Qt::WindowMaximizeButtonHint &
                   ~Qt::WindowMinimizeButtonHint);
    setWindowModality(Qt::ApplicationModal);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setFixedSize(333,380);

    UpdateWidget *_native = new UpdateWidget(&_updater, this);
    _native->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _nativeLabel = new QLabel(tr("努力检查中,请稍后..."));
    _nativeLabel->setWordWrap(true);
    _nativeLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _nativeLabel->setAlignment(Qt::AlignHCenter);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(_native, 0, 0);
    layout->addWidget(_nativeLabel, 1, 0);
    setLayout(layout);

    _timer = new QTimer(this);
    connect(_timer, &QTimer::timeout, _native, &UpdateWidget::animate);
    _timer->start(50);

    QTimer *timer2 = new QTimer(this);
    connect(timer2, &QTimer::timeout, this, &UpdateView::noupdate);
    timer2->start(5000);
}

// 没有服务端，此处不实现
void UpdateView::noupdate() {
    _nativeLabel->setText(tr("功能不实现，请到下载网站更新..."));
    _timer->stop();
}

Updater::Updater() {
    QLinearGradient gradient(QPointF(50, -20), QPointF(80, 20));
    gradient.setColorAt(0.0, Qt::white);
    gradient.setColorAt(1.0, QColor(0xa6, 0xce, 0x39));
    _background = QBrush(QColor(64, 32, 64));
    _circleBrush = QBrush(gradient);
    _circlePen = QPen(Qt::black);
    _circlePen.setWidth(1);
    _textPen = QPen(Qt::white);
    _textFont.setPixelSize(50);

}

void Updater::paint(QPainter *painter, QPaintEvent *event, int elapsed) {
    painter->fillRect(event->rect(), _background);
    painter->translate(150, 150);

    painter->save();
    painter->setBrush(_circleBrush);
    painter->setPen(_circlePen);
    painter->rotate(elapsed * 0.030);

    qreal r = elapsed / 1000.0;
    int n = 35;
    for(int i = 0; i < n; ++i) {
        painter->rotate(30);
        qreal factor = (i + r) / n;
        qreal radius = 0 + 120.0 * factor;
        qreal circleRadius = 1 + factor * 20;
        painter->drawEllipse(QRectF(radius, -circleRadius,
                                    circleRadius * 2.5, circleRadius * 2.5));
    }
    painter->restore();
    painter->setPen(_textPen);
    painter->setFont(_textFont);
    painter->drawText(QRect(-75, -75, 150, 150), Qt::AlignCenter,
                      QStringLiteral("RV"));
}

UpdateWidget::UpdateWidget(Updater *updater, QWidget *parent)
    : QWidget(parent), _updater(updater) {
    _elapsed = 0;
    setFixedSize(300, 300);
}

void UpdateWidget::animate() {
    _elapsed = (_elapsed + qobject_cast<QTimer *>(sender())->interval()) % 1000;
    update();
}

void UpdateWidget::paintEvent(QPaintEvent *event) {
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    _updater->paint(&painter, event, _elapsed);
    painter.end();
}
