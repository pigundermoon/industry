#ifndef BUTTONACTIONADAPTER_H
#define BUTTONACTIONADAPTER_H
#include "QAction"
#include "QToolButton"

class ButtonActionAdapter : public QObject
{
    Q_OBJECT
public:
    explicit ButtonActionAdapter(QObject *parent = 0, QAction *a = nullptr, QToolButton *b = nullptr);
    void local_connect();
signals:
    void send_enable_to_button(bool enable);
public slots:
    void receive_change_from_action();
private:
    QAction *action;
    QToolButton *button;
};

#endif // BUTTONACTIONADAPTER_H
