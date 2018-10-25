/***************************************************************************
 * Copyright (C) 2018 by Savoir-faire Linux                                *
 * Author: Isa Nanic <isa.nanic@savoirfairelinux.com>                      *
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
#pragma once
#include <QComboBox>
#include "accountlistmodel.h"
#include "accountitemdelegate.h"

namespace Ui {
    class CurrentAccountComboBox;
}

class CurrentAccountComboBox : public QComboBox
{
    Q_OBJECT
        CurrentAccountComboBox(const CurrentAccountComboBox& cpy);

public:
    explicit CurrentAccountComboBox(QWidget* parent = nullptr);
    ~CurrentAccountComboBox();
    void importLabelPhoto(const int& index);

private:
    void paintEvent(QPaintEvent* e);

    AccountItemDelegate* accountItemDelegate_;
    std::unique_ptr<AccountListModel> accountListModel_;

    QPixmap currentAccountAvatarImage_;
    int cellHeight_ = 48; // [screen awareness]
};