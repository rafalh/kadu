/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2014 Michał Ziąbkowski (mziab@o2.pl)
 * Copyright 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtConcurrent/QtConcurrentRun>
#include <QtCore/QDir>
#include <QtCore/QMutexLocker>
#include <QtCore/QThread>
#include <QtGui/QTextDocument>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-storage.h"
#include "chat/buddy-chat-manager.h"
#include "chat/chat-details-buddy.h"
#include "chat/chat-details.h"
#include "chat/chat-manager.h"
#include "chat/chat-storage.h"
#include "chat/type/chat-type-contact.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "contacts/contact-manager.h"
#include "contacts/contact-set.h"
#include "contacts/contact-storage.h"
#include "formatted-string/composite-formatted-string.h"
#include "formatted-string/formatted-string-factory.h"
#include "formatted-string/formatted-string-plain-text-visitor.h"
#include "gui/widgets/chat-widget/chat-widget.h"
#include "gui/windows/message-dialog.h"
#include "gui/windows/progress-window.h"
#include "html/html-conversion.h"
#include "html/html-string.h"
#include "icons/icons-manager.h"
#include "message/message-storage.h"
#include "message/message.h"
#include "message/sorted-messages.h"
#include "misc/misc.h"
#include "plugin/plugin-injected-factory.h"
#include "status/status-type-data.h"
#include "status/status-type-manager.h"
#include "talkable/talkable-converter.h"
#include "talkable/talkable.h"
#include "debug.h"

#include "plugins/history/history-query-result.h"
#include "plugins/history/history-query.h"
#include "plugins/history/history.h"
#include "plugins/history/search/history-search-parameters.h"

#include "storage/sql-accounts-mapping.h"
#include "storage/sql-chats-mapping.h"
#include "storage/sql-contacts-mapping.h"
#include "storage/sql-initializer.h"
#include "storage/sql-messages-chat-storage.h"
#include "storage/sql-messages-sms-storage.h"
#include "storage/sql-messages-status-storage.h"

#include "history-sql-storage.h"

#define DATE_TITLE_LENGTH 120

HistorySqlStorage::HistorySqlStorage(QObject *parent) :
		// using C++ initializers breaks Qt's lupdate
		HistoryStorage(parent), InitializerThread(), ImportProgressWindow(),
		AccountsMapping(), ContactsMapping(), ChatsMapping(), DatabaseMutex(QMutex::NonRecursive),
		m_historyChatStorage(), StatusStorage(), SmsStorage()
{
}

HistorySqlStorage::~HistorySqlStorage()
{
	kdebugf();

	if (InitializerThread && InitializerThread->isRunning())
	{
		InitializerThread->terminate();
		InitializerThread->wait(2000);
	}

	delete InitializerThread;
	InitializerThread = 0;

	if (Database.isOpen())
		Database.commit();
}

void HistorySqlStorage::setBuddyChatManager(BuddyChatManager *buddyChatManager)
{
	m_buddyChatManager = buddyChatManager;
}

void HistorySqlStorage::setBuddyManager(BuddyManager *buddyManager)
{
	m_buddyManager = buddyManager;
}

void HistorySqlStorage::setBuddyStorage(BuddyStorage *buddyStorage)
{
	m_buddyStorage = buddyStorage;
}

void HistorySqlStorage::setChatManager(ChatManager *chatManager)
{
	m_chatManager = chatManager;
}

void HistorySqlStorage::setChatStorage(ChatStorage *chatStorage)
{
	m_chatStorage = chatStorage;
}

void HistorySqlStorage::setContactStorage(ContactStorage *contactStorage)
{
	m_contactStorage = contactStorage;
}

void HistorySqlStorage::setFormattedStringFactory(FormattedStringFactory *formattedStringFactory)
{
	m_formattedStringFactory = formattedStringFactory;
}

void HistorySqlStorage::setIconsManager(IconsManager *iconsManager)
{
	m_iconsManager = iconsManager;
}

void HistorySqlStorage::setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory)
{
	m_pluginInjectedFactory = pluginInjectedFactory;
}

void HistorySqlStorage::setMessageStorage(MessageStorage *messageStorage)
{
	m_messageStorage = messageStorage;
}

void HistorySqlStorage::setStatusTypeManager(StatusTypeManager *statusTypeManager)
{
	m_statusTypeManager = statusTypeManager;
}

void HistorySqlStorage::setTalkableConverter(TalkableConverter *talkableConverter)
{
	m_talkableConverter = talkableConverter;
}

