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

#include "utils.h"
#include "lrcinstance.h"

#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QDesktopWidget>
#include <QMenu>
#include <QFileDialog>
#include <QMimeData>
#include <QSplitter>
#include <QScreen>
#include <QWindow>

#include <memory>

#include "videooverlay.h"
#include "selectareadialog.h"

VideoView::VideoView(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::VideoView)
{
    ui->setupUi(this);

    overlay_ = new VideoOverlay(this);
    auto effect = new QGraphicsOpacityEffect(overlay_);
    effect->setOpacity(maxOverlayOpacity_);
    overlay_->setGraphicsEffect(effect);
    fadeAnim_ = new QPropertyAnimation(this);
    fadeAnim_->setTargetObject(effect);
    fadeAnim_->setPropertyName("opacity");
    fadeAnim_->setDuration(fadeOverlayTime_);
    fadeAnim_->setStartValue(effect->opacity());
    fadeAnim_->setEndValue(0);
    fadeAnim_->setEasingCurve(QEasingCurve::OutQuad);

    // Setup the timer to start the fade when the mouse stops moving
    this->setMouseTracking(true);
    overlay_->setMouseTracking(true);
    fadeTimer_.setSingleShot(true);
    connect(&fadeTimer_, SIGNAL(timeout()), this, SLOT(fadeOverlayOut()));

    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
        this, SLOT(showContextMenu(const QPoint&)));
    connect(overlay_, &VideoOverlay::setChatVisibility, [=](bool visible) {
        emit this->setChatVisibility(visible);
    });

}

VideoView::~VideoView()
{
    delete ui;
    delete overlay_;
    delete fadeAnim_;
}

void
VideoView::resizeEvent(QResizeEvent* event)
{
    QRect& previewRect = ui->videoWidget->getPreviewRect();
    int deltaW = event->size().width() - event->oldSize().width();
    int deltaH = event->size().height() - event->oldSize().height();

    QPoint previewCenter = ui->videoWidget->getPreviewRect().center();
    int cx = (event->oldSize().width()) / 2;
    int cy = (event->oldSize().height()) / 2;
    QPoint center = QPoint(cx, cy);

    // first we check if we want to displace the preview
    if (previewRect.x() + deltaW > 0 && previewRect.y() + deltaH > 0) {
        // then we check which way
        if (center.x() - previewCenter.x() < 0 && center.y() - previewCenter.y() < 0)
            ui->videoWidget->getPreviewRect().translate(deltaW, deltaH);
        else if (center.x() - previewCenter.x() > 0 && center.y() - previewCenter.y() < 0)
            ui->videoWidget->getPreviewRect().translate(0, deltaH);
        else if (center.x() - previewCenter.x() < 0 && center.y() - previewCenter.y() > 0)
            ui->videoWidget->getPreviewRect().translate(deltaW, 0);
    }

    if (previewRect.left() <= 0)
        previewRect.moveLeft(1);

    if (previewRect.right() >= width())
        previewRect.moveRight(width() - 1);

    if (previewRect.top() <= 0)
        previewRect.moveTop(1);

    if (previewRect.bottom() >= height())
        previewRect.moveBottom(height() - 1);

    overlay_->resize(this->size());
    overlay_->show();
    overlay_->raise();
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
    fadeAnim_->stop();
    fadeAnim_->targetObject()->setProperty(fadeAnim_->propertyName(), fadeAnim_->startValue());
}

void
VideoView::fadeOverlayOut()
{
    if (!overlay_->isDialogVisible() && !overlay_->shouldShowOverlay()) {
        fadeAnim_->start(QAbstractAnimation::KeepWhenStopped);
    }
}

void
VideoView::slotCallStatusChanged(const std::string& callId)
{
    using namespace lrc::api::call;
    auto call = LRCInstance::getCurrentCallModel()->getCall(callId);
    switch (call.status) {
    case Status::IN_PROGRESS:
    {
        ui->videoWidget->show();
        auto convInfo = Utils::getConversationFromCallId(call.id);
        if (!convInfo.uid.empty()) {
            auto contactInfo = LRCInstance::getCurrentAccountInfo().contactModel->getContact(convInfo.participants[0]);
            auto contactName = Utils::bestNameForContact(contactInfo);
            overlay_->setName(QString::fromStdString(contactName));
        }
        return;
    }
    default:
        emit closing(call.id);
        break;
    }
    QObject::disconnect(timerConnection_);
}

void
VideoView::showChatviewIfToggled()
{
    emit setChatVisibility(overlay_->getShowChatView());
}

void
VideoView::simulateShowChatview(bool checked)
{
    Q_UNUSED(checked);
    overlay_->simulateShowChatview(true);
}

void
VideoView::mouseDoubleClickEvent(QMouseEvent* e) {
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
                LRCInstance::avModel().switchInputTo(deviceName.toStdString());
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
        sharingEntireScreen_ ?
            shareAction->setChecked(true) :
            shareAreaAction->setChecked(true);
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
VideoView::pushRenderer(const std::string& callId) {
    auto callModel = LRCInstance::getCurrentCallModel();

    QObject::disconnect(ui->videoWidget);
    QObject::disconnect(callStatusChangedConnection_);

    if (!callModel->hasCall(callId)) {
        return;
    }

    auto call = callModel->getCall(callId);

    if (call.isAudioOnly) {
        return;
    }

    this->overlay_->callStarted(callId);
    this->overlay_->setVideoMuteVisibility(!LRCInstance::getCurrentCallModel()->getCall(callId).isAudioOnly);

    callStatusChangedConnection_ = QObject::connect(callModel, &lrc::api::NewCallModel::callStatusChanged,
        this, &VideoView::slotCallStatusChanged);

    ui->videoWidget->connectRendering();
    ui->videoWidget->setPreviewDisplay(call.type != lrc::api::call::Type::CONFERENCE);
}

void
VideoView::mousePressEvent(QMouseEvent* event)
{
    QPoint clickPosition = event->pos();
    if (ui->videoWidget->getPreviewRect().contains(clickPosition)) {
        QLine distance = QLine(clickPosition, ui->videoWidget->getPreviewRect().bottomRight());
        if (distance.dy() < resizeGrip_ and distance.dx() < resizeGrip_) {
            QApplication::setOverrideCursor(Qt::SizeFDiagCursor);
            resizingPreview_ = true;
        } else {
            originMouseDisplacement_ = event->pos() - ui->videoWidget->getPreviewRect().topLeft();
            QApplication::setOverrideCursor(Qt::SizeAllCursor);
            draggingPreview_ = true;
        }
    }
}

void
VideoView::mouseReleaseEvent(QMouseEvent* event)
{
    Q_UNUSED(event)

    draggingPreview_ = false;
    resizingPreview_ = false;
    QApplication::setOverrideCursor(Qt::ArrowCursor);
}

void
VideoView::mouseMoveEvent(QMouseEvent* event)
{
    // start/restart the timer after which the overlay will fade
    if (fadeTimer_.isActive()) {
        showOverlay();
    } else {
        fadeTimer_.start(startfadeOverlayTime_);
    }

    QRect& previewRect =  ui->videoWidget->getPreviewRect();
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

    QLine distance = QLine(previewRect.topLeft(), event->pos());

    if (resizingPreview_
            and distance.dx() > minimalSize_
            and distance.dy() > minimalSize_
            and geometry().contains(event->pos()))
        previewRect.setBottomRight(event->pos());
}
