/***************************************************************************
 * Copyright (C) 2019 by Savoir-faire Linux                                *
 * Author: Mingrui Zhang <mingrui.zhang@savoirfairelinux.com>              *
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
#include "ui_updateconfirmdialog.h"
#include "updateconfirmdialog.h"

#include <QDesktopServices>
#include <QFontMetrics>
#include <QUrl>

UpdateConfirmDialog::UpdateConfirmDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::UpdateConfirmDialog)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

UpdateConfirmDialog::~UpdateConfirmDialog()
{
    delete ui;
}

void
UpdateConfirmDialog::changeToUpdateToBetaVersionText()
{

    connect(ui->labelWarning, &QLabel::linkActivated,
        [this] (const QString& link) {
            QDesktopServices::openUrl(QUrl(link));
        });

    ui->updateAcceptBtn->setToolTip("Install Beta Version");
    ui->updateCancelBtn->setToolTip("Cancel Installation");

    setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
    setMaximumHeight(INT_MAX);
    setMaximumWidth(INT_MAX);

    // inital constraint of Text
    ui->labelWarning->setMinimumHeight(350);
    ui->labelWarning->setMinimumWidth(450);

    QString warning { QString("First, please note that this will install a beta version of Jami which is "
                              "still undergoing final testing before its official release. We do not "
                              "give any warranties, whether express or implied, as to the suitability "
                              "or usability of this software. In addition, this will also uninstall your "
                              "current Release version.<br><br>") +
                      QString("Second, should you encounter any bugs, glitches, lack of functionality or other "
                              "problems on Jami, please let us know immediately so we can rectify these "
                              "accordingly. Your help in this regard is greatly appreciated! "
                              "<a href=\"https://git.jami.net/savoirfairelinux/ring-project\">You can write "
                              "to us at this address</a>.<br><br>") +
                      QString("You can always download the newest Release version on our website.<br><br>") };

    // reset size to be able to contain all text
    auto rect = ui->labelWarning->fontMetrics().boundingRect(ui->labelWarning->rect(), Qt::AlignCenter | Qt::TextWordWrap, warning);
    ui->labelWarning->setText(warning);
    ui->labelWarning->setMinimumHeight(rect.height());
    ui->labelWarning->setMinimumWidth(rect.width());
    setWindowTitle("Jami Beta Installation");
    ui->labelDeletion->setText("Install the newest Beta version?");

    setMaximumHeight(height() + rect.height());
    setMinimumHeight(height() + rect.height());
    setMaximumWidth(rect.width() + 50);
    setMinimumWidth(rect.width() + 50);
    setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
}

void
UpdateConfirmDialog::on_updateCancelBtn_clicked()
{
    done(DialogCode::Rejected);
}

void
UpdateConfirmDialog::on_updateAcceptBtn_clicked()
{
    done(DialogCode::Accepted);
}
