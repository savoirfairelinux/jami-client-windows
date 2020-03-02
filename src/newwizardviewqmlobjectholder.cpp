/***************************************************************************
* Copyright (C) 2019-2020 by Savoir-faire Linux                            *
* Author: Yang Wang <yang.wang@savoirfairelinux.com>                       *
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 3 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
* This program is distributed in the hope that it will be useful,          *
* but WITHOUT ANY WARRANTY; without even the implied warranty of           *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
* GNU General Public License for more details.                             *
*                                                                          *
* You should have received a copy of the GNU General Public License        *
* along with this program.  If not, see <http://www.gnu.org/licenses/>.    *
***************************************************************************/

#include "newwizardviewqmlobjectholder.h"

NewWizardViewQmlObjectHolder::NewWizardViewQmlObjectHolder(QObject* parent)
    : QObject(parent)
{
    setRegistrationStateOk(false);
}

void
NewWizardViewQmlObjectHolder::setRegistrationStateOk(bool ok)
{
    if (registrationStateOk_ != ok) {
        registrationStateOk_ = ok;
        emit registrationStateOkChanged(ok);
    }
}

void
NewWizardViewQmlObjectHolder::setWizardMode(WizardMode mode)
{
    if (wizardMode_ != mode) {
        wizardMode_ = mode;
        emit wizardModeChanged(mode);
    }
}

Q_INVOKABLE void
NewWizardViewQmlObjectHolder::setNewWizardViewQmlObject(QObject* obj)
{
    newWizardViewQmlObject = obj;
}

Q_INVOKABLE void
NewWizardViewQmlObjectHolder::clearInputPara()
{
    inputPara_.clear();
}

Q_INVOKABLE void
NewWizardViewQmlObjectHolder::setInputParaHashPair(QString key, QString value)
{
    inputPara_[key] = value;
}

Q_INVOKABLE bool
NewWizardViewQmlObjectHolder::getIfKeyExistInParaHashPair(QString key)
{
    return(inputPara_.find(key) == inputPara_.end());
}

Q_INVOKABLE void
NewWizardViewQmlObjectHolder::createAccount()
{

}
