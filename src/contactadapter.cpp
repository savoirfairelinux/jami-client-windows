#include "contactadapter.h"

#include "lrcinstance.h"

ContactAdapter::ContactAdapter(QObject *parent)
    : QmlAdapterBase(parent)
{}

ContactAdapter::~ContactAdapter() {}

QVariant
ContactAdapter::getContactSelectableModel(int type)
{
    listModeltype_ = fromQMLContactPickerType(type);
    selectableProxyModel_.reset(new SelectableProxyModel(nullptr));
    smartListModel_.reset(new SmartListModel(LRCInstance::getCurrAccId(),
                                             this,
                                             listModeltype_,
                                             LRCInstance::getCurrentConvUid()));
    selectableProxyModel_->setSourceModel(smartListModel_.get());
    /*
     * Adjust filter.
     */
    switch (listModeltype_) {
    case SmartListModel::Type::CONFERENCE:
        selectableProxyModel_->setPredicate(
            [this](const QModelIndex &, const QRegExp &) { return true; });
        break;
    case SmartListModel::Type::TRANSFER:
        selectableProxyModel_->setPredicate([this](const QModelIndex &index, const QRegExp &regexp) {
            // Regex to remove current callee
            QRegExp matchExcept = QRegExp(QString("\\b(?!" + CalleeDisplayName_ + "\\b)\\w+"));
            bool match = false;
            bool match_non_self = matchExcept.indexIn(index.data(Qt::DisplayRole).toString()) != -1;
            if (match_non_self) {
                match = regexp.indexIn(index.data(Qt::DisplayRole).toString()) != -1;
            }
            return match && !index.parent().isValid();
        });
        break;
    default:
        break;
    }
    selectableProxyModel_->invalidate();

    return QVariant::fromValue(selectableProxyModel_.get());
}

void
ContactAdapter::setSearchFilter(const QString &filter)
{
    if (listModeltype_ == SmartListModel::Type::CONFERENCE) {
        smartListModel_->setConferenceableFilter(filter);
    }
    selectableProxyModel_->setFilterRegExp(
        QRegExp(filter, Qt::CaseInsensitive, QRegExp::FixedString));
}

void
ContactAdapter::contactSelected(int index)
{
    auto contactIndex = selectableProxyModel_->index(index, 0);
    auto callModel = LRCInstance::getCurrentCallModel();
    auto conversation = LRCInstance::getCurrentConversation();

    if (contactIndex.isValid()) {
        switch (listModeltype_) {
        case SmartListModel::Type::CONFERENCE: {
            /*
             * Conference.
             */
            auto sectionName = contactIndex.data(SmartListModel::Role::SectionName).value<QString>();
            if (!sectionName.isEmpty()) {
                smartListModel_->toggleSection(sectionName);
                return;
            }

            auto convUid = contactIndex.data(SmartListModel::Role::UID).value<QString>();
            auto accId = contactIndex.data(SmartListModel::Role::AccountId).value<QString>();
            auto callId = LRCInstance::getCallIdForConversationUid(convUid, accId);

            if (!callId.isEmpty()) {
                if (conversation.uid.isEmpty()) {
                    return;
                }
                auto thisCallId = conversation.confId.isEmpty() ? conversation.callId
                                                                : conversation.confId;

                callModel->joinCalls(thisCallId, callId);
            } else {
                auto contactUri = contactIndex.data(SmartListModel::Role::URI).value<QString>();
                auto call = LRCInstance::getCallInfoForConversation(conversation);
                if (!call) {
                    return;
                }
                callModel->callAndAddParticipant(contactUri, call->id, call->isAudioOnly);
            }
        } break;
        case SmartListModel::Type::TRANSFER: {
            /*
             * SIP Transfer.
             */
            auto contactUri = contactIndex.data(SmartListModel::Role::URI).value<QString>();

            if (conversation.uid.isEmpty()) {
                return;
            }
            auto callId = conversation.confId.isEmpty() ? conversation.callId : conversation.confId;

            QString destCallId;

            try {
                /*
                 * Check if the call exist - (check non-finished calls).
                 */
                auto callInfo = callModel->getCallFromURI(contactUri, true);
                destCallId = callInfo.id;
            } catch (std::exception &e) {
                qDebug().noquote() << e.what();
                destCallId = "";
            }
            /*
             * If no second call -> blind transfer.
             * If there is a second call -> attended transfer.
             */
            if (destCallId.size() == 0) {
                callModel->transfer(callId, "sip:" + contactUri);
                callModel->hangUp(callId);
            } else {
                callModel->transferToCall(callId, destCallId);
                callModel->hangUp(callId);
                callModel->hangUp(destCallId);
            }
        } break;
        default:
            break;
        }
    }
}

void
ContactAdapter::initQmlObject()
{}

SmartListModel::Type
ContactAdapter::fromQMLContactPickerType(int type)
{
    /*
     * Convert from enum in qml.
     */
    switch (type) {
    case 10:
        return SmartListModel::Type::CONFERENCE;
    case 11:
        return SmartListModel::Type::TRANSFER;
    default:
        break;
    }

    return SmartListModel::Type::CONFERENCE;
}
