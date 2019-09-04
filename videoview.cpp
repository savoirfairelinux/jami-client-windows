/***************************************************************************
 * Copyright (C) 2015-2019 by Savoir-faire Linux                           *
 * Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
 * Author: Andreas Traczyk <andreas.traczyk@savoirfairelinux.com>          *
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 3 of the License, or       *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 **************************************************************************/

#include "videoview.h"
#include "ui_videoview.h"

#include "lrcinstance.h"
#include "utils.h"

#include <QDesktopWidget>
#include <QFileDialog>
#include <QGraphicsOpacityEffect>
#include <QMenu>
#include <QMimeData>
#include <QPropertyAnimation>
#include <QScreen>
#include <QSplitter>
#include <QWindow>

#include <memory>

#include "selectareadialog.h"
#include "videooverlay.h"

VideoView::VideoView(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::VideoView)
{
    ui->setupUi(this);

    this->setMouseTracking(true);

    // video overlay
    overlay_ = new VideoOverlay(this);

    // context menu
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(showContextMenu(const QPoint&)));

    // chat panel
    connect(overlay_, &VideoOverlay::setChatVisibility,
        [=](bool visible) {
            emit this->setChatVisibility(visible);
            // what is this?
            connect(this, SIGNAL(toggleFullScreenClicked()),
                    ui->videoWidget, SLOT(slotToggleFullScreenClicked()));
        });

    // audio only overlay
    audioOnlyAvatar_ = new CallAudioOnlyAvatarOverlay(this);

    // preview widget
    previewRenderer_ = PreviewRenderWidget::attachPreview();

    // preview widget animation
    moveAnim_ = new QPropertyAnimation(previewRenderer_, "geometry");
    moveAnim_->setDuration(100);
    moveAnim_->setEasingCurve(QEasingCurve::InQuad);
}

VideoView::~VideoView()
{
    closing(pConvInfo_->callId);
    delete ui;
    delete overlay_;

}

void
VideoView::resizeEvent(QResizeEvent* event)
{
    moveAnim_->stop();
    previewRenderer_->setCurrentConainerGeo(event->size().width(), event->size().height());
    previewRenderer_->resetPreview();

    // force preview to repaint since the geo may be changed to hide
    // the preview renderer
    previewRenderer_->repaint();

    audioOnlyAvatar_->resize(this->size());

    overlay_->resize(this->size());
    overlay_->show();
    overlay_->raise();
}

void
VideoView::slotCallStatusChanged(const std::string& callId)
{
    if (callId != pConvInfo_->callId) {
        return;
    }

    using namespace lrc::api::call;
    auto call = LRCInstance::getCurrentCallModel()->getCall(pConvInfo_->callId);
    switch (call.status) {
    case Status::IN_PROGRESS:
    {
        // close chat ?
        emit overlay_->setChatVisibility(false);
        // TODO videoWidget --> distantRnderer
        ui->videoWidget->show();
        auto convInfo = Utils::getConversationFromCallId(pConvInfo_->callId);
        if (!convInfo.uid.empty()) {
            auto contactInfo = LRCInstance::getCurrentAccountInfo().contactModel->getContact(convInfo.participants[0]);
            auto contactName = Utils::bestNameForContact(contactInfo);
            overlay_->setName(QString::fromStdString(contactName));
        }
        return;
    }
    case Status::ENDED:
        // this will trigger a removal of the this entry in callwidget's
        // videoview map
        emit closing(pConvInfo_->callId);
    default:
        break;
    }
    QObject::disconnect(timerConnection_);
}

void
VideoView::simulateShowChatview(bool checked)
{
    Q_UNUSED(checked);
    overlay_->simulateShowChatview(true);
}

void
VideoView::mouseDoubleClickEvent(QMouseEvent* e)
{
    QWidget::mouseDoubleClickEvent(e);
    toggleFullScreen();
}

void
VideoView::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls())
        event->accept();
}

