/* 
   Copyright (c) 2004 Christian Muehlhaeuser <chris@chris.de>
   Copyright (c) 2004 Sami Nieminen <sami.nieminen@iki.fi>
   Copyright (c) 2006 Shane King <kde@dontletsstart.com>
   Copyright (c) 2006 Iain Benson <iain@arctos.me.uk>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
*/

#ifndef AMAROK_SCROBBLER_H
#define AMAROK_SCROBBLER_H

#include "engineobserver.h"

#include <QDomDocument>
#include <QDomElement>
#include <QHash>
#include <QList>
#include <QObject>
#include <QTimer>

//some setups require this
#undef PROTOCOL_VERSION

namespace KIO { class Job; }

class QStringList;
class SubmitItem;
class SubmitQueue;
class ScrobblerSubmitter;

class Scrobbler : public QObject, public EngineObserver
{
    friend class MediaDevice;

    Q_OBJECT

    public:
        static Scrobbler *instance();

        void similarArtists( const QString & /*artist*/ );
        void applySettings();

    signals:
        void similarArtistsFetched( const QString& artist, const QStringList& suggestions );

    public slots:
        void subTrack( long currentPos, long startPos, long endPos ); // cuefiles can update length without track change

    protected:
        Scrobbler();
        ~Scrobbler();

        void engineNewMetaData( const MetaBundle& /*bundle*/, bool /*state*/ );
        void engineTrackPositionChanged( long /*position*/ , bool /*userSeek*/ );

    private slots:
        void audioScrobblerSimilarArtistsResult( KIO::Job* /*job*/ );
        void audioScrobblerSimilarArtistsData(
            KIO::Job* /*job*/, const QByteArray& /*data*/ );

    private:
        QTimer m_timer; //works around xine bug
                        //http://sourceforge.net/tracker/index.php?func=detail&aid=1401026&group_id=9655&atid=109655
        QByteArray m_similarArtistsBuffer;
        KIO::Job* m_similarArtistsJob;
        QString m_artist;
        bool m_validForSending;
        long m_startPos;
        ScrobblerSubmitter* m_submitter;
        SubmitItem* m_item;
};


class SubmitItem
{
    friend class ScrobblerSubmitter;

    public:
        SubmitItem(
            const QString& /*artist*/,
            const QString& /*album*/,
            const QString& /*title*/,
            int /*length*/,
            bool now = true );
        SubmitItem( const QDomElement& /* domElement */ );
        SubmitItem();

        bool operator==( const SubmitItem& item );

        void setArtist( const QString& artist ) { m_artist = artist; }
        void setAlbum( const QString& album ) { m_album = album; }
        void setTitle( const QString& title ) { m_title = title; }
        const QString artist() const { return m_artist; }
        const QString album() const { return m_album; }
        const QString title() const { return m_title; }
        int length() const { return m_length; }
        uint playStartTime() const { return m_playStartTime; }

        QDomElement toDomElement( QDomDocument& /* document */ ) const;

        bool valid() const { return !m_artist.isEmpty() && !m_title.isEmpty() && m_length >= 30; }

    private:
        QString m_artist;
        QString m_album;
        QString m_title;
        int m_length;
        uint m_playStartTime;
};


class SubmitQueue : public QList<SubmitItem*>
{
    public:
        static bool compareItems( SubmitItem *sItem1, SubmitItem *sItem2 );
};


class ScrobblerSubmitter : public QObject
{
    Q_OBJECT

    public:
        static QString PROTOCOL_VERSION;
        static QString CLIENT_ID;
        static QString CLIENT_VERSION;
        static QString HANDSHAKE_URL;

        ScrobblerSubmitter();
        ~ScrobblerSubmitter();

        void submitItem( SubmitItem* /* item */ );

        void configure( const QString& /*username*/, const QString& /* password*/, bool /*enabled*/ );

        void syncComplete();

    private slots:
        void scheduledTimeReached();
        void audioScrobblerHandshakeResult( KIO::Job* /*job*/ );
        void audioScrobblerSubmitResult( KIO::Job* /*job*/ );
        void audioScrobblerSubmitData(
            KIO::Job* /*job*/, const QByteArray& /*data*/ );

    private:
        bool canSubmit() const;
        void enqueueItem( SubmitItem* /* item */ );
        SubmitItem* dequeueItem();
        void enqueueJob( KIO::Job* /* job */ );
        void finishJob( KIO::Job* /* job */ );
        void announceSubmit(
            SubmitItem* /* item */, int /* tracks */, bool /* success */ ) const;
        void saveSubmitQueue();
        void readSubmitQueue();
        bool schedule( bool failure );
        void performHandshake();
        void performSubmit();

        // on failure, start at MIN_BACKOFF, and double on subsequent failures
        // until MAX_BACKOFF is reached
        static const int MIN_BACKOFF = 60;
        static const int MAX_BACKOFF = 60 * 60;

        QString m_submitResultBuffer;
        QString m_username;
        QString m_password;
        QString m_submitUrl;
        QString m_challenge;
        QString m_savePath;
        bool m_scrobblerEnabled;
        bool m_holdFakeQueue;
        bool m_inProgress;
        bool m_needHandshake;
        uint m_prevSubmitTime;
        uint m_interval;
        uint m_backoff;
        uint m_lastSubmissionFinishTime;
        uint m_fakeQueueLength;

        //Q3PtrDict<SubmitItem> m_ongoingSubmits;
        QHash<KIO::Job*, SubmitItem* > m_ongoingSubmits;
        SubmitQueue m_submitQueue; // songs played by Amarok
        SubmitQueue m_fakeQueue; // songs for which play times have to be faked (e.g. when submitting from media device)

        QTimer m_timer;
};


#endif /* AMAROK_SCROBBLER_H */
