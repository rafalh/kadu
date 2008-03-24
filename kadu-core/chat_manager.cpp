/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qcursor.h>
#include <qbitmap.h>
#include <qtooltip.h>
//Added by qt3to4:
#include <QList>

#include "action.h"
#include "debug.h"
#include "chat_manager.h"
#include "chat_widget.h"
#include "custom_input.h"
#include "icons_manager.h"
#include "ignore.h"
#include "kadu.h"
#include "kadu_splitter.h"
#include "message_box.h"
#include "misc.h"
#include "pending_msgs.h"
#include "search.h"
#include "protocols_manager.h"
#include "toolbar.h"
#include "userbox.h"

#include "activate.h"

ChatManager::ChatManager(QObject* parent, const char* name) : QObject(parent, name),
	ChatWidgets(), ClosedChatUsers(), addons(), refreshTitlesTimer()
{
	kdebugf();

	autoSendActionDescription = new ActionDescription(
		ActionDescription::TypeChat, "autoSendAction",
		this, SLOT(autoSendActionActivated(QWidget *, bool)),
		"AutoSendMessage", tr("%1 sends message").arg(config_file.readEntry("ShortCuts", "chat_newline")), true
	);

	clearChatActionDescription = new ActionDescription(
		ActionDescription::TypeChat, "clearChatAction",
		this, SLOT(clearActionActivated(QWidget *, bool)),
		"ClearChat", tr("Clear messages in chat window")
	);

	insertImageActionDescription = new ActionDescription(
		ActionDescription::TypeChat, "insertImageAction",
		this, SLOT(insertImageActionActivated(QWidget *, bool)),
		"ChooseImage", tr("Insert image")
	);

	boldActionDescription = new ActionDescription(
		ActionDescription::TypeChat, "boldAction",
		this, SLOT(boldActionActivated(QWidget *, bool)),
		"Bold", tr("Bold"), true
	);

	italicActionDescription = new ActionDescription(
		ActionDescription::TypeChat, "italicAction",
		this, SLOT(italicActionActivated(QWidget *, bool)),
		"Italic", tr("Italic"), true
	);

	underlineActionDescription = new ActionDescription(
		ActionDescription::TypeChat, "underlineAction",
		this, SLOT(underlineActionActivated(QWidget *, bool)),
		"Underline", tr("Underline"), true
	);

	sendActionDescription = new ActionDescription(
		ActionDescription::TypeChat, "sendAction",
		this, SLOT(sendActionActivated(QWidget *, bool)),
		"SendMessage", tr("&Send"), true
	);

	whoisActionDescription = new ActionDescription(
		ActionDescription::TypeChat, "whoisAction",
		this, SLOT(whoisActionActivated(QWidget *, bool)),
		"LookupUserInfo", tr("Search this user in directory")
	);

	ignoreUserActionDescription = new ActionDescription(
		ActionDescription::TypeUser, "ignoreUserAction",
		this, SLOT(ignoreUserActionActivated(QWidget *, bool)),
		"Ignore", tr("Ignore user")
	);

	blockUserActionDescription = new ActionDescription(
		ActionDescription::TypeUser, "blockUserAction",
		this, SLOT(blockUserActionActivated(QWidget *, bool)),
		"Blocking", tr("Block user")
	);

	chatActionDescription = new ActionDescription(
		ActionDescription::TypeUser, "chatAction",
		this, SLOT(chatActionActivated(QWidget *, bool)),
		"OpenChat", tr("&Chat")
	);

	openChatWithActionDescription = new ActionDescription(
		ActionDescription::TypeGlobal, "openChatWithAction",
		kadu, SLOT(openChatWith()),
		"OpenChat", tr("Open chat with...")
	);


/*
	Action* insert_emot_action = new Action("ChooseEmoticon", tr("Insert emoticon"),
		"insertEmoticonAction", Action::TypeChat);
	connect(insert_emot_action, SIGNAL(activated(const UserGroup*, const QWidget*, bool)),
		this, SLOT(insertEmoticonActionActivated(const UserGroup*, const QWidget*)));
	connect(insert_emot_action, SIGNAL(addedToToolbar(const UserGroup*, ToolButton*, ToolBar*)),
		this, SLOT(insertEmoticonActionAddedToToolbar(const UserGroup*, ToolButton*, ToolBar*)));


	Action* color_action = new Action("Black", tr("Change color"), "colorAction", Action::TypeChat);
	connect(color_action, SIGNAL(activated(const UserGroup*, const QWidget*, bool)),
		this, SLOT(colorActionActivated(const UserGroup*, const QWidget*)));

*/

	ToolBar::addDefaultAction("Chat toolbar 1", "autoSendAction");
	ToolBar::addDefaultAction("Chat toolbar 1", "clearChatAction");
	ToolBar::addDefaultAction("Chat toolbar 1", "insertEmoticonAction");
	ToolBar::addDefaultAction("Chat toolbar 1", "whoisAction");
	ToolBar::addDefaultAction("Chat toolbar 1", "insertImageAction");

	ToolBar::addDefaultAction("Chat toolbar 2", "boldAction");
	ToolBar::addDefaultAction("Chat toolbar 2", "italicAction");
	ToolBar::addDefaultAction("Chat toolbar 2", "underlineAction");
	ToolBar::addDefaultAction("Chat toolbar 2", "colorAction");

	ToolBar::addDefaultAction("Chat toolbar 3", "sendAction", -1, true);

	if (config_file.readBoolEntry("Chat", "RememberPosition"))
	{
		userlist->addPerContactNonProtocolConfigEntry("chat_geometry", "ChatGeometry");
		userlist->addPerContactNonProtocolConfigEntry("chat_vertical_sizes", "VerticalSizes");
	}
	connect(&refreshTitlesTimer, SIGNAL(timeout()), this, SLOT(refreshTitles()));
	connect(userlist, SIGNAL(usersStatusChanged(QString)), this, SLOT(refreshTitlesLater()));

	kdebugf2();
}

