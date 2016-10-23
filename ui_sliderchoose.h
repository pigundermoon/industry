#ifndef SLIDERCHOOSE_H
#define SLIDERCHOOSE_H

#include <QDialog>

namespace Ui {
class ui_sliderchoose;
}

class ui_sliderchoose : public QDialog
{
    Q_OBJECT

public:
    explicit ui_sliderchoose(QWidget *parent = 0);
    ~ui_sliderchoose();

signals:
    s_value(float degree);

private:
    Ui::ui_sliderchoose *ui;
};

#endif // SLIDERCHOOSE_H
