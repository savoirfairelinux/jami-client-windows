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

    ui->labelWarning->setWordWrap(true);
    ui->updateAcceptBtn->setToolTip("Install Beta Version");
    ui->updateCancelBtn->setToolTip("Cancel Installation");

    setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
    setMaximumHeight(INT_MAX);
    setMaximumWidth(INT_MAX);

    // inital constraint of Text
    ui->labelWarning->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    ui->labelWarning->setMinimumHeight(300);
    ui->labelWarning->setMinimumWidth(350);

    QString warning { QString("Please be aware that this will uninstall your current Release version.<br>"
                              "You can always download the latest Release version on our website.<br>") };

    // reset size to be able to contain all text
    auto rect = ui->labelWarning->fontMetrics().boundingRect(ui->labelWarning->rect(), Qt::AlignCenter | Qt::TextWordWrap, warning);
    ui->labelWarning->setText(warning);
    ui->labelWarning->setMinimumHeight(rect.height());
    ui->labelWarning->setMinimumWidth(rect.width());
    setWindowTitle("Jami Beta Installation");
    ui->labelDeletion->setText("Install the latest Beta version?");

    setMaximumHeight(height() + rect.height());
    setMinimumHeight(height() + rect.height());
    setMaximumWidth(rect.width() + 40);
    setMinimumWidth(rect.width() + 40);
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
