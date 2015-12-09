/***************************************************************************
 * Copyright (C) 2015 by Savoir-faire Linux                                *
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

#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QDesktopWidget>
#include <QMenu>
#include <QFileDialog>
#include <QMimeData>

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
    Q_UNUSED(event)
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
    Video::SourceModel::instance().setFile(urls.at(0));
}

void
VideoView::toggleFullScreen()
{
    if(isFullScreen()) {
        this->setParent(oldParent_);
        oldParent_->layout()->addWidget(this);
        this->resize(oldSize_.width(), oldSize_.height());
        this->showNormal();
    } else {
        oldSize_ = this->size();
        oldParent_ = static_cast<QWidget*>(this->parent());
        this->setParent(0);
        this->showFullScreen();
    }
}

void
VideoView::showContextMenu(const QPoint& pos)
{
    QPoint globalPos = this->mapToGlobal(pos);

    QMenu menu;

    for (auto device : Video::DeviceModel::instance().devices()) {
        std::unique_ptr<QAction> deviceAction(new QAction(device->name(), this));
        deviceAction->setCheckable(true);
        if (device == Video::DeviceModel::instance().activeDevice())
            deviceAction->setChecked(true);
        auto ptr = deviceAction.release();
        menu.addAction(ptr);
        connect(ptr, &QAction::toggled, [=](bool checked) {
            if (checked == true) {
                Video::SourceModel::instance().switchTo(device);
                Video::DeviceModel::instance().setActive(device);
            }
        });
    }

    menu.addSeparator();

    auto shareAction = new QAction(tr("Share entire screen"), this);
    menu.addAction(shareAction);
    connect(shareAction, &QAction::triggered, [=]() {
        Video::SourceModel::instance().setDisplay(0, QApplication::desktop()->rect());
    });
    auto shareAreaAction = new QAction(tr("Share screen area"), this);
    menu.addAction(shareAreaAction);
    connect(shareAreaAction, &QAction::triggered, [=]() {
        SelectAreaDialog selec;
        selec.exec();
    });
    auto shareFileAction = new QAction(tr("Share file"), this);
    menu.addAction(shareFileAction);
    connect(shareFileAction, &QAction::triggered, [=]() {
        QFileDialog dialog(this);
        dialog.setFileMode(QFileDialog::AnyFile);
        QStringList fileNames;
        if (!dialog.exec())
            return;
        fileNames = dialog.selectedFiles();
        Video::SourceModel::instance().setFile(QUrl::fromLocalFile(fileNames.at(0)));
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

