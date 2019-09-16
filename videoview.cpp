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

    // Setup the timer to start the fade when the mouse stops moving
    this->setMouseTracking(true);

    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
        this, SLOT(showContextMenu(const QPoint&)));

    audioOnlyAvatar_ = new CallAudioOnlyAvatarOverlay(this);
    previewRenderer_ = PreviewRenderWidget::attachPreview();

    moveAnim_ = new QPropertyAnimation(previewRenderer_, "geometry");
    moveAnim_->setDuration(1000);

    QPalette pal(palette());
    pal.setColor(QPalette::Background, Qt::black);
    this->setAutoFillBackground(true);
    this->setPalette(pal);
}

VideoView::~VideoView()
{
    delete ui;
}

void
VideoView::resizeEvent(QResizeEvent* event)
{
    moveAnim_->stop();
    previewRenderer_->setCurrentConainerGeo(event->size().width(), event->size().height());
    previewRenderer_->resetPreview();
    // force preview to repaint since the geo may be changed to hide
    // the preview renderer
    previewRenderer_->forceRepaint();

    audioOnlyAvatar_->resize(this->size());

    currentOverLayRendererPair_.first->resize(this->size());
    currentOverLayRendererPair_.first->show();
    currentOverLayRendererPair_.first->raise();
}

void
VideoView::enterEvent(QEvent* event)
{
    Q_UNUSED(event)
    showOverlay();
}

void
VideoView::leaveEvent(QEvent* event)
{
    Q_UNUSED(event)
    fadeOverlayOut();
}

void
VideoView::showOverlay()
{
}

void
VideoView::fadeOverlayOut()
{
}

void
VideoView::slotCallStatusChanged(const std::string& callId)
{
    using namespace lrc::api::call;
    auto call = LRCInstance::getCurrentCallModel()->getCall(callId);
    switch (call.status) {
    case Status::IN_PROGRESS:
    {
        auto convInfo = Utils::getConversationFromCallId(call.id);
        if (!convInfo.uid.empty()) {
            auto contactInfo = LRCInstance::getCurrentAccountInfo().contactModel->getContact(convInfo.participants[0]);
            auto contactName = Utils::bestNameForContact(contactInfo);
            currentOverLayRendererPair_.first->setName(QString::fromStdString(contactName));
        }
        return;
    }
    case Status::ENDED:
        previewRenderer_->hide();
        // reset parent incase deletion of distant renderer causes error
        previewRenderer_->setParent(nullptr);
        ui->gridLayout->removeWidget(distantRendererMap_[callId].second);
        distantRendererMap_[callId].first->hide();
        distantRendererMap_[callId].second->hide();
        delete distantRendererMap_[callId].second;
        delete distantRendererMap_[callId].first;
        distantRendererMap_.erase(callId);
        emit closing(call.id);
    default:
        //emit closing(call.id);
        break;
    }
    QObject::disconnect(timerConnection_);
}

