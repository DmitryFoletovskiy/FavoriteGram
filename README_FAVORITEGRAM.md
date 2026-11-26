# FavoriteGram

Кастомный форк **Telegram Desktop (C++ / Qt)** со встроенными функциями приватности, анти-удаления сообщений (в стиле AyuGram) и кастомизации интерфейса.

## Что добавлено в FavoriteGram

Весь новый код изолирован в `Telegram/SourceFiles/favoritegram/`, с минимальными точками интеграции в основной клиент:

- **Анти-удаление и история (в стиле AyuGram)**:
  - Сохранение удалённых сообщений в чатах и каналах (сообщения не исчезают и получают пометку `удалено` рядом со временем).
  - Автоматическое логирование удалённых и отредактированных сообщений в `tdata/favoritegram_history.jsonl`.
  - Кнопка очистки кэша меток и быстрого открытия папки с логами.
- **Режим призрака (Ghost Mode)**:
  - Не отправлять статус прочтения (`messages.readHistory` / `channels.readHistory`).
  - Скрыть статус «печатает...» и статусы отправки медиа (`messages.setTyping`).
  - Всегда показывать статус «не в сети» (`account.updateStatus`).
- **Кастомизация интерфейса**:
  - Отображение секунд во времени сообщений (`ЧЧ:ММ:СС`).
  - Отключение рекламных сообщений (Sponsored Messages).
  - Собственный раздел **Настройки → FavoriteGram**, все настройки сохраняются в `tdata/favoritegram_settings.json`.

---

## Как собрать `.exe` через GitHub Actions

1. Создай новый репозиторий на GitHub (например, `FavoriteGram`) и загрузи туда этот проект:
   ```bash
   git add .
   git commit -m "Initial FavoriteGram fork"
   git remote set-url origin https://github.com/<ТВОЙ_ЛОГИН>/FavoriteGram.git
   git push -u origin HEAD:main
   ```
2. Получи API-ключи Telegram на [my.telegram.org](https://my.telegram.org) (**API development tools** → `api_id` и `api_hash`).
3. В репозитории на GitHub открой **Settings → Secrets and variables → Actions → New repository secret** и добавь два секрета:
   - `TDESKTOP_API_ID` — твой `api_id`
   - `TDESKTOP_API_HASH` — твой `api_hash`
4. Перейди во вкладку **Actions** → выбери workflow **FavoriteGram Windows x64 Build** → нажми **Run workflow**.
5. После завершения сборки скачай архив `FavoriteGram-win64-portable.zip` из раздела **Artifacts** (или создай тег `v1.0.0`, чтобы архив автоматически прикрепился к GitHub Release).
