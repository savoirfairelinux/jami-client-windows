/*
 * Copyright (C) 2019-2020 by Savoir-faire Linux
 * Author: Yang Wang   <yang.wang@savoirfairelinux.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "accountadapter.h"
#include "accountlistmodel.h"
#include "audiocodeclistmodel.h"
#include "avadapter.h"
#include "bannedlistmodel.h"
#include "calladapter.h"
#include "contactadapter.h"
#include "conversationsadapter.h"
#include "deviceitemlistmodel.h"
#include "distantrenderer.h"
#include "globalinstances.h"
#include "globalsystemtray.h"
#include "messagesadapter.h"
#include "namedirectory.h"
#include "pixbufmanipulator.h"
#include "previewrenderer.h"
#include "qrimageprovider.h"
#include "settingsadaptor.h"
#include "utils.h"
#include "version.h"
#include "videocodeclistmodel.h"

#include <QObject>

class ClientWrapper : public QObject
{
    Q_OBJECT

    Q_PROPERTY(UtilsAdapter *utilsAdaptor READ getUtilsAdapter)
    Q_PROPERTY(SettingsAdaptor *settingsAdaptor READ getSettingsAdaptor)
    Q_PROPERTY(NameDirectory *nameDirectory READ getNameDirectory)
    Q_PROPERTY(LRCInstance *lrcInstance READ getLRCInstance)
    Q_PROPERTY(AccountAdapter *accountAdaptor READ getAccountAdapter)
    Q_PROPERTY(RenderManager *renderManager READ getRenderManager)
    Q_PROPERTY(NewAccountModel *accountModel READ getAccountModel)
    Q_PROPERTY(AVModel *avmodel READ getAvModel)
    Q_PROPERTY(DataTransferModel *dataTransferModel READ getDataTransferModel)
    Q_PROPERTY(ContactModel *contactModel READ getContactModel)
    Q_PROPERTY(NewDeviceModel *deviceModel READ getDeviceModel)
public:
    explicit ClientWrapper(QObject *parent = nullptr);

    NameDirectory *getNameDirectory();
    UtilsAdapter *getUtilsAdapter();
    SettingsAdaptor *getSettingsAdaptor();
    LRCInstance *getLRCInstance();
    AccountAdapter *getAccountAdapter();

    RenderManager *getRenderManager();
    NewAccountModel *getAccountModel();
    AVModel *getAvModel();
    DataTransferModel *getDataTransferModel();

    ContactModel *getContactModel();
    NewDeviceModel *getDeviceModel();

signals:
};
#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
Q_DECLARE_METATYPE(ClientWrapper *)
#endif