void HistorySqlStorage::init()
{
	kdebugf();

	if (!QSqlDatabase::isDriverAvailable("QSQLITE"))
	{
		MessageDialog::show(m_iconsManager->iconByPath(KaduIcon("dialog-warning")), tr("Kadu"),
				tr("It seems your Qt library does not provide support for selected database. "
				   "Please install Qt with %1 plugin.").arg("QSQLITE"));
		return;
	}

	qRegisterMetaType<QSqlError>("QSqlError");

	InitializerThread = new QThread();

	// this object cannot have parent as it will be moved to a new thread
	auto initializer = m_pluginInjectedFactory->makeInjected<SqlInitializer>();
	initializer->moveToThread(InitializerThread);

	connect(InitializerThread, SIGNAL(started()), initializer, SLOT(initialize()));
	connect(initializer, SIGNAL(progressMessage(QString,QString)),
	        this, SLOT(initializerProgressMessage(QString,QString)));
	connect(initializer, SIGNAL(progressFinished(bool,QString,QString)),
	        this, SLOT(initializerProgressFinished(bool,QString,QString)));
	connect(initializer, SIGNAL(databaseReady(bool)), this, SLOT(databaseReady(bool)));

	InitializerThread->start();

	m_historyChatStorage = new SqlMessagesChatStorage(this);
	StatusStorage = new SqlMessagesStatusStorage(this);
	SmsStorage = new SqlMessagesSmsStorage(this);
}

void HistorySqlStorage::done()
{
	if (Database.isOpen())
		Database.close();
	Database = QSqlDatabase();

	QSqlDatabase::removeDatabase("kadu-history");
}

void HistorySqlStorage::ensureProgressWindowReady()
{
	if (ImportProgressWindow)
		return;

	ImportProgressWindow = m_pluginInjectedFactory->makeInjected<ProgressWindow>(tr("Preparing history database..."));
	ImportProgressWindow->setWindowTitle(tr("History"));
	ImportProgressWindow->show();
}

void HistorySqlStorage::initializerProgressMessage(const QString &iconName, const QString &message)
{
	ensureProgressWindowReady();
	ImportProgressWindow->addProgressEntry(iconName, message);
}

void HistorySqlStorage::initializerProgressFinished(bool ok, const QString &iconName, const QString &message)
{
	if (!ok)
		ensureProgressWindowReady();

	if (ImportProgressWindow)
		ImportProgressWindow->progressFinished(ok, iconName, message);
}

void HistorySqlStorage::databaseReady(bool ok)
{
	if (ok)
	{
		// Recreate database connection because it can be used only in the thread in which it was created
		Database = QSqlDatabase::addDatabase("QSQLITE", "kadu-history");
		Database.open();
	}

	if (!Database.isOpen() || Database.isOpenError())
	{
		initializerProgressFinished(false, "dialog-error",
				tr("Opening database failed. Error message:\n%1").arg(Database.lastError().text()));
		// History::instance()->unregisterStorage(this); TODO: may be needed

		if (InitializerThread)
			InitializerThread->quit();
		return;
	}

	Database.transaction();
	initQueries();

	AccountsMapping = m_pluginInjectedFactory->makeInjected<SqlAccountsMapping>(Database, this);
	ContactsMapping = m_pluginInjectedFactory->makeInjected<SqlContactsMapping>(Database, AccountsMapping, this);
	ChatsMapping = m_pluginInjectedFactory->makeInjected<SqlChatsMapping>(Database, AccountsMapping, ContactsMapping, this);

	if (InitializerThread)
		InitializerThread->quit();
}

bool HistorySqlStorage::isDatabaseReady()
{
	if (InitializerThread && InitializerThread->isRunning())
		return false;

	return Database.isOpen();
}

bool HistorySqlStorage::waitForDatabase()
{
	if (InitializerThread && InitializerThread->isRunning())
		InitializerThread->wait();

	return Database.isOpen();
}

void HistorySqlStorage::initQueries()
{
	AppendMessageQuery = QSqlQuery(Database);
	AppendMessageQuery.prepare("INSERT INTO kadu_messages (chat_id, contact_id, send_time, receive_time, date_id, is_outgoing, content_id) VALUES "
			"(:chat_id, :contact_id, :send_time, :receive_time, :date_id, :is_outgoing, :content_id)");

	AppendStatusQuery = QSqlQuery(Database);
	AppendStatusQuery.prepare("INSERT INTO kadu_statuses (contact_id, status, set_time, description) VALUES "
			"(:contact_id, :status, :set_time, :description)");

	AppendSmsQuery = QSqlQuery(Database);
	AppendSmsQuery.prepare("INSERT INTO kadu_sms (receipient, send_time, content) VALUES "
			"(:receipient, :send_time, :content)");
}

QString HistorySqlStorage::chatIdList(const Chat &chat)
{
	if (!chat)
		return QStringLiteral("(0)");

	ChatDetailsBuddy *buddyChat = qobject_cast<ChatDetailsBuddy *>(chat.details());
	if (!buddyChat)
		return QString("(%1)").arg(ChatsMapping->idByChat(chat, false));

	QStringList ids;
	foreach (const Chat &aggregatedChat, buddyChat->chats())
		ids.append(QString::number(ChatsMapping->idByChat(aggregatedChat, false)));

	return QString("(%1)").arg(ids.join(QStringLiteral(", ")));
}

