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
#include <q3popupmenu.h>
//Added by qt3to4:
#include <Q3GridLayout>
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>
#include <QLabel>
#include <Q3Frame>
#include <QCloseEvent>



class Q3VBoxLayout;
class Q3HBoxLayout;
class Q3GridLayout;
class Q3Frame;
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

class CdCatMainWidget : public QWidget {
    Q_OBJECT

public:
    CdCatMainWidget ( CdCatConfig *ccp,QApplication *appp,QWidget* parent = 0, const char* name = 0, Qt::WFlags fl = 0 );
    ~CdCatMainWidget();

    CdCatConfig  *cconfig;
    GuiSlave     *guis;

    DataBase      *db;
    CommentWidget * commentWidget;
    Q3Frame* Toolbar;
    QToolButton* ButtonOpen;
    QToolButton* ButtonSave;
    QToolButton* ButtonSaveAs;
    QToolButton* ButtonClose;
    QToolButton* ButtonAdd;
    QToolButton* ButtonRescan;
    QToolButton* ButtonDelete;
    QToolButton* ButtonConfig;
    QToolButton* ButtonHelp;
    QToolButton* ButtonNew;
    QToolButton* ButtonFind;
    QToolButton* ButtonAbout;
    QToolButton* ButtonAboutQt;
    HDirectoryView* DirView;
    HQListView* listView;
    QLabel *statusl;
    QApplication *app;
    Q3PopupMenu *historyMenu;
    QSplitter *splitMain;

protected:
    Q3HBoxLayout *lToolbar;
    Q3VBoxLayout *lvMain;
    void closeEvent ( QCloseEvent *e );

protected slots:
    virtual void languageChange();

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

};

#endif // CDCATMAINWIDGET_H