void ChatManager::closeAllWindows()
{
	kdebugf();

	while (!ChatWidgets.empty())
	{
		ChatWidget *chat=ChatWidgets.first();
		delete chat;
	}

	kdebugf2();
}

void ChatManager::loadOpenedWindows()
{
	kdebugf();
	QDomElement root_elem = xml_config_file->rootElement();
	QDomElement chats_elem = xml_config_file->findElement(root_elem, "ChatWindows");
	if (!chats_elem.isNull())
	{
		for (QDomNode win = chats_elem.firstChild(); !win.isNull(); win = win.nextSibling())
		{
			const QDomElement &window_elem = win.toElement();
			if (window_elem.isNull())
				continue;
			if (window_elem.tagName() != "Window")
				continue;
			QString protocolId = window_elem.attribute("protocol");
			QString accountId = window_elem.attribute("id");
			UserListElements users;
			for (QDomNode contact = window_elem.firstChild(); !contact.isNull(); contact = contact.nextSibling())
			{
				const QDomElement &contact_elem = contact.toElement();
				if (contact_elem.isNull())
					continue;
				if (contact_elem.tagName() != "Contact")
					continue;
				QString id = contact_elem.attribute("id");
				users.append(userlist->byID(protocolId, id));
			}
			Protocol *protocol = protocols_manager->byID(protocolId, accountId);
			if (protocol)
				openChatWidget(protocol, users);
			else
				kdebugm(KDEBUG_WARNING, "protocol %s/%s not found!\n", protocolId.local8Bit().data(), accountId.local8Bit().data());
		}
	}
	kdebugf2();
}

void ChatManager::saveOpenedWindows()
{
	kdebugf();
	QDomElement root_elem = xml_config_file->rootElement();
	QDomElement chats_elem = xml_config_file->accessElement(root_elem, "ChatWindows");
	xml_config_file->removeChildren(chats_elem);
	CONST_FOREACH(chat, ChatWidgets)
	{
		QDomElement window_elem = xml_config_file->createElement(chats_elem, "Window");
		Protocol *protocol = (*chat)->currentProtocol();
		QString protoId = protocol->protocolID();
		window_elem.setAttribute("protocol", protoId);
		window_elem.setAttribute("id", protocol->ID());
		const UserGroup *users = (*chat)->users();
		CONST_FOREACH(user, *users)
		{
			QDomElement user_elem = xml_config_file->createElement(window_elem, "Contact");
 			user_elem.setAttribute("id", (*user).ID(protoId));
		}
	}
	kdebugf2();
}

