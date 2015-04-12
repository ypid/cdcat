/****************************************************************************
*                            Hyper's CD Catalog
*               A multiplatform qt and xml based catalog program
*
*  Author    : Christoph Thielecke (crissi99@gmx.de)
*  License   : GPL
*  Copyright : (C) 2003 Christoph Thielecke
****************************************************************************/
#include "info.h"

#include <QDebug>
#include <QVariant>
#include <QPushButton>
#include <QTabWidget>
#include <QWidget>
#include <QLabel>
#include <QTextBrowser>
#include <QLayout>
#include <QToolTip>
#include <QGridLayout>
#include <QLocale>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QHeaderView>

#include <iostream>

#include "cdcat.h"
#include "icons.h"
#include "mustache.h"


/*
 * Constructs a InfoDialog as a child of 'parent', with the
 * name 'name' and widget flags set to 'f'.
 *
 * The dialog will by default be modeless, unless you set 'modal' to
 * true to construct a modal dialog.
 */
InfoDialog::InfoDialog ( QWidget *parent, const char *name, bool modal, Qt::WindowFlags fl )
    : QDialog( parent, fl ) {

    if (!name) {
        setObjectName( "helpDialog" );
    }
    setModal( modal );
    setWindowIcon( *get_t_about_icon());

    helpDialogLayout = new QGridLayout( this );

    closeButton = new QPushButton( this );

    helpDialogLayout->addWidget( closeButton, 1, 0 );

    tabWidget2 = new QTabWidget( this );

    tab = new QWidget( tabWidget2 );
    tabLayout = new QGridLayout( tab );

    textBrowser2 = new QTextBrowser( tab );

    tabLayout->addWidget( textBrowser2, 0, 0 );
    tabWidget2->insertTab( 0, tab, QString( "" ));

    tab_2 = new QWidget( tabWidget2 );
    tabLayout_2 = new QGridLayout( tab_2 );

    textBrowser1 = new QTextBrowser( tab_2 );

    tabLayout_2->addWidget( textBrowser1, 0, 0 );
    tabWidget2->insertTab( 1, tab_2, QString( "" ));

    TabPage = new QWidget( tabWidget2 );
    TabPageLayout = new QGridLayout( TabPage );

    textThanx = new QTextBrowser( TabPage );

    TabPageLayout->addWidget( textThanx, 0, 0 );
    tabWidget2->insertTab( 2, TabPage, QString( "" ));

    helpDialogLayout->addWidget( tabWidget2, 0, 0 );

    /******************************************************************************************/

    infotext += "<center>";
    infotext += "<h1>";
    infotext += PROGRAM_NAME;
    infotext += "</h1>";
    infotext += HOMEPAGE;
    infotext += "</center>";

    /* Version {{{ */
    infotext += "<h2>";
    infotext += tr( "Version:" ) + " ";

#ifdef QT_DEBUG
    QString DEBUG_INFO = tr( " (with debug)" );
#else
    QString DEBUG_INFO = "";
#endif

#ifdef IS_RELEASE
    infotext += QString( VERSION );
#else
    infotext += QString( VERSION );
#endif

    infotext += "</h2>\n";

#ifdef IS_RELEASE
    infotext += QString( "<b>(Release" ) + ", Qt " + QString( QT_VERSION_STR ) + ")" + QString( DEBUG_INFO ) + "</b>";
#else
    infotext += "(" + tr( "Development version build at" ) + " " + __DATE__ + " " + __TIME__ + ", Qt " + QString( QT_VERSION_STR ) + ")" + DEBUG_INFO;
#endif
    /* }}} */

    /* Features {{{ */
#if defined(USE_LIB7ZIP) || defined(MEDIAINFO_STATIC) || defined(MEDIAINFO_UNICODE) || defined(USE_LIBEXIF) || defined(CATALOG_ENCRYPTION)
    infotext += "<h2>";
    infotext += tr( "Compiled with the following features:" );
    infotext += "</h2>";
    infotext += "<ul>";
#endif

#ifdef USE_LIB7ZIP
    infotext += "<li>";
    infotext += tr( "archive read support using lib7zip" );
    infotext += "</li>";
#endif

#ifdef MEDIAINFO_STATIC
    infotext += "<li>";
    infotext += tr( "mediainfo (compiled in)" );
    infotext += "</li>";
#endif

#ifdef MEDIAINFO_UNICODE
    infotext += "<li>";
    infotext += tr( "mediainfo" );
    infotext += "</li>";
#endif

#ifdef USE_LIBEXIF
    infotext += "<li>";
    infotext += tr( "exif data read support" );
    infotext += "</li>";
#endif

#ifdef CATALOG_ENCRYPTION
    infotext += "<li>";
    infotext += tr( "encrypted catalog support" );
    infotext += "</li>";
#endif

#if defined(USE_LIB7ZIP) || defined(MEDIAINFO_STATIC) || defined(MEDIAINFO_UNICODE) || defined(USE_LIBEXIF) || defined(CATALOG_ENCRYPTION)
    infotext += "</ul>";
#endif
    /* }}} */

    /* Authors {{{ */
    // TODO: Maybe link names to homepages?
    infotext += "<h2>";
    infotext += tr( "Authors" );
    infotext += "</h2>";
    infotext += "<table cellspacing=\"10\">";
    infotext += formatAuthorTR("Robin Schneider"    , "ypid@riseup.net"   , "2015");
    infotext += formatAuthorTR("Christoph Thielecke", "crissi99@gmx.de"   , "2010–2014");
    infotext += formatAuthorTR("Peter Deak"         , "hyperr@freemail.hu", "2003");
    infotext += "</table>";
    /* }}} */

    /* Maintainer {{{ */
    infotext += "<h2>";
    infotext += tr( "Current Maintainer" );
    infotext += "</h2>";
    infotext += MAINTAINER_NAME;
    /* }}} */

    /******************************************************************************************/
    contribution += formatAuthor("Robin Schneider", "ypid@riseup.net", "2015");
    contribution += "<ul>";
    contribution += "<li>" + tr( "Ported to Qt5" ) + "</li>";
    contribution += "</ul>";

    contribution += formatAuthor("Christoph Thielecke", "crissi99@gmx.de", "2010");
    contribution += "<ul>";
    contribution += "<li>" + tr( "German translation & additional programming" ) + "</li>";
    contribution += "<li>" + tr( "Ported to Qt4" ) + "</li>";
    contribution += "</ul>";

    contribution += formatAuthor("Techno", "techno@punkt.pl", "");
    contribution += "<ul>";
    contribution += "<li>" + tr( "AVI reader plugin & Polish translation" ) + "</li>";
    contribution += "</ul>";

    contribution += formatAuthor("Rafael", "rcb6@alu.ua.es", "");
    contribution += "<ul>";
    contribution += "<li>" + tr( "Spanish translation" ) + "</li>";
    contribution += "</ul>";

    contribution += formatAuthor("Vlada ", "vladovi@altas.cz", "");
    contribution += "<ul>";
    contribution += "<li>" + tr( "Czech translation" ) + "</li>";
    contribution += "</ul>";

    /******************************************************************************************/
    license = tr( "For more details about the GPL license and to read in other languages, visit %1." ).arg( "<a href=\"http://www.gnu.org/licenses/gpl.html\">%1</a>" ).arg( tr( "GPL page on GNU website" )) + "<br><br>";
    license +=
        "<center>GNU GENERAL PUBLIC LICENSE</center>"
        "<center>Version 2, June 1991</center>"
        "<center>Copyright (C) 1989, 1991 Free Software Foundation, Inc.</center>"
        "<center>675 Mass Ave, Cambridge, MA 02139, USA</center>"
        "<center>Everyone is permitted to copy and distribute verbatim copies</center>"
        "<center>of this license document, but changing it is not allowed.</center>"
        "\n"
        "<center>Preamble</center>\n"
        "<ul>"
        "<li>The licenses for most software are designed to take away your\n"
        "freedom to share and change it.  By contrast, the GNU General Public\n"
        "License is intended to guarantee your freedom to share and change free\n"
        "software--to make sure the software is free for all its users.  This\n"
        "General Public License applies to most of the Free Software\n"
        "Foundation's software and to any other program whose authors commit to\n"
        "using it.  (Some other Free Software Foundation software is covered by\n"
        "the GNU Library General Public License instead.)  You can apply it to\n"
        "your programs, too.</li>"
        "<li>When we speak of free software, we are referring to freedom, not\n"
        "price.  Our General Public Licenses are designed to make sure that you\n"
        "have the freedom to distribute copies of free software (and charge for\n"
        "this service if you wish), that you receive source code or can get it\n"
        "if you want it, that you can change the software or use pieces of it\n"
        "in new free programs; and that you know you can do these things.</li>"
        "<li>To protect your rights, we need to make restrictions that forbid\n"
        "anyone to deny you these rights or to ask you to surrender the rights.\n"
        "These restrictions translate to certain responsibilities for you if you\n"
        "distribute copies of the software, or if you modify it.</li>"
        "<li>For example, if you distribute copies of such a program, whether\n"
        "gratis or for a fee, you must give the recipients all the rights that\n"
        "you have.  You must make sure that they, too, receive or can get the\n"
        "source code.  And you must show them these terms so they know their\n"
        "rights.</li>"
        "<li>We protect your rights with two steps: (1) copyright the software, and\n"
        "(2) offer you this license which gives you legal permission to copy,\n"
        "distribute and/or modify the software.</li>"
        "<li>Also, for each author's protection and ours, we want to make certain\n"
        "that everyone understands that there is no warranty for this free\n"
        "software.  If the software is modified by someone else and passed on, we\n"
        "want its recipients to know that what they have is not the original, so\n"
        "that any problems introduced by others will not reflect on the original\n"
        "authors' reputations.</li>"
        "<li>Finally, any free program is threatened constantly by software\n"
        "patents.  We wish to avoid the danger that redistributors of a free\n"
        "program will individually obtain patent licenses, in effect making the\n"
        "program proprietary.  To prevent this, we have made it clear that any\n"
        "patent must be licensed for everyone's free use or not licensed at all.<li>"
        "<li>The precise terms and conditions for copying, distribution and"
        "modification follow.</li>"
        "</ul>"
        "<center>GNU GENERAL PUBLIC LICENSE</center>"
        "<center>TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION</center>"
        "<ol>"
        "<li>This License applies to any program or other work which contains\n"
        "a notice placed by the copyright holder saying it may be distributed\n"
        "under the terms of this General Public License.  The \\Program\\, below,\n"
        "refers to any such program or work, and a \\work based on the Program\\\n"
        "means either the Program or any derivative work under copyright law:\n"
        "that is to say, a work containing the Program or a portion of it,\n"
        "either verbatim or with modifications and/or translated into another\n"
        "language.  (Hereinafter, translation is included without limitation in\n"
        "the term \\modification\\.)  Each licensee is addressed as \\you\\.\n"
        "\n"
        "Activities other than copying, distribution and modification are not\n"
        "covered by this License; they are outside its scope.  The act of\n"
        "running the Program is not restricted, and the output from the Program\n"
        "is covered only if its contents constitute a work based on the\n"
        "Program (independent of having been made by running the Program).\n"
        "Whether that is true depends on what the Program does.</li>"
        "<li>You may copy and distribute verbatim copies of the Program's\n"
        "source code as you receive it, in any medium, provided that you\n"
        "conspicuously and appropriately publish on each copy an appropriate\n"
        "copyright notice and disclaimer of warranty; keep intact all the\n"
        "notices that refer to this License and to the absence of any warranty;\n"
        "and give any other recipients of the Program a copy of this License\n"
        "along with the Program.\n"
        "\n"
        "You may charge a fee for the physical act of transferring a copy, and\n"
        "you may at your option offer warranty protection in exchange for a fee.</li>"
        "<li>You may modify your copy or copies of the Program or any portion\n"
        "of it, thus forming a work based on the Program, and copy and\n"
        "distribute such modifications or work under the terms of Section 1\n"
        "above, provided that you also meet all of these conditions:\n"
        "\n"
        "    a) You must cause the modified files to carry prominent notices\n"
        "    stating that you changed the files and the date of any change.\n"
        "\n"
        "    b) You must cause any work that you distribute or publish, that in\n"
        "    whole or in part contains or is derived from the Program or any\n"
        "    part thereof, to be licensed as a whole at no charge to all third\n"
        "    parties under the terms of this License.\n"
        "\n"
        "    c) If the modified program normally reads commands interactively\n"
        "    when run, you must cause it, when started running for such\n"
        "    interactive use in the most ordinary way, to print or display an\n"
        "    announcement including an appropriate copyright notice and a\n"
        "    notice that there is no warranty (or else, saying that you provide\n"
        "    a warranty) and that users may redistribute the program under\n"
        "    these conditions, and telling the user how to view a copy of this\n"
        "    License.  (Exception: if the Program itself is interactive but\n"
        "    does not normally print such an announcement, your work based on\n"
        "    the Program is not required to print an announcement.)\n"
        " \n"
        "These requirements apply to the modified work as a whole.  If\n"
        "identifiable sections of that work are not derived from the Program,\n"
        "and can be reasonably considered independent and separate works in\n"
        "themselves, then this License, and its terms, do not apply to those\n"
        "sections when you distribute them as separate works.  But when you\n"
        "distribute the same sections as part of a whole which is a work based\n"
        "on the Program, the distribution of the whole must be on the terms of\n"
        "this License, whose permissions for other licensees extend to the\n"
        "entire whole, and thus to each and every part regardless of who wrote it.\n"
        "\n"
        "Thus, it is not the intent of this section to claim rights or contest\n"
        "your rights to work written entirely by you; rather, the intent is to\n"
        "exercise the right to control the distribution of derivative or\n"
        "collective works based on the Program.\n"
        "\n"
        "In addition, mere aggregation of another work not based on the Program\n"
        "with the Program (or with a work based on the Program) on a volume of\n"
        "a storage or distribution medium does not bring the other work under\n"
        "the scope of this License.</li>"
        "<li>You may copy and distribute the Program (or a work based on it,\n"
        "under Section 2) in object code or executable form under the terms of\n"
        "Sections 1 and 2 above provided that you also do one of the following:\n"
        "\n"
        "    a) Accompany it with the complete corresponding machine-readable\n"
        "    source code, which must be distributed under the terms of Sections\n"
        "    1 and 2 above on a medium customarily used for software interchange; or,\n"
        "\n"
        "    b) Accompany it with a written offer, valid for at least three\n"
        "    years, to give any third party, for a charge no more than your\n"
        "    cost of physically performing source distribution, a complete\n"
        "    machine-readable copy of the corresponding source code, to be\n"
        "    distributed under the terms of Sections 1 and 2 above on a medium\n"
        "    customarily used for software interchange; or,\n"
        "\n"
        "    c) Accompany it with the information you received as to the offer\n"
        "    to distribute corresponding source code.  (This alternative is\n"
        "    allowed only for noncommercial distribution and only if you\n"
        "    received the program in object code or executable form with such\n"
        "    an offer, in accord with Subsection b above.)\n"
        "\n"
        "The source code for a work means the preferred form of the work for\n"
        "making modifications to it.  For an executable work, complete source\n"
        "code means all the source code for all modules it contains, plus any\n"
        "associated interface definition files, plus the scripts used to\n"
        "control compilation and installation of the executable.  However, as a\n"
        "special exception, the source code distributed need not include\n"
        "anything that is normally distributed (in either source or binary\n"
        "form) with the major components (compiler, kernel, and so on) of the\n"
        "operating system on which the executable runs, unless that component\n"
        "itself accompanies the executable.\n"
        "\n"
        "If distribution of executable or object code is made by offering\n"
        "access to copy from a designated place, then offering equivalent\n"
        "access to copy the source code from the same place counts as\n"
        "distribution of the source code, even though third parties are not\n"
        "compelled to copy the source along with the object code.</li>"
        "<li>You may not copy, modify, sublicense, or distribute the Program\n"
        "except as expressly provided under this License.  Any attempt\n"
        "otherwise to copy, modify, sublicense or distribute the Program is\n"
        "void, and will automatically terminate your rights under this License.\n"
        "However, parties who have received copies, or rights, from you under\n"
        "this License will not have their licenses terminated so long as such\n"
        "parties remain in full compliance.</li>"
        "<li>You are not required to accept this License, since you have not\n"
        "signed it.  However, nothing else grants you permission to modify or\n"
        "distribute the Program or its derivative works.  These actions are\n"
        "prohibited by law if you do not accept this License.  Therefore, by\n"
        "modifying or distributing the Program (or any work based on the\n"
        "Program), you indicate your acceptance of this License to do so, and\n"
        "all its terms and conditions for copying, distributing or modifying\n"
        "the Program or works based on it.</li>"
        "<li>Each time you redistribute the Program (or any work based on the\n"
        "Program), the recipient automatically receives a license from the\n"
        "original licensor to copy, distribute or modify the Program subject to\n"
        "these terms and conditions.  You may not impose any further\n"
        "restrictions on the recipients' exercise of the rights granted herein.\n"
        "You are not responsible for enforcing compliance by third parties to\n"
        "this License.</li>"
        "<li>If, as a consequence of a court judgment or allegation of patent\n"
        "infringement or for any other reason (not limited to patent issues),\n"
        "conditions are imposed on you (whether by court order, agreement or\n"
        "otherwise) that contradict the conditions of this License, they do not\n"
        "excuse you from the conditions of this License.  If you cannot\n"
        "distribute so as to satisfy simultaneously your obligations under this\n"
        "License and any other pertinent obligations, then as a consequence you\n"
        "may not distribute the Program at all.  For example, if a patent\n"
        "license would not permit royalty-free redistribution of the Program by\n"
        "all those who receive copies directly or indirectly through you, then\n"
        "the only way you could satisfy both it and this License would be to\n"
        "refrain entirely from distribution of the Program.\n"
        "\n"
        "If any portion of this section is held invalid or unenforceable under\n"
        "any particular circumstance, the balance of the section is intended to\n"
        "apply and the section as a whole is intended to apply in other\n"
        "circumstances.\n"
        "\n"
        "It is not the purpose of this section to induce you to infringe any\n"
        "patents or other property right claims or to contest validity of any\n"
        "such claims; this section has the sole purpose of protecting the\n"
        "integrity of the free software distribution system, which is\n"
        "implemented by public license practices.  Many people have made\n"
        "generous contributions to the wide range of software distributed\n"
        "through that system in reliance on consistent application of that\n"
        "system; it is up to the author/donor to decide if he or she is willing\n"
        "to distribute software through any other system and a licensee cannot\n"
        "impose that choice.\n"
        "\n"
        "This section is intended to make thoroughly clear what is believed to\n"
        "be a consequence of the rest of this License.</li>"
        "<li>If the distribution and/or use of the Program is restricted in\n"
        "certain countries either by patents or by copyrighted interfaces, the\n"
        "original copyright holder who places the Program under this License\n"
        "may add an explicit geographical distribution limitation excluding\n"
        "those countries, so that distribution is permitted only in or among\n"
        "countries not thus excluded.  In such case, this License incorporates\n"
        "the limitation as if written in the body of this License.</li>"
        "<li>The Free Software Foundation may publish revised and/or new versions\n"
        "of the General Public License from time to time.  Such new versions will\n"
        "be similar in spirit to the present version, but may differ in detail to\n"
        "address new problems or concerns.\n"
        "\n"
        "Each version is given a distinguishing version number.  If the Program\n"
        "specifies a version number of this License which applies to it and \\any\n"
        "later version\\, you have the option of following the terms and conditions\n"
        "either of that version or of any later version published by the Free\n"
        "Software Foundation.  If the Program does not specify a version number of\n"
        "this License, you may choose any version ever published by the Free Software\n"
        "Foundation.</li>"
        "<li>If you wish to incorporate parts of the Program into other free\n"
        "programs whose distribution conditions are different, write to the author\n"
        "to ask for permission.  For software which is copyrighted by the Free\n"
        "Software Foundation, write to the Free Software Foundation; we sometimes\n"
        "make exceptions for this.  Our decision will be guided by the two goals\n"
        "of preserving the free status of all derivatives of our free software and\n"
        "of promoting the sharing and reuse of software generally.\n"
        "\n"
        "<center>NO WARRANTY</center></li>"
        "<li>BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY\n"
        "FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW.  EXCEPT WHEN\n"
        "OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES\n"
        "PROVIDE THE PROGRAM \\AS IS\\ WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED\n"
        "OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF\n"
        "MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE ENTIRE RISK AS\n"
        "TO THE QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU.  SHOULD THE\n"
        "PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING,\n"
        "REPAIR OR CORRECTION.<li>"
        "<li>IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING\n"
        "WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MAY MODIFY AND/OR\n"
        "REDISTRIBUTE THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES,\n"
        "INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING\n"
        "OUT OF THE USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED\n"
        "TO LOSS OF DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY\n"
        "YOU OR THIRD PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER\n"
        "PROGRAMS), EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE\n"
        "POSSIBILITY OF SUCH DAMAGES.</li>"
        "</ul>"
        "<center>END OF TERMS AND CONDITIONS</center>"
        "\n";

    /******************************************************************************************/

    languageChange();
    resize( QSize( 600, 550 ).expandedTo( minimumSizeHint()));

    // signals and slots connections
    connect( closeButton, SIGNAL( clicked()), this, SLOT( close()));
}

