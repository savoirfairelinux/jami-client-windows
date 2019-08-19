#ifndef CALLAUDIOONLYAVATAROVERLAY_H
#define CALLAUDIOONLYAVATAROVERLAY_H

#include <QWidget>
#include "lrcinstance.h"
#include "utils.h"

namespace Ui {
class CallAudioOnlyAvatarOverlay;
}

class CallAudioOnlyAvatarOverlay : public QWidget
{
    Q_OBJECT

public:
    explicit CallAudioOnlyAvatarOverlay(QWidget *parent = nullptr);
    ~CallAudioOnlyAvatarOverlay();
    void setAvatarVisible(bool visible);
    void writeAvatarOverlay(const std::string& accountId, const lrc::api::conversation::Info& convInfo);

private:
    Ui::CallAudioOnlyAvatarOverlay *ui;

public slots:
    void respondToPauseLabel(bool pauseButtonDisplayed);
};

#endif // CALLAUDIOONLYAVATAROVERLAY_H
