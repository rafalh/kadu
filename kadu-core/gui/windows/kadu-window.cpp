/*
 * %kadu copyright begin%
 * Copyright 2010 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2009, 2010, 2010, 2011, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2010 Przemysław Rudy (prudy1@o2.pl)
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2010, 2010, 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtGui/QCloseEvent>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QSplitter>
#include <QtGui/QVBoxLayout>

#ifdef Q_WS_WIN
#include <windows.h>
#endif

#include "accounts/account-manager.h"
#include "buddies/buddy-set.h"
#include "chat/model/chat-data-extractor.h"
#include "chat/recent-chat-manager.h"
#include "chat/type/chat-type-manager.h"
#include "configuration/configuration-file.h"
#include "contacts/contact-set.h"
#include "contacts/contact.h"
#include "core/core.h"
#include "gui/actions/action.h"
#include "gui/actions/chat/add-conference-action.h"
#include "gui/actions/chat/add-room-chat-action.h"
#include "gui/hot-key.h"
#include "gui/widgets/buddy-info-panel.h"
#include "gui/widgets/chat-widget-actions.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/chat-widget.h"
#include "gui/widgets/roster-widget.h"
#include "gui/widgets/status-buttons.h"
#include "gui/widgets/talkable-tree-view.h"
#include "gui/windows/kadu-window-actions.h"
#include "gui/windows/proxy-action-context.h"
#include "notify/notification-manager.h"
#include "os/generic/url-opener.h"
#include "url-handlers/url-handler-manager.h"
#include "activate.h"
#include "kadu-application.h"

#include "icons/icons-manager.h"
#include "icons/kadu-icon.h"
#include "misc/misc.h"
#include "debug.h"

#include "kadu-window.h"

#ifdef Q_OS_MAC
extern void qt_mac_set_menubar_icons(bool enable);
#endif

KaduWindow::KaduWindow() :
		MainWindow(new ProxyActionContext(), QString(), 0), Docked(false),
		WindowParent(0), CompositingEnabled(false)
{
	setWindowRole("kadu-main");

#ifdef Q_WS_WIN
	HiddenParent = new QWidget();
	setHiddenParent();
#endif

#ifdef Q_OS_MAC
	/* Dorr: workaround for Qt window geometry bug when unified toolbars enabled */
	setUnifiedTitleAndToolBarOnMac(false);
	// Create global menu for OS X.
	MenuBar = new QMenuBar(0);
#endif

	setWindowTitle(QLatin1String("Kadu"));

	// we need to create gui first, then actions, then menus
	// TODO: fix it in 0.10 or whenever
	createGui();

	Context = static_cast<ProxyActionContext *>(actionContext());
	Context->setForwardActionContext(Roster->actionContext());

	Actions = new KaduWindowActions(this);
	loadToolBarsFromConfig();
	createMenu();

	configurationUpdated();

	loadWindowGeometry(this, "General", "Geometry", 0, 50, 350, 650);

#if defined(Q_OS_MAC)
	/* Dorr: workaround for Qt window geometry bug when unified toolbars enabled */
	setUnifiedTitleAndToolBarOnMac(true);
#endif
}

KaduWindow::~KaduWindow()
{
	storeConfiguration();
}

void KaduWindow::createGui()
{
	MainWidget = new QWidget();
	MainLayout = new QVBoxLayout(MainWidget);
	MainLayout->setMargin(0);
	MainLayout->setSpacing(0);

	Split = new QSplitter(Qt::Vertical, MainWidget);

	Roster = new RosterWidget(Split);
	InfoPanel = new BuddyInfoPanel(Split);

	connect(Roster, SIGNAL(currentChanged(Talkable)), InfoPanel, SLOT(displayItem(Talkable)));
	connect(Roster, SIGNAL(talkableActivated(Talkable)), this, SLOT(talkableActivatedSlot(Talkable)));

	ChangeStatusButtons = new StatusButtons(this);

	if (!config_file.readBoolEntry("Look", "ShowInfoPanel"))
		InfoPanel->setVisible(false);
	if (!config_file.readBoolEntry("Look", "ShowStatusButton"))
		ChangeStatusButtons->hide();

	QList<int> splitSizes;

	splitSizes.append(config_file.readNumEntry("General", "UserBoxHeight"));
	splitSizes.append(config_file.readNumEntry("General", "DescriptionHeight"));

	Split->setSizes(splitSizes);

	MainLayout->addWidget(Split);
	MainLayout->addWidget(ChangeStatusButtons);

	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	setCentralWidget(MainWidget);
	setFocusProxy(Roster);
	Roster->setFocus();
}