/*
 *  Destroys the object and frees any allocated resources
 */
InfoDialog::~InfoDialog() {
    // no need to delete child widgets, Qt does it all for us
}

QString InfoDialog::formatAuthor(QString name, QString email, QString year) {
    QVariantHash author;

    author["name"]  = name;
    author["email"] = email;
    author["year"]  = year;

    QString contactTemplate = "<h2>"
        "<b>{{name}}</b> "
        "&lt;<a href=\"mailto:{{email}}\">{{email}}</a>&gt;"
        // ", {{year}}"
        "</h2>";

    Mustache::Renderer renderer;
    Mustache::QtVariantContext context(author);

    return renderer.render(contactTemplate, &context);
}

QString InfoDialog::formatAuthorTR(QString name, QString email, QString year) {
    QVariantHash author;

    author["name"]  = name;
    author["email"] = email;
    author["year"]  = year;

    QString contactTemplate = "<tr>"
        "<td>Copyright (C) {{year}}</td>"
        "<td><b>{{name}}</b></td>"
        "<td>&lt;<a href=\"mailto:{{email}}\">{{email}}</a>&gt;</td>"
        "</tr>";

    Mustache::Renderer renderer;
    Mustache::QtVariantContext context(author);

    return renderer.render(contactTemplate, &context);
}


