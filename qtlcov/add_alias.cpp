/****************************************************************************
    Qt-LCoV Library Editor for Qt-BSch3V
    Copyright (C) 2002-2005 H.Okada

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*****************************************************************************/

#include "add_alias.h"

#include <qvariant.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
//#include <q3whatsthis.h>
#include <qimage.h>
#include <qpixmap.h>
//Added by qt3to4:
#include <QGridLayout>

/*
 *  Constructs a add_alias as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
add_alias::add_alias( QWidget* parent, const char* name, bool modal, Qt::WFlags fl )
    : QDialog( parent, fl )
{
    if ( !name )
        setObjectName("add_alias" );
    else
        setObjectName(name);
    setModal(modal);
    add_aliasLayout = new QGridLayout( this);
    add_aliasLayout->setMargin(11);
    add_aliasLayout->setSpacing(6);
    add_aliasLayout->setObjectName("add_aliasLayout");

    editAlias = new QLineEdit( this );
    editAlias->setObjectName("editAlias");
    editAlias->setMaxLength( 15 );

    add_aliasLayout->addWidget( editAlias, 1, 1 );

    textLabel1 = new QLabel( this );
    textLabel1->setObjectName( "textLabel1");
    //Qt4    textLabel1->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
    textLabel1->setAlignment( Qt::AlignVCenter | Qt::AlignRight );

    add_aliasLayout->addWidget( textLabel1, 0, 0 );

    buttonOK = new QPushButton( this);
    buttonOK->setObjectName("buttonOK" );

    add_aliasLayout->addWidget( buttonOK, 0, 2 );

    buttonCancel = new QPushButton( this);
    buttonCancel->setObjectName("buttonCancel" );

    add_aliasLayout->addWidget( buttonCancel, 1, 2 );

    labelAliasOf = new QLabel( this);
    labelAliasOf->setObjectName("labelAliasOf");

    add_aliasLayout->addWidget( labelAliasOf, 0, 1 );
    languageChange();
    resize( QSize(316, 82).expandedTo(minimumSizeHint()) );
    //?    clearWState( WState_Polished );

    // signals and slots connections
    connect( buttonOK, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( close() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
add_alias::~add_alias()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void add_alias::languageChange()
{
    setWindowTitle(tr( "Add Alias" ) );
    textLabel1->setText( tr( "Alias of " ) );
    buttonOK->setText( tr( "OK" ) );
    buttonCancel->setText( tr( "Cancel" ) );
    labelAliasOf->setText( tr( "textLabel1" ) );
}