void KaduWindow::createMenu()
{
#ifdef Q_OS_MAC
	qt_mac_set_menubar_icons(false);
#endif
	createKaduMenu();
	createContactsMenu();
	createToolsMenu();
	createHelpMenu();
}

void KaduWindow::createKaduMenu()
{
	KaduMenu = new QMenu(this);
#ifdef Q_OS_MAC
	KaduMenu->setTitle(tr("General"));
#else
	KaduMenu->setTitle("&Kadu");
#endif
	RecentChatsMenu = new QMenu(this);
	RecentChatsMenu->setIcon(KaduIcon("internet-group-chat").icon());
	RecentChatsMenu->setTitle(tr("Recent chats"));
	RecentChatsMenuNeedsUpdate = true;
	connect(IconsManager::instance(), SIGNAL(themeChanged()), this, SLOT(iconThemeChanged()));
	connect(ChatWidgetManager::instance(), SIGNAL(chatWidgetCreated(ChatWidget*)), this, SLOT(invalidateRecentChatsMenu()));
	connect(ChatWidgetManager::instance(), SIGNAL(chatWidgetDestroying(ChatWidget*)), this, SLOT(invalidateRecentChatsMenu()));
	connect(RecentChatManager::instance(), SIGNAL(recentChatAdded(Chat)), this, SLOT(invalidateRecentChatsMenu()));
	connect(RecentChatManager::instance(), SIGNAL(recentChatRemoved(Chat)), this, SLOT(invalidateRecentChatsMenu()));
	connect(KaduMenu, SIGNAL(aboutToShow()), this, SLOT(updateRecentChatsMenu()));
	connect(RecentChatsMenu, SIGNAL(triggered(QAction *)), this, SLOT(openRecentChats(QAction *)));

	insertMenuActionDescription(Actions->Configuration, MenuKadu);
	insertMenuActionDescription(Actions->ShowYourAccounts, MenuKadu);
	insertMenuActionDescription(Actions->ManageModules, MenuKadu);

	KaduMenu->addSeparator();
	RecentChatsMenuAction = KaduMenu->addMenu(RecentChatsMenu);
	KaduMenu->addSeparator();

	insertMenuActionDescription(NotificationManager::instance()->silentModeActionDescription(), MenuKadu);
	KaduMenu->addSeparator();

	insertMenuActionDescription(Actions->ExitKadu, MenuKadu);

	menuBar()->addMenu(KaduMenu);
}

void KaduWindow::createContactsMenu()
{
	ContactsMenu = new QMenu(this);
	ContactsMenu->setTitle(tr("&Buddies"));

	insertMenuActionDescription(Actions->AddUser, MenuBuddies);
	AddConference = insertMenuActionDescription(Actions->addConference(), MenuBuddies);
	AddRoomChat = insertMenuActionDescription(Actions->addRoomChat(), MenuBuddies);
	insertMenuActionDescription(Actions->AddGroup, MenuBuddies);
	insertMenuActionDescription(Actions->OpenSearch, MenuBuddies);

	ContactsMenu->addSeparator();
	insertMenuActionDescription(ChatWidgetManager::instance()->actions()->openChatWith(), MenuBuddies);

	ContactsMenu->addSeparator();
	insertMenuActionDescription(Actions->InactiveUsers, MenuBuddies);
	insertMenuActionDescription(Actions->ShowBlockedBuddies, MenuBuddies);
	insertMenuActionDescription(Actions->ShowMyself, MenuBuddies);
	insertMenuActionDescription(Actions->ShowInfoPanel, MenuBuddies);

	menuBar()->addMenu(ContactsMenu);

	connect(AccountManager::instance(), SIGNAL(accountRegistered(Account)),
	        this, SLOT(updateAddChatMenuItem()));
	connect(AccountManager::instance(), SIGNAL(accountUnregistered(Account)),
	        this, SLOT(updateAddChatMenuItem()));

	updateAddChatMenuItem();
}

