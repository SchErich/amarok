/***************************************************************************
 * copyright            : (C) 2007 Leo Franchi <lfranchi@gmail.com>        *
 **************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "WikipediaEngine.h"

#include "amarok.h"
#include "debug.h"
#include "ContextObserver.h"
#include "ContextView.h"
#include "enginecontroller.h"

using namespace Context;

WikipediaEngine::WikipediaEngine( QObject* parent, const QList<QVariant>& /*args*/ )
    : DataEngine( parent )
    , ContextObserver( ContextView::self() )
    , m_wikiJob( 0 )
{
    DEBUG_BLOCK
    m_requested = true; // testing
    m_wikiLocale = "en";
    m_currentSelection = "artist";
}

QStringList WikipediaEngine::sources() const
{
    return QStringList();
}

bool WikipediaEngine::sourceRequested( const QString& name )
{
    Q_UNUSED( name )
    m_requested = true; // someone is asking for data, so we turn ourselves on :)
    setData( "wikipedia", QVariant() );
    //FIXME return something!
    return true;
}

void WikipediaEngine::message( const ContextState& state )
{
    DEBUG_BLOCK
    if( state == Current && m_requested )
        update();
}

void WikipediaEngine::update()
{
    DEBUG_BLOCK
    QString tmpWikiStr;
    

    if( selection() == "artist" ) // default, or applet told us to fetch artist 
    {
        if ( (EngineController::instance()->currentTrack()->playableUrl().protocol() == "lastfm") ||
             (EngineController::instance()->currentTrack()->playableUrl().protocol() == "daap") ||
             !EngineController::engine()->isStream() )
        {
            tmpWikiStr = EngineController::instance()->currentTrack()->artist()->name();
            tmpWikiStr += wikiArtistPostfix();
        } else
            tmpWikiStr = EngineController::instance()->currentTrack()->prettyName();
    } else if( selection() == "title" )
        tmpWikiStr = EngineController::instance()->currentTrack()->prettyName();
    else if( selection() == "album" )
    {
        if ( (EngineController::instance()->currentTrack()->playableUrl().protocol() == "lastfm") ||
             (EngineController::instance()->currentTrack()->playableUrl().protocol() == "daap") ||
             !EngineController::engine()->isStream() )
        {
            tmpWikiStr = EngineController::instance()->currentTrack()->album ()->name();
            tmpWikiStr += wikiAlbumPostfix();
        }
    }
    
    //Hack to make wiki searches work with magnatune preview tracks
    
    if ( tmpWikiStr.contains( "PREVIEW: buy it at www.magnatune.com" ) ) {
        tmpWikiStr = tmpWikiStr.remove(" (PREVIEW: buy it at www.magnatune.com)" );
        int index = tmpWikiStr.indexOf( '-' );
        if ( index != -1 ) {
            tmpWikiStr = tmpWikiStr.left (index - 1);
        }
        
    }
    m_wikiCurrentEntry = tmpWikiStr;
    
    m_wikiCurrentUrl = wikiURL( tmpWikiStr );
    
    setData( "wikipedia", "message", "fetching" );
    m_wikiJob = KIO::storedGet( m_wikiCurrentUrl, false, false );
    connect( m_wikiJob, SIGNAL( result( KJob* ) ), SLOT( wikiResult( KJob* ) ) );
}

