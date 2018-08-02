/***************************************************************************
 * Copyright (C) 2015-2017 by Savoir-faire Linux                           *
 * Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
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

#include "video/devicemodel.h"
#include "video/sourcemodel.h"
#include "recentmodel.h"
#include "media/video.h"

#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QDesktopWidget>
#include <QMenu>
#include <QFileDialog>
#include <QMimeData>
#include <QSplitter>
#include <QScreen>

#include <memory>

#include "videooverlay.h"
#include "selectareadialog.h"

VideoView::VideoView(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::VideoView)
{
    ui->setupUi(this);

    connect(&CallModel::instance(), SIGNAL(callStateChanged(Call*, Call::State)),
            this, SLOT(callStateChanged(Call*, Call::State)));

    overlay_ = new VideoOverlay(this);
    auto effect = new QGraphicsOpacityEffect(overlay_);
    effect->setOpacity(1.0);
    overlay_->setGraphicsEffect(effect);
    fadeAnim_ = new QPropertyAnimation(this);
    fadeAnim_->setTargetObject(effect);
    fadeAnim_->setPropertyName("opacity");
    fadeAnim_->setDuration(fadeOverlayTime_);
    fadeAnim_->setStartValue(effect->opacity());
    fadeAnim_->setEndValue(0);
    fadeAnim_->setEasingCurve(QEasingCurve::OutQuad);

    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
        this, SLOT(showContextMenu(const QPoint&)));
    connect(overlay_, &VideoOverlay::setChatVisibility, [=](bool visible) {
        emit this->setChatVisibility(visible);
    });
    connect(overlay_, &VideoOverlay::videoCfgBtnClicked, [=](){emit videoSettingsClicked();});
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
    fadeAnim_->stop();
    fadeAnim_->targetObject()->setProperty(fadeAnim_->propertyName(), fadeAnim_->startValue());
}

void
VideoView::leaveEvent(QEvent* event)
{
    Q_UNUSED(event)
    if(not overlay_->isDialogVisible())
        fadeAnim_->start(QAbstractAnimation::KeepWhenStopped);
}

void
VideoView::callStateChanged(Call* call, Call::State previousState)
{
   Q_UNUSED(previousState)

    if (call->state() == Call::State::CURRENT) {
        ui->videoWidget->show();
        timerConnection_ = connect(call, SIGNAL(changed()), this, SLOT(updateCall()));
    }
    else {
        QObject::disconnect(timerConnection_);
        emit setChatVisibility(false);
        if (isFullScreen())
            toggleFullScreen();
    }
}

void
VideoView::updateCall()
{
    if (auto call = CallModel::instance().selectedCall()) {
        overlay_->setName(call->formattedName());
        overlay_->setTime(call->length());
    }
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
        event->acceptProposedAction();
}

void
VideoView::dropEvent(QDropEvent* event)
{
    auto urls = event->mimeData()->urls();
    if (auto call = CallModel::instance().selectedCall()) {
        if (auto outVideo = call->firstMedia<Media::Video>(Media::Media::Direction::OUT)) {
            outVideo->sourceModel()->setFile(urls.at(0));
        }
    }
}

void
VideoView::toggleFullScreen()
{
    overlay_->toggleContextButtons(isFullScreen());
    if(isFullScreen()) {
        dynamic_cast<QSplitter*>(oldParent_)->insertWidget(0,this);
        this->resize(oldSize_.width(), oldSize_.height());
        this->showNormal();
    } else {
        oldSize_ = this->size();
        oldParent_ = static_cast<QWidget*>(this->parent());
        this->setParent(0);
        this->showFullScreen();
    }
    ui->videoWidget->setResetPreview(true);
}

void
VideoView::showContextMenu(const QPoint& pos)
{
    QPoint globalPos = this->mapToGlobal(pos);

    QMenu menu;
    Media::Video* outVideo = nullptr;
    int activeIndex = -1;

    if (auto call = CallModel::instance().selectedCall()) {
        outVideo = call->firstMedia<Media::Video>(Media::Media::Direction::OUT);
        if (outVideo)
            activeIndex = outVideo->sourceModel()->activeIndex();
    }

    for (auto device : Video::DeviceModel::instance().devices()) {
        std::unique_ptr<QAction> deviceAction(new QAction(device->name(), this));
        deviceAction->setCheckable(true);
        if (outVideo)
            if (outVideo->sourceModel()->getDeviceIndex(device) == activeIndex)
                deviceAction->setChecked(true);
        auto ptr = deviceAction.release();
        menu.addAction(ptr);
        connect(ptr, &QAction::toggled, [=](bool checked) {
            if (checked == true) {
                if (outVideo)
                    outVideo->sourceModel()->switchTo(device);
                Video::DeviceModel::instance().setActive(device);
            }
        });
    }

    menu.addSeparator();

    auto shareAction = new QAction(tr("Share entire screen"), this);
    menu.addAction(shareAction);
    shareAction->setCheckable(true);
    auto shareAreaAction = new QAction(tr("Share screen area"), this);
    menu.addAction(shareAreaAction);
    shareAreaAction->setCheckable(true);
    connect(shareAreaAction, &QAction::triggered, [=]() {
        SelectAreaDialog selec;
        selec.exec();
    });
    auto shareFileAction = new QAction(tr("Share file"), this);
    menu.addAction(shareFileAction);
    shareFileAction->setCheckable(true);

    switch(activeIndex) {

    case Video::SourceModel::ExtendedDeviceList::SCREEN:
        shareAction->setChecked(true);
        break;
    case Video::SourceModel::ExtendedDeviceList::FILE:
        shareFileAction->setChecked(true);
        break;

    }

    connect(shareAction, &QAction::triggered, [=]() {
        if (outVideo) {
            auto realRect = QApplication::desktop()->geometry();
            realRect.setWidth(static_cast<int>(realRect.width() * QApplication::primaryScreen()->devicePixelRatio()));
            realRect.setHeight(static_cast<int>(realRect.height() * QApplication::primaryScreen()->devicePixelRatio()));
            outVideo->sourceModel()->setDisplay(0, realRect);
        }
    });
    connect(shareFileAction, &QAction::triggered, [=]() {
        QFileDialog dialog(this);
        dialog.setFileMode(QFileDialog::AnyFile);
        QStringList fileNames;
        if (!dialog.exec())
            return;
        fileNames = dialog.selectedFiles();
        if (outVideo)
            outVideo->sourceModel()->setFile(QUrl::fromLocalFile(fileNames.at(0)));
    });

    menu.exec(globalPos);
}

void
VideoView::pushRenderer(Call* call) {
    if (not call) {
        disconnect(videoStartedConnection_);
        return;
    }
    if (auto renderer = call->videoRenderer()) {
        slotVideoStarted(renderer);
    } else {
        disconnect(videoStartedConnection_);
        videoStartedConnection_ = connect(call,
                SIGNAL(videoStarted(Video::Renderer*)),
                this,
                SLOT(slotVideoStarted(Video::Renderer*)));
    }
    ui->videoWidget->setPreviewDisplay(call->type() != Call::Type::CONFERENCE);
}

void
VideoView::slotVideoStarted(Video::Renderer* renderer) {
    ui->videoWidget->show();
    ui->videoWidget->setDistantRenderer(renderer);
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