void KaduWindow::createToolsMenu()
{
	ToolsMenu = new QMenu(this);
	ToolsMenu->setTitle(tr("&Tools"));

	insertMenuActionDescription(Actions->ShowMultilogons, MenuTools);

	menuBar()->addMenu(ToolsMenu);
}

void KaduWindow::createHelpMenu()
{
	HelpMenu = new QMenu(this);
	HelpMenu->setTitle(tr("&Help"));

	// insertMenuActionDescription(Actions->Help, MenuHelp);
	insertMenuActionDescription(Actions->Bugs, MenuHelp);
	HelpMenu->addSeparator();
	insertMenuActionDescription(Actions->GetInvolved, MenuHelp);
	insertMenuActionDescription(Actions->Translate, MenuHelp);
	HelpMenu->addSeparator();
	insertMenuActionDescription(Actions->About, MenuHelp);

	menuBar()->addMenu(HelpMenu);
}

#ifdef Q_OS_MAC
QMenuBar* KaduWindow::menuBar() const
{
	return MenuBar;
}
#endif

void KaduWindow::compositingEnabled()
{
	if (!config_file.readBoolEntry("Look", "UserboxTransparency"))
	{
		compositingDisabled();
		return;
	}

	if (CompositingEnabled)
		return;

	CompositingEnabled = true;
	setTransparency(true);
	menuBar()->setAutoFillBackground(true);
	InfoPanel->setAutoFillBackground(true);
	ChangeStatusButtons->setAutoFillBackground(true);
	for (int i = 1; i < Split->count(); ++i)
	{
		QSplitterHandle *splitterHandle = Split->handle(i);
		splitterHandle->setAutoFillBackground(true);
	}

	configurationUpdated();
}

void KaduWindow::compositingDisabled()
{
	if (!CompositingEnabled)
		return;

	CompositingEnabled = false;
	menuBar()->setAutoFillBackground(false);
	InfoPanel->setAutoFillBackground(false);
	ChangeStatusButtons->setAutoFillBackground(false);
	for (int i = 1; i < Split->count(); ++i)
	{
		QSplitterHandle *splitterHandle = Split->handle(i);
		splitterHandle->setAutoFillBackground(false);
	}
	setTransparency(false);

	configurationUpdated();
}

void KaduWindow::talkableActivatedSlot(const Talkable &talkable)
{
	const Chat &chat = talkable.toChat();
	if (chat && !chat.contacts().toBuddySet().contains(Core::instance()->myself()))
	{
		ChatWidget * const chatWidget = ChatWidgetManager::instance()->byChat(chat, true);
		if (chatWidget)
			chatWidget->activate();
		return;
	}

	const Buddy &buddy = talkable.toBuddy();
	if (buddy.contacts().isEmpty() && buddy.mobile().isEmpty() && !buddy.email().isEmpty())
		if (buddy.email().indexOf(UrlHandlerManager::instance()->mailRegExp()) == 0)
			UrlOpener::openEmail(buddy.email().toUtf8());

	emit talkableActivated(talkable);
}

void KaduWindow::invalidateRecentChatsMenu()
{
	RecentChatsMenuNeedsUpdate = true;
}

void KaduWindow::updateRecentChatsMenu()
{
	kdebugf();

	if (!RecentChatsMenuNeedsUpdate)
		return;

	RecentChatsMenu->clear();

	foreach (const Chat &chat, RecentChatManager::instance()->recentChats())
		if (!ChatWidgetManager::instance()->byChat(chat, false))
		{
			ChatType *type = ChatTypeManager::instance()->chatType(chat.type());
			QAction *action = new QAction(type ? type->icon().icon() : QIcon(),
			                              ChatDataExtractor::data(chat, Qt::DisplayRole).toString(),
			                              RecentChatsMenu);
			action->setData(QVariant::fromValue<Chat>(chat));
			RecentChatsMenu->addAction(action);
		}

	RecentChatsMenuAction->setEnabled(!RecentChatsMenu->actions().isEmpty());
	RecentChatsMenuNeedsUpdate = false;

	kdebugf2();
}

void KaduWindow::updateAddChatMenuItem()
{
	AddConference->setVisible(false);
	AddRoomChat->setVisible(false);

	foreach (const Account &account, AccountManager::instance()->items())
		if (account.protocolName() == "gadu")
			AddConference->setVisible(true);
		else if (account.protocolName() == "jabber")
			AddRoomChat->setVisible(true);
}

