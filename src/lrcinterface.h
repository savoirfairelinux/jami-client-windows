/*
| Copyright (C) 2020 by Savoir-faire Linux
| Author: Andreas Traczyk <andreas.traczyk@savoirfairelinux.com>
|
| This program is free software; you can redistribute it and/or modify
| it under the terms of the GNU General Public License as published by
| the Free Software Foundation; either version 3 of the License, or
| (at your option) any later version.
|
| This program is distributed in the hope that it will be useful,
| but WITHOUT ANY WARRANTY; without even the implied warranty of
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
| GNU General Public License for more details.
|
| You should have received a copy of the GNU General Public License
| along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include "lrcinstance.h"

#include <QQmlEngine>
#include <QJSEngine>

using namespace lrc::api;

class AccountModelAdapter : public QObject
{
    Q_OBJECT;
    //Q_PROPERTY(QString time READ time WRITE setTime NOTIFY timeChanged);

public:
    explicit AccountModelAdapter(QObject* parent = 0);
    ~AccountModelAdapter();

    Q_INVOKABLE QVariant getAccountListSize();

signals:

private slots:

private:

    // <updated> currently selected account id
    // <updated> list of all account items

    NewAccountModel& model_;
};

class ConversationModelAdapter : public QObject
{
    Q_OBJECT;

public:
    ConversationModelAdapter(QObject* parent = 0);
    ~ConversationModelAdapter();

    Q_INVOKABLE QVariant getConversationsSize();

signals:

private slots:

private:
   ConversationModel& model_;
};

struct MyStruct
{
    Q_GADGET
    Q_PROPERTY(int val MEMBER m_val)
public:
    int m_val = 10;
};
Q_DECLARE_METATYPE(MyStruct)

class LrcGeneralAdapter : public QObject
{
    Q_OBJECT;
    Q_PROPERTY(MyStruct mystruct READ getMyStruct)

public:
    explicit LrcGeneralAdapter(QObject* parent = 0);
    ~LrcGeneralAdapter();

    MyStruct strObj;
    // Edit: changed get function
    Q_INVOKABLE MyStruct getMyStruct() const
    {
        return strObj;
    }

    Q_INVOKABLE const QString& getCurrAccId();
};