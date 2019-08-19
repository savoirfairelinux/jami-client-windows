#include "ui_callaudioonlyavataroverlay.h"
#include "callaudioonlyavataroverlay.h"

CallAudioOnlyAvatarOverlay::CallAudioOnlyAvatarOverlay(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CallAudioOnlyAvatarOverlay)
{
    ui->setupUi(this);
}

CallAudioOnlyAvatarOverlay::~CallAudioOnlyAvatarOverlay()
{
    delete ui;
}

void CallAudioOnlyAvatarOverlay::setAvatarVisible(bool visible)
{
    this->setVisible(visible);
    ui->avatarLabel->setVisible(visible);
    ui->nameLabel->setVisible(visible);
}

void CallAudioOnlyAvatarOverlay::writeAvatarOverlay(const std::string& accountId, const lrc::api::conversation::Info& convInfo)
{
    Q_UNUSED(accountId);
    auto contact = LRCInstance::getCurrentAccountInfo().contactModel->getContact(convInfo.participants.at(0));
    std::string peerAvatarInfo = contact.profileInfo.avatar;
    QPixmap avatarPix;
    QImage avatarImg;
    avatarPix.loadFromData(QByteArray::fromBase64(QByteArray::fromStdString(peerAvatarInfo)));
    avatarImg = avatarPix.toImage();
    int size;
    if (avatarImg.width() <= avatarImg.height())
    {
        size = avatarImg.width();
    } else {
        size = avatarImg.height();
    }

    QImage CircledAvatar = Utils::getCirclePhoto(avatarImg, size);
    ui->avatarLabel->setPixmap(QPixmap::fromImage(CircledAvatar));

    std::string name = contact.registeredName;
    std::string id = contact.profileInfo.uri;
    ui->nameLabel->setText(QString::fromStdString(name+":\n"+id));
}

void CallAudioOnlyAvatarOverlay::respondToPauseLabel(bool pauseButtonDisplayed)
{
    setAvatarVisible(!pauseButtonDisplayed);
}
