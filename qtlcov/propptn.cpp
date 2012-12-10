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

#include "propptn.h"

#include <qvariant.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
//#include <q3whatsthis.h>
#include <qimage.h>
#include <qpixmap.h>
//Added by qt3to4:
#include <QGridLayout>

/*
 *  Constructs a prop_ptn as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
prop_ptn::prop_ptn( QWidget* parent, const char* name, bool modal, Qt::WFlags fl )
    : QDialog( parent, fl )
{
    if ( !name )
        setObjectName( "prop_ptn" );
    else
        setObjectName(name);
    setModal(modal);
    //setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, sizePolicy().hasHeightForWidth() ) );
    setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    setBaseSize( QSize( 0, 0 ) );
    setModal( TRUE );
    setWindowModality(Qt::WindowModal);
    //prop_ptnLayout = new QGridLayout( this, 1, 1, 11, 6, "prop_ptnLayout");
    prop_ptnLayout = new QGridLayout( this);
    prop_ptnLayout->setObjectName( "prop_ptnLayout");

    textLabel1 = new QLabel( this );
    textLabel1->setObjectName( "textLabel1");
    textLabel1->setAlignment(Qt::AlignCenter);

    prop_ptnLayout->addWidget( textLabel1, 0, 0 );

    checkDot = new QCheckBox( this );
    checkDot->setObjectName("checkDot");

    prop_ptnLayout->addWidget(checkDot,1,0,1,2);
    //prop_ptnLayout->addMultiCellWidget( checkDot, 1, 1, 0, 1 );
    prop_ptnLayout->addWidget(checkDot,1,0,1,2);

    checkFill = new QCheckBox( this);
    checkFill->setObjectName("checkFill");

    prop_ptnLayout->addWidget(checkFill,2,0,1,2);
    //prop_ptnLayout->addMultiCellWidget( checkFill, 2, 2, 0, 1 );

    comboWidth = new QComboBox( this);
    comboWidth->setObjectName("comboWidth");

    prop_ptnLayout->addWidget( comboWidth, 0, 1 );

    buttonOK = new QPushButton( this);
    buttonOK->setObjectName("buttonOK");

    prop_ptnLayout->addWidget( buttonOK, 1, 2 );

    buttonCancel = new QPushButton( this );
    buttonCancel->setObjectName("buttonCancel");

    prop_ptnLayout->addWidget( buttonCancel, 2, 2 );
    languageChange();
    resize( QSize(299, 100).expandedTo(minimumSizeHint()) );
    //clearWState( WState_Polished );

    // signals and slots connections
    connect( buttonOK, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( close() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
prop_ptn::~prop_ptn()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void prop_ptn::languageChange()
{
    setWindowTitle(tr( "Pattern Property" ) );
    textLabel1->setText( tr( "Width" ) );
    checkDot->setText( tr( "Dot line" ) );
    checkFill->setText( tr( "Fill" ) );
    buttonOK->setText( tr( "OK" ) );
    buttonCancel->setText( tr( "Cancel" ) );
}

