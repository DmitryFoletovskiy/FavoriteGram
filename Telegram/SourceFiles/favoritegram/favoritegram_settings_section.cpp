/*
This file is part of FavoriteGram, a custom fork of Telegram Desktop.
*/
#include "favoritegram/favoritegram_settings_section.h"

#include "favoritegram/favoritegram_settings.h"
#include "favoritegram/favoritegram_store.h"
#include "settings/settings_common.h"
#include "settings/settings_common_session.h"
#include "ui/toast/toast.h"
#include "ui/ui_utility.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/wrap/vertical_layout.h"
#include "window/main_window.h"
#include "window/window_controller.h"
#include "window/window_session_controller.h"

#include "styles/style_menu_icons.h"
#include "styles/style_settings.h"

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QUrl>
#include <QtGui/QDesktopServices>

namespace Settings {
namespace {

class FavoriteGramSection : public Section<FavoriteGramSection> {
public:
	FavoriteGramSection(
		QWidget *parent,
		not_null<Window::SessionController*> controller);

	[[nodiscard]] rpl::producer<QString> title() override;

private:
	void setupContent();
};

FavoriteGramSection::FavoriteGramSection(
	QWidget *parent,
	not_null<Window::SessionController*> controller)
: Section(parent, controller) {
	setupContent();
}

rpl::producer<QString> FavoriteGramSection::title() {
	return rpl::single(u"FavoriteGram"_q);
}

void FavoriteGramSection::setupContent() {
	const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);
	auto &fg = FavoriteGram::Settings::Instance();

	const auto addToggle = [&](
			const QString &label,
			bool initial,
			Fn<void(bool)> onChanged) {
		const auto button = AddButtonWithIcon(
			content,
			rpl::single(label),
			st::settingsButtonNoIcon);
		button->toggleOn(rpl::single(initial));
		button->toggledChanges(
		) | rpl::on_next([=](bool checked) {
			onChanged(checked);
		}, button->lifetime());
		return button;
	};

	// --- Ghost Mode ---
	Ui::AddSkip(content);
	Ui::AddSubsectionTitle(content, rpl::single(u"Режим призрака (Ghost Mode)"_q));

	addToggle(
		u"Включить режим призрака"_q,
		fg.ghostModeEnabled(),
		[](bool v) { FavoriteGram::Settings::Instance().setGhostModeEnabled(v); });

	addToggle(
		u"Не отправлять статус прочтения"_q,
		fg.dontSendReadMessages(),
		[](bool v) { FavoriteGram::Settings::Instance().setDontSendReadMessages(v); });

	addToggle(
		u"Скрыть статус «печатает...»"_q,
		fg.dontSendTyping(),
		[](bool v) { FavoriteGram::Settings::Instance().setDontSendTyping(v); });

	addToggle(
		u"Всегда показывать «не в сети»"_q,
		fg.dontSendOnline(),
		[](bool v) { FavoriteGram::Settings::Instance().setDontSendOnline(v); });

	Ui::AddSkip(content);
	Ui::AddDividerText(
		content,
		rpl::single(u"Когда режим призрака включён, выбранные выше действия не отправляются на сервер Telegram."_q));

	// --- Anti-Delete & History ---
	Ui::AddSkip(content);
	Ui::AddSubsectionTitle(content, rpl::single(u"Анти-удаление и история (AyuGram)"_q));

	addToggle(
		u"Сохранять удалённые сообщения в чатах"_q,
		fg.saveDeletedMessages(),
		[](bool v) { FavoriteGram::Settings::Instance().setSaveDeletedMessages(v); });

	addToggle(
		u"Записывать историю изменений в лог"_q,
		fg.saveEditedHistory(),
		[](bool v) { FavoriteGram::Settings::Instance().setSaveEditedHistory(v); });

	const auto openLogsBtn = AddButtonWithIcon(
		content,
		rpl::single(u"Открыть папку с логами истории"_q),
		st::settingsButtonNoIcon);
	openLogsBtn->setClickedCallback([=] {
		const auto logPath = FavoriteGram::Settings::Instance().historyLogPath();
		const auto dirPath = QFileInfo(logPath).absolutePath();
		QDir().mkpath(dirPath);
		QDesktopServices::openUrl(QUrl::fromLocalFile(dirPath));
	});

