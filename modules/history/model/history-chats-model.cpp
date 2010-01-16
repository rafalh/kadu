/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include "chat/chat.h"
#include "chat/type/chat-type-manager.h"
#include "model/roles.h"

#include "history-chats-model.h"

HistoryChatsModel::HistoryChatsModel(QObject *parent) :
		QAbstractItemModel(parent)
{
	triggerAllChatTypesRegistered();
}

HistoryChatsModel::~HistoryChatsModel()
{
	triggerAllChatTypesUnregistered();
}

void HistoryChatsModel::chatTypeRegistered(ChatType *chatType)
{
	if (ChatKeys.contains(chatType))
		return;

	beginInsertRows(QModelIndex(), Chats.size(), Chats.size());
	ChatKeys.append(chatType);
	Chats.insert(ChatKeys.size() - 1, QList<Chat>());
	endInsertRows();
}

void HistoryChatsModel::chatTypeUnregistered(ChatType *chatType)
{
	if (!ChatKeys.contains(chatType))
		return;

	int index = ChatKeys.indexOf(chatType);
	beginRemoveRows(QModelIndex(), index, index);
	Chats.removeAt(index);
	ChatKeys.removeAt(index);
	endRemoveRows();
}

int HistoryChatsModel::columnCount(const QModelIndex &parent) const
{
	return 1;
}

int HistoryChatsModel::rowCount(const QModelIndex &parent) const
{
	if (!parent.isValid())
		return ChatKeys.size() + 2;

	if (parent.parent().isValid())
		return 0;

	if (parent.row() < 0 || parent.row() >= Chats.size())
		return 0;

	return Chats[parent.row()].size();
}

QModelIndex HistoryChatsModel::index(int row, int column, const QModelIndex &parent) const
{
	if (!parent.isValid())
		return createIndex(row, column, -1); // ROOT

	return createIndex(row, column, parent.row());
}

QModelIndex HistoryChatsModel::parent(const QModelIndex &child) const
{
	if (-1 == child.internalId())
		return QModelIndex();

	return createIndex(child.internalId(), 0, -1);
}

QVariant HistoryChatsModel::chatTypeData(const QModelIndex &index, int role) const
{
	if (index.row() < 0 )
		return QVariant();

	if (index.row() >= ChatKeys.count())
		return otherData(index.row() - ChatKeys.count(), role);

	ChatType *chatType = ChatKeys.at(index.row());
	switch (role)
	{
		case Qt::DisplayRole:
			return chatType->displayName();

		case Qt::DecorationRole:
			return chatType->icon();

		case ChatTypeRole:
			return QVariant::fromValue<ChatType *>(chatType);
	}

	return QVariant();
}

QVariant HistoryChatsModel::chatData(const QModelIndex &index, int role) const
{
	if (index.internalId() < 0 || index.internalId() >= Chats.size())
		return QVariant();

	const QList<Chat> &chats = Chats[index.internalId()];
	if (index.row() < 0 || index.row() >= chats.size())
		return QVariant();

	Chat chat = chats.at(index.row());

	switch (role)
	{
		case Qt::DisplayRole:
			return chat.name();

		case ChatRole:
			return QVariant::fromValue<Chat>(chat);
	}

	return QVariant();
}

QVariant HistoryChatsModel::otherData(int index, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();

	switch (index)
	{
		case 0: return tr("Statuses");
		case 1: return tr("SMS");
	}
}

QVariant HistoryChatsModel::data(const QModelIndex &index, int role) const
{
	if (index.parent().isValid())
		return chatData(index, role);
	else
		return chatTypeData(index, role);
}

void HistoryChatsModel::clear()
{
	int count = Chats.size();
	for (int i = 0; i < count; i++)
	{
		beginRemoveRows(index(i, 0), 0, rowCount(index(i, 0)));
		Chats[i].clear();
		endRemoveRows();
	}
}

void HistoryChatsModel::addChat(Chat chat)
{
	ChatType *chatType = ChatTypeManager::instance()->chatType(chat.type());
	if (!chatType)
		return;

	int id = ChatKeys.indexOf(chatType);

	QModelIndex idx = index(id, 0, QModelIndex());
	int count = rowCount(idx);

	beginInsertRows(idx, count, count);
	Chats[id].append(chat);
	endInsertRows();
}

void HistoryChatsModel::addChats(QList<Chat> chats)
{
	foreach (Chat chat, chats)
		addChat(chat);
}

QModelIndex HistoryChatsModel::chatTypeIndex(ChatType *type) const
{
	int row = ChatKeys.indexOf(type);
	if (row < 0)
		return QModelIndex();

	return index(row, 0, QModelIndex());
}

QModelIndex HistoryChatsModel::chatIndex(Chat chat) const
{
	QString typeName = chat.type();
	ChatType *chatType = ChatTypeManager::instance()->chatType(typeName);
	if (!chatType)
		return QModelIndex();

	if (!ChatKeys.contains(chatType))
		return QModelIndex();

	QModelIndex typeIndex = chatTypeIndex(chatType);
	if (!typeIndex.isValid())
		return QModelIndex();

	int row = Chats[typeIndex.row()].indexOf(chat);
	return index(row, 0, typeIndex);
}
