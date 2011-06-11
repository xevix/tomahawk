/* === This file is part of Tomahawk Player - <http://tomahawk-player.org> ===
 *
 *   Copyright 2010-2011, Christian Muehlhaeuser <muesli@tomahawk-player.org>
 *
 *   Tomahawk is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Tomahawk is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Tomahawk. If not, see <http://www.gnu.org/licenses/>.
 */

#include "collectionproxymodel.h"

#include <QDebug>
#include <QTreeView>

#include "album.h"
#include "query.h"


CollectionProxyModel::CollectionProxyModel( QObject* parent )
    : TrackProxyModel( parent )
{
}


bool
CollectionProxyModel::lessThan( const QModelIndex& left, const QModelIndex& right ) const
{
    TrackModelItem* p1 = itemFromIndex( left );
    TrackModelItem* p2 = itemFromIndex( right );

    if ( !p1 )
        return true;
    if ( !p2 )
        return false;

    const Tomahawk::query_ptr& q1 = p1->query();
    const Tomahawk::query_ptr& q2 = p2->query();

    QString artist1 = q1->artist();
    QString artist2 = q2->artist();
    QString album1 = q1->album();
    QString album2 = q2->album();
    QString track1 = q1->track();
    QString track2 = q2->track();
    unsigned int albumpos1 = 0, albumpos2 = 0;
    unsigned int bitrate1 = 0, bitrate2 = 0;
    unsigned int mtime1 = 0, mtime2 = 0;
    unsigned int id1 = 0, id2 = 0;
    unsigned int size1 = 0, size2 = 0;

    if ( q1->numResults() )
    {
        const Tomahawk::result_ptr& r = q1->results().at( 0 );
        artist1 = r->artist()->name();
        album1 = r->album()->name();
        track1 = r->track();
        albumpos1 = r->albumpos();
        bitrate1 = r->bitrate();
        mtime1 = r->modificationTime();
        id1 = r->dbid();
        size1 = r->size();
    }
    if ( q2->numResults() )
    {
        const Tomahawk::result_ptr& r = q2->results().at( 0 );
        artist2 = r->artist()->name();
        album2 = r->album()->name();
        track2 = r->track();
        albumpos2 = r->albumpos();
        bitrate2 = r->bitrate();
        mtime2 = r->modificationTime();
        id2 = r->dbid();
        size2 = r->size();
    }

    if ( left.column() == TrackModel::Artist ) // sort by artist
    {
        if ( artist1 == artist2 )
        {
            if ( album1 == album2 )
            {
                if ( albumpos1 == albumpos2 )
                    return id1 < id2;

                return albumpos1 < albumpos2;
            }

            return QString::localeAwareCompare( album1, album2 ) < 0;
        }

        return QString::localeAwareCompare( artist1, artist2 ) < 0;
    }
    else if ( left.column() == TrackModel::Album ) // sort by album
    {
        if ( album1 == album2 )
        {
            if ( albumpos1 == albumpos2 )
                return id1 < id2;

            return albumpos1 < albumpos2;
        }

        return QString::localeAwareCompare( album1, album2 ) < 0;
    }
    else if ( left.column() == TrackModel::Bitrate ) // sort by bitrate
    {
        if ( bitrate1 == bitrate2 )
            return id1 < id2;

        return bitrate1 < bitrate2;
    }
    else if ( left.column() == TrackModel::Age ) // sort by mtime
    {
        if ( mtime1 == mtime2 )
            return id1 < id2;

        return mtime1 < mtime2;
    }
    else if ( left.column() == TrackModel::Filesize ) // sort by file size
    {
        if ( size1 == size2 )
            return id1 < id2;

        return size1 < size2;
    }
    return QString::localeAwareCompare( sourceModel()->data( left ).toString(),
                                        sourceModel()->data( right ).toString() ) < 0;
}
