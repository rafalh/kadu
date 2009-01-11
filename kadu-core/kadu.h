#ifndef KADU_H
#define KADU_H

#include <QtCore/QEvent>
#include <QtCore/QDateTime>
#include <QtCore/QTimer>
#include <QtGui/QKeyEvent>

#include "configuration_aware_object.h"
#include "kadu_main_window.h"
#include "protocols/protocol.h"
#include "exports.h"

class QAction;
class QActionGroup;
class QFile;
class QMenu;
class QMenuBar;
class QPushButton;
class QVBoxLayout;

// TODO: remove
class Q3ListBoxItem;

class Account;
class ActionDescription;
class ContactsListWidget;
class KaduTabBar;
class KaduTextBrowser;
class UserGroup;
class UserStatusChanger;

/**
	G��wne okno Kadu
**/
class KADUAPI Kadu : public KaduMainWindow, ConfigurationAwareObject
{
	Q_OBJECT
public:
	enum MenuType
	{
		MenuKadu,
		MenuContacts,
		MenuHelp
	};

private:

	struct MenuAction
	{
		MenuAction() {}
		MenuAction(KaduAction *action, MenuType menu)
		{
			Action = action;
			Menu = menu;
		}

 		KaduAction* Action;
 		MenuType Menu;
	};


	ActionDescription *inactiveUsersAction;
	ActionDescription *descriptionUsersAction;
	ActionDescription *onlineAndDescriptionUsersAction;
	ActionDescription *configurationActionDescription;
	ActionDescription *editUserActionDescription;
	ActionDescription *addUserActionDescription;
	ActionDescription *openSearchActionDescription;
	ActionDescription *offlineToUserActionDescription;
	ActionDescription *hideDescriptionActionDescription;
	ActionDescription *notifyAboutUserActionDescription;
	ActionDescription *showStatusActionDescription;
	ActionDescription *deleteUsersActionDescription;
	ActionDescription *useProxyActionDescription;

	QActionGroup *changeStatusActionGroup;
	QAction *changeStatusToOnline;
	QAction *changeStatusToOnlineDesc;
	QAction *changeStatusToBusy;
	QAction *changeStatusToBusyDesc;
	QAction *changeStatusToInvisible;
	QAction *changeStatusToInvisibleDesc;
	QAction *changeStatusToOffline;
	QAction *changeStatusToOfflineDesc;
	QAction *changePrivateStatus;

	QMap<ActionDescription *, MenuAction> MenuActions;

	// TODO: remove
	friend class Wizard;

	Contact Myself;

	static bool Closing;
	KaduTextBrowser *InfoPanel;
	QString InfoPanelSyntax;
	QMenuBar *MenuBar;
	QMenu *KaduMenu;
	QMenu *ContactsMenu;
	QMenu *HelpMenu;
	QMenu *RecentChatsMenu;
	KaduTabBar *GroupBar;

	UserBox *Userbox;
	ContactsListWidget *ContactsWidget;

	QWidget *MainWidget;
	QVBoxLayout *MainLayout;

	QMenu *statusMenu;
	QPushButton *statusButton;
	QPoint lastPositionBeforeStatusMenuHide;
	QDateTime StartTime;
	QTimer updateInformationPanelTimer;

	QString infoPanelStyle;

	UserStatus NextStatus;
	UserGroup *selectedUsers; //don't touch! TODO: remove in near future

	UserStatusChanger *userStatusChanger;
#if 0
	SplitStatusChanger *splitStatusChanger;
#endif

	bool ShowMainWindowOnStart;
	bool DoBlink;
	bool BlinkOn;
	bool Docked;
	bool dontHideOnClose;

	void createMenu();
	void createStatusPopupMenu();

	void showStatusOnMenu(int);

