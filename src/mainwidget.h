/****************************************************************************
*                            Hyper's CD Catalog
*               A multiplatform qt and xml based catalog program
*
*  Author    : Peter Deak (hyperr@freemail.hu)
*  License   : GPL
*  Copyright : (C) 2003 Peter Deak
****************************************************************************/

#ifndef CDCATMAINWIDGET_H
#define CDCATMAINWIDGET_H

#include <QVariant>
#include <QPixmap>
#include <qwidget.h>
#include <QApplication>
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

#include "adddialog.h"

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
class QScrollArea;

class CdCatMainWidget : public QMainWindow {
    Q_OBJECT
public:
    CdCatMainWidget ( CdCatConfig *ccp, QApplication *appp, QWidget *parent = 0, const char *name = 0, Qt::WindowFlags fl = 0 );
    ~CdCatMainWidget();

    CdCatConfig *cconfig;
    GuiSlave *guis;

    DataBase *db;
    CommentWidget *commentWidget;
    QToolBar *Toolbar;
    QDockWidget *CommentDock;
    QMenuBar *mainMenu;
    HDirectoryView *DirView;
    HQListView *listView;
    QApplication *app;
    QMenu *historyMenu;
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *viewMenu;
    QMenu *findMenu;
    QMenu *inoutMenu;
    QMenu *othersMenu;
    QMenu *helpMenu;
    QSplitter *splitMain;
    QWidget *main;
    PWw *pww;
    void setDockSize( QDockWidget *dock, int, int );

protected:
    QVBoxLayout *lvMain;
    void closeEvent( QCloseEvent *e );
    QString lastScanPath;             // last scanned path, used for restore if extrainfo changes

protected slots:
    virtual void languageChange();
    void toolBarToogled();
    void statusBarToogled();
    void commentWidgetToogled();
    void dockToogled();

public slots:
    void pathScanned( QString path );
    void extraInfoAppend( QString extraInfo );
    void setTrayToolTipInfo( QString tooltipinfo );
    void setTrayText( QString text, QString header = tr("processing file"));

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
    QScrollArea *scrollArea;

public:
    QSystemTrayIcon *trayIcon;
    QAction *new_action;
    QAction *open_action;
    QAction *save_action;
    QAction *saveas_action;
#ifdef CATALOG_ENCRYPTION
    QAction *changepass_action;
    QAction *disableencryption_action;
    QAction *enableencryption_action;
#endif
    // QAction *history_action;
    QAction *close_action;
    QAction *quit_action;
    QAction *add_action;
    QAction *addlink_action;
    QAction *rescan_action;
    QAction *insert_action;
    QAction *insert_cdcat_xml_action;
    QAction *rename_action;
    QAction *renumber_action;
    // QAction *changemediatype_action;
    QAction *delete_action;
    QAction *sortnu_ascending_action;
    QAction *sortnu_descending_action;
    QAction *sortna_ascending_action;
    QAction *sortna_descending_action;
    QAction *sortti_ascending_action;
    QAction *sortti_descending_action;
    QAction *sortty_ascending_action;
    QAction *sortty_descending_action;
    QAction *find_action;
    QAction *findpanel_action;
    QAction *borrow_action;
    QAction *size_action;
    QAction *config_action;
    QAction *color_action;
    QAction *import_action;
    QAction *export_action;
    QAction *help_action;
    QAction *about_action;
    QAction *aboutqt_action;
    QAction *minimizeAction;
    QAction *maximizeAction;
    QAction *restoreAction;
    QAction *quitAction;
    QAction *view_toolbar_action;
    QAction *view_statusbar_action;
    QAction *view_comment_widget_action;
    QAction *view_tray_action;
    QAction *keybindings_action;
    QMenu *trayIconMenu;
    void createTrayIcon();
    void startTrayIconAnim();
    void stopTrayIconAnim();

private slots:
    void trayIconActivated( QSystemTrayIcon::ActivationReason reason );
    void showTrayMessage();
    void trayIconMessageClicked();
    void trayIconAnimateTimerSlot();
    void returnToOldMaxMinSizes();

signals:
    void minimizedToTray();
    void restoredFromTray();
};

#endif // CDCATMAINWIDGET_H
