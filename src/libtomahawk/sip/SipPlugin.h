/* === This file is part of Tomahawk Player - <http://tomahawk-player.org> ===
 *
 *   Copyright 2010-2011, Christian Muehlhaeuser <muesli@tomahawk-player.org>
 *             2011, Dominik Schmidt <dev@dominik-schmidt.de>
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

#ifndef SIPPLUGIN_H
#define SIPPLUGIN_H

#include "sourceinfo.h"
#include "sipinfo.h"

#include <QObject>
#include <QString>
#include <QMenu>
#include <QNetworkProxy>


#include "dllmacro.h"
#include "sourceinfo.h"

class SipPlugin;

class DLLEXPORT SipPluginFactory : public QObject
{
    Q_OBJECT
public:
    SipPluginFactory() {}
    virtual ~SipPluginFactory() {}

    // display name for plugin
    virtual QString prettyName() const = 0;
    // internal name
    virtual QString factoryId() const = 0;
    // if the user can create multiple
    virtual QIcon icon() const { return QIcon(); }
    virtual bool isUnique() const { return false; }

    virtual SipPlugin* createPlugin( const QString& pluginId = QString() ) = 0;

protected:
    QString generateId();
};

class DLLEXPORT SipPlugin : public QObject
{
    Q_OBJECT

public:
    enum SipErrorCode { AuthError, ConnectionError }; // Placeholder for errors, to be defined
    enum ConnectionState { Disconnected, Connecting, Connected, Disconnecting };

    explicit SipPlugin( const QString& pluginId, QObject* parent = 0 );
    virtual ~SipPlugin() {}

    // plugin id is "pluginfactoryname_someuniqueid".  get it from SipPluginFactory::generateId
    QString pluginId() const;

    virtual bool isValid() const = 0;
    virtual const QString name() const = 0;
    virtual const QString friendlyName() const = 0;
    virtual const QString accountName() const = 0;
    virtual ConnectionState connectionState() const = 0;
    virtual QString errorMessage() const;
    virtual QMenu* menu();
    virtual QWidget* configWidget();
    virtual void saveConfig() {} // called when the widget has been edited
    virtual QIcon icon() const;

    // peer infos
    virtual const QStringList peersOnline() const;

public slots:
    virtual bool connectPlugin( bool startup = false ) = 0;
    virtual void disconnectPlugin() = 0;
    virtual void checkSettings() = 0;
    virtual void refreshProxy();

    virtual void addContact( const QSharedPointer< SourceInfo >& sourceInfo, const QString& msg = QString() ) = 0;
    virtual void sendMessage( const QSharedPointer< SourceInfo >& to, const QString& msg ) = 0;

signals:
    void error( int, const QString& );
    void stateChanged( SipPlugin::ConnectionState state );

    void addMenu( QMenu* menu );
    void removeMenu( QMenu* menu );

    void newSourceInfo( QSharedPointer<SourceInfo*> sourceInfo );
    void newMessage( QSharedPointer<SourceInfo*> sourceInfo, const QString& message );

private slots:
    void onError( int, const QString& );
    void onStateChange( SipPlugin::ConnectionState state );

private:
    QString m_pluginId;
    QString m_cachedError;
};

Q_DECLARE_INTERFACE( SipPluginFactory, "tomahawk.SipFactory/1.0" )

#endif
