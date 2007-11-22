/*
   Copyright (C) 2007 Maximilian Kossick <maximilian.kossick@googlemail.com>

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

#ifndef AMAROK_METAPROXY_H
#define AMAROK_METAPROXY_H

#include "Meta.h"
#include "meta/Capability.h"

#include <QObject>

namespace MetaProxy
{
    class Track : public Meta::Track
    {
        public:
            class Private;

            Track( const KUrl &url );
            virtual ~Track();

        //methods inherited from Meta::MetaBase
            virtual QString name() const;
            virtual QString prettyName() const;
            virtual QString fullPrettyName() const;
            virtual QString sortableName() const;

        //methods inherited from Meta::Track
            virtual KUrl playableUrl() const;
            virtual QString prettyUrl() const;
            virtual QString url() const;

            virtual bool isPlayable() const;

            virtual Meta::AlbumPtr album() const;
            virtual Meta::ArtistPtr artist() const;
            virtual Meta::GenrePtr genre() const;
            virtual Meta::ComposerPtr composer() const;
            virtual Meta::YearPtr year() const;

            virtual QString comment() const;

            virtual double score() const;
            virtual void setScore( double newScore );

            virtual int rating() const;
            virtual void setRating( int newRating );

            virtual int trackNumber() const;
            virtual int discNumber() const;

            virtual int length() const;
            virtual int filesize() const;
            virtual int sampleRate() const;
            virtual int bitrate() const;
            virtual uint lastPlayed() const;
            virtual int playCount() const;

            virtual QString type() const;

            virtual void finishedPlaying( double playedFraction );

            virtual bool inCollection() const;
            virtual Collection *collection() const;

            virtual void subscribe( Meta::Observer *observer );
            virtual void unsubscribe( Meta::Observer *observer );

            virtual bool hasCapabilityInterface( Meta::Capability::Type type ) const;
            virtual Meta::Capability* asCapabilityInterface( Meta::Capability::Type type );

            virtual bool operator==( const Meta::Track &track ) const;

        private:
            Private * const d;
    };

}

#endif
