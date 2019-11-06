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
#include "selectareadialog.h"
#include "videooverlay.h"

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

VideoView::VideoView(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::VideoView)
{
    ui->setupUi(this);

    vignette_ = new VignetteWidget(this);
    overlay_ = new VideoOverlay(this);
    audioOnlyAvatar_ = new CallAudioOnlyAvatarOverlay(this);
    previewWidget_ = new VideoCallPreviewWidget(this);

    // setup layer order
    vignette_->stackUnder(previewWidget_);
    overlay_->raise();

    overlay_->setMouseTracking(true);

    // preview widget animation
    moveAnim_ = new QPropertyAnimation(previewWidget_, "geometry");
    moveAnim_->setDuration(250);
    moveAnim_->setEasingCurve(QEasingCurve::OutExpo);

    // this connects the vignette's opacity to the overlay
    connect(overlay_, &FadeOutable::willFadeOut,
            vignette_, &VignetteWidget::slotWillFadeOut);
    connect(overlay_, &FadeOutable::willReset,
            vignette_, &VignetteWidget::slotWillResetOpacity);

    // context menu
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(showContextMenu(const QPoint&)));

    // chat panel
    connect(overlay_, &VideoOverlay::setChatVisibility,
        [this](bool visible) {
            emit this->setChatVisibility(visible);
        });

    // plug/unplug
    connect(LRCInstance::renderer(),
        &RenderManager::videoDeviceListChanged,
        [this] {
            resetPreview();
        });

    // possible device change
    connect(LRCInstance::renderer(),
        &RenderManager::previewRenderingStarted,
        [this] {
            resetPreview();
        });

    connect(overlay_,
        &VideoOverlay::videoMuteStateChanged,
        this,
        &VideoView::slotVideoMuteStateChanged,
        Qt::UniqueConnection);
}

VideoView::~VideoView()
{
    delete ui;
}

void
VideoView::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event);

    moveAnim_->stop();

    resetPreview();

    vignette_->resize(this->size());
    audioOnlyAvatar_->resize(this->size());
    overlay_->resize(this->size());
}

void
VideoView::simulateShowChatview(bool checked)
{
    overlay_->simulateShowChatview(checked);
}