	void createDefaultConfiguration();
	void createAllDefaultToolbars();

private slots:
	void inactiveUsersActionActivated(QAction *sender, bool toggled);
	void descriptionUsersActionActivated(QAction *sender, bool toggled);
	void onlineAndDescUsersActionActivated(QAction *sender, bool toggled);
	void configurationActionActivated(QAction *sender, bool toggled);
	void addUserActionActivated(QAction *sender, bool toggled);
	void editUserActionActivated(QAction *sender, bool toggled);
	void searchInDirectoryActionActivated(QAction *sender, bool toggled);
	void notifyAboutUserActionActivated(QAction *sender, bool toggled);
	void offlineToUserActionActivated(QAction *sender, bool toggled);
	void hideDescriptionActionActivated(QAction *sender, bool toggled);
	void writeEMailActionActivated(QAction *sender, bool toggled);
	void copyDescriptionActionActivated(QAction *sender, bool toggled);
	void openDescriptionLinkActionActivated(QAction *sender, bool toggled);
	void copyPersonalInfoActionActivated(QAction *sender, bool toggled);
	void lookupInDirectoryActionActivated(QAction *sender, bool toggled);
	void deleteUsersActionActivated(QAction *sender, bool toggled);
	void addGroupActionActivated(QAction *sender, bool toggled);

	void messageReceived(Account *, ContactList s, const QString &msg, time_t time);
	void createRecentChatsMenu();
	void openRecentChats(QAction *action);
	void openChatWith();

	void changeStatusSlot();

	void changePrivateStatusSlot(bool toggled);

	void wentOnline(const QString &);
	void wentBusy(const QString &);
	void wentInvisible(const QString &);
	void wentOffline(const QString &);
	void connected();
	void connecting();
	void disconnected();
	void imageReceivedAndSaved(UinType sender, uint32_t size, uint32_t crc32, const QString &path);
	void systemMessageReceived(const QString &msg);
	void deleteOldConfigFiles();

	void currentChanged(Contact contact);

	void statusMenuAboutToHide(void);
	void dockMenuAboutToHide(void);

	void changeStatus(UserStatus status);

	void inactiveUsersActionCreated(KaduAction *action);
	void descriptionUsersActionCreated(KaduAction *action);
	void onlineAndDescUsersActionCreated(KaduAction *action);

	void quitApplication();

	// new API
	void accountRegistered(Account *);

protected:
	void keyPressEvent(QKeyEvent *e);
	virtual void customEvent(QEvent *);
	virtual void closeEvent(QCloseEvent *event);
		
	virtual void configurationUpdated();
//	virtual void moveEvent(QMoveEvent *);

public:
	Kadu(QWidget *parent = 0);
	~Kadu();

	static void createDefaultToolbars(QDomElement parentConfig);
	static void addAction(const QString &actionName, bool showLabel = false);

	virtual bool supportsActionType(ActionDescription::ActionType type) {
		return type & (ActionDescription::TypeGlobal | ActionDescription::TypeUserList | ActionDescription::TypeUser); }
	virtual UserBox * userBox() { return Userbox; }
	virtual ContactList contacts();
	virtual ChatWidget * chatWidget() { return 0; }

	Contact myself() { return Myself; }

	bool userInActiveGroup(UinType uin);
	void removeUsers(ContactList contacts);

	/**
		Zwraca wskaznik do zakladek z nazwami grup.
	**/
	KaduTabBar * groupBar() const;

	/**
		Zwraca wskaznik do userbox-a w glownym oknie.
	**/
	UserBox * userbox() const;

	/**
		Zwraca wska�nik na g��wny layout w obiekcie, teraz jest to QVBoxLayout.
		(hint) U�ywaj�c nale�y bada� nazwy i klasy obiekt�w GUI dost�pnych przez ten obiekt.
	**/
	QVBoxLayout * mainLayout() const;

	/**
		Zwraca true je�li kadu jest zadokowane.
	**/
	bool docked() const;

	/**
		Set default status based on values in configuration file
	**/
	void setDefaultStatus();

	void startupProcedure();

