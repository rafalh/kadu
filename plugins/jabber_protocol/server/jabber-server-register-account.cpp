/*
 * %kadu copyright begin%
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "debug.h"

#include "gui/windows/message-dialog.h"
#include "jabber-server-register-account.h"

JabberServerRegisterAccount::JabberServerRegisterAccount(const QString &server, const QString &username, const QString &password, bool legacySSLProbe, bool legacySSL, bool forceSSL, const QString &host, quint16 port)
	: QObject(), Result(0), Server(server), Username(username), Password(password)
{
	Q_UNUSED(legacySSLProbe);
	Q_UNUSED(legacySSL);
	Q_UNUSED(forceSSL);
	Q_UNUSED(host);
	Q_UNUSED(port);
	/*
	Client = new MiniClient;
	connect(Client, SIGNAL(handshaken()), SLOT(clientHandshaken()));
	connect(Client, SIGNAL(error()), SLOT(clientError()));
	Client->connectToServer(Jid(Server), legacySSLProbe, legacySSL, forceSSL, host, port);*/
}

void JabberServerRegisterAccount::clientHandshaken()
{/*
	kdebugf();
	// try to register an account
	JT_Register *reg = new JT_Register(Client->client()->rootTask());
	connect(reg, SIGNAL(finished()), SLOT(sendRegistrationData()));
	reg->getForm(Server);
	reg->go(true);
	kdebugf();*/
}

void JabberServerRegisterAccount::clientError()
{
	kdebugf();
	Result = false;
	emit finished(this);
	kdebugf2();
}

void JabberServerRegisterAccount::sendRegistrationData()
{/*
	kdebugf();
	JT_Register *reg = (JT_Register *)sender();
	if (reg->success()) {
		XData xdata;
	//TODO: upewnić się, że to to jest potrzebne tak jak jest
		bool old;
		if (reg->hasXData()) {
			old = false;
			xdata = reg->xdata();
		}
		else {
			old = true;
			xdata = convertToXData(reg->form());
		}
	//
		JT_Register *reg = new JT_Register(Client->client()->rootTask());
		connect(reg, SIGNAL(finished()), this, SLOT(actionFinished()));
		if (old) {
			Form form = convertFromXData(fields);
			form.setJid(Server);
			reg->setForm(form);
		}
		else {
			reg->setForm(Server,fields);
		}
		reg->go(true);

	}
	else {
		Result = false;
		emit finished(this);
		MessageDialog::show(KaduIcon("dialog-warning"), tr("Kadu"), tr("This server does not support registration"));
	}
	kdebugf2();*/
}
/*
XData JabberServerRegisterAccount::convertToXData(const Form& form)
{
	kdebugf();
	// Convert the fields
	XData::FieldList fields;
	foreach (const FormField &f, form) {
		XData::Field field;
		field.setLabel(f.fieldName());
		field.setVar(f.realName());
		field.setRequired(true);
		if (f.isSecret()) {
			field.setType(XData::Field::Field_TextPrivate);
		}
		else {
			field.setType(XData::Field::Field_TextSingle);
		}
		fields.push_back(field);
	}

	// Create the form
	XData xdata;
	xdata.setInstructions(form.instructions());
	xdata.setFields(fields);
	kdebugf2();
	return xdata;
}

Form JabberServerRegisterAccount::convertFromXData(const XData& xdata)
{
	kdebugf();
	Form form;
	foreach(const XData::Field &field, xdata.fields()) {
		if (!field.value().isEmpty()) {
			FormField f;
			f.setType(field.var());
			f.setValue(field.value().at(0));
			form.push_back(f);
		}
	}
	kdebugf2();
	return form;
}
*/
void JabberServerRegisterAccount::actionFinished()
{/*
	kdebugf();
	JT_Register *reg = (JT_Register *)sender();
	if (reg->success()) {
		Client->close();
		Result = true;
		emit finished(this);
	}
	else {
		Result = false;
		MessageDialog::show(KaduIcon("dialog-warning"), tr("Kadu"), tr("There was an error registering the account.\nReason: %1").arg(reg->statusString()));
		emit finished(this);
	}
	kdebugf2();*/
}

void JabberServerRegisterAccount::performAction()
{/*
	kdebugf();
	XData::FieldList fs;

	XData::Field username;
	username.setLabel("Username");
	username.setVar("username");
	username.setValue(QStringList(Username));
	username.setRequired(true);
	username.setType(XData::Field::Field_TextSingle);
	fs.push_back(username);

	Jid = Jid(Username, Server).bare();

	XData::Field pass;
	pass.setLabel("password");
	pass.setVar("password");
	pass.setValue(QStringList(Password));
	pass.setRequired(true);
	pass.setType(XData::Field::Field_TextPrivate);
	fs.push_back(pass);

	fields.setFields(fs);
	kdebugf2();*/
}


#include "moc_jabber-server-register-account.cpp"