ChatManager::~ChatManager()
{
	kdebugf();
	disconnect(&refreshTitlesTimer, SIGNAL(timeout()), this, SLOT(refreshTitles()));
	disconnect(userlist, SIGNAL(usersStatusChanged(QString)), this, SLOT(refreshTitlesLater()));

	closeAllWindows();

#if DEBUG_ENABLED
	// for valgrind
	QStringList chatActions;
	chatActions << "autoSendAction" << "clearChatAction"
				<< "insertEmoticonAction" << "whoisAction" << "insertImageAction"
				<< "ignoreUserAction" << "blockUserAction" << "boldAction"
				<< "italicAction" << "underlineAction" << "colorAction"
				<< "sendAction" << "chatAction" << "openChatWithAction";
	CONST_FOREACH(act, chatActions)
	{
		ActionDescription *a = KaduActions[*act];
		delete a;
	}
#endif

	kdebugf2();
}

void ChatManager::autoSendActionActivated(QWidget *parent, bool toggled)
{
	kdebugf();

	ChatWidget *chatWidget = dynamic_cast<ChatWidget *>(parent->parent()->parent());
	if (chatWidget)
	{
		chatWidget->setAutoSend(toggled);
		// autoSendActionDescription->setAllChecked(toggled);
		config_file.writeEntry("Chat", "AutoSend", toggled);
	}

	kdebugf2();
}

void ChatManager::clearActionActivated(QWidget *parent, bool toggled)
{
	kdebugf();

	ChatWidget *chatWidget = dynamic_cast<ChatWidget *>(parent->parent()->parent());
	if (chatWidget)
		chatWidget->clearChatWindow();

	kdebugf2();
}

void ChatManager::insertImageActionActivated(QWidget *parent, bool toggled)
{
	kdebugf();
	ChatWidget *chatWidget = dynamic_cast<ChatWidget *>(parent->parent()->parent());
	if (chatWidget)
		chatWidget->insertImage();
	kdebugf2();
}

void ChatManager::boldActionActivated(QWidget *parent, bool toggled)
{
	kdebugf();
	ChatWidget *chatWidget = dynamic_cast<ChatWidget *>(parent->parent()->parent());
	if (chatWidget)
		chatWidget->edit()->setBold(toggled);
	kdebugf2();
}

void ChatManager::italicActionActivated(QWidget *parent, bool toggled)
{
	kdebugf();
	ChatWidget *chatWidget = dynamic_cast<ChatWidget *>(parent->parent()->parent());
	if (chatWidget)
		chatWidget->edit()->setItalic(toggled);
	kdebugf2();
}

void ChatManager::underlineActionActivated(QWidget *parent, bool toggled)
{
	kdebugf();
	ChatWidget *chatWidget = dynamic_cast<ChatWidget *>(parent->parent()->parent());
	if (chatWidget)
		chatWidget->edit()->setUnderline(toggled);
	kdebugf2();
}

void ChatManager::sendActionActivated(QWidget *parent, bool toggled)
{
	kdebugf();
	ChatWidget *chatWidget = dynamic_cast<ChatWidget *>(parent->parent()->parent());
	if (chatWidget)
		if (chatWidget->waitingForACK())
			chatWidget->cancelMessage();
		else
			chatWidget->sendMessage();
	kdebugf2();
}

void ChatManager::whoisActionActivated(QWidget *parent, bool toggled)
{
	kdebugf();

	ActionWindow *window = dynamic_cast<ActionWindow *>(parent);
	if (!window)
	{
		(new SearchDialog(kadu, "search_user"))->show();
		return;
	}

	UserListElements users = window->getUserListElements();

	if (users.count() == 0)
		(new SearchDialog(kadu, "search_user"))->show();
	else
	{
		if (users[1].usesProtocol("Gadu"))
		{
			SearchDialog *sd = new SearchDialog(kadu, "user_info", users[1].ID("Gadu").toUInt());
			sd->show();
			sd->firstSearch();
		}
	}

	kdebugf2();
}

