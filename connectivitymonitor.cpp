/***************************************************************************
 * Copyright (C) 2019 by Savoir-faire Linux                                *
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

#include "connectivitymonitor.h"

#include "lrcinstance.h"

ConnectivityMonitor::ConnectivityMonitor(QObject* parent)
    : QObject(parent)
{
    netManager_ = new QNetworkConfigurationManager(this);
    const auto conf = netManager_->defaultConfiguration();
    netSession_ = new QNetworkSession(conf, this);
    netSession_->open();

    connect(netManager_, &QNetworkConfigurationManager::configurationChanged,
        [this](const QNetworkConfiguration& config) {
            Q_UNUSED(config);
            emit configurationChanged();
        });

    connect(netManager_, &QNetworkConfigurationManager::onlineStateChanged,
        [=](bool online) {
            emit onlineStateChanged(online);
        });
}

ConnectivityMonitor::~ConnectivityMonitor()
{
    netSession_->close();
    delete netManager_;
    delete netSession_;
}

bool
ConnectivityMonitor::isOnline()
{
    auto activeConfigs = netManager_->allConfigurations(QNetworkConfiguration::Active);
    if (activeConfigs.count() > 0) {
        return netManager_->isOnline();
    }
    return false;
}