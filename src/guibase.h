/****************************************************************************
                             Hyper's CD Catalog
		A multiplatform qt and xml based catalog program

 Author    : Peter Deak (hyperr@freemail.hu)
 License   : GPL
 Copyright : (C) 2003 Peter Deak
****************************************************************************/

#ifndef GUIBASE_CDCAT
#define GUIBASE_CDCAT


#include <qobject.h>
#include <qdialog.h>
#include <q3listview.h>
#include <q3popupmenu.h>
//Added by qt3to4:
#include <QKeyEvent>

class Node;
class CdCatMainWidget;
class QKeyEvent;
class QLineEdit;
class QWidget;
class QComboBox;
class QLabel;
class QPushButton;
class Q3VBoxLayout;
class QPainter;
class DataBase;
class HQListViewItem;


class GuiSlave : public QObject {
    Q_OBJECT

public:
    CdCatMainWidget *mainw;
    Node *standON;
    Node *NodePwd;
    Node *tmpParent;

    Q3PopupMenu *mPopup;

    GuiSlave ( CdCatMainWidget *p );
    Node *getNodeFromFullName ( Node *root,const QString& newloc );
    int updateListFromNode ( Node *pdir = 0 );
    int isIdentical ( int i );
    int isIdentical ( QString q );
    bool haveContent ( Node *node );
    void updateStatusl ( Node *n );
    void checkversion ( QWidget *p,DataBase *db );

public slots:

    int hotKeys ( QKeyEvent *ke );
    int listUpdate ( const QString& newloc );
    int standOn ( Q3ListViewItem *on );
    int doubleClickOn ( Q3ListViewItem *on );
    void showListviewContextMenu ( Q3ListViewItem *, const QPoint &, int );
    void showTreeContextMenu ( Q3ListViewItem *, const QPoint &, int );

    int cHcaption ( void );

    void panelsOFF ( void ); /*I do this if the user do something on the database*/
    void panelsON ( void );

    /*Mainwinow Buttons*/
    int newEvent ( void );
    int openEvent ( void );
    int saveEvent ( void );
    int saveasEvent ( void );
    int closeEvent ( void );
    int deleteEvent ( void );
    int addEvent ( void );
    int rescanEvent ( void );
    int findEvent ( void );
    int configEvent ( void );
    int aboutEvent ( void );
    int aboutQtEvent ( void );
    int helpEvent ( void );
    int insertcEvent ( void );
    int renameEvent ( void );
    int typeChangeEvent( void );
    int sizeEvent ( void );
    int importEvent ( void );
    int exportEvent ( void );
    int renumberEvent ( void );
    int borrowingEvent ( void );
    int sborrowEvent ( void );
    int cborrowEvent ( void );

    int sortNuEvent ( void );
    int sortNaEvent ( void );
    int sortTyEvent ( void );
    int sortTiEvent ( void );
    int closeBranch ( void );
    int editComment ( void );
    int showContent ( void );
    int editCategory ( void );
    int colorEvent ( void );
    int followLnk ( void );
    int addlnkEvent ( void );

    int searchDuplicatesEvent( void );

    int posEvent ( void );

    int openHistoryElementEvent ( int id );

private:
	HQListViewItem *context_item;

};

/**********************************************************************/
class HQListViewItem : public Q3ListViewItem {
public:
    int etype;
    HQListViewItem ( Q3ListView *parent );
    HQListViewItem ( Q3ListView *parent,QString label1,QString label2,QString label3 );
    HQListViewItem ( Q3ListView *parent,Q3ListViewItem *after,QString label1,QString label2,QString label3 );

//   virtual int compare(QListViewItem *i,int col,bool ascending) const;
    virtual QString key ( int column,bool ascending ) const;
protected:
    void paintCell ( QPainter *p,const QColorGroup & cg,int column,int width,int align );
};

class HQListView : public Q3ListView {
    Q_OBJECT
public:
    CdCatMainWidget *mainw;
    HQListView ( CdCatMainWidget *mw,QWidget *parent=0,const char *name=0,Qt::WFlags f=0 );
    void start ( void );
    void curr_vis ( void );
    void changed ( void );

protected:
    void keyPressEvent ( QKeyEvent *ke );

    //This is need for memoryze the current sorting state.
    //It is'nt necessary under QT 3.X becouse there is sorting query function,
    //but I didn't found it in QT 2.3.0 (Non-Commercial for Win)
public:
    virtual void setSorting ( int column, bool increasing = TRUE );
    int  scol;
    bool sasc;
};

//**********************************************************************
class QPosDialog : public QDialog {
    Q_OBJECT

public:
    QLineEdit *le;
    CdCatMainWidget *p;
    QPosDialog ( CdCatMainWidget *parent );

public slots:
    int pos ( const QString & str );
protected:
    void keyPressEvent ( QKeyEvent *ke );
private:
    int maxp,minp;
};

class CatalogTypeEditDialog : public QDialog {
	Q_OBJECT

public:
	CatalogTypeEditDialog (CdCatMainWidget *parent, Node *n);
	QLabel *TextLabel;
	QComboBox *cbType;
	QPushButton *buttonOK;
	QPushButton *buttonCancel;
	Q3VBoxLayout* layout1;
	bool changeOk;

public slots:
	void languageChange();
	void okExit();
	void cancel();
	void cbTypeToggeled(int index);

private:
	Node *n;
};



#endif



