#ifndef SPELLCHECKER_H
#define SPELLCHECKER_H

#include <QStringList>
#include <QMap>

#include "configuration_aware_object.h"
#include "main_configuration_window.h"

#ifdef HAVE_ASPELL
class AspellSpeller;
class AspellConfig;
typedef QMap<QString, AspellSpeller *> Checkers;
#else
namespace enchant 
{
	class Dict;
}
typedef QMap<QString, enchant::Dict*> Checkers;
#endif
class ChatWidget;

class QListWidgetItem;

class SpellChecker : public ConfigurationUiHandler, ConfigurationAwareObject
{
    Q_OBJECT

#ifdef HAVE_ASPELL
	AspellConfig *spellConfig;
#endif
	Checkers checkers;

	QListWidget *availList;
	QListWidget *checkList;

	void import_0_5_0_Configuration();
	void createDefaultConfiguration();

private slots:
	void configurationWindowApplied();

protected:
	virtual void configurationUpdated();

public:
	SpellChecker();
	virtual ~SpellChecker();
	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

	QStringList notCheckedLanguages();
	QStringList checkedLanguages();
	bool addCheckedLang(QString& name);
	void removeCheckedLang(QString& name);
	void buildMarkTag();
	bool buildCheckers();
	bool checkWord(QString word);

public slots:
	void chatCreated(ChatWidget *chatWidget);
	void configForward();
	void configBackward();
	void configForward2(QListWidgetItem *item);
	void configBackward2(QListWidgetItem *item);

};

extern SpellChecker* spellcheck;

#endif
