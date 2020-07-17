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

import QtQuick 2.15
import QtQuick.Controls 2.15

ProgressBar {
    id: levelMeter

    value: {
        return clamp(rmsLevel * 300.0, 0.0, 100.0)
    }

    property real rmsLevel: 0

    function clamp(num,a,b){
        return Math.max(Math.min(num, Math.max(a, b)), Math.min(a, b))
    }

    function start(){
        rmsLevel = 0
    }

    function stop(){

    }

    function setLevel(rmsLevelIn){
        rmsLevel = rmsLevelIn
    }

}