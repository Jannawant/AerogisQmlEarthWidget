#ifndef AEROGISCONFIG_H
#define AEROGISCONFIG_H

#include <QQuickItem>
#include <QtXml/QDomElement>
#include <memory>

/** QML item - Config reader */
class AerogisConfig : public QQuickItem
{
   Q_OBJECT
   Q_PROPERTY(QDomElement config READ config)

public:
	explicit AerogisConfig(QQuickItem *parent = 0);

   const QDomElement config() const;
   const QDomElement info() const;

   QString confStr(const QString &name);
   int confInt(const QString &name);
   float confFloat(const QString &name);
signals:
	/** got a property from config */
   void request_config(QString address);  // if true, then every else parameters in this config are not used, wait for another config

   void earth_fileChanged(const QString &_earth_file);
   void request_earthChanged(bool _request_earth);
   void earth_request_addressChanged(const QString &_earth_request_address);

   void display_modeChanged(const QString &_display_mode);
   void manual_mode_timeoutChanged(int _manual_mode_timeout);
	void min_camera_altitude_manualChanged(double _min_camera_altitude_manual);
	void max_camera_altitude_manualChanged(double _max_camera_altitude_manual);

   void save_imagesChanged(bool _save_images);
   void save_images_guiChanged(bool _save_images_gui);
   void save_images_intervalChanged(int _save_images_interval);
   void save_images_when_new_pointChanged(bool _save);
   void save_images_stack_sizeChanged(double _save_images_stack_size);
   void save_images_pathChanged(const QString &_save_images_path);
   void save_images_top(int _low, int _med, int _hi);
   void languageChanged(const QString &_language);

   void request_flight_info_comChanged(bool _request_flight_info_com);
   void request_flight_info_webChanged(bool _request_flight_info_web);
   void request_flight_info_intervalChanged(int _request_flight_info_interval);
   void request_flight_archiveChanged(bool _request_flight_archive);

   void show_flight_infowindowChanged(bool _show_flight_infowindow);
   void flight_infowindow_intervalChanged(int _flight_infowindow_interval);
	void flight_infowindow_showtimeChanged(int _flight_infowindow_showtime);

   void show_object_infowindowChanged(bool _show_object_infowindow);
	void object_infowindow_intervalChanged(int _object_infowindow_interval);
	void object_infowindow_showtimeChanged(int _object_infowindow_showtime);
	void infowindow_header_fontChanged(const QString &_infowindow_header_font);
	void infowindow_header_font_sizeChanged(int _infowindow_header_font_size);
	void infowindow_body_fontChanged(const QString &_infowindow_body_font);
	void infowindow_body_font_sizeChanged(int _infowindow_body_font_size);
   void max_object_distanceChanged(double _max_object_distance);
   void objects_features_nameChanged(const QString &_objects_features_name);
   void objects_photo_dirChanged(const QString &_objects_photo_dir);
   void objects_text_dirChanged(const QString &_objects_text_dir);

   void com_portChanged(const QString &_com_port);
   void web_addressChanged(const QString &_web_address);
   void config_request_addressChanged(const QString &_config_request_address);
	void flight_archive_request_addressChanged(const QString &_flight_archive_request_address);

   void model_pathChanged(const QString &_model_path);
   void trace_colorChanged(const QString &_trace_color);
   void fixed_flight_heightChanged(int _fixed_flight_height);
   void log_pathChanged(const QString &_log_path);

   void infowindow_bgcolorChanged(const QString &_infowindow_bgcolor);
   void infowindow_textcolorChanged(const QString &_infowindow_textcolor);

   void configLoaded();

public slots:
   /** (re)load config from file */
   void loadConfig(const QString &confName, bool mayRequestFromWeb);

   /** use this text as config to (re)load */
   void loadTextConfig(const QString &confText, bool mayRequestFromWeb);

private:
   QDomElement _config;
   QDomElement _info;

   bool isConfigValid();
};

#endif // AEROGISCONFIG_H
