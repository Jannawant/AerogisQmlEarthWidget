import QtQuick 2.0
import QtQuick.Window 2.0
import Aerogis 1.0

/** Aerogis plugin is shipped as sole QtQuick2 widget - AerogisWidget
  * (use import Aerogis 1.0)
  */
Rectangle {
    id: mainContainer
    color: "transparent"
    anchors.fill: parent

// public properties:
    property alias display_mode: earthWidget.display_mode    // string "auto"/"manual"
    property alias manual_mode_timeout: earthWidget.manual_mode_timeout	// int, seconds
    property alias min_camera_altitude_manual: earthWidget.min_camera_altitude_manual   // metres
    property alias max_camera_altitude_manual: earthWidget.max_camera_altitude_manual   // metres

    property bool save_images: false
    property bool save_images_gui: false
    property bool save_images_top: false
    property int save_images_interval: 60   // seconds
    property bool save_images_when_new_point: false
    property alias save_images_stack_size: earthWidget.save_images_stack_size
    property alias save_images_path: earthWidget.save_images_path
    property string language: ""
    onLanguageChanged:
    {
       earthWidget.language = language;
       aerogisCore.setGuiLanguage(language);
    }

    property alias request_flight_info_com: earthWidget.request_flight_info_com
    property alias request_flight_info_web: earthWidget.request_flight_info_web
    property int request_flight_info_interval: 10

    property bool show_flight_infowindow: false
    property int flight_infowindow_interval: 20
    property int flight_infowindow_showtime: 10

    property bool show_object_infowindow: false
    property int object_infowindow_interval: 30
    property int object_infowindow_showtime: 10
    property string infowindow_header_font: "../arial.ttf"
    property int infowindow_header_font_size: 14
    property string infowindow_body_font: "../arial.ttf"
    property int infowindow_body_font_size: 16

    property alias max_object_distance: earthWidget.max_object_distance
    property alias objects_features_name: earthWidget.objects_features_name
    property alias objects_photo_dir: earthWidget.objects_photo_dir
    property alias objects_text_dir: earthWidget.objects_text_dir

    property alias com_port: earthWidget.com_port
    property alias web_address: earthWidget.web_address

    property alias model_path: earthWidget.model_path
    property alias fixed_flight_height: earthWidget.fixed_flight_height
    property alias log_path: aerogisCore.logFileName
    property alias trace_color: earthWidget.trace_color

    // current flight parameters
    property double longitude: 0
    property double latitude: 0
    property double altitude: 0
    property var cur_date: null
    property string time_to_dest: ""
    property double km_to_dest: 0
    property double temperature: 0
    property double speed: 0

    property string infowindow_bgcolor: "#80202020";
    property string infowindow_textcolor: "#ffffff";

// public slots:

   /** Set new airplane coordinates and other current parameters */
   function moveToEx(Lng, Lat, Alt, MoscowDateTime, TimeToDest, KmToDest, Temp, Speed)
   {
      longitude = Lng;
      latitude = Lat;
      altitude = Alt;
      cur_date = MoscowDateTime;
      time_to_dest = TimeToDest;
      km_to_dest = KmToDest;
      temperature = Temp;
      speed = Speed;

      earthWidget.moveTo( Lng, Lat, Alt, 2000 );
      aerogisCore.log("lng="+Lng+" lat="+Lat+" Alt="+Alt+" TimeToDest="+TimeToDest+" KmToDest="+KmToDest+" Temp="+Temp+" Speed="+Speed);

      if( mainContainer.show_flight_infowindow )
      {
         flightInfoWindow.flightInfoText =
                '<b>'+qsTr('Долгота')+':</b> ' + ( Lng < 0 ? -Lng.toFixed(1) + ' ('+qsTr('З.Д.')+')<br>' : Lng.toFixed(1) + ' ('+qsTr('В.Д.')+')<br>') +
                '<b>'+qsTr('Широта')+':</b> ' + (Lat < 0 ? -Lat.toFixed(1) + ' ('+qsTr('Ю.Ш.')+')<br>' : Lat.toFixed(1) + ' ('+qsTr('С.Ш.')+')<br>') +
                '<b>'+qsTr('Дата')+':</b> ' + MoscowDateTime.toDateString() + '<br>' +
                '<b>'+qsTr('Московское время')+':</b> ' + MoscowDateTime.getHours() + ':' + MoscowDateTime.getMinutes() + '<br>' +
                '<b>'+qsTr('Оставшееся время')+':</b> ' + TimeToDest + '<br>' +
                '<b>'+qsTr('Оставшееся расстояние')+':</b> ' + Math.round(KmToDest) + ' '+qsTr('км')+'<br>' +
                '<b>'+qsTr('Температура')+':</b> ' + Math.round(Temp) + '°C<br>' +
                '<b>'+qsTr('Высота')+':</b> ' + Math.round(Alt) + qsTr('м')+'<br>' +
                '<b>'+qsTr('Скорость относительно земли')+':</b> ' + Math.round(Speed) + ' '+qsTr('км/ч')+'<br>'
         ;
      }
      statusBar.text = qsTr('Оставшееся время')+': ' + TimeToDest + '  ' + qsTr('Оставшееся расстояние')+': ' + Math.round(KmToDest) + ' '+qsTr('км');
   }

    /** Assign path passed by airplane.
      * Array of QVector3D elements */
    function setArchivePath( PathArr )
    {
        earthWidget.setArchivePath(PathArr);
    }




    /** item for core logic of Aerogis plugin */
    AerogisCore {
       id: aerogisCore
       Component.onDestruction: {
          log( "Aerogis widget is exiting." );
       }
    }

    /** plugin's config reader */
    AerogisConfig {
        id: aerogisConfig
        onRequest_config: {
           // config_request_address is used only here
           var confText = earthWidget.request_config(address);
           if( confText != "" ) { aerogisConfig.loadTextConfig(confText,false); }
           else { aerogisConfig.loadConfig("conf/aero.conf",false); }
        }

        onEarth_fileChanged: earthWidget.earth_file = _earth_file
        onRequest_earthChanged: earthWidget.request_earth = _request_earth
        onEarth_request_addressChanged: earthWidget.earth_request_address = _earth_request_address

        onDisplay_modeChanged: mainContainer.display_mode = _display_mode
        onManual_mode_timeoutChanged: mainContainer.manual_mode_timeout = _manual_mode_timeout
        onMin_camera_altitude_manualChanged: mainContainer.min_camera_altitude_manual = _min_camera_altitude_manual
        onMax_camera_altitude_manualChanged: mainContainer.max_camera_altitude_manual = _max_camera_altitude_manual

        onSave_imagesChanged: mainContainer.save_images = _save_images
        onSave_images_guiChanged: mainContainer.save_images_gui = _save_images_gui
        onSave_images_intervalChanged: mainContainer.save_images_interval = _save_images_interval
        onSave_images_when_new_pointChanged: mainContainer.save_images_when_new_point = _save
        onSave_images_stack_sizeChanged: mainContainer.save_images_stack_size = _save_images_stack_size
        onSave_images_pathChanged: mainContainer.save_images_path = _save_images_path
        onSave_images_top: { mainContainer.save_images_top = true; earthWidget.setSave_images_top(_low,_med,_hi); }
        onLanguageChanged: mainContainer.language = _language

        onRequest_flight_info_comChanged: mainContainer.request_flight_info_com = _request_flight_info_com
        onRequest_flight_info_webChanged: mainContainer.request_flight_info_web = _request_flight_info_web
        onRequest_flight_info_intervalChanged: mainContainer.request_flight_info_interval = _request_flight_info_interval
        onRequest_flight_archiveChanged: earthWidget.request_flight_archive = _request_flight_archive

        onShow_flight_infowindowChanged: mainContainer.show_flight_infowindow = _show_flight_infowindow
        onFlight_infowindow_intervalChanged: mainContainer.flight_infowindow_interval = _flight_infowindow_interval
        onFlight_infowindow_showtimeChanged: mainContainer.flight_infowindow_showtime = _flight_infowindow_showtime

        onShow_object_infowindowChanged: mainContainer.show_object_infowindow = _show_object_infowindow
        onObject_infowindow_intervalChanged: mainContainer.object_infowindow_interval = _object_infowindow_interval
        onObject_infowindow_showtimeChanged: mainContainer.object_infowindow_showtime = _object_infowindow_showtime
        onInfowindow_header_fontChanged: mainContainer.infowindow_header_font = _infowindow_header_font
        onInfowindow_header_font_sizeChanged: mainContainer.infowindow_header_font_size = _infowindow_header_font_size
        onInfowindow_body_fontChanged: mainContainer.infowindow_body_font = _infowindow_body_font
        onInfowindow_body_font_sizeChanged: mainContainer.infowindow_body_font_size = _infowindow_body_font_size

        onMax_object_distanceChanged: mainContainer.max_object_distance = _max_object_distance
        onObjects_features_nameChanged: mainContainer.objects_features_name = _objects_features_name
        onObjects_photo_dirChanged: mainContainer.objects_photo_dir = _objects_photo_dir
        onObjects_text_dirChanged: mainContainer.objects_text_dir = _objects_text_dir

        onCom_portChanged: mainContainer.com_port = _com_port
        onWeb_addressChanged: mainContainer.web_address = _web_address
        onFlight_archive_request_addressChanged: earthWidget.flight_archive_request_address = _flight_archive_request_address

        onModel_pathChanged: mainContainer.model_path = _model_path
        onTrace_colorChanged: mainContainer.trace_color = _trace_color

        onFixed_flight_heightChanged: mainContainer.fixed_flight_height = _fixed_flight_height
        onLog_pathChanged: { mainContainer.log_path = _log_path; aerogisCore.log("Aerogis widget is starting. (loading config)"); }

        onInfowindow_bgcolorChanged: mainContainer.infowindow_bgcolor = _infowindow_bgcolor
        onInfowindow_textcolorChanged: mainContainer.infowindow_textcolor = _infowindow_textcolor

        onConfigLoaded: { earthWidget.onConfigLoaded(); }
    }

    /** background widget with 3D Earth */
    EarthWidget {
        id: earthWidget
        //parent: mainContainer.parent  //uncomment this to work stable in example_app. But will be still unstable if attached in different way.
        anchors.fill: parent
        //color: "transparent"
        //opacity: 0.999
        mode: OSGViewport.Buffer	// Buffer/Native
        focus: true

        onNewObjectInfo: {
           if( mainContainer.show_object_infowindow )
           {
              objectInfoWindow.objCaption = objCaption;
              objectInfoWindow.objDescr = objDescr;
              objectInfoWindow.objImgFile = objImgFile;
              objectInfoWindow.needShow = true;
              mainContainer.showInfo();
           }
        }


        onNewFlightInfo: {
              mainContainer.moveToEx(Lng, Lat, Alt, MoscowDateTime, TimeToDest, KmToDest, Temp, Speed);
        }

        function requestScreenshots()
        {
           // at closest good moment, picture will be taken
           if( mainContainer.save_images )
              earthWidget.needTake3Screenshots = true;
           if( mainContainer.save_images_gui )
              earthWidget.needTakeGUIScreenshot = true;
           if( mainContainer.save_images_top )
              earthWidget.needTakeTopScreenshots = true;
        }

        /** screenshot timer */
        Timer {
           id: screenshotTimer
           interval: mainContainer.save_images_interval*1000 // milliseconds
           running: mainContainer.save_images || mainContainer.save_images_gui || mainContainer.save_images_top
           repeat: true
           onTriggered: {
              earthWidget.requestScreenshots();
           }
        }

        onMoveAirplaneAnimationDone: {
           if (mainContainer.save_images_when_new_point)
              earthWidget.requestScreenshots();
        }

        onError: { aerogisCore.log(msg); }
    }

    /** font for UI */
    FontLoader { id: arialbdFont; source: "../arialbd.ttf" }

    /** camera height change timer */
    Timer {
        id: heightTimer
        interval: 10000 // milliseconds
        running: true
        repeat: true
        property int cameraHeightState: 0
        onTriggered: {
            cameraHeightState++;
            cameraHeightState %= 4;
			var r = cameraHeightState;
			if (r==3) r = 1;
			r++;
            var h = r*r*0.5;
            earthWidget.moveCameraTo( h*400000, r*400000, 5000 );
        }

    }

    /** request flight info timer (not configured from config) */
    Timer {
        id: requestFlightInfoTimer
        interval: mainContainer.request_flight_info_interval*1000 // milliseconds
        running: earthWidget.request_flight_info_com || earthWidget.request_flight_info_web
        repeat: true
        onTriggered: {
           earthWidget.requestFlightInfo();
        }

    }

    /** RGBA to ARGB string conversion. if not 9-symbol string, returns it itself unchanged */
    function rgbaToArgb( rgba ) {
       return ( rgba.length == 9 ? "#"+rgba.substring(7)+rgba.substring(1,7) : rgba )
    }

    /** Status bar at bottom of the screen */
    Rectangle {
        id: statusBar
        height: 20
        width: parent.width
        anchors.bottom: parent.bottom
        color: rgbaToArgb( mainContainer.infowindow_bgcolor )

        property string text: qsTr("")
        Text {
            text: parent.text
            color: rgbaToArgb( mainContainer.infowindow_textcolor )
            font.family: arialbdFont.name
            anchors.leftMargin: 10
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    function plusCamera()
    {
        earthWidget.pCamera();
    }

    function minusCamera()
    {
        earthWidget.mCamera();
    }

    ////////////////////// InfoWindows /////////////////////////////
    function showInfo() {
        if(!flightInfoWindow.visible && !objectInfoWindow.visible)
        {
            if( flightInfoWindow.needShow && flightInfoWindow.flightInfoText!="" )
            {
               flightInfoWindow.showInfo();
            }
            else if( objectInfoWindow.needShow )
            {
               objectInfoWindow.showInfo();
            }
        }
    }

   /////////////////// Object Info /////////////////////////////////
   Rectangle {
      id: objectInfoWindow
      color: rgbaToArgb( mainContainer.infowindow_bgcolor )
      property string objCaption
      property string objDescr
      property string objImgFile: "../airport_64.tga"

      property bool needShow: false

      onObjImgFileChanged: {
      if( objImgFile == "")
         objectImage.height = 0;
      else
         objectImage.source = objImgFile;
      }
      visible: false

      function showInfo() { // show infowindow, and start timer when to close it
         needShow = false;
         visible = true;
         oInfoTimer.restart();
      }

      width: parent.width * 0.3
      height: childrenRect.height
      anchors.top: parent.top
      anchors.topMargin: 20
      anchors.right: parent.right
      anchors.rightMargin: 20

      Image {
         id: objectImage
         width: parent.width
         height: width * sourceSize.height/sourceSize.width
         anchors.top: parent.top
         anchors.left: parent.left
         fillMode: Image.PreserveAspectCrop
      }

        Text {
            id: objCaptText
            width: parent.width-40
            text: parent.objCaption
            color: rgbaToArgb( mainContainer.infowindow_textcolor )
            FontLoader { id: objCaptionFont; source: mainContainer.infowindow_header_font }
            font.family: objCaptionFont.name
            font.pointSize: mainContainer.infowindow_header_font_size
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: objectImage.bottom
            anchors.topMargin: 20
            wrapMode: Text.WordWrap
        }

        Text {
            id: objDescrText
            width: parent.width-40
            text: parent.objDescr
            color: rgbaToArgb( mainContainer.infowindow_textcolor )
            FontLoader { id: objDescrFont; source: mainContainer.infowindow_body_font }
            font.family: objDescrFont.name
            font.pointSize: mainContainer.infowindow_body_font_size
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: objCaptText.bottom
            anchors.topMargin: 20
            wrapMode: Text.WordWrap
        }

        /** close X */
        MouseArea {
            anchors.right: parent.right
            anchors.top: parent.top
            width: 32
            height: 32
            onClicked:
            {
                parent.visible = false;
                mainContainer.showInfo(); // delegate infowindows management to mainContainer
            }
            Text {
                text: "×"
                color: rgbaToArgb( mainContainer.infowindow_textcolor )
                anchors.centerIn: parent
                font.pointSize: 32
            }
        }

        /** timer for search nearby objects */
        Timer {
            id: objectsTimer
            interval: mainContainer.object_infowindow_interval*1000
            running: mainContainer.show_object_infowindow
            repeat: true
            onTriggered: {
               earthWidget.searchForNearbyObject();
               mainContainer.showInfo();  // delegate infowindows management to mainContainer
            }
        }

        /** timer for show infoWindow */
        Timer {
            id: oInfoTimer
            interval: mainContainer.object_infowindow_showtime*1000
            running: false
            repeat: false
            onTriggered:
            {
                parent.visible = false;
                mainContainer.showInfo(); // delegate infowindows management to mainContainer
            }
        }


    }

    ///////// Flight Info ////////////////////////////////
    Rectangle {
        id: flightInfoWindow
        color: rgbaToArgb( mainContainer.infowindow_bgcolor )
        visible: false

        property string flightInfoText: ""
        property bool needShow: false

        function showInfo() { // show infowindow, and start timer when to close it
            needShow = false;
            visible = true;
            fInfoTimer.restart()
        }

        width: parent.width * 0.3
        height: childrenRect.height
        anchors.top: parent.top
        anchors.topMargin: 20
        anchors.right: parent.right
        anchors.rightMargin: 20

        Text {
            id: flightinfoText
            width: parent.width-40
            color: rgbaToArgb( mainContainer.infowindow_textcolor )
            text: parent.flightInfoText
            textFormat: Text.StyledText
            FontLoader { id: flightinfoFont; source: mainContainer.infowindow_body_font }
            font.family: flightinfoFont.name
            font.pointSize: mainContainer.infowindow_body_font_size
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: parent.top
            anchors.topMargin: 20
            wrapMode: Text.WordWrap
        }

        /** close X */
        MouseArea {
            anchors.right: parent.right
            anchors.top: parent.top
            width: 32
            height: 32
            onClicked:
            {
                parent.visible = false;
                mainContainer.showInfo(); // delegate infowindows management to mainContainer
            }
            Text {
                text: "×"
                color: rgbaToArgb( mainContainer.infowindow_textcolor )
                anchors.centerIn: parent
                font.pointSize: 32
            }
        }

        /** timer when to show */
        Timer {
            id: whenTimer
            interval: mainContainer.flight_infowindow_interval*1000
            running: mainContainer.show_flight_infowindow
            repeat: true
            onTriggered: {
               parent.needShow = true;
               mainContainer.showInfo(); // delegate infowindows management to mainContainer
            }
        }

        /** timer for show infoWindow */
        Timer {
            id: fInfoTimer
            interval: mainContainer.flight_infowindow_showtime*1000
            running: false
            repeat: false
            onTriggered: {
                parent.visible = false;
                mainContainer.showInfo(); // delegate infowindows management to mainContainer
            }
        }


    }

    Component.onCompleted: {
        aerogisConfig.loadConfig("conf/aero.conf",true);
    }
}
