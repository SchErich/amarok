/****************************************************************************************
 * Copyright (c) 2010 Rick W. Chen <stuffcorpse@archlinux.us>                           *
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

#include "NetworkProgressBar.h"

NetworkProgressBar::NetworkProgressBar( QWidget *parent, QNetworkReply *reply )
    : ProgressBar( parent )
{
    connect( reply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(infoMessage(QNetworkReply::NetworkError)) );
    connect( reply, SIGNAL(finished()), SLOT(finished()) );

    switch( reply->operation() )
    {
    case QNetworkAccessManager::HeadOperation:
    case QNetworkAccessManager::GetOperation:
        connect( reply, SIGNAL(downloadProgress(qint64,qint64)), SLOT(progressChanged(qint64,qint64)) );
        break;

    case QNetworkAccessManager::PutOperation:
    case QNetworkAccessManager::PostOperation:
        connect( reply, SIGNAL(uploadProgress(qint64,qint64)), SLOT(progressChanged(qint64,qint64)) );
        break;

    default:
        break;
    }
}

NetworkProgressBar::~NetworkProgressBar()
{
}

void NetworkProgressBar::progressChanged( qint64 bytesChanged, qint64 bytesTotal )
{
    qreal percent = qreal(bytesChanged) / bytesTotal;
    setValue( int(percent) * 100 );
}

void NetworkProgressBar::infoMessage( QNetworkReply::NetworkError code )
{
    if( code != QNetworkReply::NoError )
    {
        QNetworkReply *reply = qobject_cast<QNetworkReply*>( sender() );
        setDescription( reply->errorString() );
    }
}

void NetworkProgressBar::finished()
{
    // complete the progress no matter what; the finished() signal may be
    // emitted by abort() or any other error.
    setValue( maximum() );
}

#include "NetworkProgressBar.moc"