/*
void ChatManager::colorActionActivated(const UserGroup* users, const QWidget* source)
{
	kdebugf();
	findChatWidget(users)->changeColor(source);
	kdebugf2();
}

void ChatManager::insertEmoticonActionActivated(const UserGroup* users, const QWidget* source)
{
	kdebugf();
	findChatWidget(users)->openEmoticonSelector(source);
	kdebugf2();
}

void ChatManager::insertEmoticonActionAddedToToolbar(const UserGroup* users,
	ToolButton* button, ToolBar* toolba/)
{
	kdebugf();
	if((EmoticonsStyle)config_file.readNumEntry("Chat","EmoticonsStyle") == EMOTS_NONE)
	{
		QToolTip::remove(button);
		QToolTip::add(button, tr("Insert emoticon - enable in configuration"));
		button->setEnabled(false);
	}
	kdebugf2();
}*/



void ChatManager::ignoreUserActionActivated(QWidget *parent, bool toggled)
{
	kdebugf();

	ActionWindow *window = dynamic_cast<ActionWindow *>(parent);
	if (!window)
		return;

	UserListElements users = window->getUserListElements();
	if (users.count() > 0)
	{
		bool ContainsBad = false;
		CONST_FOREACH(user, users)
		{
			QString uid = (*user).ID("Gadu");
			if (!gadu->validateUserID(uid))
			{
				ContainsBad = true;
				break;
			}
		}

		if (!ContainsBad)
		{
			if (IgnoredManager::isIgnored(users))
				IgnoredManager::remove(users);
			else
			{
				IgnoredManager::insert(users);
				ChatWidget *chat = findChatWidget(users);
				if (chat)
				{
				ChatContainer * container = dynamic_cast<ChatContainer *>(chat->topLevelWidget());
					if (container)
						container->closeChatWidget(chat);
				}
			}
			kadu->userbox()->refresh();
			IgnoredManager::writeToConfiguration();
		}
	}
	kdebugf2();
}

void ChatManager::blockUserActionActivated(QWidget *parent, bool toggled)
{
	kdebugf();

	ActionWindow *window = dynamic_cast<ActionWindow *>(parent);
	if (!window)
		return;

	UserListElements users = window->getUserListElements();
	if (users.count() > 0)
	{
		bool on = true;
		bool blocked_anonymous = false; // true, if we blocked at least one anonymous user

		UserListElements copy = users;

		CONST_FOREACH(user, copy)
			if (!(*user).usesProtocol("Gadu") || !(*user).protocolData("Gadu", "Blocking").toBool())
			{
				on = false;
				break;
			}

		FOREACH(user, copy)
		{
			QString uid = (*user).ID("Gadu");
			if (gadu->validateUserID(uid) && (*user).protocolData("Gadu", "Blocking").toBool() != !on)
			{
				(*user).setProtocolData("Gadu", "Blocking", !on);
				if ((!on) && (!blocked_anonymous) && ((*user).isAnonymous()))
					blocked_anonymous = true;
			}
		}

		if (!on) // if we were blocking, we also close the chat (and show info if blocked anonymous)
		{
			if (blocked_anonymous)
				MessageBox::msg(tr("Anonymous users will be unblocked after restarting Kadu"), false, "Information", kadu);

			ChatWidget *chat = findChatWidget(users);
			if (chat)
			{
				ChatContainer * container = dynamic_cast<ChatContainer *>(chat->topLevelWidget());
				if (container)
					container->closeChatWidget(chat);
			}
		}

		userlist->writeToConfig();
	}
	kdebugf2();
}

void ChatManager::chatActionActivated(QWidget *parent, bool toggled)
{
	kdebugf();

	ActionWindow *window = dynamic_cast<ActionWindow *>(parent);
	if (!window)
		return;

	UserListElements users = window->getUserListElements();
	if (users.count() > 0)
		openChatWidget(gadu, users);

	kdebugf2();
}

const ChatList& ChatManager::chats() const
{
	return ChatWidgets;
}

const QList<UserListElements> ChatManager::closedChatUsers() const
{
	return ClosedChatUsers;
}

int ChatManager::registerChatWidget(ChatWidget* chat)
{
	kdebugf();

	ClosedChatUsers.remove(chat->users()->toUserListElements());
	ChatWidgets.append(chat);

	return ChatWidgets.count() - 1;
}

