/***************************************************************************
 * Copyright (C) 2017 by Savoir-faire Linux                                *
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

#include <QImage>

#include <memory>
#include <interfaces/pixmapmanipulatori.h>

Q_DECLARE_METATYPE(QImage);

class Person;

QByteArray QImageToByteArray(QImage image);

namespace Interfaces {

class PixbufManipulator : public PixmapManipulatorI {
public:
    QVariant callPhoto(Call* c, const QSize& size, bool displayPresence = true) override;
    QVariant callPhoto(const ContactMethod* n, const QSize& size, bool displayPresence = true) override;
    QVariant contactPhoto(Person* c, const QSize& size, bool displayPresence = true) override;
    QVariant personPhoto(const QByteArray& data, const QString& type = "PNG") override;

    /* TODO: the following methods return an empty QVariant/QByteArray */
    QVariant   numberCategoryIcon(const QVariant& p, const QSize& size, bool displayPresence = false, bool isPresent = false) override;
    QVariant   securityIssueIcon(const QModelIndex& index) override;
    QByteArray toByteArray(const QVariant& pxm) override;
    QVariant   collectionIcon(const CollectionInterface* interface, PixmapManipulatorI::CollectionIconHint hint = PixmapManipulatorI::CollectionIconHint::NONE) const override;
    QVariant   securityLevelIcon(const SecurityEvaluationModel::SecurityLevel level) const override;
    QVariant   historySortingCategoryIcon(const CategorizedHistoryModel::SortedProxy::Categories cat) const override;
    QVariant   contactSortingCategoryIcon(const CategorizedContactModel::SortedProxy::Categories cat) const override;
    QVariant   userActionIcon(const UserActionElement& state) const override;
    QVariant   decorationRole(const QModelIndex& index) override;
    QVariant   decorationRole(const Call* c) override;
    QVariant   decorationRole(const ContactMethod* cm) override;
    QVariant   decorationRole(const Person* p) override;
    QVariant   decorationRole(const Account* acc) override;

private:
    QImage scaleAndFrame(const QImage photo, const QSize& size);
    const QSize imgSize_ {48, 48};
    static QImage fallbackAvatar(const QSize size, const char color, const char letter);
};

} // namespace Interfaces

