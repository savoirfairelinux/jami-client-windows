/****************************************************************************
 *   Copyright (C) 2018 Savoir-faire Linux                                  *
 *   Author: Isa Nanic <isa.nanic@savoirfairelinux.com>                     *
 *                                                                          *
 *   This library is free software; you can redistribute it and/or          *
 *   modify it under the terms of the GNU Lesser General Public             *
 *   License as published by the Free Software Foundation; either           *
 *   version 2.1 of the License, or (at your option) any later version.     *
 *                                                                          *
 *   This library is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
 *   Lesser General Public License for more details.                        *
 *                                                                          *
 *   You should have received a copy of the GNU General Public License      *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/
#include <QListWidgetItem>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>



class SettingsItemWidget : public QListWidgetItem
{
public:
    SettingsItemWidget(int height, int index, bool banned, QListWidget* parent = nullptr);
    ~SettingsItemWidget();

    QPushButton* button_;

private:
    bool selected_ = false;
    bool banned_ = false;
    const int index_;
    const int height_;

    const int horMargin_ = 15;
    const int verMargin_ = 8;

    QLineEdit* nameEdit_;
    QLabel* idlabel_;

    void toggleEditable();
    void finishEdit();

    bool editable_ = false;
};

