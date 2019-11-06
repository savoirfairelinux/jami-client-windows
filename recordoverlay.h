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
    enum class RecorderStatus
    {
        aboutToRecord,
        recording,
        recorded
    };

public:
    // TODO: change container widget to the specific record widget
    explicit RecordOverlay(QWidget *parent = nullptr, QWidget *containerWidget = nullptr);
    ~RecordOverlay();

private slots:
    void on_recordOverlayStartOrFinishRecordingBtn_toggled(bool checked);

    void on_recordOverlaySendBtn_pressed();
    void on_recordOverlayDeleteBtn_pressed();
    void on_recordOverlayRerecordBtn_pressed();

private:
    Ui::RecordOverlay *ui;

};

#endif // RECORDOVERLAY_H
