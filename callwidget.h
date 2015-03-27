#ifndef CALLWIDGET_H
#define CALLWIDGET_H

#include <QWidget>

#include "navwidget.h"

#include "callmodel.h"
#include "video/renderer.h"
#include "video/previewmanager.h"

namespace Ui {
class CallWidget;
}

class CallWidget : public NavWidget
{
    Q_OBJECT

public:
    explicit CallWidget(QWidget *parent = 0);
    ~CallWidget();

private slots:
    void callIncoming(Call *call);
    void on_acceptButton_clicked();
    void on_refuseButton_clicked();
    void on_holdButton_toggled(bool checked);
    void on_hangupButton_clicked();
    void videoStarted(Video::Renderer *renderer);
    void frameUpdatedToRender();

    void startRendering();
    void stopRendering();
    void on_pushButton_clicked();
    void addedCall(Call *call, Call *parent);
    void on_callStateChanged(Call *call, Call::State previousState);
    void on_callList_activated(const QModelIndex &index);
    void on_muteSpeakerButton_toggled(bool checked);
    void on_muteMicButton_toggled(bool checked);

    void on_speakerSlider_sliderMoved(int position);

    void on_speakerSlider_sliderReleased();

    void on_micSlider_sliderMoved(int position);

    void on_micSlider_sliderReleased();

private:
    Ui::CallWidget *ui;
    Call* actualCall_;
    Video::Renderer* videoRenderer_;
    CallModel* callModel_;
    int outputVolume_;
    int inputVolume_;
};

#endif // CALLWIDGET_H