	/**
		w zwi�zku z tym, �e opis sk�adni dla parsera jest u�ywany w kilku miejscach
		dodane zosta�o to pole, �eby nie trzeba by�o zmienia� tekstu w kilku miejscach
	**/
	static const char *SyntaxText;
	static const char *SyntaxTextExtended;
	static const char *SyntaxTextNotify;

	static bool closing() { return Closing; }
	static void setClosing() { Closing = true; }

	const QDateTime &startTime() const;
	void refreshPrivateStatusFromConfigFile();

	const QString & panelStyle() { return infoPanelStyle;}

	void insertMenuActionDescription(ActionDescription *actionDescription, MenuType Type, int pos = -1);
	void removeMenuActionDescription(ActionDescription *actionDescription);

public slots:
	virtual void show();
	virtual void hide();
	void mouseButtonClicked(int, Q3ListBoxItem *);
	void updateInformationPanel(Contact contact);
	void updateInformationPanel();
	void updateInformationPanelLater();

	void sendMessage(Contact contact);

	void setStatus(const UserStatus &status);
	void setOnline(const QString &description = QString::null);
	void setBusy(const QString &description = QString::null);
	void setInvisible(const QString &description = QString::null);
	void setOffline(const QString &description = QString::null);

	void slotHandleState(int command);
	void changeAppearance();
	void blink();
	void showdesc(bool show = true);
	virtual bool close(bool quit = false);

	/**
		Potrzebne dla modu�u dokuj�cego �eby
		g��wne okno nie miga�o przy starcie...
	**/
	void setShowMainWindowOnStart(bool show);

	/**
		Modu� dokowania powinien to ustawic, aby kadu
		wiedzialo, ze jest zadokowane.
	**/
	void setDocked(bool docked, bool dontHideOnClose);

	void about(QAction *sender, bool toggled);

	void bugs(QAction *sender, bool toggled);
	void support(QAction *sender, bool toggled);
	void getInvolved(QAction *sender, bool toggled);

	void help(QAction *sender, bool toggled);
	void hideKadu(QAction *sender, bool toggled);
	void importExportUserlist(QAction *sender, bool toggled);
	void manageIgnored(QAction *sender, bool toggled);
	void yourAccounts(QAction *sender, bool toggled);
	void quit();

	// odczytuje z obrazka tekst i zapisuje go w drugim parametrze
	void readTokenValue(QPixmap, QString &);

	void setMainWindowIcon(const QPixmap &);

	void editUserActionCreated(KaduAction *action);
	void editUserActionSetParams(QString protocolName, UserListElement user);
	void showStatusActionActivated(QAction *sender, bool toggled);
	void showStatusActionCreated(KaduAction *action);
	void setStatusActionsIcon();
	void useProxyActionActivated(QAction *sender, bool toggled);
	void useProxyActionCreated(KaduAction *action);
	void setProxyActionsStatus(bool checked);
	void setProxyActionsStatus();

signals:
	void keyPressed(QKeyEvent *e);
	void statusPixmapChanged(const QIcon &icon, const QString &icon_name);

	// TODO: workaround
	void messageReceivedSignal(Account *, ContactList, const QString &, time_t);

	/**
		wywo�ana zosta�a funkcja show() na g��wnym oknie
	**/
	void shown();
	void hiding();

	/**
		u�ywany przez modu� hints do zdobycia pozycji traya od modu�u docking
		TODO: trzeba wymy�li� jaki� elegancki spos�b na komunikacj� pomi�dzy modu�ami, kt�re nie zale�� od siebie
	**/
	void searchingForTrayPosition(QPoint &point);
	void settingMainIconBlocked(bool &);
	void removingUsers(UserListElements users);

};

class OpenGGChatEvent : public QEvent
{
	int Number;

public:
	OpenGGChatEvent(int num) : QEvent((Type)5432), Number(num) {}
	int number() const { return Number; }

};

void disableContainsSelfUles(KaduAction *action);

extern KADUAPI Kadu *kadu;
extern KADUAPI QMenu *dockMenu;
extern int lockFileHandle;
extern QFile *lockFile;
struct flock;
extern struct flock *lock_str;

#endif
