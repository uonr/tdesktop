/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "history/history_item_edition.h"

#include "api/api_text_entities.h"
#include "data/data_session.h"
#include "data/data_user.h"
#include "history/history_block_words.h"
#include "iv/iv_rich_page.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"

HistoryMessageEdition::HistoryMessageEdition(
	not_null<Main::Session*> session,
	const MTPDmessage &message)
: suggest(HistoryMessageSuggestInfo(message.vsuggested_post())) {
	isEditHide = message.is_edit_hide();
	isMediaUnread = message.is_media_unread();
	repeatPeriod = message.vschedule_repeat_period().value_or_empty();
	editDate = message.vedit_date().value_or(-1);

	const auto peerId = message.vfrom_id()
		? peerFromMTP(*message.vfrom_id())
		: PeerId(0);
	const auto user = session->data().peerLoaded(peerId);
	const auto messageText = qs(message.vmessage());
	const auto hiddenByBlockedUser = GetEnhancedBool("blocked_user_spoiler_mode")
		&& (blockExist(peerId.value) || (user && user->isBlocked()));
	const auto hiddenByKeyword = IsBlockWordMessage(messageText);
	if (hiddenByBlockedUser || hiddenByKeyword) {
		const auto blkMsg = Lang::GetOriginalValue(
			tr::lng_blocked_user_hint.base);
		textWithEntities = TextWithEntities{
			blkMsg + messageText,
			Api::EntitiesFromMTP(
				session,
				message.ventities().value_or_empty(),
				blkMsg.length(),
				messageText.length())
		};
	} else {
		textWithEntities = TextWithEntities{
			messageText,
			Api::EntitiesFromMTP(
				session,
				message.ventities().value_or_empty())
		};
	}

	replyMarkup = HistoryMessageMarkupData(message.vreply_markup());
	mtpMedia = message.vmedia();
	mtpReactions = message.vreactions();
	mtpFactcheck = message.vfactcheck();
	if (const auto data = message.vrich_message()) {
		richPage = Iv::ParseRichPage(session, *data);
	}
	views = message.vviews().value_or(-1);
	forwards = message.vforwards().value_or(-1);
	if (const auto mtpReplies = message.vreplies()) {
		replies = HistoryMessageRepliesData(mtpReplies);
	}
	invertMedia = message.is_invert_media();
	if (const auto rank = message.vfrom_rank()) {
		fromRank = qs(*rank);
	}

	const auto period = message.vttl_period();
	ttl = (period && period->v > 0) ? (message.vdate().v + period->v) : 0;
}