void WikipediaEngine::wikiResult( KJob* job )
{
    DEBUG_BLOCK
    if( !m_wikiJob ) return; //track changed while we were fetching
    
    if ( !job->error() == 0 && job == m_wikiJob )
    { // make sure its not the wrong job (e.g. wiki request for now changed song
        setData( "wikipedia", "error" );
        m_wikiJob = 0; // clear job
        return;
    }
    if ( job != m_wikiJob )
        return; //not the right job, so let's ignore it
    
    KIO::StoredTransferJob* const storedJob = static_cast<KIO::StoredTransferJob*>( job );
    m_wiki = QString( storedJob->data() );
    
    // FIXME: Get a safer Regexp here, to match only inside of <head> </head> at least.
    if ( m_wiki.contains( "charset=utf-8"  ) ) {
        m_wiki = QString::fromUtf8( storedJob->data().data(), storedJob->data().size() );
    }
    
    if( m_wiki.indexOf( "var wgArticleId = 0" ) != -1 )
    {
        // article was not found
        if( m_wikiCurrentEntry.endsWith( wikiArtistPostfix() ) )
        {
            m_wikiCurrentEntry = m_wikiCurrentEntry.left( m_wikiCurrentEntry.length() - wikiArtistPostfix().length() );
            reloadWikipedia();
            return;
        }
        else if( m_wikiCurrentEntry.endsWith( wikiAlbumPostfix() ) )
        {
            m_wikiCurrentEntry = m_wikiCurrentEntry.left( m_wikiCurrentEntry.length() - wikiAlbumPostfix().length() );
            reloadWikipedia();
            return;
        }
        else if( m_wikiCurrentEntry.endsWith( wikiTrackPostfix() ) )
        {
            m_wikiCurrentEntry = m_wikiCurrentEntry.left( m_wikiCurrentEntry.length() - wikiTrackPostfix().length() );
            reloadWikipedia();
            return;
        }
    }
    
    //remove the new-lines and tabs(replace with spaces IS needed).
    m_wiki.replace( "\n", " " );
    m_wiki.replace( "\t", " " );
    
    m_wikiLanguages.clear();
    // Get the available language list
    if ( m_wiki.indexOf("<div id=\"p-lang\" class=\"portlet\">") != -1 )
    {
        m_wikiLanguages = m_wiki.mid( m_wiki.indexOf("<div id=\"p-lang\" class=\"portlet\">") );
        m_wikiLanguages = m_wikiLanguages.mid( m_wikiLanguages.indexOf("<ul>") );
        m_wikiLanguages = m_wikiLanguages.mid( 0, m_wikiLanguages.indexOf( "</div>" ) );
    }
    
    QString copyright;
    QString copyrightMark = "<li id=\"f-copyright\">";
    if ( m_wiki.indexOf( copyrightMark ) != -1 )
    {
        copyright = m_wiki.mid( m_wiki.indexOf(copyrightMark) + copyrightMark.length() );
        copyright = copyright.mid( 0, copyright.indexOf( "</li>" ) );
        copyright.replace( "<br />", QString() );
        //only one br at the beginning
        copyright.prepend( "<br />" );
    }
    
    // Ok lets remove the top and bottom parts of the page
    m_wiki = m_wiki.mid( m_wiki.indexOf( "<h1 class=\"firstHeading\">" ) );
    m_wiki = m_wiki.mid( 0, m_wiki.indexOf( "<div class=\"printfooter\">" ) );
    // Adding back license information
    m_wiki += copyright;
    m_wiki.append( "</div>" );
    m_wiki.replace( QRegExp("<h3 id=\"siteSub\">[^<]*</h3>"), QString() );
    
    m_wiki.replace( QRegExp( "<span class=\"editsection\"[^>]*>[^<]*<[^>]*>[^<]*<[^>]*>[^<]*</span>" ), QString() );
    
    m_wiki.replace( QRegExp( "<a href=\"[^\"]*\" class=\"new\"[^>]*>([^<]*)</a>" ), "\\1" );
    
    // Remove anything inside of a class called urlexpansion, as it's pointless for us
    m_wiki.replace( QRegExp( "<span class= *'urlexpansion'>[^(]*[(][^)]*[)]</span>" ), QString() );
    
    // Remove hidden table rows as well
    QRegExp hidden( "<tr *class= *[\"\']hiddenStructure[\"\']>.*</tr>", Qt::CaseInsensitive );
    hidden.setMinimal( true ); //greedy behaviour wouldn't be any good!
    m_wiki.replace( hidden, QString() );
    
    // we want to keep our own style (we need to modify the stylesheet a bit to handle things nicely)
    m_wiki.replace( QRegExp( "style= *\"[^\"]*\"" ), QString() );
    m_wiki.replace( QRegExp( "class= *\"[^\"]*\"" ), QString() );
    // let's remove the form elements, we don't want them.
    m_wiki.replace( QRegExp( "<input[^>]*>" ), QString() );
    m_wiki.replace( QRegExp( "<select[^>]*>" ), QString() );
    m_wiki.replace( "</select>\n" , QString() );
    m_wiki.replace( QRegExp( "<option[^>]*>" ), QString() );
    m_wiki.replace( "</option>\n" , QString() );
    m_wiki.replace( QRegExp( "<textarea[^>]*>" ), QString() );
    m_wiki.replace( "</textarea>" , QString() );
    
    //first we convert all the links with protocol to external, as they should all be External Links.
    m_wiki.replace( QRegExp( "href= *\"http:" ), "href=\"externalurl:" );
    //m_wiki.replace( QRegExp( "href= *\"/" ), "href=\"" +m_wikiBaseUrl );
    m_wiki.replace( QRegExp( "href= *\"#" ), "href=\"" +m_wikiCurrentUrl + '#' );
    
    QString m_wikiHTMLSource = "<html><body>\n";
    m_wikiHTMLSource.append( m_wiki );
    if ( !m_wikiLanguages.isEmpty() )
    {
        m_wikiHTMLSource.append( i18n( "Wikipedia Other Languages: <br>" )+ m_wikiLanguages );
    }
    m_wikiHTMLSource.append( "</body></html>\n" );
    
    clearData( "wikipedia" );
//     debug() << "sending wiki page:" << m_wikiHTMLSource;
    setData( "wikipedia", selection(), m_wikiHTMLSource );
    
    m_wikiJob = 0;
}

QString
WikipediaEngine::wikiLocale()
{
    if( m_wikiLocale.isEmpty() )
        return QString( "en" );
    
    return m_wikiLocale;
}


QString
WikipediaEngine::wikiArtistPostfix()
{
    if( wikiLocale() == "en" )
        return " (band)";
    else if( wikiLocale() == "de" )
        return " (Band)";
    else
        return "";
}

QString
WikipediaEngine::wikiAlbumPostfix()
{
    if( wikiLocale() == "en" )
        return " (album)";
    else
        return "";
}

QString
WikipediaEngine::wikiTrackPostfix()
{
    if( wikiLocale() == "en" )
        return " (song)";
    else
        return "";
}

QString
WikipediaEngine::wikiURL( const QString &item )
{
    return QString( "http://%1.wikipedia.org/wiki/" ).arg( wikiLocale() )
        + KUrl::toPercentEncoding( item, "/" );
}

void
WikipediaEngine::reloadWikipedia()
{
    m_wikiJob = NULL;
//     showWikipediaEntry( m_wikiCurrentEntry, true );
}

#include "WikipediaEngine.moc"