void
VideoView::dragLeaveEvent(QDragLeaveEvent* event)
{
    event->accept();
}
void
VideoView::dragMoveEvent(QDragMoveEvent* event)
{
    if (event->mimeData()->hasUrls())
        event->accept();
}

void
VideoView::dropEvent(QDropEvent* event)
{
    // take only the first file
    QString urlString = event->mimeData()->urls().at(0).toString();
    auto selectedConvUid = LRCInstance::getSelectedConvUid();
    auto convModel = LRCInstance::getCurrentConversationModel();
    auto conversation = Utils::getConversationFromUid(selectedConvUid, *convModel);
    auto callIdList = LRCInstance::getActiveCalls();
    for (auto callId : callIdList) {
        if (callId == conversation->callId) {
            LRCInstance::avModel().setInputFile(urlString.toStdString());
            break;
        }
    }
}

void
VideoView::toggleFullScreen()
{
    emit toggleFullScreenClicked();
}

void
VideoView::showContextMenu(const QPoint& pos)
{
    QPoint globalPos = this->mapToGlobal(pos);

    QMenu menu;

    auto selectedConvUid = LRCInstance::getSelectedConvUid();
    auto convModel = LRCInstance::getCurrentConversationModel();
    auto conversation = Utils::getConversationFromUid(selectedConvUid, *convModel);
    auto callIdList = LRCInstance::getActiveCalls();
    std::string thisCallId;
    for (auto callId : callIdList) {
        if (callId == conversation->callId) {
            thisCallId = callId;
            break;
        }
    }
    if (thisCallId.empty()) {
        return;
    }

    auto activeDevice = LRCInstance::avModel().getCurrentRenderedDevice(thisCallId);

    // video input devices
    auto devices = LRCInstance::avModel().getDevices();
    auto device = LRCInstance::avModel().getDefaultDeviceName();
    for (auto d : devices) {
        auto deviceName = QString::fromStdString(d).toUtf8();
        auto deviceAction = new QAction(deviceName, this);
        menu.addAction(deviceAction);
        deviceAction->setCheckable(true);
        if (d == activeDevice.name) {
            deviceAction->setChecked(true);
        }
        connect(deviceAction, &QAction::triggered,
            [this, deviceName, thisCallId]() {
                previewRenderer_->hide();
                previewRenderer_->setCurrentConainerGeo(this->width(), this->height());
                // since there is the possiblity of image not reloaded properly
                // after rendering reconnect
                previewRenderer_->connectRendering();
                ui->videoWidget->connectDistantRendering();

                auto decive = deviceName.toStdString();
                LRCInstance::avModel().switchInputTo(decive);
                LRCInstance::avModel().setCurrentVideoCaptureDevice(decive);
            });
    }

    menu.addSeparator();

    // entire screen share
    auto shareAction = new QAction(tr("Share entire screen"), this);
    menu.addAction(shareAction);
    shareAction->setCheckable(true);
    connect(shareAction, &QAction::triggered,
        [this]() {
            auto screenNumber = qApp->desktop()->screenNumber(this);
            QScreen* screen = qApp->screens().at(screenNumber);
            QRect rect = screen ? screen->geometry() : qApp->primaryScreen()->geometry();
#if defined(Q_OS_WIN) && (PROCESS_DPI_AWARE)
            rect.setSize(Utils::getRealSize(screen));
#endif
            LRCInstance::avModel().setDisplay(screenNumber,
                rect.x(), rect.y(), rect.width(), rect.height()
            );
            sharingEntireScreen_ = true;
        });

    // area of screen share
    auto shareAreaAction = new QAction(tr("Share screen area"), this);
    menu.addAction(shareAreaAction);
    shareAreaAction->setCheckable(true);
    connect(shareAreaAction, &QAction::triggered,
        [this]() {
            SelectAreaDialog selectAreaDialog;
            selectAreaDialog.exec();
            sharingEntireScreen_ = false;
        });

    // share a media file
    auto shareFileAction = new QAction(tr("Share file"), this);
    menu.addAction(shareFileAction);
    shareFileAction->setCheckable(true);
    connect(shareFileAction, &QAction::triggered,
        [this]() {
            QFileDialog fileDialog(this);
            fileDialog.setFileMode(QFileDialog::AnyFile);
            QStringList fileNames;
            if (!fileDialog.exec())
                return;
            fileNames = fileDialog.selectedFiles();
            auto resource = QUrl::fromLocalFile(fileNames.at(0)).toString();
            LRCInstance::avModel().setInputFile(resource.toStdString());
        });

    // possibly select the alternative video sharing device
    switch (activeDevice.type) {
    case lrc::api::video::DeviceType::DISPLAY:
        sharingEntireScreen_ ? shareAction->setChecked(true) : shareAreaAction->setChecked(true);
        break;
    case lrc::api::video::DeviceType::FILE:
        shareFileAction->setChecked(true);
        break;
    default:
        // a camera must have already been selected
        break;
    }

    menu.exec(globalPos);
}

