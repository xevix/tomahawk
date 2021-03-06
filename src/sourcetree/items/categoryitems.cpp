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

#include "categoryitems.h"

#include <QMimeData>

#include "tomahawkapp.h"
#include "viewmanager.h"
#include "viewpage.h"
#include "sourcelist.h"
#include "sourcetreeview.h"
#include "utils/tomahawkutils.h"
#include "widgets/newplaylistwidget.h"
#include "widgets/playlisttypeselectordlg.h"
#include <playlist/dynamic/GeneratorInterface.h>
#include "utils/logger.h"
#include <globalactionmanager.h>

using namespace Tomahawk;


/// CategoryAddItem

CategoryAddItem::CategoryAddItem( SourcesModel* model, SourceTreeItem* parent, SourcesModel::CategoryType type )
    : SourceTreeItem( model, parent, SourcesModel::CategoryAdd )
    , m_categoryType( type )
{
}


CategoryAddItem::~CategoryAddItem()
{
}


QString
CategoryAddItem::text() const
{
    switch( m_categoryType ) {
        case SourcesModel::PlaylistsCategory:
            return tr( "New Playlist" );
        case SourcesModel::StationsCategory:
            return tr( "New Station" );
    }

    return QString();
}


void
CategoryAddItem::activate()
{
    switch( m_categoryType )
    {
        case SourcesModel::PlaylistsCategory: {

            PlaylistTypeSelectorDlg playlistSelectorDlg( TomahawkApp::instance()->mainWindow() );
            int successfulReturn = playlistSelectorDlg.exec();

            if ( !playlistSelectorDlg.playlistTypeIsAuto() && successfulReturn ) {

                // only show if none is shown yet
                if( !ViewManager::instance()->isNewPlaylistPageVisible() ) {
                    //fix this namespace resolution problem, was not there before
                    Tomahawk::ViewPage* p = ViewManager::instance()->show( new NewPlaylistWidget() );
                    model()->linkSourceItemToPage( this, p );
                }

            } else if ( playlistSelectorDlg.playlistTypeIsAuto() && successfulReturn ) {
               // create Auto Playlist
               QString playlistName = playlistSelectorDlg.playlistName();
               APP->mainWindow()->createAutomaticPlaylist( playlistName );
            } else if ( !successfulReturn ) {
                model()->viewPageActivated( ViewManager::instance()->currentPage() );
            }

            break;
                }
        case SourcesModel::StationsCategory:
            APP->mainWindow()->createStation();
            break;
    }
}


Qt::ItemFlags
CategoryAddItem::flags() const
{
    switch ( m_categoryType )
    {
        case SourcesModel::PlaylistsCategory:
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDropEnabled;
        case SourcesModel::StationsCategory:
        default:
            return Qt::ItemIsEnabled;
            break;
    }
}


QIcon
CategoryAddItem::icon() const
{
    return QIcon( RESPATH "images/add.png" );
}


bool
CategoryAddItem::willAcceptDrag( const QMimeData* data ) const
{
    if ( ( m_categoryType == SourcesModel::PlaylistsCategory || m_categoryType == SourcesModel::StationsCategory ) && GlobalActionManager::instance()->acceptsMimeData( data ) )
    {
        return true;
    }
    return false;
}


bool
CategoryAddItem::dropMimeData( const QMimeData* data, Qt::DropAction )
{
    // Create a new playlist seeded with these items
    connect( GlobalActionManager::instance(), SIGNAL( tracks( QList< Tomahawk::query_ptr > ) ), this, SLOT( parsedDroppedTracks( QList< Tomahawk::query_ptr > ) ) );
    GlobalActionManager::instance()->tracksFromMimeData( data );

    return true;
}

void
CategoryAddItem::parsedDroppedTracks( const QList< query_ptr >& tracks )
{
    disconnect( GlobalActionManager::instance(), SIGNAL( tracks( QList< Tomahawk::query_ptr > ) ), this, SLOT( parsedDroppedTracks( QList< Tomahawk::query_ptr > ) ) );
    if( m_categoryType == SourcesModel::PlaylistsCategory ) {

        playlist_ptr newpl = Playlist::create( SourceList::instance()->getLocal(), uuid(), "New Playlist", "", SourceList::instance()->getLocal()->friendlyName(), false, tracks );
        ViewManager::instance()->show( newpl );

        // Give a shot to try to rename it. The playlist has to be created first. ugly.
        QTimer::singleShot( 300, APP->mainWindow()->sourceTreeView(), SLOT( renamePlaylist() ) );
    } else if( m_categoryType == SourcesModel::StationsCategory ) {
        // seed the playlist with these song filters
        QString name = tracks.isEmpty() ? tr( "New Station" ) : tr( "%1 Station" ).arg( tracks.first()->track() );
        dynplaylist_ptr newpl = DynamicPlaylist::create( SourceList::instance()->getLocal(), uuid(), name, "", SourceList::instance()->getLocal()->friendlyName(), OnDemand, false );
        newpl->setMode( OnDemand );

        // now we want to add each query as a song filter...
        QList< dyncontrol_ptr > contrls;
        foreach( const Tomahawk::query_ptr& q, tracks ) {
            dyncontrol_ptr c = newpl->generator()->createControl( "Song" );
            c->setInput( QString( "%1 %2" ).arg( q->track() ).arg( q->artist() ) );
            contrls << c;
        }

        newpl->createNewRevision( uuid(), newpl->currentrevision(), newpl->type(), contrls );

        ViewManager::instance()->show( newpl );
        // Give a shot to try to rename it. The playlist has to be created first. ugly.
        QTimer::singleShot( 300, APP->mainWindow()->sourceTreeView(), SLOT( renamePlaylist() ) );
    }
}


int
CategoryAddItem::peerSortValue() const
{
    return INT_MAX; // after any siblings
}


/// CategoryItem

CategoryItem::CategoryItem( SourcesModel* model, SourceTreeItem* parent, SourcesModel::CategoryType category, bool showAddItem )
    : SourceTreeItem( model, parent, SourcesModel::Category )
    , m_category( category )
    , m_addItem( 0 )
    , m_showAdd( showAddItem )
{
    // in the constructor we're still being added to the parent, so we don't exist to have rows addded yet. so this is safe.
    //     beginRowsAdded( 0, 0 );
    if( m_showAdd ) {
        m_addItem = new CategoryAddItem( model, this, m_category );
    }
    //     endRowsAdded();
}


void
CategoryItem::insertItem( SourceTreeItem* item )
{
    insertItems( QList< SourceTreeItem* >() << item );
}


void
CategoryItem::insertItems( QList< SourceTreeItem* > items )
{
    // add the items to the category, and connect to the signals
    int curCount = children().size();
    if( m_showAdd ) // if there's an add item, add it before that
        curCount--;
    beginRowsAdded( curCount, curCount + items.size() - 1 );
    foreach( SourceTreeItem* item, items ) {
        insertChild( children().count() - 1, item );
    }
    endRowsAdded();
}


int
CategoryItem::peerSortValue() const
{
    if( m_category == SourcesModel::PlaylistsCategory )
        return -100;
    else if( m_category == SourcesModel::StationsCategory )
        return 100;
    else
        return 0;
}


void
CategoryItem::activate()
{
    if( m_category == SourcesModel::StationsCategory ) {
        // TODO activate stations page
    }

}
