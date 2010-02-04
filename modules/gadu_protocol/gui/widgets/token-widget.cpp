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

#include <QtGui/QFrame>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMovie>
#include <QtGui/QPushButton>

#include "server/token-fetcher.h"
#include "icons-manager.h"

#include "token-widget.h"

TokenWidget::TokenWidget(QWidget *parent) : QWidget(parent)
{
	WaitMovie = new QMovie(IconsManager::instance()->iconPath("PleaseWait"));

	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	TokenImage = new QLabel(this);
	TokenImage->setFixedHeight(32);
	TokenImage->setScaledContents(true);

	QPushButton *refreshButton = new QPushButton(IconsManager::instance()->loadIcon("ReloadPersonalInfoButton"), "" ,this);
	refreshButton->setIconSize(IconsManager::instance()->getIconsSize());
	connect(refreshButton, SIGNAL(clicked()), this, SLOT(refreshToken()));

	QFrame *tokenFrame = new QFrame(this);
	tokenFrame->setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);

	QHBoxLayout *frameLayout = new QHBoxLayout(tokenFrame);
	frameLayout->setContentsMargins(0, 0, 0, 0);
	frameLayout->addWidget(refreshButton);
	frameLayout->addWidget(TokenImage);

	TokenCode = new QLineEdit(this);
	connect(TokenCode, SIGNAL(textChanged(QString)), this, SIGNAL(modified()));

	QHBoxLayout *mainLayout = new QHBoxLayout(this);
	frameLayout->setContentsMargins(0, 0, 0, 0);
	mainLayout->addWidget(tokenFrame);
	mainLayout->addWidget(TokenCode);

	Fetcher = new TokenFetcher(false, this);
	connect(Fetcher, SIGNAL(tokenFetched(const QString &, QPixmap)), this, SLOT(tokenFetched(const QString &, QPixmap)));
	refreshToken();
}

TokenWidget::~TokenWidget()
{
	if (WaitMovie)
		delete WaitMovie;
}

void TokenWidget::tokenFetched(const QString &tokenId, QPixmap tokenImage)
{
	WaitMovie->stop();
	TokenImage->setMovie(0);

	TokenImage->setPixmap(tokenImage);
	TokenId = tokenId;
}

void TokenWidget::refreshToken()
{
	WaitMovie->start();
	TokenImage->setMovie(WaitMovie);

	Fetcher->fetchToken();
}

QString TokenWidget::tokenId()
{
	return TokenId;
}

QString TokenWidget::tokenValue()
{
	return TokenCode->text();
}
