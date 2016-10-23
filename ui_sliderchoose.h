#ifndef UI_SLIDERCHOOSE_H
#define UI_SLIDERCHOOSE_H

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
    void s_value(float degree);
    void s_ok_value(float degree);

private slots:

    void on_ok_button_clicked();

    void on_cancel_button_clicked();

    void on_degree_textEdited(const QString &arg1);

private:
    Ui::ui_sliderchoose *ui;
};

#endif // UI_SLIDERCHOOSE_H
