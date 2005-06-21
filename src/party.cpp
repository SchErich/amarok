/***************************************************************************
 * copyright            : (C) 2005 Seb Ruiz <me@sebruiz.net>               *
 **************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "actionclasses.h"    //see toolbar construction
#include "amarok.h"
#include "amarokconfig.h"
#include "browserToolBar.h"
#include "collectiondb.h"
#include "party.h"
#include "partydialogbase.h"
#include "playlist.h"
#include "playlistbrowser.h"
#include "statusbar.h"

#include <qfile.h>

#include <kaction.h>
#include <kapplication.h>
#include <kiconloader.h>       //smallIcon
#include <klocale.h>
#include <kpushbutton.h>
#include <ktoolbar.h>
#include <kurldrag.h>          //dragObject()
#include <kurllabel.h>

/////////////////////////////////////////////////////////////////////////////
///    CLASS Party
////////////////////////////////////////////////////////////////////////////

Party *Party::s_instance = 0;

Party::Party( QWidget *parent, const char *name )
    : QVBox( parent, name )
    , m_visible( true )
{
    s_instance = this;

    QFrame *container = new QVBox( this, "container" );
    container->hide();

    //<Toolbar>
    m_ac = new KActionCollection( this );
    KAction *repopulate = new KAction( i18n("Repopulate"), "rebuild", 0,
                                       this, SLOT( repopulate() ), m_ac, "Repopulate Upcoming Tracks" );

    m_applyButton = new KAction( i18n("Apply"), "apply", 0, this, SLOT( applySettings() ), m_ac, "Apply Settings" );

    m_toolbar = new Browser::ToolBar( container );
    m_toolbar->setIconText( KToolBar::IconTextRight, false ); //we want the buttons to have text on right
    repopulate->plug( m_toolbar );
    m_toolbar->insertLineSeparator();
    m_applyButton->plug( m_toolbar );


    m_base = new PartyDialogBase( container );

    m_base->m_previousIntSpinBox->setEnabled( m_base->m_cycleTracks->isEnabled() );

    connect( m_base->m_cycleTracks, SIGNAL( toggled(bool) ), m_base->m_previousIntSpinBox, SLOT( setEnabled(bool) ) );

    //Update buttons
    connect( m_base->m_appendCountIntSpinBox, SIGNAL( valueChanged( int ) ), SLOT( updateApplyButton() ) );
    connect( m_base->m_previousIntSpinBox,    SIGNAL( valueChanged( int ) ), SLOT( updateApplyButton() ) );
    connect( m_base->m_upcomingIntSpinBox,    SIGNAL( valueChanged( int ) ), SLOT( updateApplyButton() ) );
    connect( m_base->m_cycleTracks,   SIGNAL( stateChanged( int ) ), SLOT( updateApplyButton() ) );
    connect( m_base->m_markHistory,   SIGNAL( stateChanged( int ) ), SLOT( updateApplyButton() ) );
    connect( m_base->m_appendType,    SIGNAL( activated( int ) ),    SLOT( updateApplyButton() ) );

    KPushButton *button = new KPushButton( KGuiItem( i18n("Enable Dynamic Mode..."), "party" ), this );
    button->setToggleButton( true );
    connect( button, SIGNAL(toggled( bool )), SLOT(toggle( bool )) );

    connect( amaroK::actionCollection()->action( "dynamic_mode" ), SIGNAL( toggled( bool ) ),
             button, SLOT( setOn( bool ) ) );

    restoreSettings();
    button->setOn( AmarokConfig::partyMode() );

    if( button->isOn() )
    {
//         Although random mode should be off, we uncheck it, just in case (eg amarokrc tinkering)
        static_cast<KToggleAction*>(amaroK::actionCollection()->action( "random_mode" ))->setChecked( false );
        amaroK::actionCollection()->action( "random_mode" )->setEnabled( false );
    }
}

void
Party::restoreSettings()
{
    m_base->m_upcomingIntSpinBox->setValue( AmarokConfig::partyUpcomingCount() );
    m_base->m_previousIntSpinBox->setValue( AmarokConfig::partyPreviousCount() );
    m_base->m_appendCountIntSpinBox->setValue( AmarokConfig::partyAppendCount() );
    m_base->m_cycleTracks->setChecked( AmarokConfig::partyCycleTracks() );
    m_base->m_markHistory->setChecked( AmarokConfig::partyMarkHistory() );

    if ( AmarokConfig::partyType() == "Random" )
        m_base->m_appendType->setCurrentItem( RANDOM );

    else if ( AmarokConfig::partyType() == "Suggestion" )
        m_base->m_appendType->setCurrentItem( SUGGESTION );

    else // Custom
        m_base->m_appendType->setCurrentItem( CUSTOM );

    m_applyButton->setEnabled( false );

}

void
Party::loadConfig( PartyEntry *config )
{
    m_base->m_upcomingIntSpinBox->setValue( config->upcoming() );
    m_base->m_previousIntSpinBox->setValue( config->previous() );
    m_base->m_appendCountIntSpinBox->setValue( config->appendCount() );
    m_base->m_cycleTracks->setChecked( config->isCycled() );
    m_base->m_markHistory->setChecked( config->isMarked() );
    m_base->m_appendType->setCurrentItem( config->appendType() );

    AmarokConfig::setPartyCustomList( config->items() );

    applySettings();

    Playlist::instance()->repopulate();
}

void
Party::applySettings() //SLOT
{
    //TODO this should be in app.cpp or the dialog's class implementation, here is not the right place

    if( CollectionDB::instance()->isEmpty() )
        return;

    QString type;
    if( appendType() == RANDOM )
        type = "Random";
    else if( appendType() == SUGGESTION )
        type = "Suggestion";
    else if( appendType() == CUSTOM )
        type = "Custom";

    AmarokConfig::setPartyType( type );
    PlaylistBrowser::instance()->loadDynamicItems();

    if ( AmarokConfig::partyPreviousCount() != previousCount() )
    {
        Playlist::instance()->adjustPartyPrevious( previousCount() );
        AmarokConfig::setPartyPreviousCount( previousCount() );
    }

    if ( AmarokConfig::partyUpcomingCount() != upcomingCount() )
    {
        AmarokConfig::setPartyUpcomingCount( upcomingCount() );
        Playlist::instance()->adjustPartyUpcoming( upcomingCount(), type );
    }

    AmarokConfig::setPartyCycleTracks( cycleTracks() );
    AmarokConfig::setPartyAppendCount( appendCount() );
    AmarokConfig::setPartyMarkHistory( markHistory() );

    amaroK::actionCollection()->action( "prev" )->setEnabled( !AmarokConfig::partyMode() );
    amaroK::actionCollection()->action( "random_mode" )->setEnabled( !AmarokConfig::partyMode() );
    amaroK::actionCollection()->action( "playlist_shuffle" )->setEnabled( !AmarokConfig::partyMode() );

    m_applyButton->setEnabled( false );
}

void
Party::statusChanged( bool enable ) // SLOT
{
    if( !enable )
        Playlist::instance()->alterHistoryItems( true, true ); //enable all items

    applySettings();
}

void
Party::repopulate() // SLOT
{
    Playlist::instance()->repopulate();
}

void
Party::updateApplyButton() //SLOT
{
    if( cycleTracks()   != AmarokConfig::partyCycleTracks()   ||
        markHistory()   != AmarokConfig::partyMarkHistory()   ||
        previousCount() != AmarokConfig::partyPreviousCount() ||
        upcomingCount() != AmarokConfig::partyUpcomingCount() ||
        appendCount()   != AmarokConfig::partyAppendCount() )
    {
        m_applyButton->setEnabled( true );
        return;
    }

    QString type = AmarokConfig::partyType();
    int typeValue = CUSTOM;

    if( type == "Random" )          typeValue = RANDOM;
    else if( type == "Suggestion" ) typeValue = SUGGESTION;

    if( typeValue != appendType() )
        m_applyButton->setEnabled( true );
    else
        m_applyButton->setEnabled( false );

}

void
Party::toggle( bool enable ) //SLOT
{
    static_cast<QWidget*>(child("container"))->setShown( enable );

    if( enable )
    {
        // uncheck before disabling
        static_cast<KToggleAction*>(amaroK::actionCollection()->action( "random_mode" ))->setChecked( false );
        amaroK::actionCollection()->action( "random_mode" )->setEnabled( false );
        amaroK::actionCollection()->action( "playlist_shuffle" )->setEnabled( false );
        static_cast<KToggleAction*>(amaroK::actionCollection()->action( "dynamic_mode" ))->setChecked( true );

        Playlist::instance()->repopulate();
    }
    else
    {
        Playlist::instance()->alterHistoryItems( true, true ); //enable all items

        // Random mode was being enabled without notification on leaving dynamic mode.  Remember to re-enable first!
        amaroK::actionCollection()->action( "random_mode" )->setEnabled( true );
        static_cast<KToggleAction*>(amaroK::actionCollection()->action( "random_mode" ))->setChecked( false );
        amaroK::actionCollection()->action( "playlist_shuffle" )->setEnabled( true );
        static_cast<KToggleAction*>(amaroK::actionCollection()->action( "dynamic_mode" ))->setChecked( false );
    }
}

int     Party::previousCount() { return m_base->m_previousIntSpinBox->value(); }
int     Party::upcomingCount() { return m_base->m_upcomingIntSpinBox->value(); }
int     Party::appendCount()   { return m_base->m_appendCountIntSpinBox->value(); }
int     Party::appendType()    { return m_base->m_appendType->currentItem(); }
bool    Party::cycleTracks()   { return m_base->m_cycleTracks->isChecked(); }
bool    Party::markHistory()   { return m_base->m_markHistory->isChecked(); }


#include "party.moc"
