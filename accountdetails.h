#ifndef ACCOUNTDETAILS_H
#define ACCOUNTDETAILS_H

#include <QWidget>

#include <QTableWidgetItem>

#include "accountmodel.h"
#include "audio/codecmodel.h"

namespace Ui {
class AccountDetails;
}

class AccountDetails : public QWidget
{
    Q_OBJECT

public:
    explicit AccountDetails(QWidget *parent = 0);
    ~AccountDetails();

    void setAccount(Account *currentAccount);
    void reloadCodec();
    void save();
private slots:
    void audio_codec_checked(int row, int column);
    void setVideoEnabled(bool enabled);
    void video_codec_checked(int row, int column);
    void setAutoAnswer(bool enabled);
    void aliasChanged();
    void bootstrapChanged();
    void on_upAudioButton_clicked();
    void on_downAudioButton_clicked();
    void on_upVideoButton_clicked();
    void on_downVideoButton_clicked();
    void on_audioCodecView_itemSelectionChanged();
    void on_videoCodecView_itemSelectionChanged();

private:
    Ui::AccountDetails *ui;
    CodecModel* codecModel_;
    Account*    currentAccount_;

};

#endif // ACCOUNTDETAILS_H
