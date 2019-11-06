#ifndef RECORDOVERLAY_H
#define RECORDOVERLAY_H

#include <QWidget>

namespace Ui {
class RecordOverlay;
}

class RecordOverlay : public QWidget
{
    Q_OBJECT

public:
    explicit RecordOverlay(QWidget *parent = nullptr);
    ~RecordOverlay();

private:
    Ui::RecordOverlay *ui;
};

#endif // RECORDOVERLAY_H
