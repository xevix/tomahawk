/*
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License along
 *    with this program; if not, write to the Free Software Foundation, Inc.,
 *    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "collectionitem.h"

#include "utils/tomahawkutils.h"
#include "categoryitems.h"
#include "playlistitems.h"
#include "viewmanager.h"
#include "playlist.h"
#include "genericpageitems.h"

/// CollectionItem

using namespace Tomahawk;

CollectionItem::CollectionItem(  SourcesModel* mdl, SourceTreeItem* parent, const Tomahawk::source_ptr& source )
    : SourceTreeItem( mdl, parent, SourcesModel::Collection )
    , m_source( source )
    , m_playlists( 0 )
    , m_stations( 0 )
    , m_tempItem( 0 )
    , m_curTempPage( 0 )
{
    if( m_source.isNull() ) { // super collection
        connect( ViewManager::instance(), SIGNAL( tempPageActivated( Tomahawk::ViewPage*) ), this, SLOT( tempPageActivated( Tomahawk::ViewPage* ) ) );

        return;
    }
    // create category items if there are playlists to show, or stations to show
    QList< playlist_ptr > playlists = source->collection()->playlists();
    QList< dynplaylist_ptr > autoplaylists = source->collection()->autoPlaylists();
    QList< dynplaylist_ptr > stations = source->collection()->stations();

    if ( !playlists.isEmpty() || !autoplaylists.isEmpty() || source->isLocal() )
    {
        m_playlists = new CategoryItem( model(), this, SourcesModel::PlaylistsCategory, source->isLocal() );
        onPlaylistsAdded( playlists );
        onAutoPlaylistsAdded( autoplaylists );
    }
    if ( !stations.isEmpty() || source->isLocal() )
    {
        m_stations = new CategoryItem( model(), this, SourcesModel::StationsCategory, source->isLocal() );
        onStationsAdded( stations );
    }

    if( ViewManager::instance()->pageForCollection( source->collection() ) )
        model()->linkSourceItemToPage( this, ViewManager::instance()->pageForCollection( source->collection() ) );

    // load auto playlists and stations!

    connect( source.data(), SIGNAL( stats( QVariantMap ) ), this, SIGNAL( updated() ) );
    connect( source.data(), SIGNAL( syncedWithDatabase() ), this, SIGNAL( updated() ) );
    connect( source.data(), SIGNAL( playbackStarted( Tomahawk::query_ptr ) ), this, SIGNAL( updated() ) );
    connect( source.data(), SIGNAL( stateChanged() ), this, SIGNAL( updated() ) );
    connect( source.data(), SIGNAL( offline() ), this, SIGNAL( updated() ) );
    connect( source.data(), SIGNAL( online() ), this, SIGNAL( updated() ) );

    connect( source->collection().data(), SIGNAL( playlistsAdded( QList<Tomahawk::playlist_ptr> ) ),
             SLOT( onPlaylistsAdded( QList<Tomahawk::playlist_ptr> ) ), Qt::QueuedConnection );
    connect( source->collection().data(), SIGNAL( autoPlaylistsAdded( QList< Tomahawk::dynplaylist_ptr > ) ),
             SLOT( onAutoPlaylistsAdded( QList<Tomahawk::dynplaylist_ptr> ) ), Qt::QueuedConnection );
    connect( source->collection().data(), SIGNAL( stationsAdded( QList<Tomahawk::dynplaylist_ptr> ) ),
             SLOT( onStationsAdded( QList<Tomahawk::dynplaylist_ptr> ) ), Qt::QueuedConnection );
}


Tomahawk::source_ptr
CollectionItem::source() const
{
    return m_source;
}


QString
CollectionItem::text() const
{
    return m_source.isNull() ? tr( "Super Collection" ) : m_source->friendlyName();
}


int
CollectionItem::peerSortValue() const
{
    if( m_source.isNull() )
        return -1;
    if( m_source->isLocal() )
        return 0;

    return m_source->id();
}


void
CollectionItem::activate()
{
    ViewPage* p = 0;
    if ( source().isNull() )
        p = ViewManager::instance()->showSuperCollection();
    else
        p = ViewManager::instance()->show( source()->collection() );

    model()->linkSourceItemToPage( this, p );
}


QIcon
CollectionItem::icon() const
{
    if( m_source.isNull() )
        return QIcon( RESPATH "images/supercollection.png" );
    else
    {
        if( m_source->avatar().isNull() )
            return QIcon( RESPATH "images/user-avatar.png" );
        else
            return QIcon( m_source->avatar() );
    }
}


void
CollectionItem::playlistsAddedInternal( SourceTreeItem* parent, const QList< dynplaylist_ptr >& playlists )
{
    QList< SourceTreeItem* > items;
    int addOffset = playlists.first()->author()->isLocal() ? 1 : 0;

    int from = parent->children().count() - addOffset;
    parent->beginRowsAdded( from, from + playlists.count() - 1 );
    foreach( const dynplaylist_ptr& p, playlists )
    {
        DynamicPlaylistItem* plItem = new DynamicPlaylistItem( model(), parent, p, parent->children().count() - addOffset );
//        qDebug() << "Dynamic Playlist added:" << p->title() << p->creator() << p->info();
        p->loadRevision();
        items << plItem;

        if( p->mode() == Static ) {
            connect( p.data(), SIGNAL( aboutToBeDeleted( Tomahawk::dynplaylist_ptr ) ),
                     SLOT( onAutoPlaylistDeleted( Tomahawk::dynplaylist_ptr ) ), Qt::QueuedConnection );
        } else {
            connect( p.data(), SIGNAL( aboutToBeDeleted( Tomahawk::dynplaylist_ptr ) ),
                     SLOT( onStationDeleted( Tomahawk::dynplaylist_ptr ) ), Qt::QueuedConnection );
        }
    }
    parent->endRowsAdded();
}


template< typename T >
void
CollectionItem::playlistDeletedInternal( SourceTreeItem* parent, const T& p )
{
    Q_ASSERT( parent ); // How can we delete playlists if we have none?
    int curCount = parent->children().count();
    for( int i = 0; i < curCount; i++ ) {
        PlaylistItem* pl = qobject_cast< PlaylistItem* >( parent->children().at( i ) );
        if( pl && pl->playlist() == p ) {
            parent->beginRowsRemoved( i, i );
            parent->removeChild( pl );
            parent->endRowsRemoved();
            break;
        }
    }
}


void
CollectionItem::onPlaylistsAdded( const QList< playlist_ptr >& playlists )
{
//    qDebug() << Q_FUNC_INFO << m_source->friendlyName() << playlists.count();

    if( playlists.isEmpty() )
        return;

    if( !m_playlists )
    {
        // add the category too
        int cur = children().count();
        beginRowsAdded( cur, cur );
        m_playlists = new CategoryItem( model(), this, SourcesModel::PlaylistsCategory, source()->isLocal() );
        endRowsAdded();
    }

    QList< SourceTreeItem* > items;
    int addOffset = playlists.first()->author()->isLocal() ? 1 : 0;

    int from = m_playlists->children().count() - addOffset;
    m_playlists->beginRowsAdded( from, from + playlists.count() - 1 );
    foreach( const playlist_ptr& p, playlists )
    {
        PlaylistItem* plItem = new PlaylistItem( model(), m_playlists, p, m_playlists->children().count() - addOffset );
//        qDebug() << "Playlist added:" << p->title() << p->creator() << p->info();
        p->loadRevision();
        items << plItem;

        connect( p.data(), SIGNAL( aboutToBeDeleted( Tomahawk::playlist_ptr ) ),
                 SLOT( onPlaylistDeleted( Tomahawk::playlist_ptr ) ), Qt::QueuedConnection );

    }
    m_playlists->endRowsAdded();
}


void
CollectionItem::onPlaylistDeleted( const  playlist_ptr& playlist )
{
    playlistDeletedInternal( m_playlists, playlist );
}


void
CollectionItem::onAutoPlaylistsAdded( const QList< dynplaylist_ptr >& playlists )
{
    if( playlists.isEmpty() )
        return;

    if( !m_playlists ) { // add the category too
        int cur = children().count();
        beginRowsAdded( cur, cur );
        m_playlists = new CategoryItem( model(), this, SourcesModel::PlaylistsCategory, source()->isLocal() );
        endRowsAdded();
    }

    playlistsAddedInternal( m_playlists, playlists );
}


void
CollectionItem::onAutoPlaylistDeleted( const dynplaylist_ptr& playlist )
{
    if( !m_playlists )
        qDebug() << "NO playlist category item for a deleting playlist..";

    playlistDeletedInternal( m_playlists, playlist );
}


void
CollectionItem::onStationsAdded( const QList< dynplaylist_ptr >& stations )
{
    if( stations.isEmpty() )
        return;

    if( !m_stations ) { // add the category too
        int cur = children().count();
        beginRowsAdded( cur, cur );
        m_stations = new CategoryItem( model(), this, SourcesModel::StationsCategory, source()->isLocal() );
        endRowsAdded();
    }

    playlistsAddedInternal( m_stations, stations );
}


void
CollectionItem::onStationDeleted( const dynplaylist_ptr& station )
{
    playlistDeletedInternal( m_stations, station );
}

void
CollectionItem::tempPageActivated( Tomahawk::ViewPage* v )
{
    QString name = v->title();
    m_curTempPage = v;
    if( !m_tempItem ) {
        emit beginRowsAdded( children().count(), children().count() );
        m_tempItem = new GenericPageItem( model(), this, name, QIcon( RESPATH "images/playlist-icon.png" ),
                                          boost::bind( &CollectionItem::tempItemClicked, this ),
                                          boost::bind( &CollectionItem::getTempPage, this )
                                        );
        emit endRowsAdded();
    } else {
        m_tempItem->setText( name );
    }

    model()->linkSourceItemToPage( m_tempItem, v );
    emit selectRequest( m_tempItem );
}

ViewPage*
CollectionItem::tempItemClicked()
{
    if( m_curTempPage ) {
        // show the last temporary page the user displayed
        return ViewManager::instance()->show( m_curTempPage );
    }

    return 0;
}

ViewPage*
CollectionItem::getTempPage() const
{
    return m_curTempPage;
}
