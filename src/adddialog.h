/****************************************************************************
                             Hyper's CD Catalog
		A multiplatform qt and xml based catalog program

 Author    : Peter Deak (hyperr@freemail.hu)
 License   : GPL
 Copyright : (C) 2003 Peter Deak
****************************************************************************/

#ifndef ADDDIALOG_H
#define ADDDIALOG_H

#include <qvariant.h>
#include <qpixmap.h>
#include <qdialog.h>
#include <qdatetime.h>
#include <QList>
//Added by qt3to4:
#include <QGridLayout>
#include <QHBoxLayout>
#include <Q3VBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QCheckBox>
#include <QPushButton>

class Q3VBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QComboBox;
class QLabel;
class QLineEdit;
class Q3ListView;
class Q3ListViewItem;
class QPushButton;
class QSpinBox;
class QTextEdit;
class QCheckbox;
class DirectoryView;
class GuiSlave;
class QApplication;
class DataBase;

class addDialog : public QDialog {
    Q_OBJECT

public:

    addDialog ( GuiSlave *c,QWidget* parent = 0, const char* name = 0, bool modal = FALSE, Qt::WFlags fl = 0 );
    ~addDialog();

    int volumename;
    GuiSlave *caller;

    QLabel* textLabel6;
    QLabel* textLabel1;
    QLabel* textLabel2;
    QLabel* textLabel5;
    QLabel* textLabel4;
    QLabel* textLabel3;
    QLabel* textLabelCategory;
    QPushButton* buttonCancel;
    QPushButton* buttonOK;
    QPushButton* buttonPli;
    QTextEdit *teComm;
    QLineEdit* leOwner;
    QLineEdit* leCategory;
    QComboBox* cbType;
    QSpinBox* sbNumber;
    QLineEdit* leName;
    DirectoryView* dirView;
#ifndef _WIN32
    QCheckBox *cbAutoDetectAtMount;
#endif

protected:
    QGridLayout* addDialogLayout;
    Q3VBoxLayout* layout10;
    QHBoxLayout* layout11;
    Q3VBoxLayout* layout12;
    QHBoxLayout* layout9;
    Q3VBoxLayout* layout8;
    Q3VBoxLayout* layout7;
    QHBoxLayout* layout2;
    QHBoxLayout* layout3;
    QHBoxLayout* layout4;
    QGridLayout* layout5;
    QHBoxLayout* layout1;

protected slots:
    virtual void languageChange();
    int bOk();
    int bCan();
    int sread();
    void autoDetectAtMountToggled();
    void cbTypeToggeled(int type);

public slots:
	int setMediaName ( const QString & ds );

private:
    QPixmap image0;
    QPixmap image1;
    QPixmap image2;
    QPixmap image3;
    QPixmap image4;
    QPixmap image5;
    QPixmap image6;

public:
    int OK;
    int type;
    int serial;
    QString dComm,dName,dOwner,dDir, dCategory;

};


class PWw : public QWidget {
Q_OBJECT
public:
    int lastx,lasty;
    int s;
    int refreshTime;
    QApplication *appl;
    PWw ( QWidget *parent,QApplication *qapp = NULL, bool showProgress=false, long long int steps=0, QString progresstext="", bool showCancel=false );
    ~PWw();
    int begintext;
    int begincanceltext;
    int myheight;
    int mywidth;
    int baseheight;
    int fontheight;
    bool showProgress;
    bool doCancel;
    long long int steps;
    long long int progress_step;
    QString progresstext;
    QPushButton *cancelButton;

    void setProgressText ( QString progresstext );
    void setCancel(bool showCancel);
    void step ( long long int progress_step=0 );
    void end ( void );

signals:
	void cancelReceivedByUser(bool state);

protected:
    QTime t;
    QList<QPixmap> anim_list;
    void paintEvent ( QPaintEvent *pe );
    void mouseMoveEvent ( QMouseEvent *me );
    void mousePressEvent ( QMouseEvent *me );

private:
	bool showCancel;


public slots:
	void doCancelReceived(bool);
};

void progress ( PWw *p, long long int progress_step=0 );


class AddLnk : public QDialog {
    Q_OBJECT

public:
    AddLnk ( GuiSlave *c, QWidget *parent );

    bool ok;
    QLineEdit *fname;
    QPushButton *bselect;
    QPushButton *buttonOk;
    QPushButton *buttonCancel;
    QLabel *label;

    Q3VBoxLayout *vbox;

    QHBoxLayout *hbox1;
    QHBoxLayout *hbox2;

public slots:

    int sselect ( void );
    int sok ( void );
    int scancel ( void );

protected slots:
    virtual void languageChange();

protected:
    GuiSlave *caller;

};

#endif // ADDDIALOG_H
