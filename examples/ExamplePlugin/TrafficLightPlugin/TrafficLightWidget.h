#pragma once

#include <QWidget>

class TrafficLightWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(State state READ state WRITE setState NOTIFY stateChanged)

public:
    enum class State {
        Red,
        Green
    };
    Q_ENUM(State)

    explicit TrafficLightWidget(QWidget* parent = nullptr);

    State state() const { return m_state; }
    void setState(State state);

signals:
    void stateChanged(State state);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void updateStyleSheet();

private:
    State m_state;
};
