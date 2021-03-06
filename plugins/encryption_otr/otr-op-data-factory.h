/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class Contact;

class OtrErrorMessageService;
class OtrFingerprintService;
class OtrInstanceTagService;
class OtrIsLoggedInService;
class OtrMessageEventService;
class OtrMessageService;
class OtrOpData;
class OtrPeerIdentityVerificationService;
class OtrPolicyService;
class OtrPrivateKeyService;
class OtrSessionService;
class OtrTimerService;
class OtrTrustLevelService;

class OtrOpDataFactory : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE OtrOpDataFactory();
	virtual ~OtrOpDataFactory();

	OtrOpData opData();
	OtrOpData opDataForContact(const Contact &contact);

private slots:
	INJEQT_SET void setErrorMessageService(OtrErrorMessageService *errorMessageService);
	INJEQT_SET void setFingerprintService(OtrFingerprintService *fingerprintService);
	INJEQT_SET void setInstanceTagService(OtrInstanceTagService *instanceTagService);
	INJEQT_SET void setIsLoggedInService(OtrIsLoggedInService *isLoggedInService);
	INJEQT_SET void setMessageEventService(OtrMessageEventService *messageEventService);
	INJEQT_SET void setMessageService(OtrMessageService *messageService);
	INJEQT_SET void setPeerIdentityVerificationService(OtrPeerIdentityVerificationService *peerIdentityVerificationService);
	INJEQT_SET void setPolicyService(OtrPolicyService *policyService);
	INJEQT_SET void setPrivateKeyService(OtrPrivateKeyService *privateKeyService);
	INJEQT_SET void setSessionService(OtrSessionService *sessionService);
	INJEQT_SET void setTimerService(OtrTimerService *timerService);
	INJEQT_SET void setTrustLevelService(OtrTrustLevelService *trustLevelService);

private:
	QPointer<OtrErrorMessageService> ErrorMessageService;
	QPointer<OtrFingerprintService> FingerprintService;
	QPointer<OtrInstanceTagService> InstanceTagService;
	QPointer<OtrIsLoggedInService> IsLoggedInService;
	QPointer<OtrMessageEventService> MessageEventService;
	QPointer<OtrMessageService> MessageService;
	QPointer<OtrPeerIdentityVerificationService> PeerIdentityVerificationService;
	QPointer<OtrPolicyService> PolicyService;
	QPointer<OtrPrivateKeyService> PrivateKeyService;
	QPointer<OtrSessionService> SessionService;
	QPointer<OtrTimerService> TimerService;
	QPointer<OtrTrustLevelService> TrustLevelService;

};
