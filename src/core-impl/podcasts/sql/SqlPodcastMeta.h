/****************************************************************************************
 * Copyright (c) 2008 Bart Cerneels <bart.cerneels@kde.org>                             *
 *                                                                                      *
 * This program is free software; you can redistribute it and/or modify it under        *
 * the terms of the GNU General Public License as published by the Free Software        *
 * Foundation; either version 2 of the License, or (at your option) any later           *
 * version.                                                                             *
 *                                                                                      *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY      *
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A      *
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.             *
 *                                                                                      *
 * You should have received a copy of the GNU General Public License along with         *
 * this program.  If not, see <http://www.gnu.org/licenses/>.                           *
 ****************************************************************************************/

#ifndef SQLPODCASTMETA_H
#define SQLPODCASTMETA_H

#include "core/podcasts/PodcastMeta.h"
#include "core-impl/meta/file/File.h"
#include "core/playlists/PlaylistProvider.h"

namespace Podcasts
{

class SqlPodcastEpisode;
class SqlPodcastChannel;
class SqlPodcastProvider;

typedef KSharedPtr<SqlPodcastEpisode> SqlPodcastEpisodePtr;
typedef KSharedPtr<SqlPodcastChannel> SqlPodcastChannelPtr;

typedef QList<SqlPodcastEpisodePtr> SqlPodcastEpisodeList;
typedef QList<SqlPodcastChannelPtr> SqlPodcastChannelList;

class SqlPodcastEpisode : public Podcasts::PodcastEpisode
{
    public:
        static Meta::TrackList toTrackList( SqlPodcastEpisodeList episodes );
        static PodcastEpisodeList toPodcastEpisodeList( SqlPodcastEpisodeList episodes );

        SqlPodcastEpisode( const QStringList &queryResult, SqlPodcastChannelPtr sqlChannel );

        /** Copy from another PodcastEpisode
        */
        SqlPodcastEpisode( PodcastEpisodePtr episode );
        SqlPodcastEpisode( PodcastChannelPtr channel, PodcastEpisodePtr episode );

        ~SqlPodcastEpisode();

        //PodcastEpisode methods
        PodcastChannelPtr channel() const { return PodcastChannelPtr::dynamicCast( m_channel ); }
        virtual bool isKeep() const { return m_isKeep; }

        virtual void setNew( bool isNew );
        virtual void setKeep( bool isKeep );
        virtual void setLocalUrl( const KUrl &url );

        //Track Methods
        virtual QString name() const;
        virtual QString prettyName() const;
        virtual void setTitle( const QString &title );
        virtual qint64 length() const;
        virtual bool hasCapabilityInterface( Capabilities::Capability::Type type ) const;
        virtual Capabilities::Capability* createCapabilityInterface( Capabilities::Capability::Type type );
        virtual void finishedPlaying( double playedFraction );

        virtual Meta::ArtistPtr artist() const;
        virtual Meta::ComposerPtr composer() const;
        virtual Meta::GenrePtr genre() const;
        virtual Meta::YearPtr year() const;

        virtual Meta::TrackEditorPtr editor();

        //SqlPodcastEpisode specific methods
        bool writeTagsToFile();
        int dbId() const { return m_dbId; }

        void updateInDb();
        void deleteFromDb();

    private:
        /**
         * Establishes m_localFile using MetaProxy::Track if m_localUrl is valid.
         */
        void setupLocalFile();

        int m_dbId; //database ID
        bool m_isKeep; //Keep the download after purge or not?
        SqlPodcastChannelPtr m_channel; //the parent of this episode

        Meta::TrackPtr m_localFile;
};

class SqlPodcastChannel : public Podcasts::PodcastChannel
{
    public:
        static Playlists::PlaylistPtr toPlaylistPtr( SqlPodcastChannelPtr sqlChannel );
        static SqlPodcastChannelPtr fromPlaylistPtr( Playlists::PlaylistPtr playlist );

        SqlPodcastChannel( SqlPodcastProvider *provider, const QStringList &queryResult );

        /** Copy a PodcastChannel
        */
        SqlPodcastChannel( SqlPodcastProvider *provider, PodcastChannelPtr channel );

        ~SqlPodcastChannel();
        // Playlists::Playlist methods
        virtual void syncTrackStatus( int position, Meta::TrackPtr otherTrack );
        virtual int trackCount() const;

        virtual QString filenameLayout() const { return m_filenameLayout; }

        virtual Meta::TrackList tracks();
        virtual void addTrack( Meta::TrackPtr track, int position = -1 );

        virtual void triggerTrackLoad();
        virtual Playlists::PlaylistProvider *provider() const;

        virtual QStringList groups();
        virtual void setGroups( const QStringList &groups );

        //Podcasts::PodcastChannel methods
        virtual KUrl uidUrl() const;
        virtual void setTitle( const QString &title );
        virtual Podcasts::PodcastEpisodeList episodes() const;
        virtual bool hasImage() const { return !m_image.isNull(); }
        virtual void setImage( const QImage &image );
        virtual QImage image() const { return m_image; }
        virtual KUrl imageUrl() const { return m_imageUrl; }
        virtual void setImageUrl( const KUrl &imageUrl );
        virtual void setFilenameLayout( const QString &filenameLayout ) { m_filenameLayout = filenameLayout; }


        PodcastEpisodePtr addEpisode( PodcastEpisodePtr episode );

        //SqlPodcastChannel specific methods
        int dbId() const { return m_dbId; }
        //void addEpisode( SqlPodcastEpisodePtr episode ) { m_episodes << episode; }

        bool writeTags() const { return m_writeTags; }
        void setWriteTags( bool writeTags ) { m_writeTags = writeTags; }
        void updateInDb();
        void deleteFromDb();

        const SqlPodcastEpisodeList sqlEpisodes() { return m_episodes; }

        void loadEpisodes();
        void applyPurge();

    private:
        bool m_writeTags;
        int m_dbId; //database ID
        bool m_episodesLoaded;

        SqlPodcastEpisodeList m_episodes;
        bool m_trackCacheIsValid;
        Meta::TrackList m_episodesAsTracksCache;
        SqlPodcastProvider *m_provider;
        QString m_filenameLayout; //specifies filename layout for episodes
};

} //namespace Podcasts

Q_DECLARE_METATYPE( Podcasts::SqlPodcastEpisodePtr )
Q_DECLARE_METATYPE( Podcasts::SqlPodcastEpisodeList )
Q_DECLARE_METATYPE( Podcasts::SqlPodcastChannelPtr )
Q_DECLARE_METATYPE( Podcasts::SqlPodcastChannelList )

#endif