QString HistorySqlStorage::talkableContactsWhere(const Talkable &talkable)
{
	if (talkable.isValidBuddy())
		return buddyContactsWhere(m_talkableConverter->toBuddy(talkable));
	else if (talkable.isValidContact())
		return QString("contact_id = %1").arg(ContactsMapping->idByContact(m_talkableConverter->toContact(talkable), true));

	return QStringLiteral("1");
}

QString HistorySqlStorage::buddyContactsWhere(const Buddy &buddy)
{
	if (!buddy || buddy.contacts().isEmpty())
		return QStringLiteral("0");

	QStringList ids;
	foreach (const Contact &contact, buddy.contacts())
		ids.append(QString("%1").arg(ContactsMapping->idByContact(contact, true)));

	return QString("contact_id IN (%1)").arg(ids.join(QStringLiteral(", ")));
}

void HistorySqlStorage::sync()
{
	if (!isDatabaseReady())
		return; // nothing to sync yet

	QMutexLocker locker(&DatabaseMutex);

	Database.commit();
	Database.transaction();
}

void HistorySqlStorage::messageReceived(const Message &message)
{
	appendMessage(message);
}

void HistorySqlStorage::messageSent(const Message &message)
{
	appendMessage(message);
}

int HistorySqlStorage::findOrCreateDate(const QDate &date)
{
	QString stringDate = date.toString("yyyyMMdd");
	if (DateMap.contains(stringDate))
		return DateMap.value(stringDate);

	QSqlQuery query(Database);
	QString queryString = "SELECT id FROM kadu_dates WHERE date=:date";

	query.prepare(queryString);
	query.bindValue(":date", stringDate);

	int dateId = -1;

	executeQuery(query);

	if (query.next())
	{
		dateId = query.value(0).toInt();
		// Q_ASSERT(!query.next());
	}
	else
	{
		QSqlQuery query(Database);
		QString queryString = "INSERT INTO kadu_dates (date) VALUES (:date)";

		query.prepare(queryString);
		query.bindValue(":date", stringDate);

		executeQuery(query);
		dateId = query.lastInsertId().toInt();
	}

	DateMap.insert(stringDate, dateId);

	return dateId;
}

int HistorySqlStorage::saveMessageContent(const Message& message)
{
	QSqlQuery saveMessageQuery = QSqlQuery(Database);
	saveMessageQuery.prepare("INSERT INTO kadu_message_contents (content) VALUES (:content)");

	saveMessageQuery.bindValue(":content", message.content().string());

	executeQuery(saveMessageQuery);
	int contentId = saveMessageQuery.lastInsertId().toInt();

	saveMessageQuery.finish();

	return contentId;
}

void HistorySqlStorage::appendMessage(const Message &message)
{
	kdebugf();

	if (!waitForDatabase())
		return;

	QMutexLocker locker(&DatabaseMutex);

	int outgoing = (message.type() == MessageTypeSent)
			? 1
			: 0;

	AppendMessageQuery.bindValue(":chat_id", ChatsMapping->idByChat(message.messageChat(), true));
	AppendMessageQuery.bindValue(":contact_id", ContactsMapping->idByContact(message.messageSender(), true));
	AppendMessageQuery.bindValue(":send_time", message.sendDate().toString("yyyy-MM-ddThh:mm:ss.zzz"));
	AppendMessageQuery.bindValue(":receive_time", message.receiveDate().toString("yyyy-MM-ddThh:mm:ss.zzz"));
	AppendMessageQuery.bindValue(":date_id", findOrCreateDate(message.receiveDate().date()));
	AppendMessageQuery.bindValue(":is_outgoing", outgoing);
	AppendMessageQuery.bindValue(":content_id", saveMessageContent(message));

	executeQuery(AppendMessageQuery);

	AppendMessageQuery.finish();

	kdebugf2();
}

void HistorySqlStorage::appendStatus(const Contact &contact, const Status &status, const QDateTime &time)
{
	kdebugf();

	if (!waitForDatabase())
		return;

	QMutexLocker locker(&DatabaseMutex);

	StatusTypeData statusTypeData = m_statusTypeManager->statusTypeData(status.type());

	AppendStatusQuery.bindValue(":contact_id", ContactsMapping->idByContact(contact, true));
	AppendStatusQuery.bindValue(":status", statusTypeData.name());
	AppendStatusQuery.bindValue(":set_time", time);
	AppendStatusQuery.bindValue(":description", status.description());

	executeQuery(AppendStatusQuery);

	AppendStatusQuery.finish();

	kdebugf2();
}

void HistorySqlStorage::appendSms(const QString &recipient, const QString &content, const QDateTime &time)
{
	kdebugf();

	if (!waitForDatabase())
		return;

	QMutexLocker locker(&DatabaseMutex);

	AppendSmsQuery.bindValue(":contact", recipient);
	AppendSmsQuery.bindValue(":send_time", time);
	AppendSmsQuery.bindValue(":content", content);

	executeQuery(AppendSmsQuery);

	AppendSmsQuery.finish();

	kdebugf2();
}

