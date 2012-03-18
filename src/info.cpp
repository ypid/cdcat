/****************************************************************************
                             Hyper's CD Catalog
		A multiplatform qt and xml based catalog program

 Author    : Christoph Thielecke (crissi99@gmx.de)
 License   : GPL
 Copyright : (C) 2003 Christoph Thielecke
****************************************************************************/
#include "info.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <qlabel.h>
#include <QTextBrowser>
#include <qlayout.h>
#include <qtooltip.h>
//Added by qt3to4:
#include <Q3GridLayout>
#include <QLocale>

#include "cdcat.h"
#include "icons.h"



/*
 *  Constructs a InfoDialog as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
InfoDialog::InfoDialog ( QWidget* parent, const char* name, bool modal, Qt::WFlags fl )
        : QDialog ( parent, name, modal, fl ) {
    if ( !name )
        setName ( "helpDialog" );
    setIcon ( *get_t_about_icon() );

    helpDialogLayout = new Q3GridLayout ( this, 1, 1, 11, 6, "helpDialogLayout" );

    closeButton = new QPushButton ( this, "closeButton" );

    helpDialogLayout->addWidget ( closeButton, 1, 0 );

    tabWidget2 = new QTabWidget ( this, "tabWidget2" );

    tab = new QWidget ( tabWidget2, "tab" );
    tabLayout = new Q3GridLayout ( tab, 1, 1, 11, 6, "tabLayout" );

    textLabel1 = new QLabel ( tab, "textLabel1" );

    tabLayout->addWidget ( textLabel1, 0, 0 );
    tabWidget2->insertTab ( tab, QString ( "" ) );

    tab_2 = new QWidget ( tabWidget2, "tab_2" );
    tabLayout_2 = new Q3GridLayout ( tab_2, 1, 1, 11, 6, "tabLayout_2" );

    textBrowser1 = new QTextBrowser ( tab_2, "textBrowser1" );

    tabLayout_2->addWidget ( textBrowser1, 0, 0 );
    tabWidget2->insertTab ( tab_2, QString ( "" ) );

    TabPage = new QWidget ( tabWidget2, "TabPage" );
    TabPageLayout = new Q3GridLayout ( TabPage, 1, 1, 11, 6, "TabPageLayout" );

    textThanx = new QTextBrowser ( TabPage, "textLabel1_2" );

    TabPageLayout->addWidget ( textThanx, 0, 0 );
    tabWidget2->insertTab ( TabPage, QString ( "" ) );

    helpDialogLayout->addWidget ( tabWidget2, 0, 0 );

    /******************************************************************************************/
    infotext = "<p align=\"center\"><font size=\"+1\"><p align=\"center\"><h1><b>";
    infotext+= tr ( "-= CdCatalog by Hyper =-" );
    infotext+="</b></h1><br>\n<font size=\"+1\">";
    infotext+=HOMEPAGE;
    infotext+="<br></font>\n<h2>";
    infotext+=tr ( "Version:" ) +" ";
#ifdef QT_DEBUG
QString DEBUG_INFO=tr(" (with debug)")+"<br>\n";
#else
QString DEBUG_INFO="";
#endif
#ifdef IS_RELEASE
    infotext+=QString ( VERSION );
#else
    infotext+=QString ( VERSION );
#endif
    infotext+="</h2>\n";

#ifdef IS_RELEASE
    infotext+= QString("<b>(Release")+", Qt "+QString ( QT_VERSION_STR ) +")"+QString(DEBUG_INFO)+"</b>";
#else
    infotext+="<b>("+tr("Development version build at")+" "+__DATE__+" "+__TIME__+", Qt "+QString ( QT_VERSION_STR ) +")"+DEBUG_INFO+"</b>";