void
VideoView::simulateShowChatview(bool checked)
{
    Q_UNUSED(checked);
    currentOverLayRendererPair_.first->simulateShowChatview(true);
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
                previewRenderer_->disconnectRendering();
                previewRenderer_->setCurrentConainerGeo(this->width(), this->height());
                // since there is the possiblity of image not reloaded properly
                // after rendering reconnect
                previewRenderer_->triggerResetPreviewAfterImageReloaded();
                previewRenderer_->connectRendering();
                currentOverLayRendererPair_.second->connectDistantRendering();

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
VideoView::pushRenderer(const std::string& callId, bool isSIP)
{
    currentCallId_ = callId;
    auto callModel = LRCInstance::getCurrentCallModel();

    auto pairToConnectRender = distantRendererMap_[callId];

    QObject::disconnect(pairToConnectRender.second);
    QObject::disconnect(callStatusChangedConnection_);

    if (!callModel->hasCall(callId)) {
        return;
    }

    auto call = callModel->getCall(callId);

    // transfer call will only happen in SIP calls
    pairToConnectRender.first->setTransferCallAndSIPPanelAvailability(isSIP);
    pairToConnectRender.first->callStarted(callId);
    pairToConnectRender.first->setVideoMuteVisibility(!LRCInstance::getCurrentCallModel()->getCall(callId).isAudioOnly);

    callStatusChangedConnection_ = QObject::connect(callModel, &lrc::api::NewCallModel::callStatusChanged,
        this, &VideoView::slotCallStatusChanged);

    if (!LRCInstance::getIfCurrentSelectedCallIsAudioOnly()) {
        previewRenderer_->connectRendering();
        pairToConnectRender.second->connectDistantRendering();
    }
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
        QRect previewRect = previewRenderer_->geometry();
        auto previewCentral = previewRect.center();
        auto videoViewRect = this->rect();
        auto distantVideoWidgetCentral = videoViewRect.center();
        if (previewCentral.x() >= distantVideoWidgetCentral.x()) {
            if (previewCentral.y() >= distantVideoWidgetCentral.y()) {
                //Move preview to bottom right
                auto previewInitialWidth = previewRenderer_->width();
                auto previewInitialHeight = previewRenderer_->height();
                moveAnim_->setStartValue(previewRenderer_->geometry());
                moveAnim_->setEndValue(QRect(this->width() - previewMargin_ - previewInitialWidth, this->height() - previewMargin_ - previewInitialHeight,
                                             previewInitialWidth, previewInitialHeight));
                moveAnim_->start();
            } else {
                //Move preview to top right
                auto previewInitialWidth = previewRenderer_->width();
                auto previewInitialHeight = previewRenderer_->height();
                moveAnim_->setStartValue(previewRenderer_->geometry());
                moveAnim_->setEndValue(QRect(this->width() - previewMargin_ - previewInitialWidth, previewMargin_,
                                             previewInitialWidth, previewInitialHeight));
                moveAnim_->start();
            }
        } else {
            if (previewCentral.y() >= distantVideoWidgetCentral.y()) {
                //Move preview to bottom left
                auto previewInitialWidth = previewRenderer_->width();
                auto previewInitialHeight = previewRenderer_->height();
                moveAnim_->setStartValue(previewRenderer_->geometry());
                moveAnim_->setEndValue(QRect(previewMargin_, this->height() - previewMargin_ - previewInitialHeight,
                                             previewInitialWidth, previewInitialHeight));
                moveAnim_->start();
            } else {
                //Move preview to top left
                auto previewInitialWidth = previewRenderer_->width();
                auto previewInitialHeight = previewRenderer_->height();
                moveAnim_->setStartValue(previewRenderer_->geometry());
                moveAnim_->setEndValue(QRect(previewMargin_, previewMargin_,
                                             previewInitialWidth, previewInitialHeight));
                moveAnim_->start();
            }
        }
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
    QLine distance = QLine(previewRect.topLeft(), event->pos());
}

void
VideoView::setCurrentCalleeName(const QString& CalleeDisplayName)
{
    currentOverLayRendererPair_.first->setCurrentSelectedCalleeDisplayName(CalleeDisplayName);
}

void
VideoView::resetVideoOverlay(bool isAudioMuted, bool isVideoMuted, bool isRecording, bool isHolding, bool isAudioOnly, const std::string& accountId, const lrc::api::conversation::Info& convInfo)
{
    resetAvatarOverlay(isAudioOnly);
    if (isAudioOnly) {
        writeAvatarOverlay(accountId, convInfo);
    }
    emit currentOverLayRendererPair_.first->setChatVisibility(false);
    currentOverLayRendererPair_.first->resetOverlay(isAudioMuted, isVideoMuted, isRecording, isHolding, isAudioOnly);
}

void
VideoView::resetAvatarOverlay(bool isAudioOnly)
{
    audioOnlyAvatar_->setAvatarVisible(isAudioOnly);
    if (isAudioOnly) {
        disconnect(coordinateOverlays_);
        coordinateOverlays_ = connect(currentOverLayRendererPair_.first, SIGNAL(HoldStatusChanged(bool)), this, SLOT(slotHoldStatusChanged(bool)));
    } else {
        disconnect(coordinateOverlays_);
    }
}

void
VideoView::writeAvatarOverlay(const std::string& accountId, const lrc::api::conversation::Info& convInfo)
{
    audioOnlyAvatar_->writeAvatarOverlay(accountId, convInfo);
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
        LRCInstance::getCurrentCallModel()->playDTMF(currentCallId_, "#");
    } else if (keyPressed_ == static_cast<int>(Qt::Key_Asterisk)) {
        LRCInstance::getCurrentCallModel()->playDTMF(currentCallId_, "*");
    } else if (keyPressed_ >= 48 && keyPressed_ <= 57) {
        //enum Qt::Key_0 = 48, QT::Key_9 = 57
        LRCInstance::getCurrentCallModel()->playDTMF(currentCallId_, std::to_string(keyPressed_ - 48));
    }
    QWidget::keyReleaseEvent(event);
}