void HistorySqlStorage::clearChatHistory(const Talkable &talkable, const QDate &date)
{
	if (!waitForDatabase())
		return;

	QMutexLocker locker(&DatabaseMutex);

	QSqlQuery query(Database);
	QString queryString = "DELETE FROM kadu_messages WHERE chat_id IN " + chatIdList(m_talkableConverter->toChat(talkable));
	if (!date.isNull())
		queryString += " AND date_id IN (SELECT id FROM kadu_dates WHERE date = :date)";

	query.prepare(queryString);

	if (!date.isNull())
		query.bindValue(":date", date.toString("yyyyMMdd"));

	executeQuery(query);

	QString removeChatsQueryString = "DELETE FROM kadu_chats WHERE 0 = (SELECT COUNT(*) FROM kadu_messages WHERE chat_id = kadu_chats.id)";
	QSqlQuery removeChatsQuery(Database);

	removeChatsQuery.prepare(removeChatsQueryString);

	executeQuery(removeChatsQuery);

	ChatsMapping->removeChat(m_talkableConverter->toChat(talkable));
}

void HistorySqlStorage::clearStatusHistory(const Talkable &talkable, const QDate &date)
{
	if (!talkable.isValidBuddy() && !talkable.isValidContact())
		return;

	if (!waitForDatabase())
		return;

	QMutexLocker locker(&DatabaseMutex);

	QSqlQuery query(Database);
	QString queryString = "DELETE FROM kadu_statuses WHERE " + talkableContactsWhere(talkable);
	if (!date.isNull())
		queryString += " AND substr(set_time,0,11) = :date";

	query.prepare(queryString);

	if (!date.isNull())
		query.bindValue(":date", date.toString(Qt::ISODate));

	executeQuery(query);
}

void HistorySqlStorage::clearSmsHistory(const Talkable &talkable, const QDate &date)
{
	if (!talkable.isValidBuddy() || m_talkableConverter->toBuddy(talkable).mobile().isEmpty())
		return;

	if (!waitForDatabase())
		return;

	QMutexLocker locker(&DatabaseMutex);

	QSqlQuery query(Database);
	QString queryString = "DELETE FROM kadu_sms WHERE receipient = :receipient";
	if (!date.isNull())
		queryString += " AND substr(send_time,0,11) = :date";

	query.prepare(queryString);

	query.bindValue(":receipient", m_talkableConverter->toBuddy(talkable).mobile());
	if (!date.isNull())
		query.bindValue(":date", date.toString(Qt::ISODate));

	executeQuery(query);
}

void HistorySqlStorage::deleteHistory(const Talkable &talkable)
{
	foreach (const Contact &contact, m_talkableConverter->toBuddy(talkable).contacts())
	{
		Chat chat = ChatTypeContact::findChat(m_chatManager, m_chatStorage, contact, ActionReturnNull);
		clearChatHistory(chat, QDate());
	}

	clearStatusHistory(m_talkableConverter->toBuddy(talkable), QDate());
}

QVector<Talkable> HistorySqlStorage::syncChats()
{
	if (!waitForDatabase())
		return QVector<Talkable>();

	QList<Chat> chats = ChatsMapping->mapping().values();
	QVector<Talkable> talkables;

	foreach (const Chat &chat, chats)
		talkables.append(chat);

	return talkables;
}

QFuture<QVector<Talkable>> HistorySqlStorage::chats()
{
	return QtConcurrent::run(this, &HistorySqlStorage::syncChats);
}

QVector<Talkable> HistorySqlStorage::syncStatusBuddies()
{
	if (!waitForDatabase())
		return QVector<Talkable>();

	QMutexLocker locker(&DatabaseMutex);

	QSqlQuery query(Database);
	query.prepare("SELECT DISTINCT contact_id FROM kadu_statuses");
	executeQuery(query);

	QVector<Talkable> result;
	while (query.next())
	{
		Contact contact = ContactsMapping->contactById(query.value(0).toInt());
		if (!contact)
			continue;

		Buddy buddy = m_buddyManager->byContact(contact, ActionCreate);
		Q_ASSERT(buddy);

		if (!result.contains(buddy))
			result.append(buddy);
	}

	return result;
}

QFuture<QVector<Talkable>> HistorySqlStorage::statusBuddies()
{
	return QtConcurrent::run(this, &HistorySqlStorage::syncStatusBuddies);
}

QVector<Talkable> HistorySqlStorage::syncSmsRecipients()
{
	if (!waitForDatabase())
		return QVector<Talkable>();

	QMutexLocker locker(&DatabaseMutex);

	QSqlQuery query(Database);
	query.prepare("SELECT DISTINCT receipient FROM kadu_sms");
	executeQuery(query);

	QVector<Talkable> result;
	while (query.next())
	{
		Buddy buddy = m_buddyStorage->create();
		buddy.setDisplay(query.value(0).toString());
		buddy.setMobile(query.value(0).toString());
		result.append(buddy);
	}

	return result;
}

