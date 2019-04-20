#include "aerogis_plugin.h"
#include "Utility.hpp"
#include <EarthWidget.h>
#include "AerogisCore.h"
#include "AerogisConfig.h"
#include <qqml.h>

void AerogisPlugin::registerTypes(const char *uri)
{
   // @uri com.gispro.aerogis
    try{
       Q_ASSERT(uri == QLatin1String("Aerogis"));
       osgQtQuick::registerTypes(uri);
       qmlRegisterType<EarthWidget>(uri, 1, 0, "EarthWidget");
       qmlRegisterType<AerogisCore>(uri, 1, 0, "AerogisCore");
       qmlRegisterType<AerogisConfig>(uri, 1, 0, "AerogisConfig");
    }
    catch(...)
    {
        qDebug()<<"Cant register types";
    }
}


