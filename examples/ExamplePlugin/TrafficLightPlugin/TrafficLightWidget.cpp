#include "TrafficLightWidget.h"
#include <QPainter>
#include <QPaintEvent>

TrafficLightWidget::TrafficLightWidget(QWidget* parent)
    : QWidget(parent)
    , m_state(State::Green)
{
    setFixedSize(60, 60);
    updateStyleSheet();
}

void TrafficLightWidget::setState(State state)
{
    if (m_state != state) {
        m_state = state;
        updateStyleSheet();
        update();
        emit stateChanged(m_state);
    }
}

void TrafficLightWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制外圈
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor("#333333"));
    painter.drawEllipse(rect());

    // 绘制灯
    QColor color = m_state == State::Red ? QColor("#FF0000") : QColor("#00FF00");
    QRect lightRect = rect().adjusted(5, 5, -5, -5);
    painter.setBrush(color);
    painter.drawEllipse(lightRect);
}

void TrafficLightWidget::updateStyleSheet()
{
    setToolTip(m_state == State::Red ? "Failed" : "Success");
}
