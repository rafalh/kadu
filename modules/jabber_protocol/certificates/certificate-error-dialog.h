/*
 * %kadu copyright begin%
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

/*
 * Copyright (C) 2008  Remko Troncon
 * See COPYING file for the detailed license.
 */

#ifndef CERTIFICATEERRORDIALOG_H
#define CERTIFICATEERRORDIALOG_H

#include <QtCrypto>
#include <QString>

class QMessageBox;
class QPushButton;

class CertificateErrorDialog
{

		QMessageBox* messageBox_;
		QPushButton* detailsButton_;
		QPushButton* continueButton_;
		QPushButton* cancelButton_;
		QPushButton* saveButton_;
		QCA::Certificate certificate_;
		int result_;
		QCA::Validity validity_;
		QString domainOverride_;
		QString host_;
		QObject *Parent;
		QString &tlsOverrideDomain_;
		QByteArray &tlsOverrideCert_;

	public:
		CertificateErrorDialog(const QString& title, const QString& host, const QCA::Certificate& cert, int result, QCA::Validity validity, const QString &domainOverride, QObject *parent, QString &tlsOverrideDomain, QByteArray &tlsOverrideCert);

		QMessageBox* getMessageBox() {
			return messageBox_;
		}

		int exec();
};

#endif