#endif
    infotext+="<br>\n";
    infotext+="<br>\n";
    infotext+=tr ( "Author:" );
    infotext+="<br><font size=\"+2\"><b>Christoph Thielecke (crissi99@gmx.de)</b><br></font><br>\n";
    infotext+=tr ( "Copyright (C) 2003 Peter Deak (GPL)" );
    infotext+="<br>\n";
    infotext+=tr ( "Copyright (C) 2010 Christoph Thielecke (GPL)" );
    infotext+="</p></font></p>";

    /******************************************************************************************/
    contribution  =   "<br><p align=\"center\"><font size=\"+2\"><b>";
    contribution +=   "Christoph Thielecke (crissi99@gmx.de)";
    contribution +=   "</b></font><br>\n";
    contribution +=   tr ( "German translation & additional programming" );
    contribution +=   "<br>\n";
    contribution +=   tr ( "Ported to Qt4" );

    contribution +=   "<br>";
    contribution +=   "<p align=\"center\"><font size=\"+2\"><b>";
    contribution +=   "Techno (techno@punkt.pl)";
    contribution +=   "</b></font><br>\n";
    contribution +=   tr ( "AVI reader plugin & Polish translation" );

    contribution +=   "<br><br>";
    contribution +=   "<p align=\"center\"><font size=\"+1\"><b>";
    contribution +=   "Rafael (rcb6@alu.ua.es)";
    contribution +=   "</b></font><br>\n";
    contribution +=   tr ( "Spanish translation" );

    contribution +=   "<br>";
    contribution +=   "<p align=\"center\"><font size=\"+1\"><b>";
    contribution +=   "Vlada (vladovi@altas.cz)";
    contribution +=   "</b></font><br>\n";
    contribution +=   tr ( "Czech translation" );

    contribution +=    "<br>\n";
    contribution +=    "</p>\n";

    contribution +=    "</p>\n";
    /******************************************************************************************/
    license =
        "GNU GENERAL PUBLIC LICENSE\n"
        "		       Version 2, June 1991\n"
        "\n"
        " Copyright (C) 1989, 1991 Free Software Foundation, Inc.\n"
        "                          675 Mass Ave, Cambridge, MA 02139, USA\n"
        " Everyone is permitted to copy and distribute verbatim copies\n"
        " of this license document, but changing it is not allowed.\n"
        "\n"
        "			    Preamble\n"
        "\n"
        "  The licenses for most software are designed to take away your\n"
        "freedom to share and change it.  By contrast, the GNU General Public\n"
        "License is intended to guarantee your freedom to share and change free\n"
        "software--to make sure the software is free for all its users.  This\n"
        "General Public License applies to most of the Free Software\n"
        "Foundation's software and to any other program whose authors commit to\n"
        "using it.  (Some other Free Software Foundation software is covered by\n"
        "the GNU Library General Public License instead.)  You can apply it to\n"
        "your programs, too.\n"
        "\n"
        "  When we speak of free software, we are referring to freedom, not\n"
        "price.  Our General Public Licenses are designed to make sure that you\n"
        "have the freedom to distribute copies of free software (and charge for\n"
        "this service if you wish), that you receive source code or can get it\n"
        "if you want it, that you can change the software or use pieces of it\n"
        "in new free programs; and that you know you can do these things.\n"
        "\n"
        "  To protect your rights, we need to make restrictions that forbid\n"
        "anyone to deny you these rights or to ask you to surrender the rights.\n"
        "These restrictions translate to certain responsibilities for you if you\n"
        "distribute copies of the software, or if you modify it.\n"
        "\n"
        "  For example, if you distribute copies of such a program, whether\n"
        "gratis or for a fee, you must give the recipients all the rights that\n"
        "you have.  You must make sure that they, too, receive or can get the\n"
        "source code.  And you must show them these terms so they know their\n"
        "rights.\n"
        "\n"
        "  We protect your rights with two steps: (1) copyright the software, and\n"
        "(2) offer you this license which gives you legal permission to copy,\n"
        "distribute and/or modify the software.\n"
        "\n"
        "  Also, for each author's protection and ours, we want to make certain\n"
        "that everyone understands that there is no warranty for this free\n"
        "software.  If the software is modified by someone else and passed on, we\n"
        "want its recipients to know that what they have is not the original, so\n"
        "that any problems introduced by others will not reflect on the original\n"
        "authors' reputations.\n"
        "\n"
        "  Finally, any free program is threatened constantly by software\n"
        "patents.  We wish to avoid the danger that redistributors of a free\n"
        "program will individually obtain patent licenses, in effect making the\n"
        "program proprietary.  To prevent this, we have made it clear that any\n"
        "patent must be licensed for everyone's free use or not licensed at all.\n"
        "\n"
        "  The precise terms and conditions for copying, distribution and\n"
        "modification follow.\n"
        " \n"
        "		    GNU GENERAL PUBLIC LICENSE\n"
        "   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION\n"
        "\n"
        "  0. This License applies to any program or other work which contains\n"
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
        "Whether that is true depends on what the Program does.\n"
        "\n"
        "  1. You may copy and distribute verbatim copies of the Program's\n"
        "source code as you receive it, in any medium, provided that you\n"
        "conspicuously and appropriately publish on each copy an appropriate\n"
        "copyright notice and disclaimer of warranty; keep intact all the\n"
        "notices that refer to this License and to the absence of any warranty;\n"
        "and give any other recipients of the Program a copy of this License\n"
        "along with the Program.\n"
        "\n"
        "You may charge a fee for the physical act of transferring a copy, and\n"
        "you may at your option offer warranty protection in exchange for a fee.\n"
        "\n"
        "  2. You may modify your copy or copies of the Program or any portion\n"
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
        "the scope of this License.\n"
        "\n"
        "  3. You may copy and distribute the Program (or a work based on it,\n"
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
        "compelled to copy the source along with the object code.\n"
        " \n"
        "  4. You may not copy, modify, sublicense, or distribute the Program\n"
        "except as expressly provided under this License.  Any attempt\n"
        "otherwise to copy, modify, sublicense or distribute the Program is\n"
        "void, and will automatically terminate your rights under this License.\n"
        "However, parties who have received copies, or rights, from you under\n"
        "this License will not have their licenses terminated so long as such\n"
        "parties remain in full compliance.\n"
        "\n"
        "  5. You are not required to accept this License, since you have not\n"
        "signed it.  However, nothing else grants you permission to modify or\n"
        "distribute the Program or its derivative works.  These actions are\n"
        "prohibited by law if you do not accept this License.  Therefore, by\n"
        "modifying or distributing the Program (or any work based on the\n"
        "Program), you indicate your acceptance of this License to do so, and\n"
        "all its terms and conditions for copying, distributing or modifying\n"
        "the Program or works based on it.\n"
        "\n"
        "  6. Each time you redistribute the Program (or any work based on the\n"
        "Program), the recipient automatically receives a license from the\n"
        "original licensor to copy, distribute or modify the Program subject to\n"
        "these terms and conditions.  You may not impose any further\n"
        "restrictions on the recipients' exercise of the rights granted herein.\n"
        "You are not responsible for enforcing compliance by third parties to\n"
        "this License.\n"
        "\n"
        "  7. If, as a consequence of a court judgment or allegation of patent\n"
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
        "be a consequence of the rest of this License.\n"
        " \n"
        "  8. If the distribution and/or use of the Program is restricted in\n"
        "certain countries either by patents or by copyrighted interfaces, the\n"
        "original copyright holder who places the Program under this License\n"
        "may add an explicit geographical distribution limitation excluding\n"
        "those countries, so that distribution is permitted only in or among\n"
        "countries not thus excluded.  In such case, this License incorporates\n"
        "the limitation as if written in the body of this License.\n"
        "\n"
        "  9. The Free Software Foundation may publish revised and/or new versions\n"
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
        "Foundation.\n"
        "\n"
        "  10. If you wish to incorporate parts of the Program into other free\n"
        "programs whose distribution conditions are different, write to the author\n"
        "to ask for permission.  For software which is copyrighted by the Free\n"
        "Software Foundation, write to the Free Software Foundation; we sometimes\n"
        "make exceptions for this.  Our decision will be guided by the two goals\n"
        "of preserving the free status of all derivatives of our free software and\n"
        "of promoting the sharing and reuse of software generally.\n"
        "\n"
        "			    NO WARRANTY\n"
        "\n"
        "  11. BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY\n"
        "FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW.  EXCEPT WHEN\n"
        "OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES\n"
        "PROVIDE THE PROGRAM \\AS IS\\ WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED\n"
        "OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF\n"
        "MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE ENTIRE RISK AS\n"
        "TO THE QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU.  SHOULD THE\n"
        "PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING,\n"
        "REPAIR OR CORRECTION.\n"
        "\n"
        "  12. IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING\n"
        "WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MAY MODIFY AND/OR\n"
        "REDISTRIBUTE THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES,\n"
        "INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING\n"
        "OUT OF THE USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED\n"
        "TO LOSS OF DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY\n"
        "YOU OR THIRD PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER\n"
        "PROGRAMS), EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE\n"
        "POSSIBILITY OF SUCH DAMAGES.\n"
        "\n"
        "		     END OF TERMS AND CONDITIONS\n"
        "\n";

    /******************************************************************************************/

    languageChange();
    resize ( QSize ( 500, 380 ).expandedTo ( minimumSizeHint() ) );

    // signals and slots connections
    connect ( closeButton, SIGNAL ( clicked() ), this, SLOT ( close() ) );

}

/*
 *  Destroys the object and frees any allocated resources
 */
InfoDialog::~InfoDialog() {
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void InfoDialog::languageChange() {
    setCaption ( tr ( "About cdcat" ) );
    closeButton->setText ( tr ( "close" ) );
    textLabel1->setText ( infotext );
    tabWidget2->changeTab ( tab, tr ( "About Cdcat" ) );
    textBrowser1->setText ( license );
    tabWidget2->changeTab ( tab_2, tr ( "License" ) );
    textThanx->setText ( contribution );
    tabWidget2->changeTab ( TabPage, tr ( "Thanks" ) );
}