QFuture<QVector<Talkable>> HistorySqlStorage::smsRecipients()
{
	return QtConcurrent::run(this, &HistorySqlStorage::syncSmsRecipients);
}

QVector<HistoryQueryResult> HistorySqlStorage::syncChatDates(const HistoryQuery &historyQuery)
{
	if (!waitForDatabase())
		return QVector<HistoryQueryResult>();

	const Talkable &talkable = historyQuery.talkable();

	QMutexLocker locker(&DatabaseMutex);

	QSqlQuery query(Database);
	QString queryString = "SELECT count(1), date, content, chat_id FROM";
	queryString += " (SELECT km.rowid, date, date_id, chat_id, content FROM kadu_messages km "
		"LEFT JOIN kadu_message_contents kmc ON (km.content_id=kmc.id) "
		"LEFT JOIN kadu_dates d ON (km.date_id=d.id) WHERE 1";

	if (!talkable.isEmpty())
		queryString += QString(" AND chat_id IN %1").arg(chatIdList(m_talkableConverter->toChat(talkable)));
	if (!historyQuery.string().isEmpty())
		queryString += " AND kmc.content LIKE :query";
	if (historyQuery.fromDate().isValid())
		queryString += " AND date >= :fromDate";
	if (historyQuery.toDate().isValid())
		queryString += " AND date <= :toDate";

	queryString += " ORDER BY date DESC, km.rowid DESC )";
	queryString += " GROUP BY date_id, chat_id";
	queryString += " ORDER BY date ASC, rowid ASC";

	query.prepare(queryString);

	if (!historyQuery.string().isEmpty())
		query.bindValue(":query", QString("%%%1%%").arg(historyQuery.string()));
	if (historyQuery.fromDate().isValid())
		query.bindValue(":fromDate", historyQuery.fromDate().toString("yyyyMMdd"));
	if (historyQuery.toDate().isValid())
		query.bindValue(":toDate", historyQuery.toDate().toString("yyyyMMdd"));

	QVector<HistoryQueryResult> dates;
	executeQuery(query);

	Chat lastChat;
	QDate lastDate;
	QString lastTitle;
	int lastCount = 0;

	while (query.next())
	{
		int count = query.value(0).toInt();

		QString dateString = query.value(1).toString();
		QDate date = QDate::fromString(dateString, "yyyyMMdd");
		if (!date.isValid())
			continue;

		auto message = HtmlString{query.value(2).toString()};
		if (message.string().isEmpty())
			continue;

		Chat chat = ChatsMapping->chatById(query.value(3).toInt());
		Chat buddyChat = m_buddyChatManager->buddyChat(chat);
		chat = buddyChat ? buddyChat : chat;

		if (chat != lastChat || date != lastDate)
		{
			if (lastChat)
			{
				HistoryQueryResult result;
				result.setTalkable(lastChat);
				result.setDate(lastDate);
				result.setTitle(lastTitle);
				result.setCount(lastCount);
				dates.append(result);
			}

			// TODO: this should be done in different place

			QString title;
			if (m_formattedStringFactory)
			{
				auto formattedString = m_formattedStringFactory.data()->fromHtml(message);

				FormattedStringPlainTextVisitor plainTextVisitor;
				formattedString->accept(&plainTextVisitor);

				title = plainTextVisitor.result().replace('\n', ' ').replace('\r', ' ');
			}
			else
				title = QString{message.string()}.replace('\n', ' ').replace('\r', ' ');

			if (title.length() > DATE_TITLE_LENGTH)
			{
				title.truncate(DATE_TITLE_LENGTH);
				title += " ...";
			}

			lastChat = chat;
			lastDate = date;
			lastTitle = title;
			lastCount = count;
		}
		else
			lastCount += count;
	}

	if (lastChat)
	{
		HistoryQueryResult result;
		result.setTalkable(lastChat);
		result.setDate(lastDate);
		result.setTitle(lastTitle);
		result.setCount(lastCount);
		dates.append(result);
	}

	return dates;
}

QFuture<QVector<HistoryQueryResult > > HistorySqlStorage::chatDates(const HistoryQuery &historyQuery)
{
	return QtConcurrent::run(this, &HistorySqlStorage::syncChatDates, historyQuery);
}

