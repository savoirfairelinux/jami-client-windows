/***************************************************************************
 * Copyright (C) 2015 by Savoir-Faire Linux                                *
 * Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
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

#include "windowscontactbackend.h"

WindowsContactEditor::WindowsContactEditor(CollectionMediator<Person> *m
                                           , WindowsContactBackend *parent)
    : CollectionEditor<Person>(m),collection_(parent)
{

}

WindowsContactEditor::~WindowsContactEditor()
{

}

bool
WindowsContactEditor::save(const Person *item)
{
    Q_UNUSED(item)
    return false;
}

bool
WindowsContactEditor::remove(const Person *item)
{
    Q_UNUSED(item)
    return false;
}

bool
WindowsContactEditor::edit(Person *item)
{
    Q_UNUSED(item)
    return false;
}

bool
WindowsContactEditor::addNew(const Person *item)
{
    Q_UNUSED(item)
    return false;
}

bool
WindowsContactEditor::addExisting(const Person *item)
{
    items_ << const_cast<Person*>(item);
    mediator()->addItem(item);
    return true;
}

QVector<Person*>
WindowsContactEditor::items() const
{
    return items_;
}

WindowsContactBackend::WindowsContactBackend(CollectionMediator<Person>* mediator,
                                             CollectionInterface* parent)
    : CollectionInterface(new WindowsContactEditor(mediator,this), parent)
    , mediator_(mediator)
{

}

WindowsContactBackend::~WindowsContactBackend()
{

}

bool
WindowsContactBackend::load()
{
    QtConcurrent::run(this, &WindowsContactBackend::loadRun);
    return true;
}

bool
WindowsContactBackend::loadRun()
{
    QDir contactDir(QStandardPaths::writableLocation
                    (QStandardPaths::HomeLocation) + "/Contacts");
    QStringList filesList = contactDir.entryList(QStringList("*.contact"));

    for(auto contactFileName : filesList) {
        QString contactFilePath
                (contactDir.absolutePath() + "/" +  contactFileName);
        QFile contactFile(contactFilePath);
        if (contactFile.open(QIODevice::ReadOnly)) {
            QXmlStreamReader reader;
            Person *p = new Person();
            QVector<ContactMethod*> contactMethod;
            reader.setDevice(&contactFile);
            while (!reader.atEnd()) {
                reader.readNext();
                if (reader.isStartElement()) {
                    QString name = reader.name().toString();
                    if (name == "FormattedName")
                        p->setFormattedName(reader.readElementText());
                    else if (name == "NickName")
                        p->setNickName(reader.readElementText());
                    else if (name == "GivenName")
                        p->setFirstName(reader.readElementText());
                    else if (name == "FamilyName")
                        p->setFamilyName(reader.readElementText());
                    else if (name == "Company")
                        p->setOrganization(reader.readElementText());
                    else if (name == "Department")
                        p->setDepartment(reader.readElementText());
                    else if (name == "Number") {
                        ContactMethod *contact = const_cast<ContactMethod*>
                                (ContactMethod::BLANK());
                        contact->setPerson(p);

                        contact->setType(ContactMethod::Type::ACCOUNT);
                        contact->setUid(reader.readElementText());
                        contactMethod.append(contact);
                    }
                    else if (name == "Photo") {
                        //FIXME: It seems to be possible to have multiple photo...
                        reader.readNext();
                        if (reader.name().toString() == "Value") {
                            QString photoValue = reader.readElementText();
                            QByteArray byteArray;
                            byteArray.append(photoValue);
                            p->setPhoto(byteArray);
                        }
                    }
                    else if (name == "EmailAddress") {
                        QString address;
                        bool isPreferred = false;
                        reader.readNext();
                        while (reader.name().toString() != "EmailAddress"
                               && !reader.atEnd()) {
                            if (reader.isStartElement()) {
                                QString tag = reader.name().toString();
                                if (tag == "Address")
                                    address = reader.readElementText();
                                else if (tag == "Label")
                                    if (reader.readElementText() == "Preferred")
                                        isPreferred = true;
                            }
                            reader.readNext();
                        }
                        if (isPreferred)
                            p->setPreferredEmail(address);
                    }
                }
            }
            if (reader.hasError()) {
                qDebug() << reader.errorString();
            } else {
                if (contactMethod.size() > 0)
                    p->setContactMethods(contactMethod);
                editor<Person>()->addExisting(p);
            }
        } else {
            qDebug() << "Error Opening contat file : " << contactFileName;
        }
    }

    return false;
}

bool
WindowsContactBackend::reload()
{
    return false;
}

bool
WindowsContactBackend::clear()
{
    return false;
}

QString
WindowsContactBackend::name() const
{
    return "Windows Contact Backend";
}

QString
WindowsContactBackend::category() const
{
    return "Contacts";
}

bool
WindowsContactBackend::isEnabled() const
{
    return true;
}

QByteArray
WindowsContactBackend::id() const
{
    return "wincb";
}

FlagPack<CollectionInterface::SupportedFeatures> WindowsContactBackend::supportedFeatures() const
{
    return (
                CollectionInterface::SupportedFeatures::NONE |
                CollectionInterface::SupportedFeatures::LOAD);
}

