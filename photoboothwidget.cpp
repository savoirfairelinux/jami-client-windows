#include "photoboothwidget.h"
#include "ui_photoboothwidget.h"

#include <QFileDialog>
#include <QStandardPaths>

#include "video/previewmanager.h"

PhotoboothWidget::PhotoboothWidget(QWidget *parent) :
    QWidget(parent),
    fileName_(QStandardPaths::standardLocations(QStandardPaths::TempLocation).first()
              + QStringLiteral("profile.png")),
    ui(new Ui::PhotoboothWidget)
{
    ui->setupUi(this);
    ui->videoFeed->setIsFullPreview(true);
    ui->videoFeed->setPhotoMode(true);
    startBooth();
}

PhotoboothWidget::~PhotoboothWidget()
{
    Video::PreviewManager::instance().stopPreview();
    delete ui;
}

void PhotoboothWidget::startBooth()
{
    // // // //
    // stop (or start before) to give Preview manager some time to start
    // TODO go modify the daemon to ensure starting upon calling videomanager::startCamera
    Video::PreviewManager::instance().stopPreview();
    // // // //

    Video::PreviewManager::instance().startPreview();
    ui->videoFeed->show();
}

void PhotoboothWidget::stopBooth()
{
    Video::PreviewManager::instance().stopPreview();
    hide();
}

void
PhotoboothWidget::on_importButton_clicked()
{
    fileName_ = QFileDialog::getOpenFileName(this, tr("Choose File"),
                                            "",
                                            tr("Files (*)"));
    if (fileName_.isEmpty())
        fileName_ = QStandardPaths::standardLocations(
                    QStandardPaths::TempLocation).first()
                + QStringLiteral("profile.png");
    else {
        Video::PreviewManager::instance().stopPreview();
    }
    emit photoTaken(fileName_);
}

void
PhotoboothWidget::on_takePhotoButton_clicked()
{
    auto photo = ui->videoFeed->takePhoto();
    Video::PreviewManager::instance().stopPreview();
    photo.save(fileName_);
    emit photoTaken(fileName_);
}
