/*
This file is part of FavoriteGram, a custom fork of Telegram Desktop.
*/
#include "favoritegram/favoritegram_store.h"

#include "data/data_peer.h"
#include "data/data_session.h"
#include "favoritegram/favoritegram_settings.h"
#include "history/history.h"
#include "history/history_item.h"
#include "main/main_session.h"

#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

namespace FavoriteGram {

MessageStore &MessageStore::Instance() {
	static MessageStore instance;
	return instance;
}

void MessageStore::markDeleted(not_null<HistoryItem*> item) {
	const auto fullId = item->fullId();
	if (!_deletedIds.emplace(fullId).second) {
		return;
	}
	appendHistoryLog(u"deleted"_q, item);
	item->history()->session().data().requestItemViewRefresh(item);
}

bool MessageStore::isDeleted(FullMsgId id) const {
	return _deletedIds.contains(id);
}

void MessageStore::recordEdited(not_null<HistoryItem*> item) {
	if (!Settings::Instance().saveEditedHistory()) {
		return;
	}
	appendHistoryLog(u"edited"_q, item);
}

int MessageStore::deletedCount() const {
	return int(_deletedIds.size());
}

void MessageStore::clearDeletedCache() {
	_deletedIds.clear();
}

void MessageStore::appendHistoryLog(
		const QString &eventType,
		not_null<HistoryItem*> item) const {
	const auto path = Settings::Instance().historyLogPath();
	QDir().mkpath(QFileInfo(path).absolutePath());

	QJsonObject entry;
	entry.insert(u"event"_q, eventType);
	entry.insert(u"loggedAt"_q, QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
	entry.insert(u"peerId"_q, QString::number(item->history()->peer->id.value));
	entry.insert(u"peerName"_q, item->history()->peer->name());
	entry.insert(u"msgId"_q, qint64(item->id.bare));
	entry.insert(u"fromId"_q, QString::number(item->from()->id.value));
	entry.insert(u"fromName"_q, item->from()->name());
	entry.insert(u"msgDate"_q, qint64(item->date()));
	entry.insert(u"text"_q, item->originalText().text);

	QFile file(path);
	if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
		file.write(QJsonDocument(entry).toJson(QJsonDocument::Compact));
		file.write("\n");
	}
}

} // namespace FavoriteGram
