/***************************************************************************
 * Copyright (C) 2015 by Savoir-faire Linux                                *
 * Author: Jäger Nicolas <nicolas.jager@savoirfairelinux.com>              *
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

#include <QTreeView>

class SmartListDelegate;
class ComBar;

class SmartList
: public QTreeView
{
	Q_OBJECT
public:
	explicit SmartList(QWidget* parent = 0);
	~SmartList();
	void leaveEvent(QEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void paintEvent(QPaintEvent* event);
	void setSmartListItemDelegate(SmartListDelegate*);

signals:
	void MonSignal(bool) const;
	
private:
  int currentRow;
  SmartListDelegate* smartListDelegate_;
  ComBar* comBar_;
	
};