void ChatManager::unregisterChatWidget(ChatWidget* chat)
{
	kdebugf();

	FOREACH(curChat, ChatWidgets)
		if (*curChat == chat)
		{
			if (chat->body->countMessages())
			{
				ClosedChatUsers.prepend(chat->users()->toUserListElements());
				if (ClosedChatUsers.count() > 10)
					ClosedChatUsers.pop_back();
			}

			emit chatWidgetDestroying(chat);
			ChatWidgets.remove(curChat);

			kdebugf2();
			return;
		}

	kdebugmf(KDEBUG_FUNCTION_END|KDEBUG_WARNING, "NOT found\n");
}

void ChatManager::refreshTitlesLater()
{
	refreshTitlesTimer.start(0, true);
}

void ChatManager::refreshTitles()
{
	kdebugf();
 	CONST_FOREACH(chat, ChatWidgets)
		(*chat)->refreshTitle();
	emit chatWidgetTitlesUpdated();
	kdebugf2();
}

void ChatManager::refreshTitlesForUser(UserListElement user)
{
	kdebugf();
 	CONST_FOREACH(chat, ChatWidgets)
 		if ((*chat)->users()->contains(user))
 			(*chat)->refreshTitle();
	kdebugf2();
}

ChatWidget* ChatManager::findChatWidget(const UserGroup *group) const
{
	CONST_FOREACH(chat, ChatWidgets)
		if ((*chat)->users() == group)
			return *chat;
	kdebugmf(KDEBUG_WARNING, "no such chat\n");
	return NULL;
}

ChatWidget* ChatManager::findChatWidget(UserListElements users) const
{
	CONST_FOREACH(chat, ChatWidgets)
		if (users.equals((*chat)->users()))
			return *chat;
	kdebugmf(KDEBUG_WARNING, "no such chat\n");
	return NULL;
}

int ChatManager::openChatWidget(Protocol *initialProtocol, const UserListElements &users, bool forceActivate)
{
	kdebugf();

	CONST_FOREACH(user, users)
	{
		QString uid = (*user).ID(initialProtocol->protocolID());
		if (!initialProtocol->validateUserID(uid))
		{
			kdebugf2();
			return -1;
		}
	}

	unsigned int i = 0;
	CONST_FOREACH(chat, ChatWidgets)
	{
		if ((*chat)->users()->equals(users))
		{
			QWidget *win = *chat;
			kdebugm(KDEBUG_INFO, "parent: %p\n", win->parent());
			while (win->parent()) // for tabs module
			{
				kdebugm(KDEBUG_INFO, "parent type: %s\n", win->parent()->className());
				win = static_cast<QWidget *>(win->parent());
			}
#if QT_VERSION >= 0x030300
			if (qstrcmp(qVersion(), "3.3") >= 0) // dodatkowe zabezpieczenie przed idiotami używającymi opcji --force przy instalacji pakietów
				win->setWindowState(win->windowState() & ~Qt::WindowMinimized | Qt::WindowActive);
#endif
			if (forceActivate)
				activateWindow(win->winId());
			win->raise();
			(*chat)->makeActive();
			emit chatWidgetOpen(*chat);
			return i;
		}
		++i;
	}

	QStringList userNames;
	CONST_FOREACH(user, users)
		userNames.append((*user).altNick());
	userNames.sort();

	ChatWidget *chat = new ChatWidget(initialProtocol, users);

	if (forceActivate)
	{
		QWidget *win = chat;
		while (win->parent())
			win = static_cast<QWidget *>(win->parent());
		activateWindow(win->winId());
	}

	bool handled = false;
	emit handleNewChatWidget(chat, handled);
	if (!handled)
	{
		ChatWindow *window = new ChatWindow();
		chat->reparent(window, QPoint(), true);
		window->setChatWidget(chat);
		window->show();
	}
 	chat->refreshTitle();

	connect(chat, SIGNAL(messageSentAndConfirmed(UserListElements, const QString&)),
		this, SIGNAL(messageSentAndConfirmed(UserListElements, const QString&)));

//	chat->makeActive();
	emit chatWidgetCreated(chat);
// TODO: remove, it is so stupid ...
	emit chatWidgetCreated(chat, time(0));
	emit chatWidgetOpen(chat);

	kdebugf2();
	return ChatWidgets.count() - 1;
}

void ChatManager::deletePendingMsgs(UserListElements users)
{
	kdebugf();

	for (int i = 0; i < pending.count(); ++i)
		if (pending[i].users.equals(users))
		{
			pending.deleteMsg(i);
			--i;
		}

	UserBox::refreshAllLater();
	kdebugf2();
}

