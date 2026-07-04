/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include <QtCore/QString>

#include <array>

inline bool IsBlockWordMessage(const QString &text) {
	static const auto kBlockWords = std::array{
		QString::fromUtf8("zincentimeter"),
	};
	for (const auto &word : kBlockWords) {
		if (text.contains(word, Qt::CaseInsensitive)) {
			return true;
		}
	}
	return false;
}
