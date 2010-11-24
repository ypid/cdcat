#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include <qvariant.h>


#include <Qt3Support/Q3MimeSourceFactory>
#include <Qt3Support/Q3TextBrowser>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_helpDialog
{
public:
    QVBoxLayout *vboxLayout;
    Q3TextBrowser *textBrowser1;
    QPushButton *closeButton;

    void setupUi(QDialog *helpDialog)
    {
        if (helpDialog->objectName().isEmpty())
            helpDialog->setObjectName(QString::fromUtf8("helpDialog"));
        helpDialog->resize(464, 354);
        vboxLayout = new QVBoxLayout(helpDialog);
        vboxLayout->setSpacing(6);
        vboxLayout->setContentsMargins(11, 11, 11, 11);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        textBrowser1 = new Q3TextBrowser(helpDialog);
        textBrowser1->setObjectName(QString::fromUtf8("textBrowser1"));

        vboxLayout->addWidget(textBrowser1);

        closeButton = new QPushButton(helpDialog);
        closeButton->setObjectName(QString::fromUtf8("closeButton"));

        vboxLayout->addWidget(closeButton);


        retranslateUi(helpDialog);
        QObject::connect(closeButton, SIGNAL(clicked()), helpDialog, SLOT(close()));

        QMetaObject::connectSlotsByName(helpDialog);
    } // setupUi

    void retranslateUi(QDialog *helpDialog)
    {
        helpDialog->setWindowTitle(QApplication::translate("helpDialog", "help", 0, QApplication::UnicodeUTF8));
        textBrowser1->setText(QApplication::translate("helpDialog", "<p align=\"center\"><font size=\"+1\"><b>Help</b></font></p><br>\n"
"\n"
"<b>What's this?</b><br>\n"
"<blockquote>\n"
"The cdcat is graphical (QT based) multiplatform (Linux/Windows) catalog program which scans the directories/drives you want and memorize the filesystem including the tags of mp3's and other data and store it in a small file.\n"
"The database is stored in a gzipped XML format, so you can hack it, or use it if necessary :-)\n"
"And the program can store the content of some specified files up to a limit size if you want. (for example: *.nfo)\n"
"</blockquote>\n"
"<br>\n"
"<b>Usage:</b><br>\n"
"<blockquote>\n"
"<i>Create a new catalog</i>: Run the <tt>New</tt> command in the catalog menu. You have to type the name of the new catalog. You can specify the default username of the media(which you scan later), and add a comment to the catalog.<br>Before the scanning select the necessary readable components in the config dialog, which can be mp3 tags content of some files or etc.  If you done this, you "
                        "can scan one of your media with <tt>Add media</tt>command in the Edit menu. In that dialog you have to specyfi the directory/or drive the media you want to add. It's recommended to specify the name and the number of the media which has to be unique. (The program always generate one identical name and number) You can label the media to a owner, if necessary.\n"
"<br>If you scanned your media, you will be able to browse in it with the browser window (like mc) , or search in it. You can save the catalog with <tt>save as</tt> command in the Catalog menu.\n"
"<br>\n"
"<br>\n"
"<i>Open an existing catalog:</i>Run the <tt>open</tt> command in the Catalog menu, and choice the file of the catalog. (*.hcf)  After the opening you will be able browse the catalog or search in it.\n"
"<br>\n"
"</blockquote>\n"
"<br>\n"
"<b>Author:</b>\n"
"<blockquote>The program was written by  Peter Deak  (hungary)<br>\n"
"E-mail: hyperr@freemail.hu\n"
"<br>\n"
"</blockquote>\n"
"<br>\n"
"<b>License:</b>\n"
"<blockquote>General Public Lice"
                        "nse (GPL)</blockquote>\n"
"<br>\n"
"<b>Homepage:</b>\n"
"<blockquote>You can read about the program and get new versions, sources etc, in the hompage of cdcat:<br><tt>http://cdcat.sourceforge.net</tt></blockquote>", 0, QApplication::UnicodeUTF8));
        closeButton->setText(QApplication::translate("helpDialog", "close", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class helpDialog: public Ui_helpDialog {};
} // namespace Ui

QT_END_NAMESPACE

class helpDialog : public QDialog, public Ui::helpDialog
{
    Q_OBJECT

public:
    helpDialog(QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WindowFlags fl = 0);
    ~helpDialog();

protected slots:
    virtual void languageChange();

};

#endif // HELPDIALOG_H
