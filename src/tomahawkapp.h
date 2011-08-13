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

#ifndef TOMAHAWKAPP_H
#define TOMAHAWKAPP_H

#define APP TomahawkApp::instance()

#include "headlesscheck.h"
#include "config.h"

#include "mac/tomahawkapp_mac.h" // for PlatforInterface

#include <QRegExp>
#include <QFile>
#include <QSettings>
#include <QDir>

#include "QxtHttpServerConnector"
#include "QxtHttpSessionManager"

#include "typedefs.h"
#include "playlist.h"
#include "source.h"
#include "resolver.h"
#include "network/servent.h"

#include "utils/tomahawkutils.h"
#include "kdsingleapplicationguard/kdsingleapplicationguard.h"

class AudioEngine;
class Database;
class ScanManager;
class SipHandler;
class TomahawkSettings;
class XMPPBot;

namespace Tomahawk
{
    class ShortcutHandler;
    namespace InfoSystem
    {
        class InfoSystem;
    }
}

#ifdef LIBLASTFM_FOUND
#include <lastfm/NetworkAccessManager>
#include "scrobbler.h"
#endif

#ifndef TOMAHAWK_HEADLESS
class TomahawkWindow;
#endif


// this also acts as a a container for important top-level objects
// that other parts of the app need to find
// (eg, library, pipeline, friends list)
class TomahawkApp : public TOMAHAWK_APPLICATION, public Tomahawk::PlatformInterface
{
Q_OBJECT

public:
    TomahawkApp( int& argc, char *argv[] );
    virtual ~TomahawkApp();

    bool init();
    static TomahawkApp* instance();

    XMPPBot* xmppBot() { return m_xmppBot.data(); }

#ifndef TOMAHAWK_HEADLESS
    AudioControls* audioControls();
    TomahawkWindow* mainWindow() const { return m_mainwindow; }
#endif

    void enableScriptResolver( const QString& scriptPath );
    void disableScriptResolver( const QString& scriptPath );
    Tomahawk::ExternalResolver* resolverForPath( const QString& scriptPath );

    // PlatformInterface
    virtual bool loadUrl( const QString& url );

public slots:
    virtual void activate();
    void instanceStarted( KDSingleApplicationGuard::Instance );

private slots:
    void initServent();
    void initSIP();

private:
    void registerMetaTypes();

    void printHelp();

    // Start-up order: database, collection, pipeline, servent, http
    void initDatabase();
    void initLocalCollection();
    void initPipeline();

    void initHTTP();

    void loadPlugins();

    QList<Tomahawk::collection_ptr> m_collections;
    QHash<QString, Tomahawk::ExternalResolver*> m_scriptResolvers;

    QWeakPointer<Database> m_database;
    QWeakPointer<ScanManager> m_scanManager;
    QWeakPointer<AudioEngine> m_audioEngine;
    QWeakPointer<Servent> m_servent;
    QWeakPointer<Tomahawk::InfoSystem::InfoSystem> m_infoSystem;
    QWeakPointer<XMPPBot> m_xmppBot;
    QWeakPointer<Tomahawk::ShortcutHandler> m_shortcutHandler;
    bool m_scrubFriendlyName;

#ifdef LIBLASTFM_FOUND
    Scrobbler* m_scrobbler;
#endif

#ifndef TOMAHAWK_HEADLESS
    TomahawkWindow* m_mainwindow;
#endif

    bool m_headless;

    QxtHttpServerConnector m_connector;
    QxtHttpSessionManager m_session;
};

#endif // TOMAHAWKAPP_H

