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

#ifndef DYNAMIC_PLAYLIST_H
#define DYNAMIC_PLAYLIST_H

#include <QObject>
#include <QList>
#include <QSharedPointer>

#include "playlist.h"
#include "typedefs.h"
#include "playlist/dynamic/DynamicControl.h"

#include "dllmacro.h"

class DatabaseCommand_LoadAllDynamicPlaylists;
class DatabaseCommand_SetDynamicPlaylistRevision;
class DatabaseCommand_CreateDynamicPlaylist;
class DatabaseCollection;

namespace Tomahawk {

/**
 * Subclass of playlist that adds the information needed to store a dynamic playlist.
 *  It uses normal PlaylistEntries but also has a mode, a generator, and a list of controls
*/

struct DLLEXPORT DynamicPlaylistRevision : PlaylistRevision
{
    QList< dyncontrol_ptr > controls;
    Tomahawk::GeneratorMode mode;
    QString type;

    DynamicPlaylistRevision( const PlaylistRevision& other )
    {
        revisionguid = other.revisionguid;
        oldrevisionguid = other.oldrevisionguid;
        newlist = other.newlist;
        added = other.added;
        removed = other.removed;
        applied = other.applied;
    }

    DynamicPlaylistRevision() {}
};

class DLLEXPORT DynamicPlaylist : public Playlist
{
    Q_OBJECT

    // :-( int becuase qjson chokes on my enums
    Q_PROPERTY( int     mode                  WRITE setMode   READ mode )
    Q_PROPERTY( QString type                  WRITE setType   READ type )

    friend class ::DatabaseCommand_SetDynamicPlaylistRevision;
    friend class ::DatabaseCommand_CreateDynamicPlaylist;
    friend class ::DatabaseCollection; /// :-(

public:
    virtual ~DynamicPlaylist();

    /// Generate an empty dynamic playlist with default generator
    static Tomahawk::dynplaylist_ptr create( const source_ptr& author,
                                          const QString& guid,
                                          const QString& title,
                                          const QString& info,
                                          const QString& creator,
                                          GeneratorMode mode,
                                          bool shared,
                                          const QString& type = QString()
                                          );
    static bool remove( const dynplaylist_ptr& playlist );

    virtual void loadRevision( const QString& rev = "" );

    // :-( int becuase qjson chokes on my enums
    int mode() const;
    QString type() const;
    geninterface_ptr generator() const;

    // Creates a new revision from the playlist in memory. Use this is you change the controls or
    // mode of a playlist and want to save it to db/others.
    void createNewRevision( const QString& uuid = QString() );

    virtual void addEntries( const QList< query_ptr >& queries, const QString& oldrev );
    virtual void addEntry( const Tomahawk::query_ptr& query, const QString& oldrev );

    // <IGNORE hack="true">
    // these need to exist and be public for the json serialization stuff
    // you SHOULD NOT call them.  They are used for an alternate CTOR method from json.
    // maybe friend QObjectHelper and make them private?
    explicit DynamicPlaylist( const source_ptr& author, const QString& type );
    void setMode( int mode );
    void setType( const QString& /*type*/ )           { /** TODO */; }
    void setGenerator( const geninterface_ptr& gen_ptr );
    // </IGNORE>

signals:
    /// emitted when the playlist revision changes (whenever the playlist changes)
    void dynamicRevisionLoaded( Tomahawk::DynamicPlaylistRevision );

    void aboutToBeDeleted( const Tomahawk::dynplaylist_ptr& pl );
    void deleted( const Tomahawk::dynplaylist_ptr& pl );

public slots:
    // want to update the playlist from the model?
    // generate a newrev using uuid() and call this:
    // if this is a static playlist, pass it a new list of entries. implicitly sets mode to static
    void createNewRevision( const QString& newrev, const QString& oldrev, const QString& type, const QList< dyncontrol_ptr>& controls, const QList< plentry_ptr >& entries );
    // if it is ondemand, no entries are needed implicitly sets mode to ondemand
    void createNewRevision( const QString& newrev, const QString& oldrev, const QString& type, const QList< dyncontrol_ptr>& controls );

    void reportCreated( const Tomahawk::dynplaylist_ptr& self );
    void reportDeleted( const Tomahawk::dynplaylist_ptr& self );

    // called from setdynamicplaylistrevision db cmd
    // 4 options, because dbcmds can't create qwidgets:
    // static version,   qvariant controls
    // static version,   dyncontrol_ptr controls
    // ondemand version, qvariant controls
    // ondemand version, dyncontrol_ptr controls
    void setRevision( const QString& rev,
                      const QList<QString>& neworderedguids,
                      const QList<QString>& oldorderedguids,
                      const QString& type,
                      const QList< QVariantMap >& controls,
                      bool is_newest_rev,
                      const QMap< QString, Tomahawk::plentry_ptr >& addedmap,
                      bool applied );
    void setRevision( const QString& rev,
                      const QList<QString>& neworderedguids,
                      const QList<QString>& oldorderedguids,
                      const QString& type,
                      const QList< Tomahawk::dyncontrol_ptr >& controls,
                      bool is_newest_rev,
                      const QMap< QString, Tomahawk::plentry_ptr >& addedmap,
                      bool applied );
    // ondemand version
    void setRevision( const QString& rev,
                      bool is_newest_rev,
                      const QString& type,
                      const QList< QVariantMap>& controls,
                      bool applied );
    void setRevision( const QString& rev,
                      bool is_newest_rev,
                      const QString& type,
                      const QList< Tomahawk::dyncontrol_ptr>& controls,
                      bool applied );
private:
    // called from loadAllPlaylists DB cmd via databasecollection (in GUI thread)
    explicit DynamicPlaylist( const source_ptr& src,
                       const QString& currentrevision,
                       const QString& title,
                       const QString& info,
                       const QString& creator,
                       uint createdOn,
                       const QString& type,
                       GeneratorMode mode,
                       bool shared,
                       int lastmod,
                       const QString& guid = "" ); // populate db

    // called when creating new playlist
    explicit DynamicPlaylist( const source_ptr& author,
                       const QString& guid,
                       const QString& title,
                       const QString& info,
                       const QString& creator,
                       const QString& type,
                       GeneratorMode mode,
                       bool shared );

private:
    QList< dyncontrol_ptr > variantsToControl( const QList< QVariantMap >& controlsV );
    geninterface_ptr m_generator;

};

}; // namespace

#endif
