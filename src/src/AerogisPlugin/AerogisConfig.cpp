#include "AerogisConfig.h"
#include <iostream>
#include <qdatetime.h>
#include <QStringList>

AerogisConfig::AerogisConfig(QQuickItem *parent)
   : QQuickItem(parent)
   , _config()
   , _info()
{
   try {
   QDomDocument doc;
   doc.setContent(QString("<xml>"
                  "<message>"
                  "<Lng>37.5</Lng>"
                  "<Lat>55.6</Lat>"
                  "<MoscowTime>13:50:23</MoscowTime>"
                  "<Dt>19.05.2015</Dt>"
                  "<TimeToDest>04:23</TimeToDest>"
                  "<KmToDest>4450</KmToDest>"
                  "<Temp>-45</Temp>"
                  "<Alt>9867</Alt>"
                  "<Speed>856</Speed>"
                  "</message>"
                  "</xml>"));
   _info = doc.documentElement();
   }
   catch(...) {
      std::cerr << "error: Unhandled exception in AerogisConfig ctor\n";
   }
}

const QDomElement AerogisConfig::config() const
{
   return _config;
}

const QDomElement AerogisConfig::info() const
{
   return _info;
}

QString AerogisConfig::confStr(const QString &name)
{
   try {
      auto el = _config.firstChildElement(name);
      if(!el.isNull())
         return el.text();
      std::cerr << "error: Absent config param '" << name.toStdString().c_str() << "'\n";
   }
   catch(...) {
      std::cerr << "error: Unhandled exception in AerogisConfig::conf...(), param='" << name.toStdString().c_str() << "'\n";
   }
   return 0;
}

int AerogisConfig::confInt(const QString &name)
{
   return confStr(name).toInt();
}

float AerogisConfig::confFloat(const QString &name)
{
   return confStr(name).toFloat();
}

