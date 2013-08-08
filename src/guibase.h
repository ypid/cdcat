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
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QMenu>
#include <QKeyEvent>

class Node;
class CdCatMainWidget;
class QKeyEvent;
class QLineEdit;
class QWidget;
class QComboBox;
class QLabel;
class QPushButton;
class QVBoxLayout;
class QPainter;
class DataBase;
class HQListViewItem;

/**********************************************************************/
class HQListViewItem : public QTreeWidgetItem {
public:
    int etype;
    HQListViewItem ( QTreeWidget *parent );
    HQListViewItem ( QTreeWidget *parent, QString label1, QString label2, QString label3 );
    //HQListViewItem ( QTreeWidget *parent, QTreeWidgetItem *after,QString label1,QString label2,QString label3 );

//   virtual int compare(QListViewItem *i,int col,bool ascending) const;
    QString key ( int column,bool ascending ) const;
    bool operator < (const QTreeWidgetItem &other) const;
};

class HQListView : public QTreeWidget {
    Q_OBJECT
public:
    CdCatMainWidget *mainw;
    HQListView ( CdCatMainWidget *mw,QWidget *parent=0,const char *name=0,Qt::WFlags f=0 );
    void start ( void );
    void setCurrentVisible ( void );
    void changed ( void );

protected:
    void keyPressEvent ( QKeyEvent *ke );

    //This is need for memoryze the current sorting state.
    //It is'nt necessary under QT 3.X becouse there is sorting query function,
    //but I didn't found it in QT 2.3.0 (Non-Commercial for Win)
public:
    void setSorting ( int column, bool increasing = TRUE );
    int  scol;
    bool sasc;
};

class GuiSlave : public QObject {
    Q_OBJECT

public:
    CdCatMainWidget *mainw;
    Node *standON;
    Node *NodePwd;
    Node *tmpParent;

    QMenu *mPopup;

    GuiSlave ( CdCatMainWidget *p );
    ~GuiSlave();
    Node *getNodeFromFullName ( Node *root,const QString& newloc );
    int updateListFromNode ( Node *pdir = 0 );
    bool isIdentical ( int i );
    bool isIdentical ( QString q );
    bool haveContent ( Node *node );
    void updateStatusl ( Node *n );
    void checkversion ( QWidget *p,DataBase *db );
    void setGuiMenuAndToolBarEnabled(bool enable=true);

public slots:

    int hotKeys ( QKeyEvent *ke );
    int listUpdate ( const QString& newloc );
    int standOn ( QTreeWidgetItem *on, int col );
    int doubleClickOn ( QTreeWidgetItem *on, int col );
    void showListviewContextMenu ( QPoint );
    void showTreeContextMenu ( const QPoint );
    void currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*);

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
    int insertCdcatXmlEvent ( void );
    int insertcEventNoDup ( void );
    int renameEvent ( void );
    int typeChangeEvent( void );
    int sizeEvent ( void );
    int importEvent ( void );
    int exportEvent ( void );
    int renumberEvent ( void );
    int borrowingEvent ( void );
    int sborrowEvent ( void );
    int cborrowEvent ( void );

#ifdef CATALOG_ENCRYPTION
    int changePassEvent ( void );
    int enableEncryptionEvent ( void );
    int disableEncryptionEvent ( void );
#endif

    int sortNuEventAscending ( void );
    int sortNuEventDescending ( void );
    int sortNaEventAscending ( void );
    int sortNaEventDescending ( void );
    int sortTyEventAscending ( void );
    int sortTyEventDescending ( void );
    int sortTiEventAscending ( void );
    int sortTiEventDescending ( void );
    int closeBranch ( void );
    int editComment ( void );
    int showContent ( void );
    int editCategory ( void );
    int colorEvent ( void );
    int followLnk ( void );
    int addlnkEvent ( void );

    int searchDuplicatesEvent( void );

    int posEvent ( void );

    int openHistoryElementEvent ( QAction *action );

private:
	QTreeWidgetItem *context_item;
	QTreeWidgetItem *last_dirview_item;

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
	QVBoxLayout* layout1;
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



