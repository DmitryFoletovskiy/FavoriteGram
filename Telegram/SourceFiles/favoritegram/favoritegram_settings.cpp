/*
This file is part of FavoriteGram, a custom fork of Telegram Desktop.
*/
#include "favoritegram/favoritegram_settings.h"

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

namespace FavoriteGram {
namespace {

QString ResolveTdataDir() {
	const auto base = cWorkingDir();
	if (!base.isEmpty()) {
		return base + u"tdata/"_q;
	}
	return QDir::currentPath() + u"/tdata/"_q;
}

} // namespace

Settings &Settings::Instance() {
	static Settings instance;
	instance.ensureLoaded();
	return instance;
}

Settings::Settings() = default;

void Settings::ensureLoaded() const {
	if (!_loaded && !cWorkingDir().isEmpty()) {
		const_cast<Settings*>(this)->load();
	}
}

QString Settings::settingsFilePath() const {
	return ResolveTdataDir() + u"favoritegram_settings.json"_q;
}

QString Settings::historyLogPath() const {
	return ResolveTdataDir() + u"favoritegram_history.jsonl"_q;
}

void Settings::load() {
	_loaded = true;
	QFile file(settingsFilePath());
	if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
		return;
	}
	const auto doc = QJsonDocument::fromJson(file.readAll());
	if (!doc.isObject()) {
		return;
	}
	const auto obj = doc.object();
	_ghostModeEnabled = obj.value(u"ghostModeEnabled"_q).toBool(_ghostModeEnabled);
	_dontSendReadMessages = obj.value(u"dontSendReadMessages"_q).toBool(_dontSendReadMessages);
	_dontSendTyping = obj.value(u"dontSendTyping"_q).toBool(_dontSendTyping);
	_dontSendOnline = obj.value(u"dontSendOnline"_q).toBool(_dontSendOnline);

	_saveDeletedMessages = obj.value(u"saveDeletedMessages"_q).toBool(_saveDeletedMessages);
	_saveEditedHistory = obj.value(u"saveEditedHistory"_q).toBool(_saveEditedHistory);
	const auto mark = obj.value(u"deletedMark"_q).toString();
	if (!mark.isEmpty()) {
		_deletedMark = mark;
	}

	_showSecondsInTime = obj.value(u"showSecondsInTime"_q).toBool(_showSecondsInTime);
	_disableAds = obj.value(u"disableAds"_q).toBool(_disableAds);
	_hideStories = obj.value(u"hideStories"_q).toBool(_hideStories);

	_glassBackdropMode = obj.value(u"glassBackdropMode"_q).toInt(_glassBackdropMode);
	_windowOpacityPercent = obj.value(u"windowOpacityPercent"_q).toInt(_windowOpacityPercent);
	_roundedWindowCorners = obj.value(u"roundedWindowCorners"_q).toBool(_roundedWindowCorners);
}

void Settings::save() const {
	const auto dirPath = ResolveTdataDir();
	QDir().mkpath(dirPath);

	QJsonObject obj;
	obj.insert(u"ghostModeEnabled"_q, _ghostModeEnabled);
	obj.insert(u"dontSendReadMessages"_q, _dontSendReadMessages);
	obj.insert(u"dontSendTyping"_q, _dontSendTyping);
	obj.insert(u"dontSendOnline"_q, _dontSendOnline);

	obj.insert(u"saveDeletedMessages"_q, _saveDeletedMessages);
	obj.insert(u"saveEditedHistory"_q, _saveEditedHistory);
	obj.insert(u"deletedMark"_q, _deletedMark);

	obj.insert(u"showSecondsInTime"_q, _showSecondsInTime);
	obj.insert(u"disableAds"_q, _disableAds);
	obj.insert(u"hideStories"_q, _hideStories);

	obj.insert(u"glassBackdropMode"_q, _glassBackdropMode);
	obj.insert(u"windowOpacityPercent"_q, _windowOpacityPercent);
	obj.insert(u"roundedWindowCorners"_q, _roundedWindowCorners);

	QFile file(settingsFilePath());
	if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		file.write(QJsonDocument(obj).toJson(QJsonDocument::Indented));
	}
}

void Settings::setGhostModeEnabled(bool value) {
	if (_ghostModeEnabled != value) {
		_ghostModeEnabled = value;
		save();
		_changes.fire({});
	}
}

void Settings::setDontSendReadMessages(bool value) {
	if (_dontSendReadMessages != value) {
		_dontSendReadMessages = value;
		save();
		_changes.fire({});
	}
}

void Settings::setDontSendTyping(bool value) {
	if (_dontSendTyping != value) {
		_dontSendTyping = value;
		save();
		_changes.fire({});
	}
}

void Settings::setDontSendOnline(bool value) {
	if (_dontSendOnline != value) {
		_dontSendOnline = value;
		save();
		_changes.fire({});
	}
}

void Settings::setSaveDeletedMessages(bool value) {
	if (_saveDeletedMessages != value) {
		_saveDeletedMessages = value;
		save();
		_changes.fire({});
	}
}

void Settings::setSaveEditedHistory(bool value) {
	if (_saveEditedHistory != value) {
		_saveEditedHistory = value;
		save();
		_changes.fire({});
	}
}

void Settings::setDeletedMark(const QString &value) {
	if (!value.isEmpty() && _deletedMark != value) {
		_deletedMark = value;
		save();
		_changes.fire({});
	}
}

void Settings::setShowSecondsInTime(bool value) {
	if (_showSecondsInTime != value) {
		_showSecondsInTime = value;
		save();
		_changes.fire({});
	}
}

void Settings::setDisableAds(bool value) {
	if (_disableAds != value) {
		_disableAds = value;
		save();
		_changes.fire({});
	}
}

void Settings::setHideStories(bool value) {
	if (_hideStories != value) {
		_hideStories = value;
		save();
		_changes.fire({});
	}
}

void Settings::setGlassBackdropMode(int value) {
	const auto clamped = std::clamp(value, 0, 3);
	if (_glassBackdropMode != clamped) {
		_glassBackdropMode = clamped;
		save();
		_changes.fire({});
	}
}

void Settings::setWindowOpacityPercent(int value) {
	const auto clamped = std::clamp(value, 75, 100);
	if (_windowOpacityPercent != clamped) {
		_windowOpacityPercent = clamped;
		save();
		_changes.fire({});
	}
}

void Settings::setRoundedWindowCorners(bool value) {
	if (_roundedWindowCorners != value) {
		_roundedWindowCorners = value;
		save();
		_changes.fire({});
	}
}

} // namespace FavoriteGram