void
VideoView::setupForConversation(const conversation::Info& convInfo)
{
    pConvInfo_ = &convInfo;

    auto callModel = LRCInstance::getCurrentCallModel();

    QObject::disconnect(ui->videoWidget);
    QObject::disconnect(callStatusChangedConnection_);

    if (!callModel->hasCall(pConvInfo_->callId)) {
        return;
    }

    auto call = callModel->getCall(pConvInfo_->callId);

    // transfer call will only happen in SIP calls
    auto& accInfo = LRCInstance::accountModel().getAccountInfo(pConvInfo_->accountId);
    bool isSIP = accInfo.profileInfo.type == lrc::api::profile::Type::SIP;
    this->overlay_->setTransferCallAndSIPPanelAvailability(isSIP);

    this->overlay_->callStarted(pConvInfo_->callId);
    this->overlay_->setVideoMuteVisibility(!call.isAudioOnly);

    callStatusChangedConnection_ =
        QObject::connect(callModel,
                         &NewCallModel::callStatusChanged,
                         this,
                         &VideoView::slotCallStatusChanged);

    previewRenderer_->connectRendering();
    ui->videoWidget->connectDistantRendering();

    resetAvatarOverlay(call.isAudioOnly);
    if (call.isAudioOnly) {
        audioOnlyAvatar_->writeAvatarOverlay(*pConvInfo_);
    }

    // close chat ?
    emit overlay_->setChatVisibility(false);

    // setup overlay
    bool isAudioMuted = call.audioMuted && (call.status != lrc::api::call::Status::PAUSED);
    bool isVideoMuted = call.videoMuted && (call.status != lrc::api::call::Status::PAUSED) && (!call.isAudioOnly);
    bool isRecording = callModel->isRecording(pConvInfo_->callId);
    bool isPaused = call.status == lrc::api::call::Status::PAUSED;
    bool isAudioOny = call.isAudioOnly && call.status != lrc::api::call::Status::PAUSED;
    overlay_->resetOverlay(isAudioMuted, isVideoMuted, isRecording, isPaused, isAudioOny);

    // overlay name
    auto& convModel = accInfo.conversationModel;
    auto bestName = QString::fromStdString(Utils::bestNameForConversation(*pConvInfo_, *convModel));
    overlay_->setCurrentSelectedCalleeDisplayName(bestName);
}

void
VideoView::mousePressEvent(QMouseEvent* event)
{
    QPoint clickPosition = event->pos();
    if (previewRenderer_->geometry().contains(clickPosition)) {
        QLine distance = QLine(clickPosition, previewRenderer_->geometry().bottomRight());
        originMouseDisplacement_ = event->pos() - previewRenderer_->geometry().topLeft();
        QApplication::setOverrideCursor(Qt::SizeAllCursor);
        draggingPreview_ = true;
        moveAnim_->stop();
    }
}

