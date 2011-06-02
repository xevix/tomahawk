#ifndef SOURCEINFO_H
#define SOURCEINFO_H

#include <QObject>

#include "SipPlugin.h"

#include "dllmacro.h"

class DLLEXPORT SourceInfo : public QObject
{

public:
    SourceInfo() {}
    virtual ~SourceInfo() {}

    const QString id() { return "id"; }
    const QString displayName() { return "displayName"; }
    SipPlugin* sipPlugin() { return 0; }
};

#endif // SOURCEINFO_H
