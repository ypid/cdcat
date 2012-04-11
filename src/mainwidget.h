/****************************************************************************
                             Hyper's CD Catalog
		A multiplatform qt and xml based catalog program

 Author    : Peter Deak (hyperr@freemail.hu)
 License   : GPL
 Copyright : (C) 2003 Peter Deak
****************************************************************************/

#ifndef CDCATMAINWIDGET_H
#define CDCATMAINWIDGET_H

#include <qvariant.h>
#include <qpixmap.h>
#include <qwidget.h>
#include <qapplication.h>
#include <QMenu>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QCloseEvent>
#include <QSystemTrayIcon>
#include <QList>
#include <QTimer>
#include <QToolBar>
#include <QMainWindow>



class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QFrame;
class QToolButton;
class CommentWidget;
class HDirectoryView;
class DataBase;
class GuiSlave;
class HQListView;
class QLabel;
class CdCatConfig;
class QSplitter;
class QSystemTrayIcon;

class CdCatMainWidget : public QMainWindow{
	Q_OBJECT
	public:
		CdCatMainWidget ( CdCatConfig *ccp, QApplication *appp, QWidget *parent = 0, const char *name = 0, Qt::WFlags fl = 0 );
		~CdCatMainWidget();

		CdCatConfig  *cconfig;
		GuiSlave     *guis;

		DataBase      *db;
		CommentWidget *commentWidget;
		QToolBar *Toolbar;
		QDockWidget* CommentDock;
		QMenuBar   *mainMenu;
		HDirectoryView *DirView;
		HQListView *listView;
		QApplication *app;
		QMenu *historyMenu;
		QSplitter *splitMain;
		QWidget *main;
		void setDockSize(QDockWidget *dock, int, int);

	protected:
		QVBoxLayout *lvMain;
		void closeEvent ( QCloseEvent *e );
		QString lastScanPath; // last scanned path, used for restore if extrainfo changes

	protected slots:
		virtual void languageChange();
		void toolBarToogled();
		void statusBarToogled();
		void commentWidgetToogled();
		void dockToogled();

	public slots:
		void pathScanned ( QString path );
		void extraInfoAppend ( QString extraInfo );
	
	private:
		QPixmap image0;
		QPixmap image1;
		QPixmap image2;
		QPixmap image3;
		QPixmap image4;
		QPixmap image5;
		QPixmap image6;
		QPixmap image7;
		QPixmap image8;
		QPixmap image9;
		QPixmap image10;
		QPixmap image11;
		QPixmap image12;
		QList<QPixmap> anim_list;
		int trayIconAnimValue;
		QTimer trayIconTimer;
		QSize oldMaxSize, oldMinSize;

	public:
		QSystemTrayIcon *trayIcon;
		QAction *minimizeAction;
		QAction *maximizeAction;
		QAction *restoreAction;
		QAction *quitAction;

		QAction *view_toolbar_action;
		QAction *view_statusbar_action;
		QAction *view_comment_widget_action;
		QAction *view_tray_action;
		QMenu *trayIconMenu;
		void createTrayIcon();
		void startTrayIconAnim();
		void stopTrayIconAnim();
	
	private slots:
		void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
		void showTrayMessage();
		void trayIconMessageClicked();
		void trayIconAnimateTimerSlot();
		void returnToOldMaxMinSizes();
	
	signals:
		void minimizedToTray();
		void restoredFromTray();

};

#endif // CDCATMAINWIDGET_H