void KaduWindow::openRecentChats(QAction *action)
{
	ChatWidget * const chatWidget = ChatWidgetManager::instance()->byChat(action->data().value<Chat>(), true);
	if (chatWidget)
		chatWidget->activate();
}

void KaduWindow::iconThemeChanged()
{
	RecentChatsMenu->setIcon(KaduIcon("internet-group-chat").icon());
}

void KaduWindow::storeConfiguration()
{
#ifdef Q_OS_MAC
	/* Dorr: workaround for Qt window geometry bug when unified toolbars enabled */
	setUnifiedTitleAndToolBarOnMac(false);
#endif
	saveWindowGeometry(this, "General", "Geometry");
#ifdef Q_OS_MAC
	/* Dorr: workaround for Qt window geometry bug when unified toolbars enabled */
	setUnifiedTitleAndToolBarOnMac(true);
#endif

	if (config_file.readBoolEntry("Look", "ShowInfoPanel"))
	{
		config_file.writeEntry("General", "UserBoxHeight", Roster->size().height());
		config_file.writeEntry("General", "DescriptionHeight", InfoPanel->size().height());
	}
	if (config_file.readBoolEntry("Look", "ShowStatusButton"))
		config_file.writeEntry("General", "UserBoxHeight", Roster->size().height());
}

void KaduWindow::closeEvent(QCloseEvent *e)
{
	if (!isWindow())
	{
		e->ignore();
		return;
	}

	// do not block window closing when session is about to close
	if (Core::instance()->application()->sessionClosing())
	{
		MainWindow::closeEvent(e);
		return;
	}

	if (Docked)
	{
		e->ignore();
		hide();
	}
	else
	{
		MainWindow::closeEvent(e);
		Core::instance()->application()->quit();
	}
}

void KaduWindow::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Escape)
	{
		if (Docked && isWindow())
		{
			kdebugm(KDEBUG_INFO, "Kadu::keyPressEvent(Key_Escape): Kadu hide\n");
			hide();
			return;
		}
	}
	else if (e->matches(QKeySequence::Copy) && !InfoPanel->selectedText().isEmpty())
		// Do not use triggerPageAction(), see bug #2345.
		InfoPanel->pageAction(QWebPage::Copy)->trigger();

	emit keyPressed(e);

	MainWindow::keyPressEvent(e);
}

#ifdef Q_WS_WIN
/* On Windows the only way to not show a window in the taskbar without making it a toolwindow
 * is to turn off the WS_EX_APPWINDOW style and provide it with a parent (which will be hidden
 * in our case).
 */
void KaduWindow::setHiddenParent()
{
	QWidget *futureChild = window();
	bool wasVisible = futureChild->isVisible();
	Qt::WindowFlags previousFlags = futureChild->windowFlags();
	futureChild->setParent(HiddenParent);
	futureChild->setWindowFlags(previousFlags);
	futureChild->setVisible(wasVisible);

	hideWindowFromTaskbar();
}

void KaduWindow::hideWindowFromTaskbar()
{
	QWidget *w = window();
	LONG_PTR newWindowLongPtr = GetWindowLongPtr(w->winId(), GWL_EXSTYLE);
	bool hideFromTaskbar = config_file.readBoolEntry("General", "HideMainWindowFromTaskbar");
	if (hideFromTaskbar == !(newWindowLongPtr & WS_EX_APPWINDOW))
		return;

	if (hideFromTaskbar)
		newWindowLongPtr &= ~WS_EX_APPWINDOW;
	else
		newWindowLongPtr |= WS_EX_APPWINDOW;

	bool wasVisible = w->isVisible();
	w->setVisible(false);
	SetWindowLongPtr(w->winId(), GWL_EXSTYLE, newWindowLongPtr);
	w->setVisible(wasVisible);
}
#endif

