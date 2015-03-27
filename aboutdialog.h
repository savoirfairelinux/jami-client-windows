#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

namespace Ui {
class AboutDialog;
}

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = 0);
    ~AboutDialog();

private slots:
    void on_aboutButton_toggled(bool checked);

    void on_creditsButton_clicked(bool checked);

    void on_toolButton_clicked();

private:
    Ui::AboutDialog *ui;
};

#endif // ABOUTDIALOG_H
