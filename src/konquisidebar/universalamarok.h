/***************************************************************************
 *   Copyright (C) 2004 by Marco Gulino                                    *
 *   marco@Paganini                                                        *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef KONQUERORSIDEBAR_H
#define KONQUERORSIDEBAR_H

#include <konqsidebarplugin.h>

/**
@author Marco Gulino
*/

class KHTMLPart;
class QVBox;
class universalamarokwidget;
class DCOPClient;
class QFileInfo;
class QDateTime;
class UniversalAmarok : public KonqSidebarPlugin
{
Q_OBJECT
public:
    UniversalAmarok(KInstance *inst,QObject *parent,QWidget *widgetParent, QString &desktopName, const char* name=0);

    ~UniversalAmarok();

   virtual QWidget *getWidget(){return (QWidget*)widget;}
   virtual void *provides(const QString &) {return 0;}
   virtual void handleURL(const KURL &url) {}
    QString getCurrentPlaying();

private:
   QVBox* widget;
   KHTMLPart* browser;
   QString amarokPlaying;
   DCOPClient* amarokDCOP;
   QFileInfo* fileInfo;
   QDateTime fileDT;
   
public slots:
    void updateBrowser(const QString&);
    void updateStatus();
    void sendControl();
    void openURLRequest( const KURL &url );
};

#endif
