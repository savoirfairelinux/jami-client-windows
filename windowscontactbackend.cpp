/***************************************************************************
 * Copyright (C) 2015-2017 by Savoir-faire Linux                                *
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

#include <QtXml>
#include <QPainter>

#include "personmodel.h"
#include "categorizedcontactmodel.h"

#include "utils.h"

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
    QFile file(QStandardPaths::writableLocation
               (QStandardPaths::HomeLocation) + "/Contacts/"
               + item->formattedName() + ".contact");
    if (!file.open(QIODevice::ReadWrite)) {
        file.close();
        qDebug() << "Cannot open contact file";
        return false;
    }

    QDomDocument doc;
    doc.setContent(&file);

    auto root = doc.elementsByTagName("c:contact").at(0);
    auto nodes = doc.elementsByTagName("c:PhoneNumberCollection");

    //if PhoneNumberCollection already exists
    QVector<QString> nodeNumberVector;
    if (nodes.length()) {
        auto phoneNumberCollection = nodes.at(0);
        auto phoneNumbers = doc.elementsByTagName("c:Number");
        auto virtualPhoneNumber = item->phoneNumbers();
        for (int i = 0; i < phoneNumbers.length(); i++) {
            auto node = phoneNumbers.at(i).toElement();
            nodeNumberVector.append(node.text());
        }
        for (auto elem : virtualPhoneNumber) {
            if (not nodeNumberVector.contains(elem->uri())) {
                auto phoneNumber = doc.createElement("c:PhoneNumber");
                phoneNumberCollection.appendChild(phoneNumber);
                phoneNumber.setAttribute("c:ElementID", Utils::GenGUID());
                auto numberNode = doc.createElement("c:Number");
                phoneNumber.appendChild(numberNode);
                auto numberValue = doc.createTextNode(elem->uri());
                numberNode.appendChild(numberValue);
            }
        }
    } else {
        auto phoneNumberCollection = doc.createElement("c:PhoneNumberCollection");
        root.appendChild(phoneNumberCollection);
        auto phoneNumber = doc.createElement("c:PhoneNumber");
        phoneNumberCollection.appendChild(phoneNumber);
        phoneNumber.setAttribute("c:ElementID", Utils::GenGUID());
        auto numberNode = doc.createElement("c:Number");
        phoneNumber.appendChild(numberNode);
        auto numberValue = doc.createTextNode(item->phoneNumbers().at(0)->uri());
        numberNode.appendChild(numberValue);
    }
    file.resize(0);
    file.write(doc.toByteArray());
    file.close();
    return true;
}

bool
WindowsContactEditor::remove(const Person *item)
{
    items_.removeOne(const_cast<Person*>(item));
    mediator()->removeItem(item);
    return true;
}

bool
WindowsContactEditor::edit(Person *item)
{
    Q_UNUSED(item)
    return false;
}

bool
WindowsContactEditor::addNew(Person *item)
{
    QDomDocument doc;
    QFile file(QStandardPaths::writableLocation
               (QStandardPaths::HomeLocation) + "/Contacts/"
               + item->formattedName()+".contact");
    if (file.exists())
        return false;
    if (!file.open(QIODevice::ReadWrite)) {
        file.close();
        qDebug() << "Cannot create contact file";
        return false;
    }
    doc.appendChild(
                doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"utf-8\""));

    //Create root
    auto root = doc.createElement("c:contact");
    root.setAttribute("c:Version", "1");
    root.setAttribute("xmlns:c", "http://schemas.microsoft.com/Contact");
    root.setAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
    root.setAttribute("xmlns:MSP2P","http://schemas.microsoft.com/Contact/Extended/MSP2P");
    doc.appendChild(root);

    auto date = Utils::GetISODate();

    //Create creation date
    auto creationDateNode = doc.createElement("c:CreationDate");
    auto creationDateValue = doc.createTextNode(date);
    creationDateNode.appendChild(creationDateValue);
    root.appendChild(creationDateNode);

    //Create extended node
    auto extendedNode = doc.createElement("c:Extended");
    extendedNode.setAttribute("xsi:nil", "true");
    root.appendChild(extendedNode);

    //Create contactID collection
    auto contactIDCol = doc.createElement("c:ContactIDCollection");
    root.appendChild(contactIDCol);
    auto contactID = doc.createElement("c:ContactID");
    contactID.setAttribute("c:ElementID", Utils::GenGUID());
    auto contactValue = doc.createElement("c:Value");
    auto value = doc.createTextNode(item->uid());
    contactValue.appendChild(value);
    contactID.appendChild(contactValue);
    contactIDCol.appendChild(contactID);

    //Create NameCollection
    auto nameCollection = doc.createElement("c:NameCollection");
    root.appendChild(nameCollection);
    auto name = doc.createElement("c:Name");
    nameCollection.appendChild(name);
    name.setAttribute("c:ElementID", Utils::GenGUID());
    auto formattedName = doc.createElement("c:FormattedName");
    name.appendChild(formattedName);
    auto formattedNameValue = doc.createTextNode(item->formattedName());
    formattedName.appendChild(formattedNameValue);

    //Create PhoneNumberCollection
    auto phoneNumberCollection = doc.createElement("c:PhoneNumberCollection");
    root.appendChild(phoneNumberCollection);
    auto phoneNumber = doc.createElement("c:PhoneNumber");
    phoneNumberCollection.appendChild(phoneNumber);
    phoneNumber.setAttribute("c:ElementID", Utils::GenGUID());
    auto numberNode = doc.createElement("c:Number");
    phoneNumber.appendChild(numberNode);
    auto numberValue = doc.createTextNode(item->phoneNumbers().at(0)->uri());
    numberNode.appendChild(numberValue);

    //Write to file
    file.write(doc.toByteArray());
    file.close();

    return true;
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
    , mediator_(mediator), watcher_(new QFileSystemWatcher())
{

}

WindowsContactBackend::~WindowsContactBackend()
{
    delete watcher_;
}

bool
WindowsContactBackend::load()
{
    QtConcurrent::run(this, &WindowsContactBackend::loadRun);
    watcher_->addPath(QStandardPaths::writableLocation
                      (QStandardPaths::HomeLocation) + "/Contacts");

    QObject::connect(watcher_, &QFileSystemWatcher::directoryChanged, [=](QString path) {
        Q_UNUSED(path)
        QtConcurrent::run(this, &WindowsContactBackend::loadRun);
    });
    return true;
}

bool
WindowsContactBackend::loadRun()
{
    QDir contactDir(QStandardPaths::writableLocation
                    (QStandardPaths::HomeLocation) + "/Contacts");
    QStringList filesList = contactDir.entryList(QStringList("*.contact"));
    auto ret = true;
    for(auto contactFileName : filesList) {
        if (not getPersonFromContactFile(contactDir, contactFileName))
            ret = false;
    }
    return ret;
}

bool
WindowsContactBackend::getPersonFromContactFile(const QDir& contactDir,
                                                const QString &contactFileName)
{
    QString contactFilePath
            (contactDir.absolutePath() + "/" +  contactFileName);
    QFile contactFile(contactFilePath);
    if (contactFile.open(QIODevice::ReadOnly)) {
        QXmlStreamReader reader;
        Person* p = new Person(this);
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
                    QString number = reader.readElementText();
                    if (not number.isEmpty()) {
                        ContactMethod *contact =
                                PhoneDirectoryModel::instance().getNumber(number,p);
                        contactMethod.append(contact);
                    }
                } else if (name == "ContactID") {
                    while (reader.name().toString() != "Value")
                        reader.readNext();
                    p->setUid(reader.readElementText().toUtf8());
                }
                else if (name == "Photo") {
                    //FIXME: It seems to be possible to have multiple photo...
                    reader.readNext();
                    if (reader.name().toString() == "Url") {
                        QString photoValue = reader.readElementText();
                        QImage photo;
                        photo.load(photoValue);
                        if (not photo.isNull())
                            p->setPhoto(Utils::getCirclePhoto(photo, sizePhoto_));
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
            contactFile.close();
            return false;
        } else {
            Person* existing = PersonModel::instance().getPersonByUid(p->uid());
            if (existing) {
                if (contactMethod.size() > 0)
                    existing->setContactMethods ( contactMethod   );
                existing->setNickName       ( p->nickName()       );
                existing->setFirstName      ( p->firstName()      );
                existing->setFamilyName     ( p->secondName()     );
                existing->setFormattedName  ( p->formattedName()  );
                existing->setOrganization   ( p->organization()   );
                existing->setPreferredEmail ( p->preferredEmail() );
                existing->setGroup          ( p->group()          );
                existing->setDepartment     ( p->department()     );
                existing->setPhoto          ( p->photo()          );
                delete p;
            } else {
                p->setContactMethods(contactMethod);
                editor<Person>()->addExisting(p);
            }
            return true;
        }
    } else {
        qDebug() << "Error Opening contact file : " << contactFileName;
        return false;
    }
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
                CollectionInterface::SupportedFeatures::LOAD |
                CollectionInterface::SupportedFeatures::SAVE |
                CollectionInterface::SupportedFeatures::REMOVE |
                CollectionInterface::SupportedFeatures::ADD);
}

