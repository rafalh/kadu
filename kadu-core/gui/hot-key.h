#ifndef HOT_KEY_H
#define HOT_KEY_H

#include "gui/widgets/line-edit-with-clear-button.h"
#include "exports.h"

class Configuration;

class KADUAPI HotKey
{
public:
	/**
	  Pobiera skr�t klawiszowy, z pliku konfiguracyjnego
	  z grupy "groupname", o polu "name"
	 **/
	static QKeySequence shortCutFromFile(Configuration *configuration, const QString &groupname, const QString &name);

	/**
	  Sprawdza czy skr�t naci�ni�ty przy zdarzeniu QKeyEvent zgadza si�
	  ze skr�tem klawiszowym, z pliku konfiguracyjnego, o polu "name"
	 **/
	static bool shortCut(Configuration *configuration, QKeyEvent *e, const QString &groupname, const QString &name);

	static QString keyEventToString(QKeyEvent *e, QKeySequence::SequenceFormat format);

};

class KADUAPI HotKeyEdit : public LineEditWithClearButton
{
	Q_OBJECT

protected:
	virtual void keyPressEvent(QKeyEvent *e);
	virtual void keyReleaseEvent(QKeyEvent *e);

public:
	HotKeyEdit(QWidget *parent = nullptr);
	/**
	  Pobiera skr�t klawiszowy
	**/
	QString shortCutString() const;

	/**
	  Pobiera skr�t klawiszowy
	**/
	QKeySequence shortCut() const;

	/**
	  Ustawia skr�t klawiszowy
	**/
	void setShortCut(const QString &shortcut);

	/**
	  Ustawia skr�t klawiszowy
	**/
	void setShortCut(const QKeySequence &shortcut);

};

#endif // HOT_KEY_H
