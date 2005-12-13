/***************************************************************************
                         main.cpp  -  description
                            -------------------
   begin                : Mit Okt 23 14:35:18 CEST 2002
   copyright            : (C) 2002 by Mark Kretschmann
   email                : markey@web.de
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "amarok.h"
#include "app.h"
#include "crashhandler.h"
#include <kaboutdata.h>

#include "metadata/tplugins.h"

//#define AMAROK_USE_DRKONQI

extern class KAboutData aboutData; //defined in amarokcore/app.cpp

int main( int argc, char *argv[] )
{
    aboutData.addAuthor( "Alexandre '" I18N_NOOP("Ain't afraid of no bugs") "' Oliveira",
            I18N_NOOP( "Developer (Untouchable)" ), "aleprj@gmail.com" );
    aboutData.addAuthor( "Christian '" I18N_NOOP("Babe-Magnet") "' Muehlhaeuser",
            I18N_NOOP( "Stud (muesli)" ), "chris@chris.de", "http://www.chris.de" );
    aboutData.addAuthor( "Frederik 'Ich bin kein Deustcher!' Holljen",
            I18N_NOOP( "733t code, OSD improvement, patches (Larson)" ), "fh@ez.no" );
    aboutData.addAuthor( "Gábor '" I18N_NOOP("Opera owns your mom") "' Lehel",
            I18N_NOOP( "Developer (illissius)" ), "illissius@gmail.com" );
    aboutData.addAuthor( "Ian '" I18N_NOOP("The Beard") "' Monroe",
            I18N_NOOP( "Developer (eean)" ), "ian@monroe.nu" );
    aboutData.addAuthor( "Mark '"I18N_NOOP("It's good, but it's not irssi") "' Kretschmann",
            I18N_NOOP( "Project founder (markey)" ), "markey@web.de" );
    aboutData.addAuthor( "Max '" I18N_NOOP("Turtle-Power") "' Howell",
            I18N_NOOP( "Cowboy mxcl" ), "max.howell@methylblue.com", "http://www.methylblue.com" );
    aboutData.addAuthor( "Mike '" I18N_NOOP("Purple is not girly!") "' Diehl",
            I18N_NOOP( "DCOP, improvements, Preci-i-o-u-u-s handbook maintainer (madpenguin8)" ), "madpenguin8@yahoo.com" );
    aboutData.addAuthor( "Pierpaolo '" I18N_NOOP("Spaghetti Coder") "' Di Panfilo",
            I18N_NOOP( "Playlist-browser, cover-manager (teax)" ), "pippo_dp@libero.it" );
    aboutData.addAuthor( "Roman '" I18N_NOOP("And God said, let there be Mac") "' Becker",
            I18N_NOOP( "amaroK logo, splash screen, icons" ), "roman@formmorf.de", "http://www.formmorf.de" );
    aboutData.addAuthor( "Seb '" I18N_NOOP("Surfin' down under") "' Ruiz",
            I18N_NOOP( "Playlist-browser, Dynamic-mode, Queue-manager (sebr)" ), "me@sebruiz.net", "http://www.sebruiz.net" );
    aboutData.addAuthor( "Stanislav '" I18N_NOOP("All you need is DCOP") "' Karchebny",
            I18N_NOOP( "DCOP, improvements, cleanups, i18n (berkus)" ), "berkus@madfire.net" );


    aboutData.addCredit( "Adam Pigg", I18N_NOOP( "Analyzers, patches" ), "adam@piggz.fsnet.co.uk" );
    aboutData.addCredit( "Adeodato Simó", I18N_NOOP( "Patches" ), "asp16@alu.ua.es" );
    aboutData.addCredit( "Andreas Mair", I18N_NOOP( "MySQL support" ), "am_ml@linogate.com" );
    aboutData.addCredit( "Andrew de Quincey", I18N_NOOP( "Postgresql support" ), "adq_dvb@lidskialf.net" );
    aboutData.addCredit( "Christie Harris", I18N_NOOP( "roKymoter (dangle)" ), "dangle.baby@gmail.com" );
    aboutData.addCredit( "Dan Leinir Turthra Jensen", I18N_NOOP( "First-run wizard, usability" ), "admin@REMOVEleinir.dk" );
    aboutData.addCredit( "Dan Meltzer", I18N_NOOP( "roKymoter (hydrogen)" ), "hydrogen@notyetimplemented.com" );
    aboutData.addCredit( "Enrico Ros", I18N_NOOP( "Analyzers, Context Browser and systray eye-candy" ), "eros.kde@email.it" );
    aboutData.addCredit( "Florian 'da-flow' Egerer", I18N_NOOP( "amaroK application icon 'Blue Wolf'" ), "flo_for_never@gmx.at" );
    aboutData.addCredit( "Gérard Dürrmeyer", I18N_NOOP( "icons and image work" ), "gerard@randomtree.com" );
    aboutData.addCredit( "Greg Meyer", I18N_NOOP( "Live CD, Bug squashing (oggb4mp3)" ), "greg@gkmweb.com" );
    aboutData.addCredit( "Harald Sitter", I18N_NOOP( "handbook enhancements, translations, bug fixes, screenshots, roKymoter (apachelogger)" ), "harald.sitter@kdemail.net" );
    aboutData.addCredit( "Jarkko Lehti", I18N_NOOP( "Tester, IRC channel operator, whipping" ), "grue@iki.fi" );
    aboutData.addCredit( "Jeff Mitchell", I18N_NOOP( "Fixed longstanding sqlite threading issue (jefferai)" ), "kde-dev@emailgoeshere.com" );
    aboutData.addCredit( "Jocke Andersson", I18N_NOOP( "roKymoter (Firetech)" ), "ajocke@gmail.com" );
    aboutData.addCredit( "Kenneth Wesley Wimer II", I18N_NOOP( "Icons" ), "kwwii@bootsplash.org" );
    aboutData.addCredit( "Kenny Lemieux", I18N_NOOP( "amaroK webmaster" ), "swaft@pwsp.net" );
    aboutData.addCredit( "Marco Gulino", I18N_NOOP( "Konqueror Sidebar, some DCOP methods" ), "marco@kmobiletools.org" );
    aboutData.addCredit( "Melchior Franz", I18N_NOOP( "FHT routine, bugfixes" ), "mfranz@kde.org" );
    aboutData.addCredit( "Michael Pyne", I18N_NOOP( "K3B export code" ), "michael.pyne@kdemail.net" );
    aboutData.addCredit( "Nenad Grujicic", I18N_NOOP( "Splash screen" ), "mchitman@neobee.net" );
    aboutData.addCredit( "Olivier Bédard", I18N_NOOP( "Website hosting" ), "paleo@pwsp.net" );
    aboutData.addCredit( "Paul Cifarelli", I18N_NOOP( "HelixPlayer engine" ), "paul@cifarelli.net" );
    aboutData.addCredit( "Reigo Reinmets", I18N_NOOP( "Wikipedia support, patches" ), "xatax@hot.ee" );
    aboutData.addCredit( "Roland Gigler", I18N_NOOP( "MAS engine" ), "rolandg@web.de" );
    aboutData.addCredit( "Sami Nieminen", I18N_NOOP( "Audioscrobbler support" ), "sami.nieminen@iki.fi" );
    aboutData.addCredit( "Scott Wheeler", I18N_NOOP( "TagLib & ktrm code" ), "wheeler@kde.org" );
    aboutData.addCredit( "Stefan Bogner", I18N_NOOP( "Loadsa stuff" ), "bochi@online.ms" );
    aboutData.addCredit( "Stefan Siegel", I18N_NOOP( "Patches, Bugfixes" ), "kde@sdas.de" );
    aboutData.addCredit( "Sven Krohlas", I18N_NOOP( "roKymoter (sven423)" ), "sven@asbest-online.de" );
    aboutData.addCredit( "Whitehawk Stormchaser", I18N_NOOP( "Tester, patches" ), "zerokode@gmx.net" );

    KApplication::disableAutoDcopRegistration();

    App::initCliArgs( argc, argv );
    App app;

    #ifndef AMAROK_USE_DRKONQI
    KCrash::setCrashHandler( amaroK::Crash::crashHandler );
    #endif

    registerTaglibPlugins();

    return app.exec();
}
