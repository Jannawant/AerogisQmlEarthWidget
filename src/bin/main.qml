import QtQuick 2.0
import QtQuick.Window 2.0
import QtQml 2.0
import QtQuick.Controls 1.2
import Aerogis 1.0

Window {
    id: mainWindow
    visible: true
    width: 1280
    height: 720
    title: qsTr("Aerogis")

    Item {
        id: name
        anchors.fill: parent

        AerogisWidget {
            id: aero
            focus:true
            Keys.onEscapePressed: {
              mainWindow.close();
              event.accepted = true;
            }
            //Date time = new Date();
        }

        Button{
            id: plus
            anchors.top: parent.top
            text: qsTr("+")
            //onClicked: aero.plusCamera();
            onClicked: aero.plusCamera();
            //h->view->getEventQueue()->mouseScroll(osgGA::GUIEventAdapter::SCROLL_UP)
        }

        Button{
            id: minus
            anchors.bottom: parent.bottom
            text: qsTr("-")
            onClicked:  aero.minusCamera();
            //h->view->getEventQueue()->mouseScroll(osgGA::GUIEventAdapter::SCROLL_DOWN)
        }

        /** info demo update timer
          */
        /*Timer {
            id: infoTimer
            interval: 20000 // milliseconds
            running: true
            repeat: true
            onTriggered: {
                // demo:
                var t0 = new Date();
                var t = (t0.getMinutes()*60+t0.getSeconds()+t0.getMilliseconds()*0.001)*0.2;
                //var x = (t % 360.0)-180.0;
                //var y = 30.0 + 30.0*Math.sin( t*0.02 )+10.0*Math.sin( t*0.2 );

                var x = 45.0 + 3.0 * Math.sin( t*0.1 );
                var y = 42.0 + 3.0 * Math.cos( t*0.1 );

                aero.moveToEx( x, y, 30000, new Date(), "12:34", 1000, -45, 800 );
            }

        }*/

        /** Language switch demo
          */
        /*Timer {
            id: langTimer
            interval: 30000 // milliseconds
            running: true
            repeat: true

            property string demoLang: "en";
            onTriggered: {
                if( demoLang=="en" )
                    demoLang="es";
                else if( demoLang=="es" )
                    demoLang="ru";
                else demoLang="en";
                aero.language = demoLang;
            }

        }*/

        function getArchivePath() {

           var ar = new Array();
           ar.push( Qt.vector3d(30,60,150) );   // Долгота, Широта (в градусах), Высота (в метрах)
           ar.push( Qt.vector3d(29.9,59.9,350) );
           return ar;
        }

        Component.onCompleted: {
           //aero.setArchivePath( getArchivePath() );
        }
    }


}
