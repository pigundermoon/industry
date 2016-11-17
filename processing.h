#ifndef PROCESSING_H
#define PROCESSING_H

#include <QDialog>
#include "QDesktopWidget"
#include "QMovie"

namespace Ui {
class processing;
}

class processing : public QDialog
{
    Q_OBJECT
private slots:
    void r_denoise_close();
signals:
    void s_denoise_close_ok();
public:
    explicit processing(QWidget *parent = 0);
    ~processing();

private:
    Ui::processing *ui;
};

#endif // PROCESSING_H
