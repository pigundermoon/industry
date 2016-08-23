#ifndef UI_ABOUT_H
#define UI_ABOUT_H

#include <QDialog>

namespace Ui {
class ui_about;
}

class ui_about : public QDialog
{
    Q_OBJECT

public:
    explicit ui_about(QWidget *parent = 0);
    ~ui_about();

private:
    Ui::ui_about *ui;
};

#endif // UI_ABOUT_H
