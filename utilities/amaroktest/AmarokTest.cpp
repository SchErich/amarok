/***************************************************************************
 *   Copyright (C) 2009 Sven Krohlas <sven@getamarok.com>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include "AmarokTest.h"

// Amarok includes
#include "src/Amarok.h"

#include <KStandardDirs>

#include <QDateTime>
#include <QFile>
#include <QObject>
#include <QScriptValue>
#include <QTextStream>

// add includes needed for tests here
#include "src/playlistmanager/PlaylistManager.h"
#include "src/playlist/PlaylistActions.h"
#include "src/playlist/PlaylistController.h"
#include "src/scriptengine/MetaTypeExporter.h"

// our own Prototypes
#include "KUrlExporter.h"

static QTextStream s_errStream( stderr );
static QTextStream s_debugStream( stdout );

int
main( int argc, char *argv[] )
{
    AmarokTest tester( argc, argv );
    return tester.exec();
}


AmarokTest::AmarokTest( int &argc, char **argv )
        : QApplication( argc, argv )
{
    int i;
    QString logsLocation = Amarok::saveLocation( "testresults/" );
    QString currentTime = QDateTime::currentDateTime().toString( "yyyy-MM-dd.HH-mm-ss" );
    QFile logFile( logsLocation + currentTime + ".log" );

    m_measurePerf = false;

    if( !logFile.open( QIODevice::WriteOnly | QIODevice::Text ) ) {
        s_errStream << qPrintable( tr( "Unable to open log!" ) );
        ::exit( 1 );
    }

    m_log.setDevice( &logFile );
    m_log << "<testrun>" << endl;

    prepareTestEngine();

    if( arguments().size() == 1 ) /** only our own program name: run all available tests */
    {
        m_allTests = KGlobal::dirs()->findAllResources( "data", "amarok/tests/*.js", KStandardDirs::Recursive );
        i = 0;

        while( i < m_allTests.size() ) {
            m_currentlyRunning = m_allTests.at( i );
            m_log << "<testscript name=\"" << m_currentlyRunning << "\">" << endl;
            runScript();
            i++;
            m_log << "</teststript>" << endl << endl;
        }
    }

    else /** run given test(s) */
    {
        i = 1;

        while( i < arguments().size() )
        {
            m_currentlyRunning = arguments().at( i );
            runScript();
            i++;
        }
    }

    m_log << "</testrun>\n";

    QString linkLocation = logsLocation + "LATEST";
    #ifdef Q_WS_WIN /** .lnk extension is required for links on Windows */
    linkLocation = linkLocation + ".lnk";
    #endif

    QFile symlink( linkLocation );
    symlink.remove();
    if( !logFile.link( linkLocation ) )
        s_errStream << qPrintable( tr( "Unable to create link to log!" ) );

    logFile.close();
    ::exit( 0 );
}

AmarokTest::~AmarokTest()
{
    qDeleteAll( m_wrapperList.begin(), m_wrapperList.end() );
}


/**
 * Utility functions for test scripts: public slots
 */

void
AmarokTest::debug( const QString& text ) const // Slot
{
    s_debugStream << "amaroktest: Script " << m_currentlyRunning << ": " << text << endl;
}


void
AmarokTest::startTimer() // Slot
{
    // TODO: this measures real time, which is quite inaccurate
    //       find sth portable which gives us the cpu time
    m_measurePerf = true;

    if( m_testTime.isNull() )
        m_testTime.start();
    else
        m_testTime.restart();
}


void
AmarokTest::testResult( QString testName, QString expected, QString actualResult, bool expectedToFail ) // Slot
{
    m_log << "  <test>" << endl;
    m_log << "    <name>" << testName << "</name>" << endl;

    if( ( !expectedToFail && ( expected != actualResult ) ) || ( expectedToFail && ( expected == actualResult ) ) )
    {
        m_log << "    <status>FAILED</status>" << endl;
        m_log << "    <expected>" << expected << "</expected>" << endl;
        m_log << "    <result>" << actualResult << "</result>" << endl;
    }

    else
        m_log << "    <status>PASSED</status>" << endl;

    if( m_measurePerf )
        m_log << "    <time>" << m_testTime.elapsed() << "</time>" << endl; // in milliseconds

    m_log << "  </test>" << endl;

    m_testTime.restart();
}


/**
 * Private
 */


void
AmarokTest::prepareTestEngine()
{
    /** Give test scripts access to everything in qt they might need */
    m_engine.importExtension( "qt.core" );
    m_engine.importExtension( "qt.gui" );
    m_engine.importExtension( "qt.network" );
    m_engine.importExtension( "qt.sql" );
    m_engine.importExtension( "qt.uitools" );
    m_engine.importExtension( "qt.webkit" );
    m_engine.importExtension( "qt.xml" );

    m_engine.setProcessEventsInterval( 100 );

    /** Access to our test framework is a nice thing, too */
    QObject *amarokTestObject = this;
    QScriptValue amarokTestValue = m_engine.newQObject( amarokTestObject );
    m_engine.globalObject().setProperty( "AmarokTest", amarokTestValue );

    /** PlaylistManager */
    amarokTestObject = The::playlistManager();
    amarokTestValue = m_engine.newQObject( amarokTestObject );
    m_engine.globalObject().setProperty( "PlaylistManager", amarokTestValue );

    /** Playlist::Actions */
    amarokTestObject = The::playlistActions();
    amarokTestValue = m_engine.newQObject( amarokTestObject );
    m_engine.globalObject().setProperty( "PlaylistActions", amarokTestValue );

    /** Playlist::Controller */
    amarokTestObject = The::playlistController();
    amarokTestValue = m_engine.newQObject( amarokTestObject );
    m_engine.globalObject().setProperty( "PlaylistController", amarokTestValue );

    /** Meta::TrackPtr */
    MetaTrackPrototype* trackProto = new MetaTrackPrototype();
    m_engine.setDefaultPrototype( qMetaTypeId<Meta::TrackPtr>(), m_engine.newQObject( trackProto ) );
    m_wrapperList.append( trackProto );

    /** KUrl */
    KUrlPrototype* KUrlProto = new KUrlPrototype();
    m_engine.setDefaultPrototype( qMetaTypeId<KUrl>(), m_engine.newQObject( KUrlProto ) );
    m_wrapperList.append( KUrlProto );

    /**  */
}


void
AmarokTest::runScript()
{
    QFile testScript;

    testScript.setFileName( m_currentlyRunning );
    testScript.open( QIODevice::ReadOnly );
    m_engine.evaluate( testScript.readAll() );

    if( m_engine.hasUncaughtException() ) {
        s_errStream << qPrintable( tr( "Uncaught exception in test script: " ) ) << m_currentlyRunning << endl;
        s_errStream << qPrintable( tr( "Line: " ) ) << m_engine.uncaughtExceptionLineNumber() << endl;
        s_errStream << qPrintable( tr( "Exception: " ) ) << m_engine.uncaughtException().toString() << endl;

        s_errStream << qPrintable( tr( "Backtrace: " ) ) << endl;
        QStringList backtrace = m_engine.uncaughtExceptionBacktrace();

        int i = 0;
        while( i < backtrace.size() )
        {
            s_errStream << "  " << backtrace.at( i ) << endl;
            i++;
        }

        s_errStream << endl;
    }

    testScript.close();
    m_measurePerf = false;
}

#include "AmarokTest.moc"