void KaduWindow::changeEvent(QEvent *event)
{
	MainWindow::changeEvent(event);
	if (event->type() == QEvent::ActivationChange)
	{
		if (!_isActiveWindow(this))
			Roster->clearFilter();
	}
#ifdef Q_WS_WIN
	else if (event->type() == QEvent::WindowStateChange)
	{
		if (Docked && isMinimized() && config_file.readBoolEntry("General", "HideMainWindowFromTaskbar"))
			QMetaObject::invokeMethod(this, "hide", Qt::QueuedConnection);
	}
#endif
	else if (event->type() == QEvent::ParentChange)
	{
		QWidget *previousWindowParent = WindowParent;
		WindowParent = (window() != this) ? window() : 0;
		if (previousWindowParent != WindowParent)
		{
			// On Windows we reparent WindowParent, so we want it to be parentless now.
			// BTW, if WindowParent would be really needed in future, it's quite easy to support it.
			Q_ASSERT(!WindowParent || 0 == WindowParent->parentWidget());
#ifdef Q_WS_WIN
			// Without QueuedConnection I hit infinite loop here.
			QMetaObject::invokeMethod(this, "setHiddenParent", Qt::QueuedConnection);
#endif
			emit parentChanged(WindowParent);
		}
	}
}

bool KaduWindow::supportsActionType(ActionDescription::ActionType type)
{
	return type & (ActionDescription::TypeGlobal | ActionDescription::TypeUserList | ActionDescription::TypeUser);
}

TalkableTreeView * KaduWindow::talkableTreeView()
{
	return Roster->talkableTreeView();
}

TalkableProxyModel * KaduWindow::talkableProxyModel()
{
	return Roster->talkableProxyModel();
}

void KaduWindow::configurationUpdated()
{
#ifdef Q_WS_WIN
	hideWindowFromTaskbar();
#endif

	setDocked(Docked);

	ChangeStatusButtons->setVisible(config_file.readBoolEntry("Look", "ShowStatusButton"));

	triggerCompositingStateChanged();
	setBlur(config_file.readBoolEntry("Look", "UserboxTransparency") && config_file.readBoolEntry("Look", "UserboxBlur"));
}

QAction * KaduWindow::insertMenuActionDescription(ActionDescription *actionDescription, MenuType type, int pos)
{
	Q_ASSERT(actionDescription);

	Action *action = actionDescription->createAction(actionContext(), this);
	QMenu *menu = 0;

	switch (type)
	{
		case MenuKadu:
			menu = KaduMenu;
			break;
		case MenuBuddies:
			menu = ContactsMenu;
			break;
		case MenuTools:
			menu = ToolsMenu;
			break;
		case MenuHelp:
			menu = HelpMenu;
			break;
	}

	if (!menu)
	{
		delete action;
		return 0;
	}

	QList<QAction *> menuActions = menu->actions();
	if (pos < 0 || pos >= menuActions.count())
		menu->addAction(action);
	else
		menu->insertAction(menuActions.at(pos), action);

	MenuActions.insert(actionDescription, MenuAction(action, type));

	return action;
}

void KaduWindow::removeMenuActionDescription(ActionDescription *actionDescription)
{
	if (!actionDescription)
		return;

	QMap<ActionDescription *, MenuAction>::iterator it = MenuActions.find(actionDescription);
	if (it == MenuActions.end())
		return;

	Action *action = it.value().first;
	switch (it.value().second)
	{
		case MenuKadu:
			KaduMenu->removeAction(action);
			break;
		case MenuBuddies:
			ContactsMenu->removeAction(action);
			break;
		case MenuTools:
			ToolsMenu->removeAction(action);
			break;
		case MenuHelp:
			HelpMenu->removeAction(action);
			break;
	}

	MenuActions.erase(it);
	delete action;
}

void KaduWindow::createDefaultToolbars(QDomElement parentConfig)
{
	QDomElement dockAreaConfig = getDockAreaConfigElement(parentConfig, "topDockArea");
	QDomElement toolbarConfig = xml_config_file->createElement(dockAreaConfig, "ToolBar");

#ifdef Q_WS_MAEMO_5
	addToolButton(toolbarConfig, "addUserAction", Qt::ToolButtonIconOnly);
	addToolButton(toolbarConfig, "addGroupAction", Qt::ToolButtonIconOnly);
	addToolButton(toolbarConfig, "muteSoundsAction", Qt::ToolButtonIconOnly);
#else
	addToolButton(toolbarConfig, "addUserAction", Qt::ToolButtonTextUnderIcon);
	addToolButton(toolbarConfig, "addGroupAction", Qt::ToolButtonTextUnderIcon);
	addToolButton(toolbarConfig, "muteSoundsAction", Qt::ToolButtonTextUnderIcon);
#endif
}

void KaduWindow::setDocked(bool docked)
{
	Docked = docked;
}
