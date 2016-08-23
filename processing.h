#ifndef PROCESSING_H
#define PROCESSING_H

#include <QDialog>

namespace Ui {
class processing;
}

class processing : public QDialog
{
    Q_OBJECT
private slots:
    void r_number(int num);
signals:

public:
    explicit processing(QWidget *parent = 0);
    ~processing();

private:
    Ui::processing *ui;
};

#endif // PROCESSING_H
