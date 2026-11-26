/*
This file is part of FavoriteGram, a custom fork of Telegram Desktop.
*/
#pragma once

#include "data/data_types.h"

class HistoryItem;

namespace FavoriteGram {

class MessageStore final {
public:
	static MessageStore &Instance();

	void markDeleted(not_null<HistoryItem*> item);
	[[nodiscard]] bool isDeleted(FullMsgId id) const;
	void recordEdited(not_null<HistoryItem*> item);

	[[nodiscard]] int deletedCount() const;
	void clearDeletedCache();

private:
	MessageStore() = default;

	void appendHistoryLog(
		const QString &eventType,
		not_null<HistoryItem*> item) const;

	base::flat_set<FullMsgId> _deletedIds;
};

} // namespace FavoriteGram
