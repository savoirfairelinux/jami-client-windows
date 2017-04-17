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

#pragma once

#include <QObject>
#include <QStandardPaths>
#include <QDir>
#include <QXmlStreamReader>
#include <QtConcurrent/QtConcurrent>
#include <QImage>
#include <QFileSystemWatcher>

#include "person.h"
#include "collectioninterface.h"
#include "contactmethod.h"
#include "phonedirectorymodel.h"

class WindowsContactBackend : public CollectionInterface
{
public:
    explicit WindowsContactBackend(CollectionMediator<Person>* mediator, CollectionInterface* parent = nullptr);
    virtual ~WindowsContactBackend();

    virtual bool load() override;
    virtual bool reload() override;
    virtual bool clear() override;
    virtual QString    name     () const override;
    virtual QString    category () const override;
    virtual bool       isEnabled() const override;
    virtual QByteArray id       () const override;
    virtual FlagPack<CollectionInterface::SupportedFeatures>  supportedFeatures() const override;

private:
    bool loadRun();
private:
    CollectionMediator<Person>*  mediator_;
    constexpr static int sizePhoto_ = 50;
    QFileSystemWatcher* watcher_;

private:
    bool getPersonFromContactFile(const QDir &contactDir, const QString& contactFileName);
};

class WindowsContactEditor : public CollectionEditor<Person>
{
public:
    WindowsContactEditor(CollectionMediator<Person>* m, WindowsContactBackend* parent);
    ~WindowsContactEditor();
    virtual bool save       ( const Person* item ) override;
    virtual bool remove     ( const Person* item ) override;
    virtual bool edit       ( Person*       item ) override;
    virtual bool addNew     ( Person* item       ) override;
    virtual bool addExisting( const Person* item ) override;

private:
    virtual QVector<Person*> items() const override;

    QVector<Person*> items_;
    WindowsContactBackend* collection_;
};

