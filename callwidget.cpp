#include "callwidget.h"
#include "ui_callwidget.h"

#include "categorizedhistorymodel.h"
#include "imconversationmanager.h"
#include "instantmessagingmodel.h"
#include "audio/settings.h"

CallWidget::CallWidget(QWidget *parent) :
    NavWidget(Main ,parent),
    ui(new Ui::CallWidget)
{
    ui->setupUi(this);

    ui->holdButton->setCheckable(true);
    ui->muteMicButton->setCheckable(true);
    ui->muteSpeakerButton->setCheckable(true);
    ui->callInvite->setVisible(false);

    actualCall_ = NULL;
    videoRenderer_ = NULL;

    try {
        callModel_ = CallModel::instance();

        connect(callModel_, SIGNAL(incomingCall(Call*)), this, SLOT(callIncoming(Call*)));
        connect(callModel_, SIGNAL(callAdded(Call*,Call*)), this, SLOT(addedCall(Call*, Call*)));
        connect(callModel_, SIGNAL(callStateChanged(Call*, Call::State)), this, SLOT(on_callStateChanged(Call*, Call::State)));

        ui->callList->setModel(callModel_);
        ui->historyList->setModel(CategorizedHistoryModel::instance());
        ui->speakerSlider->setValue(Audio::Settings::instance()->playbackVolume());
        ui->micSlider->setValue(Audio::Settings::instance()->captureVolume());
    } catch (...) {
        qDebug() << "INIT ERROR";
    }
}

CallWidget::~CallWidget()
{
    delete ui;
}

void CallWidget::callIncoming(Call *call)
{
    ui->callInvite->setVisible(true);
    actualCall_ = call;
}

void CallWidget::on_acceptButton_clicked()
{
    if (actualCall_ != NULL)
       actualCall_->performAction(Call::Action::ACCEPT);
    ui->callInvite->setVisible(false);
}

void CallWidget::on_refuseButton_clicked()
{
    if (actualCall_ == NULL)
        return;
    actualCall_->performAction(Call::Action::REFUSE);
    actualCall_ = NULL;
    ui->callInvite->setVisible(false);
}

void CallWidget::on_holdButton_toggled(bool checked)
{
    Q_UNUSED(checked)
    if (actualCall_ == NULL)
        return;
    actualCall_->performAction(Call::Action::HOLD);
}

void CallWidget::on_hangupButton_clicked()
{
    if (actualCall_ == NULL)
        return;
    actualCall_->performAction(Call::Action::REFUSE);
}

void CallWidget::videoStarted(Video::Renderer* renderer) {
    videoRenderer_ = renderer;

    connect(videoRenderer_, SIGNAL(stopped()), this, SLOT(stopRendering()));
    connect(videoRenderer_, SIGNAL(frameUpdated()), this, SLOT(frameUpdatedToRender()));
}

void CallWidget::frameUpdatedToRender() {
    if (videoRenderer_ == NULL)
        return;
    const QSize size(videoRenderer_->size());
    QImage image((uchar*)videoRenderer_->currentFrame().data() , size.width(), size.height(), QImage::Format_RGBA8888);
    const QSize labelSize = ui->videoLabel->size();
    image = image.scaled(labelSize.width(), labelSize.height());
    QPixmap pixmap = QPixmap::fromImage(image);
    ui->videoLabel->setPixmap(pixmap);
}

void CallWidget::startRendering() {
    connect(videoRenderer_, SIGNAL(frameUpdated()), this, SLOT(frameUpdatedToRender()));
}

void CallWidget::stopRendering() {

    disconnect(videoRenderer_, SIGNAL(stopped()), this, SLOT(stopRendering()));
    disconnect(videoRenderer_, SIGNAL(frameUpdated()), this, SLOT(frameUpdatedToRender()));
    videoRenderer_ = NULL;
    ui->videoLabel->clear();
}

void CallWidget::on_pushButton_clicked()
{

}

void CallWidget::addedCall(Call* call, Call* parent) {
    Q_UNUSED(parent);
    if (call->direction() == Call::Direction::OUTGOING) {
        actualCall_ = call;
        connect(actualCall_, SIGNAL(videoStarted(Video::Renderer*)), this, SLOT(videoStarted(Video::Renderer*)));
    }
}

void CallWidget::on_callStateChanged(Call* call, Call::State previousState) {
    ui->callList->setCurrentIndex(callModel_->getIndex(actualCall_));
    if (call->state() == Call::State::OVER) {
        disconnect(actualCall_, SIGNAL(videoStarted(Video::Renderer*)), this, SLOT(videoStarted(Video::Renderer*)));
        actualCall_ = NULL;
    } else {
        ui->messageOutput->setModel(IMConversationManager::instance()->getModel(actualCall_));
    }
    if (call->state() == Call::State::CURRENT && previousState != Call::State::HOLD) {
         connect(call, SIGNAL(videoStarted(Video::Renderer*)), this, SLOT(videoStarted(Video::Renderer*)));
    } else if (previousState == Call::State::HOLD) {
        videoStarted(call->videoRenderer());
    }
}

void CallWidget::on_callList_activated(const QModelIndex &index)
{
    Call* callSelected = callModel_->getCall(index);
    if (actualCall_ != NULL) {
        if (callSelected == actualCall_)
            return;
        actualCall_->performAction(Call::Action::HOLD);
    }
    actualCall_ = callSelected;
    actualCall_->performAction(Call::Action::HOLD);
}

void CallWidget::on_muteSpeakerButton_toggled(bool checked)
{
    Audio::Settings::instance()->mutePlayback(checked);
}

void CallWidget::on_muteMicButton_toggled(bool checked)
{
    Audio::Settings::instance()->muteCapture(checked);
}

void CallWidget::on_speakerSlider_sliderMoved(int position)
{
    outputVolume_ = position;
}

void CallWidget::on_speakerSlider_sliderReleased()
{
    emit Audio::Settings::instance()->setPlaybackVolume(outputVolume_);
}

void CallWidget::on_micSlider_sliderMoved(int position)
{
    inputVolume_ = position;
}

void CallWidget::on_micSlider_sliderReleased()
{
    emit Audio::Settings::instance()->setCaptureVolume(inputVolume_);
}

//void CallWidget::on_pushButton_toggled(bool checked)
//{
//    Video::PreviewManager *previewManager = Video::PreviewManager::instance();
//    if (checked) {
//        previewManager->startPreview();
//        videoRenderer_ = previewManager->previewRenderer();
//        connect(previewManager->previewRenderer(), SIGNAL(frameUpdated()), this, SLOT(frameUpdatedToRender()));
//    } else {
//        previewManager->stopPreview();
//    }
//}