void
VideoView::mouseDoubleClickEvent(QMouseEvent* e)
{
    QWidget::mouseDoubleClickEvent(e);
    if (e->button() == Qt::LeftButton) {
        toggleFullScreen();
    }
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
    auto conversation = LRCInstance::getCurrentConversation();
    if (conversation.uid.empty()) return;
    auto callIdList = LRCInstance::getActiveCalls();
    for (auto callId : callIdList) {
        if (callId == conversation.callId) {
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
VideoView::showContextMenu(const QPoint& position)
{
    QPoint globalPos = this->mapToGlobal(position);

    QMenu menu;

    auto conversation = LRCInstance::getCurrentConversation();
    auto call = LRCInstance::getCallInfoForConversation(conversation);
    if (!call || call->isAudioOnly) {
        return;
    }

    auto activeDevice = LRCInstance::avModel().getCurrentRenderedDevice(call->id);

    // video input devices
    auto devices = LRCInstance::avModel().getDevices();
    auto device = LRCInstance::avModel().getCurrentVideoCaptureDevice();
    for (auto d : devices) {
        try {
            auto settings = LRCInstance::avModel().getDeviceSettings(d);
            QString deviceName = QString::fromStdString(settings.name).toUtf8();
            auto deviceAction = new QAction(deviceName, this);
            menu.addAction(deviceAction);
            deviceAction->setCheckable(true);
            if (d == activeDevice.name) {
                deviceAction->setChecked(true);
            }
            connect(deviceAction, &QAction::triggered,
                [this, deviceName]() {
                    auto deviceId = LRCInstance::avModel().getDeviceIdFromName(deviceName.toStdString());
                    if (deviceId.empty()) {
                        qWarning() << "Couldn't find device: " << deviceName;
                        return;
                    }
                    if (LRCInstance::avModel().getCurrentVideoCaptureDevice() == deviceId) {
                        return;
                    }
                    LRCInstance::avModel().switchInputTo(deviceId);
                    resetPreview();
                    LRCInstance::avModel().setCurrentVideoCaptureDevice(deviceId);
                });
        } catch (...) {}
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
            resetPreview();

            sharingEntireScreen_ = true;
        });

    // area of screen share
    auto shareAreaAction = new QAction(tr("Share screen area"), this);
    menu.addAction(shareAreaAction);
    shareAreaAction->setCheckable(true);
    connect(shareAreaAction, &QAction::triggered,
        [this]() {
            QRect area;
            int screenNumber = 0;
            SelectAreaDialog selectAreaDialog(screenNumber, area, this);
            int code = selectAreaDialog.exec();
            if (code == 0) {
                LRCInstance::avModel().setDisplay(screenNumber,
                    area.x(), area.y(), area.width(), area.height()
                );
            }
            resetPreview();
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
            resetPreview();
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
VideoView::updateCall(const std::string& convUid,
                      const std::string& accountId)
{
    accountId_ = accountId.empty() ? accountId_ : accountId;
    convUid_ = convUid.empty() ? convUid_ : convUid;

    auto convInfo = LRCInstance::getConversationFromConvUid(convUid_, accountId_);
    if (convInfo.uid.empty()) {
        return;
    }

    auto call = LRCInstance::getCallInfoForConversation(convInfo);
    if (!call) {
        return;
    }

    overlay_->updateCall(convInfo);
    // TODO(atraczyk): this should be part of the overlay
    audioOnlyAvatar_->setAvatarVisible(call->isAudioOnly);
    if (call->isAudioOnly) {
        audioOnlyAvatar_->writeAvatarOverlay(convInfo);
    }

    // preview
    previewWidget_->setVisible(shouldShowPreview());

    // distant
    ui->distantWidget->setRendererId(call->id);
    auto isPaused = call->status == lrc::api::call::Status::PAUSED;
    ui->distantWidget->setVisible(!isPaused);
}

void
VideoView::mousePressEvent(QMouseEvent* event)
{
    QPoint clickPosition = event->pos();
    if (!previewWidget_->geometry().contains(clickPosition)) {
        return;
    }

    QLine distance = QLine(clickPosition, previewWidget_->geometry().bottomRight());
    originMouseDisplacement_ = event->pos() - previewWidget_->geometry().topLeft();
    QApplication::setOverrideCursor(Qt::ClosedHandCursor);
    draggingPreview_ = true;
    moveAnim_->stop();
}

void
VideoView::mouseReleaseEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    if (!draggingPreview_) {
        return;
    }

    //Check preview's current central position
    auto previewCenter = previewWidget_->geometry().center();
    auto distantWidgetCenter = ui->distantWidget->rect().center();
    moveAnim_->stop();
    moveAnim_->setStartValue(previewWidget_->geometry());
    PreviewSnap newPreviewLocation;
    if (previewCenter.x() >= distantWidgetCenter.x()) {
        if (previewCenter.y() >= distantWidgetCenter.y()) {
            newPreviewLocation = PreviewSnap::SE;
        } else {
            newPreviewLocation = PreviewSnap::NE;
        }
    } else {
        if (previewCenter.y() >= distantWidgetCenter.y()) {
            newPreviewLocation = PreviewSnap::SW;
        } else {
            newPreviewLocation = PreviewSnap::NW;
        }
    }
    previewWidget_->setLocation(newPreviewLocation);
    QPoint endPoint = previewWidget_->getTopLeft();
    moveAnim_->setEndValue(QRect(endPoint, previewWidget_->size()));
    moveAnim_->start();
    draggingPreview_ = false;
    QApplication::setOverrideCursor(Qt::ArrowCursor);
}

void
VideoView::mouseMoveEvent(QMouseEvent* event)
{
    if (!draggingPreview_) {
        return;
    }

    QRect previewRect = previewWidget_->geometry();

    if (previewRect.left() > 0
        && previewRect.top() > 0
        && previewRect.right() < width()
        && previewRect.bottom() < height()) {

        previewRect.moveTo(event->pos() - originMouseDisplacement_);
        if (previewRect.left() <= 0) previewRect.moveLeft(1);
        if (previewRect.right() >= width()) previewRect.moveRight(width() - 1);
        if (previewRect.top() <= 0) previewRect.moveTop(1);
        if (previewRect.bottom() >= height()) previewRect.moveBottom(height() - 1);
    }

    previewWidget_->setGeometry(previewRect);
    previewWidget_->forceRepaint();

}

void
VideoView::slotVideoMuteStateChanged(bool state)
{
    Q_UNUSED(state);
    resetPreview();
}

bool
VideoView::shouldShowPreview()
{
    bool shouldShowPreview{ false };
    auto convInfo = LRCInstance::getConversationFromConvUid(convUid_, accountId_);
    if (convInfo.uid.empty()) {
        return shouldShowPreview;
    }
    auto call = LRCInstance::getCallInfoForConversation(convInfo);
    if (call) {
        shouldShowPreview =
            !call->isAudioOnly &&
            !(call->status == lrc::api::call::Status::PAUSED) &&
            !call->videoMuted &&
            call->type != lrc::api::call::Type::CONFERENCE;
    }
    return shouldShowPreview;
}

void
VideoView::resetPreview()
{

    if (!shouldShowPreview()) {
        previewWidget_->setVisible(false);
        return;
    }

    previewWidget_->setContainerSize(this->size());
    auto previewImage = LRCInstance::renderer()->getPreviewFrame();
    int width;
    int height;
    if (previewImage) {
        width = previewImage->width();
        height = previewImage->height();
    } else {
        auto device = LRCInstance::avModel().getCurrentVideoCaptureDevice();
        if (device.empty()) {
            device = LRCInstance::avModel().getDefaultDevice();
        }
        if (device.empty()) {
            previewWidget_->setVisible(false);
            return;
        }
        try {
            auto settings = LRCInstance::avModel().getDeviceSettings(device);
            width = QString::fromStdString(settings.size).split("x")[0].toInt();
            height = QString::fromStdString(settings.size).split("x")[1].toInt();
        } catch (...) {}
    }
    auto newSize = previewWidget_->getScaledSize(width, height);
    previewWidget_->setupGeometry(newSize);

    previewWidget_->setVisible(true);

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
    auto convInfo = LRCInstance::getCurrentConversation();
    if (convInfo.uid.empty()) {
        return;
    }
    if (keyPressed_ == static_cast<int>(Qt::Key_NumberSign)) {
        LRCInstance::getCurrentCallModel()->playDTMF(convInfo.callId, "#");
    } else if (keyPressed_ == static_cast<int>(Qt::Key_Asterisk)) {
        LRCInstance::getCurrentCallModel()->playDTMF(convInfo.callId, "*");
    } else if (keyPressed_ >= 48 && keyPressed_ <= 57) {
        //enum Qt::Key_0 = 48, QT::Key_9 = 57
        LRCInstance::getCurrentCallModel()->playDTMF(convInfo.callId, std::to_string(keyPressed_ - 48));
    }
    QWidget::keyReleaseEvent(event);
}
