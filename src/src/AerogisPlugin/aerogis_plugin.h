#ifndef AEROGISPLUGIN_PLUGIN_H
#define AEROGISPLUGIN_PLUGIN_H

#include <QQmlExtensionPlugin>

/** QtQuick2 plugin class for storing whole Aerogis project as one widget.
*/
class AerogisPlugin : public QQmlExtensionPlugin
{
   Q_OBJECT
   Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
   /** register plugin's QML classes
    */
   void registerTypes(const char *uri);
};



#endif // AEROGISPLUGIN_PLUGIN_H

