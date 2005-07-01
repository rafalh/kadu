#ifndef WMAKER_DOCKING_H
#define WMAKER_DOCKING_H

#include <qlabel.h>
#include <qpixmap.h>

class WMakerTrayIcon : public QLabel
{
	Q_OBJECT

	private:
		QWidget* WMakerMasterWidget;
		
	private slots:
		void setTrayPixmap(const QPixmap& pixmap, const QString &iconName);
		void setTrayMovie(const QMovie &movie);
		void setTrayTooltip(const QString& tooltip);
		void findTrayPosition(QPoint& pos);

	protected:
		virtual void enterEvent(QEvent* e);
		virtual void mousePressEvent(QMouseEvent* e);

	public:
		WMakerTrayIcon(QWidget *parent=0, const char *name=0);
		~WMakerTrayIcon();
		void show();
};

extern WMakerTrayIcon* wmaker_tray_icon;

#endif
