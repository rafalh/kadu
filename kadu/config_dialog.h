#ifndef CONFIG_DIALOG_H
#define CONFIG_DIALOG_H

#include <qapplication.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qfileinfo.h>
#include <qgrid.h>
#include <qhbox.h>
#include <qhgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qrect.h>
#include <qslider.h>
#include <qspinbox.h>
#include <qstringlist.h>
#include <qtabdialog.h>
#include <qvaluelist.h>
#include <qvgroupbox.h>


class HotKey : public QLineEdit
{
	public:
		HotKey::HotKey(QWidget *parent =0, const char* name =0);
		static QString keyEventToString(QKeyEvent *e);    
		static QKeySequence shortCutFromFile(const QString &name);
		static bool shortCut(QKeyEvent *e, const QString &name);    
	protected:
		virtual void keyPressEvent(QKeyEvent *e);
		virtual void keyReleaseEvent(QKeyEvent *e);

};



/**
	Okno z konfiguracją Kadu
**/
class ConfigDialog : public QTabDialog	{
	Q_OBJECT

	private:

		enum RegisteredControlType
		{
			CONFIG_CHECKBOX,
			CONFIG_COMBOBOX,
			CONFIG_GRID,
			CONFIG_HBOX,
			CONFIG_HGROUPBOX,
			CONFIG_HOTKEYEDIT,
			CONFIG_LABEL,
			CONFIG_LINEEDIT,
			CONFIG_LISTBOX,
			CONFIG_PUSHBUTTON,
			CONFIG_SLIDER,
			CONFIG_SPINBOX,
			CONFIG_TAB,
			CONFIG_VBOX,
			CONFIG_VGROUPBOX
		};

		struct ElementConnections
		{
			QString signal;
			QObject* receiver;
			QString slot;
		};

		struct RegisteredControl
		{
			RegisteredControlType type;
			QString parent;
			QString name;
			QString caption;
			QString group;
			QString entry;
			QString defaultS;
			QString tip;
			QWidget* widget;
			int nrOfControls;
			QValueList <ElementConnections> ConnectedSlots;
		};

		static QValueList <RegisteredControl> RegisteredControls;
		static QValueList <ElementConnections> SlotsOnCreate;
		static QValueList <ElementConnections> SlotsOnDestroy;
		static QApplication* appHandle;
		

		static int findTab(const int startpos);
		static int findTab(const QString& groupname, const int startpos=0);
		static int findPreviousTab(const int startpos=0);
		static int addControl(const QString& groupname,const RegisteredControl& control);
		static void updateNrOfControls(const int startpos, const int endpos, const QString& parent);

	public:
		ConfigDialog(QApplication *application, QWidget *parent = 0, const char *name = 0);
		~ConfigDialog();
		static void showConfigDialog(QApplication* application);

		
 		static void addCheckBox(const QString& groupname, 
		    	    const QString& parent, const QString& caption,
			    const QString& entry, const bool defaultS=false, const QString &tip="", const QString& name="");
			    			    
		static void addComboBox(const QString& groupname, 
			    const QString& parent, const QString& caption,
			    const QString& tip="", const QString& name="");
			    
		static void addGrid(const QString& groupname, 
			    const QString& parent, const QString& caption, const int nrColumns=3, const QString& name="");

		static void addHGroupBox(const QString& groupname, 
			    const QString& parent, const QString& caption, const QString& name="");
			    
		static void addHotKeyEdit(const QString& groupname, 
			    const QString& parent, const QString& caption,
			    const QString& entry, const QString& defaultS="", const QString& tip="", const QString& name="");
			    
		static void addLabel(const QString& groupname, 
			    const QString& parent, const QString& caption, const QString& name="");
			    
		static void addLineEdit(const QString& groupname,
			    const QString& parent, const QString& caption,
			    const QString& entry, const QString& defaultS="", const QString& tip="",const QString& name="");
			    
		static void addListBox(const QString& groupname, 
			    const QString& parent, const QString& caption, const QString& tip= "", const QString& name="");
			    
		static void addPushButton(const QString& groupname, 
			    const QString& parent, const QString& caption,
			    const QString& iconFileName="", const QString& tip="", const QString& name="");
			    
		static void addSlider(const QString& groupname, 
			    const QString& parent, const QString& caption,
			    const QString& entry,
			    const int minValue=0, const int maxValue=100,
			    const int pageStep=1, const int value=50, const QString& tip="", const QString& name="");
			    
		static void addSpinBox(const QString& groupname, 
			    const QString& parent, const QString& caption,
			    const QString& entry,
			    const int minValue=0, const int maxValue=100, const int step=1,
			    const int value=50, const QString& tip="", const QString& name="");
			    
		static void registerTab(const QString& caption);
		
		static void addVGroupBox(const QString& groupname,
			    const QString& parent, const QString& caption, const QString& name="");

		static void connectSlot(const QString& groupname, const QString& caption, const char* signal,
			    const QObject* receiver, const char* slot,const QString& name="");

		static void registerSlotOnCreate(const QObject* receiver, const char* slot);
		static void registerSlotOnDestroy(const QObject* receiver, const char* slot);
		

		static QCheckBox*   getCheckBox(const QString& groupname, const QString& caption, const QString& name="");
		static QComboBox*   getComboBox(const QString& groupname, const QString& caption, const QString& name="");
		static QGrid*       getGrid(const QString& groupname, const QString& caption, const QString& name="");
		static QHGroupBox*  getHGroupBox(const QString& groupname, const QString& caption, const QString& name="");
		static HotKey* 	    getHotKeyEdit(const QString& groupname, const QString& caption, const QString& name="");
		static QLineEdit*   getLineEdit(const QString& groupname, const QString& caption, const QString& name="");
		static QLabel*      getLabel(const QString& groupname, const QString& caption, const QString& name="");
		static QListBox*    getListBox(const QString& groupname, const QString& caption, const QString& name="");
		static QPushButton* getPushButton(const QString& groupname, const QString& caption, const QString& name="");
		static QSlider*     getSlider(const QString& groupname, const QString& caption, const QString& name="");
		static QSpinBox*    getSpinBox(const QString& groupname, const QString& caption, const QString& name="");
		static QVGroupBox*  getVGroupBox(const QString& groupname, const QString& caption, const QString& name="");
		static QWidget*     getWidget(const QString& groupname, const QString& caption, const QString& name="");

		static int existControl(const QString& groupname, const QString& caption, const QString& name="");
		
		//
	protected:

		static ConfigDialog *configdialog;
		static QString acttab;

	signals:
		void create();
		void destroy();


	protected slots:
		void updateConfig();
};
#endif
