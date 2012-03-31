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
//Added by qt3to4:
#include <QGridLayout>
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QCloseEvent>
#include <QSystemTrayIcon>
#include <QList>
#include <QTimer>



class Q3VBoxLayout;
class Q3HBoxLayout;
class QGridLayout;
class QFrame;
class Q3ListView;
class Q3ListViewItem;
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

class CdCatMainWidget : public QWidget {
		Q_OBJECT
	public:
		CdCatMainWidget ( CdCatConfig *ccp, QApplication *appp, QWidget *parent = 0, const char *name = 0, Qt::WFlags fl = 0 );
		~CdCatMainWidget();

		CdCatConfig  *cconfig;
		GuiSlave     *guis;

		DataBase      *db;
		CommentWidget *commentWidget;
		QFrame *Toolbar;
		QMenuBar   *mainMenu;
		QToolButton *ButtonOpen;
		QToolButton *ButtonSave;
		QToolButton *ButtonSaveAs;
		QToolButton *ButtonClose;
		QToolButton *ButtonAdd;
		QToolButton *ButtonRescan;
		QToolButton *ButtonDelete;
		QToolButton *ButtonConfig;
		QToolButton *ButtonHelp;
		QToolButton *ButtonNew;
		QToolButton *ButtonFind;
		QToolButton *ButtonAbout;
		QToolButton *ButtonAboutQt;
		HDirectoryView *DirView;
		HQListView *listView;
		QLabel *statusl;
		QApplication *app;
		QMenu *historyMenu;
		QSplitter *splitMain;

	protected:
		Q3HBoxLayout *lToolbar;
		Q3VBoxLayout *lvMain;
		void closeEvent ( QCloseEvent *e );
		QString lastScanPath; // last scanned path, used for restore if extrainfo changes

	protected slots:
		virtual void languageChange();

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

	public:
		QSystemTrayIcon *trayIcon;
		QAction *minimizeAction;
		QAction *maximizeAction;
		QAction *restoreAction;
		QAction *quitAction;
		QMenu *trayIconMenu;
		void createTrayIcon();
		void startTrayIconAnim();
		void stopTrayIconAnim();
	
	private slots:
		void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
		void showTrayMessage();
		void trayIconMessageClicked();
		void trayIconAnimateTimerSlot();
	
	signals:
		void minimizedToTray();
		void restoredFromTray();

};

#endif // CDCATMAINWIDGET_H