// TODO: make pending messages ChatMessages or something
ChatMessage *convertPendingToMessage(PendingMsgs::Element elem)
{
	kdebugf();

	QDateTime date;
	date.setTime_t(elem.time);
	ChatMessage *message = new ChatMessage(elem.users[0], elem.msg, TypeReceived, QDateTime::currentDateTime(), date);

	return message;
}

void ChatManager::openPendingMsgs(UserListElements users, bool forceActivate)
{
	kdebugf();

	QList<ChatMessage *> messages;
	PendingMsgs::Element elem;

	for (int i = 0; i < pending.count(); ++i)
	{
		elem = pending[i];
		if (!elem.users.equals(users))
			continue;

		if ((elem.msgclass & GG_CLASS_CHAT) == GG_CLASS_CHAT
			|| (elem.msgclass & GG_CLASS_MSG) == GG_CLASS_MSG
			|| (!elem.msgclass))
			messages.append(convertPendingToMessage(elem));
			pending.deleteMsg(i--);
	}

	if (messages.size())
	{
		// TODO: Lame API
		int i = openChatWidget(gadu, users, forceActivate);
		if (ChatWidgets[i]->countMessages() == 0)
			ChatWidgets[i]->appendMessages(messages);
		UserBox::refreshAllLater();
	}
	else
		openChatWidget(gadu, users, forceActivate);

	kdebugf2();
}

void ChatManager::openPendingMsgs(bool forceActivate)
{
	kdebugf();

	if (pending.count())
		openPendingMsgs(pending[0].users, forceActivate);

	kdebugf2();
}

void ChatManager::sendMessage(UserListElement user, UserListElements selected_users)
{
	kdebugf();

	for (int i = 0; i < pending.count(); ++i)
		if (pending[i].users.contains(user))
		{
			openPendingMsgs(pending[i].users);
			return;
		}

	openChatWidget(gadu, selected_users);

	kdebugf2();
}

QVariant& ChatManager::getChatWidgetProperty(const UserGroup *group, const QString &name)
{
	kdebugf();
	FOREACH(addon, addons)
		if (group->equals((*addon).users))
		{
			kdebugf2();
			return (*addon).map[name];
		}
	ChatInfo info;
	info.users = group->toUserListElements();
	info.map[name] = QVariant();
	addons.push_front(info);
	kdebugmf(KDEBUG_FUNCTION_END, "end: %s NOT found\n", name.local8Bit().data());
	return addons[0].map[name];
}

void ChatManager::setChatWidgetProperty(const UserGroup *group, const QString &name, const QVariant &value)
{
	kdebugf();
	FOREACH(addon, addons)
		if (group->equals((*addon).users))
		{
			(*addon).map[name] = value;
			kdebugf2();
			return;
		}
	ChatInfo info;
	info.users = group->toUserListElements();
	info.map[name] = value;
	addons.push_front(info);
	kdebugf2();
}

void ChatManager::closeModule()
{
	kdebugf();

	ChatMessage::unregisterParserTags();

	if (config_file.readBoolEntry("Chat", "SaveOpenedWindows", true))
		chat_manager->saveOpenedWindows();

	delete chat_manager;
	chat_manager = 0;
	kdebugf2();
}

void ChatManager::initModule()
{
	kdebugf();

	ChatMessage::registerParserTags();
	emoticons->setEmoticonsTheme(config_file.readEntry("Chat", "EmoticonsTheme"));
	chat_manager = new ChatManager(kadu, "chat_manager");

	kdebugf2();
}

void ChatManager::configurationUpdated()
{
	kdebugf();

	if (config_file.readBoolEntry("Chat", "RememberPosition"))
	{
		userlist->addPerContactNonProtocolConfigEntry("chat_geometry", "ChatGeometry");
		userlist->addPerContactNonProtocolConfigEntry("chat_vertical_sizes", "VerticalSizes");
	}
	else
	{
		userlist->removePerContactNonProtocolConfigEntry("chat_geometry");
		userlist->removePerContactNonProtocolConfigEntry("chat_vertical_sizes");
	}

// 	KaduActions["autoSendAction"]->setAllChecked(config_file.readBoolEntry("Chat", "AutoSend"));

	kdebugf2();
}

ChatManager* chat_manager=NULL;