QVector<HistoryQueryResult> HistorySqlStorage::syncStatusDates(const HistoryQuery &historyQuery)
{
	const Talkable &talkable = historyQuery.talkable();

	if (!waitForDatabase())
		return QVector<HistoryQueryResult>();

	QMutexLocker locker(&DatabaseMutex);

	QSqlQuery query(Database);
	QString queryString = "SELECT count(1), substr(set_time,0,11), contact_id, description FROM";
	queryString += " (SELECT set_time, contact_id, description FROM kadu_statuses WHERE " + talkableContactsWhere(talkable);

	if (!historyQuery.string().isEmpty())
		queryString += " AND kadu_statuses.description LIKE :query";
	if (historyQuery.fromDate().isValid())
		queryString += " AND replace(substr(set_time,0,11), '-', '') >= :fromDate";
	if (historyQuery.toDate().isValid())
		queryString += " AND replace(substr(set_time,0,11), '-', '') <= :toDate";

	queryString += " ORDER BY set_time DESC, rowid DESC)";
	queryString += " GROUP BY substr(set_time,0,11), contact_id ORDER BY set_time ASC";

	query.prepare(queryString);

	if (!historyQuery.string().isEmpty())
		query.bindValue(":query", QString("%%%1%%").arg(historyQuery.string()));
	if (historyQuery.fromDate().isValid())
		query.bindValue(":fromDate", historyQuery.fromDate().toString("yyyyMMdd"));
	if (historyQuery.toDate().isValid())
		query.bindValue(":toDate", historyQuery.toDate().toString("yyyyMMdd"));

	QVector<HistoryQueryResult> dates;

	executeQuery(query);

	QDate date;
	while (query.next())
	{
		date = query.value(1).toDate();
		if (!date.isValid())
			continue;

		HistoryQueryResult result;

		Contact contact = ContactsMapping->contactById(query.value(2).toInt());
		if (contact)
		{
			const Buddy &buddy = m_buddyManager->byContact(contact, ActionCreate);
			result.setTalkable(Talkable(buddy));
		}
		else
		{
			const Buddy &buddy = m_buddyStorage->create();
			buddy.setDisplay("?");

			contact = m_contactStorage->create();
			contact.setOwnerBuddy(buddy);

			result.setTalkable(Talkable(contact));
		}

		result.setDate(date);
		result.setTitle(query.value(3).toString().replace('\n', ' ').replace('\r', ' '));
		result.setCount(query.value(0).toInt());
		dates.append(result);
	}

	return dates;
}

QFuture<QVector<HistoryQueryResult>> HistorySqlStorage::statusDates(const HistoryQuery &historyQuery)
{
	return QtConcurrent::run(this, &HistorySqlStorage::syncStatusDates, historyQuery);
}

QVector<HistoryQueryResult> HistorySqlStorage::syncSmsRecipientDates(const HistoryQuery &historyQuery)
{
	const Talkable &talkable = historyQuery.talkable();

	if (!waitForDatabase())
		return QVector<HistoryQueryResult>();

	QMutexLocker locker(&DatabaseMutex);

	QSqlQuery query(Database);
	QString queryString = "SELECT count(1), substr(send_time,0,11), receipient, content";
	queryString += " FROM (SELECT send_time, receipient, content FROM kadu_sms WHERE ";

	if (talkable.isValidBuddy() && !m_talkableConverter->toBuddy(talkable).mobile().isEmpty())
		queryString += "receipient = :receipient";
	else
		queryString += "1";

	if (!historyQuery.string().isEmpty())
		queryString += " AND kadu_sms.content LIKE :query";
	if (historyQuery.fromDate().isValid())
		queryString += " AND replace(substr(send_time,0,11), '-', '') >= :fromDate";
	if (historyQuery.toDate().isValid())
		queryString += " AND replace(substr(send_time,0,11), '-', '') <= :toDate";

	queryString += " order by send_time DESC, rowid DESC)";
	queryString += " group by substr(send_time,0,11), receipient order by send_time ASC;";

	query.prepare(queryString);

	if (talkable.isValidBuddy() && !m_talkableConverter->toBuddy(talkable).mobile().isEmpty())
		query.bindValue(":receipient", m_talkableConverter->toBuddy(talkable).mobile());

	if (!historyQuery.string().isEmpty())
		query.bindValue(":query", QString("%%%1%%").arg(historyQuery.string()));
	if (historyQuery.fromDate().isValid())
		query.bindValue(":fromDate", historyQuery.fromDate().toString("yyyyMMdd"));
	if (historyQuery.toDate().isValid())
		query.bindValue(":toDate", historyQuery.toDate().toString("yyyyMMdd"));

	QVector<HistoryQueryResult> dates;
	executeQuery(query);

	while (query.next())
	{
		QDate date = query.value(1).toDate();
		if (!date.isValid())
			continue;

		HistoryQueryResult result;

		Buddy buddy = m_buddyStorage->create();
		buddy.setDisplay(query.value(2).toString());
		buddy.setMobile(query.value(2).toString());

		result.setTalkable(Talkable(buddy));
		result.setDate(date);
		result.setTitle(query.value(3).toString().replace('\n', ' ').replace('\r', ' '));
		result.setCount(query.value(0).toInt());
		dates.append(result);
	}

	return dates;
}

QFuture<QVector<HistoryQueryResult>> HistorySqlStorage::smsRecipientDates(const HistoryQuery &historyQuery)
{
	return QtConcurrent::run(this, &HistorySqlStorage::syncSmsRecipientDates, historyQuery);
}

