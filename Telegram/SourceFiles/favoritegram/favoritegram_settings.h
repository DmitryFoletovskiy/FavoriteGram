/*
This file is part of FavoriteGram, a custom fork of Telegram Desktop.
*/
#pragma once

#include <QtCore/QString>

namespace FavoriteGram {

class Settings final {
public:
	static Settings &Instance();

	void load();
	void save() const;

	// Ghost Mode (Privacy)
	[[nodiscard]] bool ghostModeEnabled() const { return _ghostModeEnabled; }
	void setGhostModeEnabled(bool value);

	[[nodiscard]] bool dontSendReadMessages() const { return _dontSendReadMessages; }
	void setDontSendReadMessages(bool value);

	[[nodiscard]] bool dontSendTyping() const { return _dontSendTyping; }
	void setDontSendTyping(bool value);

	[[nodiscard]] bool dontSendOnline() const { return _dontSendOnline; }
	void setDontSendOnline(bool value);

	// Convenience helpers combining master Ghost Mode toggle
	[[nodiscard]] bool shouldBlockRead() const {
		return _ghostModeEnabled && _dontSendReadMessages;
	}
	[[nodiscard]] bool shouldBlockTyping() const {
		return _ghostModeEnabled && _dontSendTyping;
	}
	[[nodiscard]] bool shouldBlockOnline() const {
		return _ghostModeEnabled && _dontSendOnline;
	}

	// Anti-Delete & Message History (AyuGram-style)
	[[nodiscard]] bool saveDeletedMessages() const { return _saveDeletedMessages; }
	void setSaveDeletedMessages(bool value);

	[[nodiscard]] bool saveEditedHistory() const { return _saveEditedHistory; }
	void setSaveEditedHistory(bool value);

	[[nodiscard]] QString deletedMark() const { return _deletedMark; }
	void setDeletedMark(const QString &value);

	// UI Customization
	[[nodiscard]] bool showSecondsInTime() const { return _showSecondsInTime; }
	void setShowSecondsInTime(bool value);

	[[nodiscard]] bool disableAds() const { return _disableAds; }
	void setDisableAds(bool value);

	[[nodiscard]] bool hideStories() const { return _hideStories; }
	void setHideStories(bool value);

	// Liquid Glass (Windows 11 DWM Acrylic / Mica & Opacity)
	// 0 = Off, 1 = Acrylic (Liquid Glass), 2 = Mica, 3 = Mica Alt
	[[nodiscard]] int glassBackdropMode() const { return _glassBackdropMode; }
	void setGlassBackdropMode(int value);

	[[nodiscard]] int windowOpacityPercent() const { return _windowOpacityPercent; }
	void setWindowOpacityPercent(int value);

	[[nodiscard]] bool roundedWindowCorners() const { return _roundedWindowCorners; }
	void setRoundedWindowCorners(bool value);

	[[nodiscard]] QString settingsFilePath() const;
	[[nodiscard]] QString historyLogPath() const;

	[[nodiscard]] rpl::producer<> changes() const {
		return _changes.events();
	}

private:
	Settings();

	void ensureLoaded() const;

	mutable bool _loaded = false;

	bool _ghostModeEnabled = false;
	bool _dontSendReadMessages = true;
	bool _dontSendTyping = true;
	bool _dontSendOnline = true;

	bool _saveDeletedMessages = true;
	bool _saveEditedHistory = true;
	QString _deletedMark = u"удалено"_q;

	bool _showSecondsInTime = false;
	bool _disableAds = true;
	bool _hideStories = false;

	int _glassBackdropMode = 1;
	int _windowOpacityPercent = 94;
	bool _roundedWindowCorners = true;

	rpl::event_stream<> _changes;
};

} // namespace FavoriteGram