/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void InfoDialog::languageChange() {
    setWindowTitle( tr( "About %1" ).arg(PROGRAM_NAME));
    closeButton->setText( tr( "close" ));
    textBrowser2->setHtml( infotext );
    textBrowser2->setOpenExternalLinks( true );
    tabWidget2->setTabText( 0, tr( "About %1" ).arg(PROGRAM_NAME) );
    textBrowser1->setHtml( license );
    textBrowser1->setOpenExternalLinks( true );
    tabWidget2->setTabText( 1, tr( "License" ));
    textThanx->setText( contribution );
    tabWidget2->setTabText( 2, tr( "Thanks" ));
}

KeyBindingDialog::KeyBindingDialog ( GuiSlave *gs, QWidget *parent, const char *name, bool modal, Qt::WindowFlags fl )
    : QDialog( parent, fl ) {

    if (!name) {
        setObjectName( "Key bindings" );
    }

    setModal( modal );
    setWindowIcon( *get_t_about_icon());

    keyBindingDialogLayout = new QGridLayout( this );

    TitleLabel = new QLabel( this );
    TitleLabel->setText( "<h1>" + tr( "Key bindings" ) + "</h1>" );
    keyBindingDialogLayout->addWidget( TitleLabel, 0, 0 );

    KeyBindingsTableWidget = new QTableWidget( gs->KeyShortCutList.size(), 2, this );
    QTableWidgetItem *valuesHeaderItem1 = new QTableWidgetItem( tr( "Action" ));
    KeyBindingsTableWidget->setHorizontalHeaderItem( 0, valuesHeaderItem1 );
    QTableWidgetItem *valuesHeaderItem2 = new QTableWidgetItem( tr( "Shortcut" ));
    KeyBindingsTableWidget->setHorizontalHeaderItem( 1, valuesHeaderItem2 );

    QStandardItemModel model( gs->KeyShortCutList.size(), 2 );
    for (int i = 0; i < gs->KeyShortCutList.size(); i++) {
        QTableWidgetItem *newItem1 = new QTableWidgetItem( gs->KeyShortCutList.at( i ).description );
        newItem1->setFlags( newItem1->flags() & ~Qt::ItemIsEditable );
        KeyBindingsTableWidget->setItem( i, 0, newItem1 );
        QTableWidgetItem *newItem2 = new QTableWidgetItem( gs->KeyShortCutList.at( i ).eventSequence.toString());
        newItem2->setFlags( newItem2->flags() & ~Qt::ItemIsEditable );
        KeyBindingsTableWidget->setItem( i, 1, newItem2 );
    }
    KeyBindingsTableWidget->resizeColumnsToContents();
    KeyBindingsTableWidget->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeToContents );
    KeyBindingsTableWidget->setSortingEnabled( true );
    KeyBindingsTableWidget->sortByColumn( 0, Qt::AscendingOrder );
    KeyBindingsTableWidget->verticalHeader()->hide();
    keyBindingDialogLayout->addWidget( KeyBindingsTableWidget, 2, 0 );

     DEBUG_INFO_ENABLED = init_debug_info();
     if ( *DEBUG_INFO_ENABLED && gs->KeyShortCutList.size() > 0) {
             qDebug() << "KeyShortCutList:" << gs->KeyShortCutList.size();
             for (int i = 0; i < gs->KeyShortCutList.size(); i++) {
                 qDebug("  %s: %s", qPrintable(gs->KeyShortCutList.at(i).description), qPrintable(gs->KeyShortCutList.at(i).eventSequence.toString()));
             }
     }

    closeButton = new QPushButton( this );
    keyBindingDialogLayout->addWidget( closeButton, 4, 0 );

    // force correct size of table view
    // int width = (KeyBindingsTableWidget->model()->columnCount()) + KeyBindingsTableWidget->verticalHeader()->width() + 10;
    int width = (KeyBindingsTableWidget->model()->columnCount()) + 20;
    for (int column = 0; column < KeyBindingsTableWidget->model()->columnCount(); column++)
        width = width + KeyBindingsTableWidget->columnWidth( column );
    KeyBindingsTableWidget->setMinimumWidth( width );

    int height = (KeyBindingsTableWidget->model()->rowCount()) + KeyBindingsTableWidget->verticalHeader()->height() + 10;
    for (int row = 0; row < KeyBindingsTableWidget->model()->rowCount(); row++)
        height = height + KeyBindingsTableWidget->rowHeight( row );
    // KeyBindingsTableWidget->setMinimumHeight(height);
    KeyBindingsTableWidget->setMinimumHeight( 550 );
    resize( minimumSizeHint());
    languageChange();
    connect( closeButton, SIGNAL( clicked()), this, SLOT( close()));
}

KeyBindingDialog::~KeyBindingDialog() {
}

void KeyBindingDialog::languageChange() {
    setWindowTitle( tr( "Key bindings" ));
    closeButton->setText( tr( "close" ));
}