SortedMessages HistorySqlStorage::syncMessages(const HistoryQuery &historyQuery)
{
	if (!waitForDatabase())
		return SortedMessages();

	QMutexLocker locker(&DatabaseMutex);

	const Talkable &talkable = historyQuery.talkable();

	QSqlQuery query(Database);
	QString queryString = "SELECT chat_id, contact_id, kmc.content, send_time, receive_time, is_outgoing FROM kadu_messages "
			"LEFT JOIN kadu_dates d ON (kadu_messages.date_id=d.id) "
			"LEFT JOIN kadu_message_contents kmc ON (kadu_messages.content_id=kmc.id) WHERE 1";

	if (!talkable.isEmpty())
		queryString += QString(" AND chat_id IN %1").arg(chatIdList(m_talkableConverter->toChat(talkable)));
	if (historyQuery.fromDate().isValid())
		queryString += " AND date >= :fromDate";
	if (historyQuery.toDate().isValid())
		queryString += " AND date <= :toDate";
	if (historyQuery.fromDateTime().isValid())
		queryString += " AND receive_time >= :fromDateTime";
	if (historyQuery.toDateTime().isValid())
		queryString += " AND receive_time <= :toDateTime";

	// we want last *limit* messages, so we have to invert sorting here
	// it is reverted back manually below
	if (historyQuery.limit() > 0)
	{
		queryString += " ORDER BY date DESC, kadu_messages.rowid DESC";
		queryString += " LIMIT :limit";
	}
	else
		queryString += " ORDER BY date ASC, kadu_messages.rowid DESC";

	query.prepare(queryString);

	if (historyQuery.fromDate().isValid())
		query.bindValue(":fromDate", historyQuery.fromDate().toString("yyyyMMdd"));
	if (historyQuery.toDate().isValid())
		query.bindValue(":toDate", historyQuery.toDate().toString("yyyyMMdd"));
	if (historyQuery.fromDateTime().isValid())
		query.bindValue(":fromDateTime", historyQuery.fromDateTime().toString(Qt::ISODate));
	if (historyQuery.toDateTime().isValid())
		query.bindValue(":toDateTime", historyQuery.toDateTime().toString(Qt::ISODate));

	if (historyQuery.limit() > 0)
		query.bindValue(":limit", historyQuery.limit());

	executeQuery(query);
	return messagesFromQuery(query);
}

QFuture<SortedMessages> HistorySqlStorage::messages(const HistoryQuery &historyQuery)
{
	return QtConcurrent::run(this, &HistorySqlStorage::syncMessages, historyQuery);
}

SortedMessages HistorySqlStorage::syncStatuses(const HistoryQuery &historyQuery)
{
	const Talkable &talkable = historyQuery.talkable();

	if (!waitForDatabase())
		return SortedMessages();

	QMutexLocker locker(&DatabaseMutex);

	QSqlQuery query(Database);
	QString queryString = "SELECT contact_id, status, description, set_time FROM kadu_statuses WHERE " + talkableContactsWhere(talkable);

	if (historyQuery.fromDate().isValid())
		queryString += " AND replace(substr(set_time,0,11), '-', '') >= :fromDate";
	if (historyQuery.toDate().isValid())
		queryString += " AND replace(substr(set_time,0,11), '-', '') <= :toDate";

	queryString += " ORDER BY set_time ASC";

	SortedMessages statuses;
	query.prepare(queryString);

	if (historyQuery.fromDate().isValid())
		query.bindValue(":fromDate", historyQuery.fromDate().toString("yyyyMMdd"));
	if (historyQuery.toDate().isValid())
		query.bindValue(":toDate", historyQuery.toDate().toString("yyyyMMdd"));

	executeQuery(query);
	statuses = statusesFromQuery(m_talkableConverter->toContact(talkable), query);

	return statuses;
}

QFuture<SortedMessages> HistorySqlStorage::statuses(const HistoryQuery &historyQuery)
{
	return QtConcurrent::run(this, &HistorySqlStorage::syncStatuses, historyQuery);
}

SortedMessages HistorySqlStorage::syncSmses(const HistoryQuery &historyQuery)
{
	const Talkable &talkable = historyQuery.talkable();

	if (!waitForDatabase())
		return SortedMessages();

	QMutexLocker locker(&DatabaseMutex);

	QSqlQuery query(Database);
	QString queryString = "SELECT content, send_time FROM kadu_sms WHERE 1";

	if (talkable.isValidBuddy() && !m_talkableConverter->toBuddy(talkable).mobile().isEmpty())
		queryString += " AND receipient = :receipient";
	if (historyQuery.fromDate().isValid())
		queryString += " AND replace(substr(send_time,0,11), '-', '') >= :fromDate";
	if (historyQuery.toDate().isValid())
		queryString += " AND replace(substr(send_time,0,11), '-', '') <= :toDate";

	queryString += " ORDER BY send_time ASC";

	query.prepare(queryString);

	if (talkable.isValidBuddy() && !m_talkableConverter->toBuddy(talkable).mobile().isEmpty())
		query.bindValue(":receipient", m_talkableConverter->toBuddy(talkable).mobile());

	if (historyQuery.fromDate().isValid())
		query.bindValue(":fromDate", historyQuery.fromDate().toString("yyyyMMdd"));
	if (historyQuery.toDate().isValid())
		query.bindValue(":toDate", historyQuery.toDate().toString("yyyyMMdd"));

	executeQuery(query);

	SortedMessages result = smsFromQuery(query);

	return result;
}

