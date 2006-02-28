/* NMM - Network-Integrated Multimedia Middleware
 *
 * Copyright (C) 2002-2006
 *                    NMM work group,
 *                    Computer Graphics Lab,
 *                    Saarland University, Germany
 *                    http://www.networkmultimedia.org
 *
 * Maintainer:        Robert Gogolok <gogo@graphics.cs.uni-sb.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Steet, Fifth Floor, Boston, MA  02111-1307
 * USA
 */


#ifndef NMM_ENGINE_H
#define NMM_ENGINE_H

#include <config.h>

#include "engine/enginebase.h"
#include <nmm/base/graph/CompositeNode.hpp>
#include <nmm/base/NMMApplication.hpp>
#include <nmm/base/EDObject.hpp>
#include <nmm/base/sync/MultiAudioVideoSynchronizer.hpp>

using namespace NMM;

class NmmLocation;

class NmmEngine : public Engine::Base
{
Q_OBJECT
public:
    NmmEngine();
    ~NmmEngine();

    bool init();
    bool canDecode(const KURL&) const;

    uint position() const;

    uint length() const;

    Engine::State state() const;

    amaroK::PluginConfig* configure() const;

    QValueList<NmmLocation> environmentHostList() const {return tmp_environment_list;}
    void setEnvironmentHostList(QValueList<NmmLocation> list) { tmp_environment_list = list;}

    static NmmEngine* instance() { return s_instance; }

public slots:
    bool  load(const KURL&, bool stream = false);
    bool  play(unsigned int offset = 0);
    void  stop();
    void  pause();
    void  seek(uint);

private slots:
    void endOfStreamReached();

    /**
     * Checks for local NMM security options.
     * Warns user if ~/.nmmrc doesn't have 
     * allowedwritepaths or allowedreadpaths set.
     * TODO: Should be called on NMM engine load and not from ::load.
     */
    void checkSecurity();

protected:
    void  setVolumeSW( uint );

private:

    void cleanup();

    void createEnvironmentHostList();

    /**
     * Returns sink locations for audio/video playback.
     *
     * \param audio return audio locations, else video locations
     *
     * \return the audio/video locations
     */
    QStringList getSinkHosts( bool audio = true );

    /**
     * This method is called when a setProgress event is received. The two parameters represent a rational number
     * (numerator and denominator) containing the amount of progress as a value between 0 and 1.
     */
    Result setProgress(u_int64_t&, u_int64_t&);

    /**
     * This method is called when an endTrack event is received.
     */
    Result endTrack();

    /**
     * This method is called when a syncReset event is received. When an NMM source node has finished seeking, such an event is
     * sent. Here, it is used to prevent the engine from updating the track position while receiving setProgress events before seeking
     * is done, since these setProgress events may contain old progress information. Otherwise, the progress slider would jump
     * back and forth...
     */
    Result syncReset();

    /**
     * This method is called when a trackDuration event is received. The duration of a track is represented by
     * an Interval that contains the time in seconds and nanoseconds.
     */
    Result trackDuration(Interval& duration);

    /**
     * The current track position in milliseconds.
     */
    u_int64_t __position;

    /**
     * The length of the track in milliseconds.
     */
    u_int64_t __track_length;

    /**
     * The current engine state
     */
    Engine::State __state;

    /**
     * The NMM application object.
     */
    NMMApplication* __app;

    /**
     * Event listeners for various NMM events.
     */
    TEDObject0<NmmEngine> __endTrack_listener;
    TEDObject0<NmmEngine> __syncReset_listener;
    TEDObject2<NmmEngine, u_int64_t, u_int64_t> __setProgress_listener;
    TEDObject1<NmmEngine, Interval> __trackDuration_listener;

    /**
     * The composite node that contains the graph created by the GraphBuilder.
     */
    CompositeNode* __composite;

    /**
     * The node for audio playback 
     * where the various events like endTrack, setProgress etc. are caught 
     * if video is disabled.
     */
    INode* __playback;

    /**
     * The display node for video playback
     * where the various events like endTrack, setProgress etc. are caught 
     * if video is enabled.
     */
    INode* __display;

    /**
     * synchronizer for graph builder
     */
    MultiAudioVideoSynchronizer* __av_sync;

    /**
     * synchronizer interface
     */
    IMultiAudioVideoSynchronizer* __synchronizer;

    /**
     * Indicates whether we are playing a video.
     */
    bool __with_video;

    /**
     * This flag is set during seeking.
     */
    bool __seeking;

    /**
     * Environment variables host list.
     * Only read on startup, volume can be changed via settings dialog.
     */
     QValueList<NmmLocation> tmp_environment_list;

    static NmmEngine* s_instance;
};

#endif