void
VideoView::mouseReleaseEvent(QMouseEvent* event)
{
    Q_UNUSED(event)
    if (draggingPreview_) {
        //Check preview's current central position
        auto previewCentral = previewRenderer_->geometry().center();
        auto videoViewRect = ui->videoWidget->rect();
        auto videoWidgetCentral = videoViewRect.center();
        auto previewInitialWidth = previewRenderer_->width();
        auto previewInitialHeight = previewRenderer_->height();
        moveAnim_->setStartValue(previewRenderer_->geometry());
        if (previewCentral.x() >= videoWidgetCentral.x()) {
            if (previewCentral.y() >= videoWidgetCentral.y()) {
                //Move preview to bottom right

                moveAnim_->setEndValue(QRect(this->width() - previewMargin_ - previewInitialWidth, this->height() - previewMargin_ - previewInitialHeight,
                                             previewInitialWidth, previewInitialHeight));
            } else {
                //Move preview to top right
                moveAnim_->setEndValue(QRect(this->width() - previewMargin_ - previewInitialWidth, previewMargin_,
                                             previewInitialWidth, previewInitialHeight));
            }
        } else {
            if (previewCentral.y() >= videoWidgetCentral.y()) {
                //Move preview to bottom left
                moveAnim_->setEndValue(QRect(previewMargin_, this->height() - previewMargin_ - previewInitialHeight,
                                             previewInitialWidth, previewInitialHeight));
            } else {
                //Move preview to top left
                moveAnim_->setEndValue(QRect(previewMargin_, previewMargin_,
                                             previewInitialWidth, previewInitialHeight));
            }
        }
        moveAnim_->start();
    }

    draggingPreview_ = false;
    QApplication::setOverrideCursor(Qt::ArrowCursor);
}

void
VideoView::mouseMoveEvent(QMouseEvent* event)
{
    QRect previewRect = previewRenderer_->geometry();
    if (draggingPreview_) {
        if (previewRect.left() > 0
            && previewRect.top() > 0
            && previewRect.right() < width()
            && previewRect.bottom() < height()) {

            previewRect.moveTo(event->pos() - originMouseDisplacement_);
            if (previewRect.left() <= 0)
                previewRect.moveLeft(1);

            if (previewRect.right() >= width())
                previewRect.moveRight(width() - 1);

            if (previewRect.top() <= 0)
                previewRect.moveTop(1);

            if (previewRect.bottom() >= height())
                previewRect.moveBottom(height() - 1);
        }
    }

    previewRenderer_->setGeometry(previewRect);
    previewRenderer_->repaint();
}

void
VideoView::resetAvatarOverlay(bool isAudioOnly)
{
    audioOnlyAvatar_->setAvatarVisible(isAudioOnly);
    if (isAudioOnly) {
        disconnect(coordinateOverlays_);
        coordinateOverlays_ = connect(overlay_, SIGNAL(HoldStatusChanged(bool)), this, SLOT(slotHoldStatusChanged(bool)));
    } else {
        disconnect(coordinateOverlays_);
    }
}

void
VideoView::slotHoldStatusChanged(bool pauseLabelStatus)
{
    audioOnlyAvatar_->respondToPauseLabel(pauseLabelStatus);
}

void
VideoView::keyPressEvent(QKeyEvent* event)
{
    // used to manage DTMF
    // For "#" and "*", qt will automatically read the shift + 3 or 8
    keyPressed_ = event->key();
    QWidget::keyPressEvent(event);
}

void
VideoView::keyReleaseEvent(QKeyEvent* event)
{
    if (keyPressed_ == static_cast<int>(Qt::Key_NumberSign)) {
        LRCInstance::getCurrentCallModel()->playDTMF(pConvInfo_->callId, "#");
    } else if (keyPressed_ == static_cast<int>(Qt::Key_Asterisk)) {
        LRCInstance::getCurrentCallModel()->playDTMF(pConvInfo_->callId, "*");
    } else if (keyPressed_ >= 48 && keyPressed_ <= 57) {
        //enum Qt::Key_0 = 48, QT::Key_9 = 57
        LRCInstance::getCurrentCallModel()->playDTMF(pConvInfo_->callId, std::to_string(keyPressed_ - 48));
    }
    QWidget::keyReleaseEvent(event);
}