QFuture<SortedMessages> HistorySqlStorage::smses(const HistoryQuery &historyQuery)
{
	return QtConcurrent::run(this, &HistorySqlStorage::syncSmses, historyQuery);
}

void HistorySqlStorage::executeQuery(QSqlQuery &query)
{
	kdebugf();

	query.setForwardOnly(true);

	QDateTime before = QDateTime::currentDateTime();
	query.exec();
	QDateTime after = QDateTime::currentDateTime();
	kdebugm(KDEBUG_INFO, "db query: %s\n", qPrintable(query.executedQuery()));

/*
	printf("[%s]\n[%d.%d]-[%d.%d]/%d.%d\n", qPrintable(query.executedQuery()),
			before.toTime_t(), before.time().msec(),
			after.toTime_t(), after.time().msec(),
			after.toTime_t() - before.toTime_t(),
			after.time().msec() - before.time().msec());
*/
}

SortedMessages HistorySqlStorage::messagesFromQuery(QSqlQuery &query)
{
	if (!m_formattedStringFactory)
		return {};

	auto messages = std::vector<Message>{};
	while (query.next())
	{
		bool outgoing = query.value(5).toBool();

		MessageType type = outgoing ? MessageTypeSent : MessageTypeReceived;

		// allow displaying messages for non-existing contacts
		Contact sender = ContactsMapping->contactById(query.value(1).toInt());
		if (!sender)
		{
			Contact sender = m_contactStorage->create();
			Buddy senderBuddy = m_buddyStorage->create();
			senderBuddy.setDisplay("?");
			sender.setOwnerBuddy(senderBuddy);
		}

		auto message = m_messageStorage->create();
		message.setMessageChat(ChatsMapping->chatById(query.value(0).toInt()));
		message.setType(type);
		message.setMessageSender(sender);
		message.setContent(normalizeHtml(HtmlString{query.value(2).toString()}));
		message.setSendDate(query.value(3).toDateTime());
		message.setReceiveDate(query.value(4).toDateTime());
		if (outgoing)
			message.setStatus(MessageStatusDelivered);

		messages.push_back(message);
	}

	// the data was queried in descending order, so we need to reverse it
	std::reverse(messages.begin(), messages.end());

	return SortedMessages{messages};
}

SortedMessages HistorySqlStorage::statusesFromQuery(const Contact &contact, QSqlQuery &query)
{
	if (!m_formattedStringFactory)
		return {};

	auto statuses = std::vector<Message>{};
	while (query.next())
	{
		StatusType type = m_statusTypeManager->fromName(query.value(1).toString());
		const StatusTypeData &typeData = m_statusTypeManager->statusTypeData(type);

		auto message = m_messageStorage->create();

		auto description = query.value(2).toString();
		auto htmlContent = description.isEmpty()
				? normalizeHtml(plainToHtml(typeData.name()))
				: normalizeHtml(plainToHtml(QString("%1 with description: %2").arg(typeData.name()).arg(description)));

		message.setContent(htmlContent);
		message.setType(MessageTypeSystem);
		message.setMessageSender(contact);
		message.setReceiveDate(query.value(3).toDateTime());
		message.setSendDate(query.value(3).toDateTime());

		statuses.push_back(message);
	}

	return SortedMessages{statuses};
}

SortedMessages HistorySqlStorage::smsFromQuery(QSqlQuery &query)
{
	if (!m_formattedStringFactory)
		return {};

	auto messages = std::vector<Message>{};
	while (query.next())
	{
		auto message = m_messageStorage->create();
		message.setType(MessageTypeSystem);
		message.setReceiveDate(query.value(1).toDateTime());
		message.setSendDate(query.value(1).toDateTime());
		message.setContent(normalizeHtml(plainToHtml(query.value(0).toString())));

		messages.push_back(message);
	}

	return SortedMessages{messages};
}

HistoryMessagesStorage * HistorySqlStorage::chatStorage()
{
	return m_historyChatStorage;
}

HistoryMessagesStorage * HistorySqlStorage::smsStorage()
{
	return SmsStorage;
}

HistoryMessagesStorage * HistorySqlStorage::statusStorage()
{
	return StatusStorage;
}

#include "moc_history-sql-storage.cpp"