void AerogisConfig::loadConfig(const QString &confName, bool mayRequestWeb)
{
   try {
      // read file
   QDomDocument doc;
   QFile file(confName);
   if (!file.open(QIODevice::ReadOnly))
      return;
   QString text = file.readAll();
   file.close();
   // file is read ok

   loadTextConfig(text, mayRequestWeb);

   }
   catch (QString s) {
      std::cerr << "exception in AerogisCore::loadConfig: " << s.toStdString() << "\n";
   }
   catch (...) {
      std::cerr << "error: Unhandled exception in AerogisCore::loadConfig\n";
   }
}

 void AerogisConfig::loadTextConfig(const QString &confText, bool mayRequestWeb)
 {
      try {
		// clear old content
		_config.clear();

		QDomDocument doc;
		if (!doc.setContent(confText))
			return;

		_config = doc.documentElement();

      // request config? then don't use this one:
      bool rqConfig = (_config.firstChildElement("request_config").text() == "true");
      auto cadr = _config.firstChildElement("config_request_address");
      
      if (mayRequestWeb && rqConfig && !cadr.isNull() && !cadr.text().isEmpty())
      {
         emit request_config(cadr.text());
      }
      else
      {
         // validate config
         if (!isConfigValid())
            _config.clear();

         // PROPERTIES
         // notify about all properties we got
         emit earth_fileChanged(_config.firstChildElement("earth_file").text());
         emit request_earthChanged(_config.firstChildElement("request_earth").text() == "true");
         emit earth_request_addressChanged(_config.firstChildElement("earth_request_address").text());

         emit display_modeChanged(_config.firstChildElement("display_mode").text());
         emit manual_mode_timeoutChanged(_config.firstChildElement("manual_mode_timeout").text().toInt());
         emit min_camera_altitude_manualChanged(_config.firstChildElement("min_camera_altitude_manual").text().toDouble());
         emit max_camera_altitude_manualChanged(_config.firstChildElement("max_camera_altitude_manual").text().toDouble());

         emit save_imagesChanged(_config.firstChildElement("save_images_3lang").text() == "true");
         emit save_images_guiChanged(_config.firstChildElement("save_images_gui").text() == "true");
         emit save_images_intervalChanged(_config.firstChildElement("save_images_interval").text().toInt());
         emit save_images_when_new_pointChanged(_config.firstChildElement("save_images_when_new_point").text() == "true");
         emit save_images_stack_sizeChanged(_config.firstChildElement("save_images_stack_size").text().toDouble());
         emit save_images_pathChanged(_config.firstChildElement("save_images_path").text());
         auto el = _config.firstChildElement("save_images_top");
         if (el.text()=="true")
         {
            emit save_images_top(el.attribute("low").toInt(), el.attribute("med").toInt(), el.attribute("hi").toInt());
         }
         emit languageChanged(_config.firstChildElement("language").text());

         emit request_flight_info_comChanged(_config.firstChildElement("request_flight_info_com").text() == "true");
         emit request_flight_info_webChanged(_config.firstChildElement("request_flight_info_web").text() == "true");
         emit request_flight_info_intervalChanged(_config.firstChildElement("request_flight_info_interval").text().toInt());
         emit request_flight_archiveChanged(_config.firstChildElement("request_flight_archive").text() == "true");

         emit show_flight_infowindowChanged(_config.firstChildElement("show_flight_infowindow").text() == "true");
         emit flight_infowindow_intervalChanged(_config.firstChildElement("flight_infowindow_interval").text().toInt());
         emit flight_infowindow_showtimeChanged(_config.firstChildElement("flight_infowindow_showtime").text().toInt());

         emit show_object_infowindowChanged(_config.firstChildElement("show_object_infowindow").text() == "true");
         emit object_infowindow_intervalChanged(_config.firstChildElement("object_infowindow_interval").text().toInt());
         emit object_infowindow_showtimeChanged(_config.firstChildElement("object_infowindow_showtime").text().toInt());
         emit infowindow_header_fontChanged(_config.firstChildElement("infowindow_header_font").text());
         emit infowindow_header_font_sizeChanged(_config.firstChildElement("infowindow_header_font_size").text().toInt());
         emit infowindow_body_fontChanged(_config.firstChildElement("infowindow_body_font").text());
         emit infowindow_body_font_sizeChanged(_config.firstChildElement("infowindow_body_font_size").text().toInt());

         emit max_object_distanceChanged(_config.firstChildElement("max_object_distance").text().toDouble());
         emit objects_features_nameChanged(_config.firstChildElement("objects_features_name").text());
         emit objects_photo_dirChanged(_config.firstChildElement("objects_photo_dir").text());
         emit objects_text_dirChanged(_config.firstChildElement("objects_text_dir").text());

         emit com_portChanged(_config.firstChildElement("com_port").text());
         emit web_addressChanged(_config.firstChildElement("web_address").text());
         emit config_request_addressChanged(_config.firstChildElement("config_request_address").text());
         emit flight_archive_request_addressChanged(_config.firstChildElement("flight_archive_request_address").text());

         emit model_pathChanged(_config.firstChildElement("model_path").text());
         emit fixed_flight_heightChanged(_config.firstChildElement("fixed_flight_height").text().toInt());
         emit log_pathChanged(_config.firstChildElement("log_path").text());
         emit trace_colorChanged(_config.firstChildElement("trace_color").text());

         emit infowindow_bgcolorChanged(_config.firstChildElement("infowindow_bgcolor").text());
         emit infowindow_textcolorChanged(_config.firstChildElement("infowindow_textcolor").text());

         emit configLoaded();
      }
   }
	catch (QString s) {
		std::cerr << "exception in AerogisCore::loadConfig: " << s.toStdString() << "\n";
	}
	catch (...) {
		std::cerr << "error: Unhandled exception in AerogisCore::loadConfig\n";
	}
}

bool AerogisConfig::isConfigValid()
{
   try {
      // check existence of every parameter
      std::vector<QString> nms ={
         "earth_file",

         "display_mode",
         "manual_mode_timeout",
         "min_camera_altitude_manual",
         "max_camera_altitude_manual",

         "save_images_3lang",
         "save_images_gui",
         "save_images_interval",
         "save_images_when_new_point",
         "save_images_stack_size",
         "save_images_path",
         "save_images_top",
         "language",

         "request_flight_info_com",
         "request_flight_info_web",
         "request_flight_info_interval",
         "request_flight_archive",

         "show_flight_infowindow",
         "flight_infowindow_interval",
         "flight_infowindow_showtime",

         "show_object_infowindow",
         "object_infowindow_interval",
         "object_infowindow_showtime",
         "infowindow_header_font",
         "infowindow_header_font_size",
         "infowindow_body_font",
         "infowindow_body_font_size",

         "max_object_distance",
         "objects_features_name",
         "objects_photo_dir",
         "objects_text_dir",

         "com_port",
         "web_address",
         "config_request_address",
         "flight_archive_request_address",

         "model_path",
         "fixed_flight_height",
         "log_path",
         "trace_color",

         "infowindow_bgcolor",
         "infowindow_textcolor"
      };


      for (auto &nm : nms)
      {
         if (_config.firstChildElement(nm).isNull())
         {
            std::cerr << "\nerror in aero.conf: no parameter '" << nm.toStdString() << "'\n";
            return false;
         }
      }

		return true;
	}
	catch (...) {
		std::cerr << "error: Unhandled exception in AerogisCore::isConfigValid\n";
	}
	return false;
}
