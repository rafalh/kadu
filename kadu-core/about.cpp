/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qtextbrowser.h>
#include <qtextedit.h>
#include <qvbox.h>
#include <qcursor.h>

#include "about.h"
#include "debug.h"
#include "icons_manager.h"
#include "kadu.h"
#include "misc.h"

class KaduLink : public QLabel
{
	public:
		KaduLink(QWidget *parent) : QLabel(parent)
		{
			setText("<a href=\"http://www.kadu.net/\">www.kadu.net</a>");
			setCursor(QCursor(Qt::PointingHandCursor));
			setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed));
		}
	protected:
		virtual void mousePressEvent(QMouseEvent *)
		{
			openWebBrowser("http://www.kadu.net/");
		}
};

About::About(QWidget *parent, const char *name) : QHBox(parent, name, WType_TopLevel | WDestructiveClose),
	layoutHelper(new LayoutHelper())
{
	kdebugf();

	// set window properties and flags
	setCaption(tr("About"));
	layout()->setResizeMode(QLayout::Minimum);
	// end set window properties and flags

	// create main QLabel widgets (icon and app info)
	QVBox *left=new QVBox(this);
	left->setMargin(10);
	left->setSpacing(10);

	QLabel *l_icon = new QLabel(left);
	l_icon->setPixmap(icons_manager->loadIcon("AboutIcon"));

	QWidget *blank = new QWidget(left);
	blank->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding));

	QVBox *center = new QVBox(this);
	center->setMargin(10);
	center->setSpacing(10);

	QHBox *texts = new QHBox(center);
	QLabel *l_info = new QLabel(texts);
	l_info->setText(QString("<span style=\"font-size: 12pt\">Kadu %1 %2<br />(c) 2001-2008 Kadu Team</span>").arg(VERSION)
			.arg(strlen(detailed_version) > 0 ? ("(" + QString(detailed_version) + ")") : QString::null));
	l_info->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));

//	QWidget *blank3 = new QWidget(texts);
//	blank3->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));

	new KaduLink(texts);
	// end create main QLabel widgets (icon and app info)

	// our TabWidget
	QTabWidget *tw_about = new QTabWidget(center);
	// end our TabWidget

	// create our info widgets
	// authors
	QTextBrowser *tb_authors = new QTextBrowser(tw_about);
	tb_authors->setFrameStyle(QFrame::NoFrame);
	tb_authors->setTextFormat(Qt::PlainText);
	tb_authors->setWordWrap(QTextEdit::NoWrap);
	tb_authors->setText(loadFile("AUTHORS"));

	// people to thank
	QTextBrowser *tb_thanks = new QTextBrowser(tw_about);
	tb_thanks->setFrameStyle(QFrame::NoFrame);
	tb_thanks->setTextFormat(Qt::PlainText);
	tb_thanks->setWordWrap(QTextEdit::NoWrap);
	tb_thanks->setText(loadFile("THANKS"));

	// license
	QTextBrowser *tb_license = new QTextBrowser(tw_about);
	tb_license->setFrameStyle(QFrame::NoFrame);
	tb_license->setTextFormat(Qt::PlainText);
	tb_license->setWordWrap(QTextEdit::NoWrap);
	tb_license->setText(loadFile("COPYING"));

	// changelog
	QTextBrowser *tb_changelog = new QTextBrowser(tw_about);
	tb_changelog->setFrameStyle(QFrame::NoFrame);
	tb_changelog->setTextFormat(Qt::PlainText);
	tb_changelog->setWordWrap(QTextEdit::NoWrap);
	tb_changelog->setText(loadFile("ChangeLog"));

	// add tabs
	tw_about->addTab(tb_authors, tr("A&uthors"));
	tw_about->addTab(tb_thanks, tr("&Thanks"));
	tw_about->addTab(tb_license, tr("&License"));
	tw_about->addTab(tb_changelog, tr("&ChangeLog"));
	// end create our info widgets

	// close button
	QHBox *bottom = new QHBox(center);
	QWidget *blank2 = new QWidget(bottom);
	blank2->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));
	QPushButton *pb_close = new QPushButton(icons_manager->loadIcon("CloseWindow"), tr("&Close"), bottom, "close");
	connect(pb_close, SIGNAL(clicked()), this, SLOT(close()));
	// end close button

	connect(kadu, SIGNAL(statusPixmapChanged(const QPixmap &, const QString &)), this, SLOT(refreshIcon(const QPixmap &)));

	layoutHelper->addLabel(l_info);
	loadGeometry(this, "General", "AboutGeometry", 0, 30, 640, 420);

	kdebugf2();
}

About::~About()
{
	kdebugf();

	saveGeometry(this, "General", "AboutGeometry");
	delete layoutHelper;

	kdebugf2();
}

void About::keyPressEvent(QKeyEvent *ke_event)
{
	if (ke_event->key() == Qt::Key_Escape)
		close();
}

void About::refreshIcon(const QPixmap &pix)
{
	setIcon(pix);
}

QString About::loadFile(const QString &name)
{
	kdebugf();

	QFile file(dataPath("kadu/" + name));
	if (!file.open(IO_ReadOnly))
	{
		kdebugm(KDEBUG_ERROR, "About::loadFile(%s) cannot open file\n", name.local8Bit().data());
		return QString::null;
	}

	QTextStream str(&file);
	str.setCodec(codec_latin2);
	QString data = str.read();
	file.close();

	kdebugf2();
	return data;
}

void About::resizeEvent(QResizeEvent * /*e*/)
{
	layoutHelper->resizeLabels();
}