	const auto clearCacheBtn = AddButtonWithIcon(
		content,
		rpl::single(u"Очистить кэш удалённых меток"_q),
		st::settingsButtonNoIcon);
	clearCacheBtn->setClickedCallback([=] {
		const auto count = FavoriteGram::MessageStore::Instance().deletedCount();
		FavoriteGram::MessageStore::Instance().clearDeletedCache();
		controller()->window().showToast(
			u"Очищено меток удалённых сообщений: %1"_q.arg(count));
	});

	Ui::AddSkip(content);
	Ui::AddDividerText(
		content,
		rpl::single(u"Удалённые собеседником сообщения остаются видимыми с пометкой «удалено» и сохраняются в favoritegram_history.jsonl."_q));

	// --- UI Customization ---
	Ui::AddSkip(content);
	Ui::AddSubsectionTitle(content, rpl::single(u"Кастомизация интерфейса"_q));

	addToggle(
		u"Показывать секунды во времени сообщений"_q,
		fg.showSecondsInTime(),
		[](bool v) { FavoriteGram::Settings::Instance().setShowSecondsInTime(v); });

	addToggle(
		u"Отключить рекламные сообщения (Sponsored)"_q,
		fg.disableAds(),
		[](bool v) { FavoriteGram::Settings::Instance().setDisableAds(v); });

	// --- Liquid Glass (Windows 11 DWM) ---
	Ui::AddSkip(content);
	Ui::AddSubsectionTitle(content, rpl::single(u"Жидкое стекло (Windows 11 DWM)"_q));

	const auto backdropLabel = [](int mode) -> QString {
		switch (mode) {
		case 1: return u"Acrylic (Жидкое стекло)"_q;
		case 2: return u"Mica"_q;
		case 3: return u"Mica Alt"_q;
		default: return u"Выкл"_q;
		}
	};
	const auto glassModeBtn = AddButtonWithIcon(
		content,
		rpl::single(u"Эффект стекла окна: "_q + backdropLabel(fg.glassBackdropMode())),
		st::settingsButtonNoIcon);
	glassModeBtn->setClickedCallback([=] {
		auto &s = FavoriteGram::Settings::Instance();
		const auto next = (s.glassBackdropMode() + 1) % 4;
		s.setGlassBackdropMode(next);
		controller()->window().showToast(
			u"Стекло окна: %1 (переключи тему или перезапусти окно для применения)"_q.arg(backdropLabel(next)));
	});

	const auto opacityBtn = AddButtonWithIcon(
		content,
		rpl::single(u"Прозрачность окна: %1%"_q.arg(fg.windowOpacityPercent())),
		st::settingsButtonNoIcon);
	opacityBtn->setClickedCallback([=] {
		auto &s = FavoriteGram::Settings::Instance();
		static constexpr int kSteps[] = { 100, 94, 90, 85, 80 };
		auto next = 94;
		for (auto i = 0; i < 5; ++i) {
			if (kSteps[i] == s.windowOpacityPercent()) {
				next = kSteps[(i + 1) % 5];
				break;
			}
		}
		s.setWindowOpacityPercent(next);
		controller()->window().widget()->setWindowOpacity(next / 100.0);
		controller()->window().showToast(u"Прозрачность окна: %1%"_q.arg(next));
	});

	addToggle(
		u"Скруглённые углы окна (Windows 11)"_q,
		fg.roundedWindowCorners(),
		[](bool v) { FavoriteGram::Settings::Instance().setRoundedWindowCorners(v); });

	Ui::AddSkip(content);
	Ui::AddDividerText(
		content,
		rpl::single(u"FavoriteGram — нативный C++/Qt форк Telegram Desktop с DWM Acrylic (жидкое стекло), режимом призрака и анти-удалением."_q));
	Ui::AddSkip(content);

	Ui::ResizeFitChild(this, content);
}

} // namespace

Type FavoriteGramId() {
	return FavoriteGramSection::Id();
}

} // namespace Settings
