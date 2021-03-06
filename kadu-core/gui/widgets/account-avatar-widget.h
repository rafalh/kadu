/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "accounts/account.h"
#include "exports.h"

#include <QtCore/QPointer>
#include <QtWidgets/QWidget>
#include <injeqt/injeqt.h>

class QLabel;
class QPushButton;

class AvatarManager;
class AvatarService;
class IconsManager;
class ProtocolsManager;

class KADUAPI AccountAvatarWidget : public QWidget
{
	Q_OBJECT

	enum
	{
		ModeChange,
		ModeRemove
	} Mode;

public:
	explicit AccountAvatarWidget(Account account, QWidget *parent = nullptr);
	virtual ~AccountAvatarWidget();

private:
	QPointer<AvatarManager> m_avatarManager;
	QPointer<IconsManager> m_iconsManager;
	QPointer<ProtocolsManager> m_protocolsManager;

	Account MyAccount;
	AvatarService *Service;

	QLabel *AvatarLabel;
	QMovie *WaitMovie;
	QPushButton *ChangePhotoButton;

	void createGui();
	void setupMode();

	void uploadAvatar(QImage avatar);
	void changeAvatar();
	void removeAvatar();

private slots:
	INJEQT_SET void setAvatarManager(AvatarManager *avatarManager);
	INJEQT_SET void setIconsManager(IconsManager *iconsManager);
	INJEQT_SET void setProtocolsManager(ProtocolsManager *protocolsManager);
	INJEQT_INIT void init();

	void avatarUpdated();

	void changeButtonClicked();
	void avatarUploaded(bool ok, QImage image);
	void serviceDestroyed();

	void protocolRegistered(ProtocolFactory *protocolFactory);
	void protocolUnregistered(ProtocolFactory *protocolFactory);

};