void
VideoView::reconnectRenderingVideoDeviceChanged()
{
    currentOverLayRendererPair_.second->connectDistantRendering();
}

void
VideoView::createNewDistantRenderer(const std::string& callId)
{
    if (distantRendererMap_.find(callId) == distantRendererMap_.end()) {
        VideoOverlay* overlay = new VideoOverlay(this);
        DistantRendererWidget* widget = new DistantRendererWidget(this);

        if (currentOverLayRendererPair_.first == nullptr && currentOverLayRendererPair_.second == nullptr) {
            currentOverLayRendererPair_ = std::pair<VideoOverlay*, DistantRendererWidget*>(overlay, widget);
            distantRendererMap_[callId] = currentOverLayRendererPair_;
            widget->setGeometry(this->rect());
            ui->gridLayout->addWidget(widget);
        } else {
            distantRendererMap_[callId] = std::pair<VideoOverlay*, DistantRendererWidget*>(overlay, widget);
        }
        connect(overlay, &VideoOverlay::setChatVisibility, [=](bool visible) {
            emit this->setChatVisibility(visible);
            connect(this, SIGNAL(toggleFullScreenClicked()), widget, SLOT(slotToggleFullScreenClicked()));
        });
    }
}

void
VideoView::resetCurrentOverLayRendererPair(const std::string& callId)
{
    currentOverLayRendererPair_ = distantRendererMap_[callId];
}

void
VideoView::resetDistantRenderer(const std::string & callId)
{
    if (!currentOverLayRendererPair_.first && !currentOverLayRendererPair_.second) {
        currentOverLayRendererPair_ = distantRendererMap_[callId];
    } else if (distantRendererMap_.size() <= 1) {
        return;
    }

    // unset preview
    previewRenderer_->hide();
    // reset parent incase deletion of distant renderer causes error
    previewRenderer_->setParent(nullptr);

    // unset overlay, distantrender
    currentOverLayRendererPair_.first->setVisible(false);
    currentOverLayRendererPair_.second->setVisible(false);
    ui->gridLayout->removeWidget(currentOverLayRendererPair_.second);

    // reset current working pair
    currentOverLayRendererPair_ = distantRendererMap_[callId];

    // reset distantrender
    currentOverLayRendererPair_.second->setVisible(true);
    currentOverLayRendererPair_.second->setGeometry(this->rect());
    ui->gridLayout->addWidget(currentOverLayRendererPair_.second);

    // reset overlay
    currentOverLayRendererPair_.first->setVisible(true);
    currentOverLayRendererPair_.first->raise();

    previewRenderer_->setParent(this);
    previewRenderer_->show();
}
