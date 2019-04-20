#ifndef GLOBEOSGEARTH_H
#define GLOBEOSGEARTH_H

#include <memory>
#include <map>
#include <QTime>
#include <QVector3D>
#include <QVariantList>
#include <qthread.h>

#include <osgViewer/Viewer>
#include <OSGViewport.hpp>
#include <osgEarthAnnotation/ModelNode>
#include <osg/PositionAttitudeTransform>
#include <osgEarthFeatures/Feature>
#include <osgEarthAnnotation/FeatureNode>
#include <osgEarthFeatures/FeatureModelGraph>

#include "QuickWindowViewer.hpp"
#include "../AerogisPlugin/VSSRoutines.h"
#include <atomic>

// only Windows
class EarthWidget;

struct FlightInfo
{
   double Lng, Lat, Alt;
   QDateTime MoscowDateTime;
   QString TimeToDest;
   double KmToDest, Temp, Speed;
   bool isNew;

   bool initFromBlob(const QByteArray &ba);
   bool initFromVssData(std::list<CVSSData> &vssData);
};

/// Helper class - a thread to operate with COM-port
class VSSThread : public QThread
{
   Q_OBJECT;
public:
   VSSThread(QObject *parent);

   QString mCom_port;
   QString mWeb_adr;
   FlightInfo mLastFlightInfo;

   std::atomic<bool> mIsExiting;
protected:
   void run();
};


/** Widget to show OpenSceneGraph 3D inside QtQuick2. Shows Earth and supporting 3D drawings.
*/
class EarthWidget : public osgQtQuick::OSGViewport
{
	Q_OBJECT;
   Q_PROPERTY(QString earth_file MEMBER mEarth_file);
   Q_PROPERTY(bool request_earth MEMBER mRequest_earth);
   Q_PROPERTY(QString earth_request_address MEMBER mEarth_request_address);

   Q_PROPERTY(QString display_mode READ display_mode WRITE setDisplay_mode NOTIFY display_modeChanged);
	Q_PROPERTY(int manual_mode_timeout MEMBER mManual_mode_timeout WRITE setManual_mode_timeout);
	Q_PROPERTY(double min_camera_altitude_manual MEMBER mMin_camera_altitude_manual WRITE setMin_camera_altitude_manual);
	Q_PROPERTY(double max_camera_altitude_manual MEMBER mMax_camera_altitude_manual WRITE setMax_camera_altitude_manual);

	Q_PROPERTY(double save_images_stack_size MEMBER mSave_images_stack_size);
   Q_PROPERTY(bool needTakeScreenshot MEMBER mNeedTakeScreenshot);
   Q_PROPERTY(bool needTake3Screenshots MEMBER mNeedTake3Screenshots);
   Q_PROPERTY(bool needTakeGUIScreenshot MEMBER mNeedTakeGUIScreenshot);
   Q_PROPERTY(bool needTakeTopScreenshots MEMBER mNeedTakeTopScreenshots);
   Q_PROPERTY(QString save_images_path MEMBER mSave_images_path);
   Q_PROPERTY(QString language MEMBER mLanguage WRITE setLanguage);

   Q_PROPERTY(bool request_flight_info_com MEMBER mRequest_flight_info_com WRITE setRequest_flight_info_com NOTIFY request_flight_info_comChanged);   // NOTIFY is used with timers
   Q_PROPERTY(QString com_port MEMBER mCom_port WRITE setCom_port);
   Q_PROPERTY(bool request_flight_info_web MEMBER mRequest_flight_info_web WRITE setRequest_flight_info_web NOTIFY request_flight_info_webChanged);   // NOTIFY is used with timers
   Q_PROPERTY(QString web_address MEMBER mWeb_address WRITE setWeb_address);
   Q_PROPERTY(bool request_flight_archive MEMBER mRequest_flight_archive);
   Q_PROPERTY(QString flight_archive_request_address MEMBER mFlight_archive_request_address);

   Q_PROPERTY(int flight_infowindow_interval MEMBER mFlight_infowindow_interval);
   Q_PROPERTY(int flight_infowindow_showtime MEMBER mFlight_infowindow_showtime);

   Q_PROPERTY(double max_object_distance MEMBER mMax_object_distance);
   Q_PROPERTY(QString objects_features_name MEMBER mObjects_features_name);
   Q_PROPERTY(QString objects_photo_dir MEMBER mObjects_photo_dir);
   Q_PROPERTY(QString objects_text_dir MEMBER mObjects_text_dir);

   Q_PROPERTY(QString model_path MEMBER mModel_path WRITE setModel_path);
   Q_PROPERTY(QString trace_color MEMBER mTrace_color WRITE setTrace_color);
   Q_PROPERTY(int fixed_flight_height MEMBER mFixed_flight_height);

   friend void receiveVSSData(EarthWidget *e);

public:
   explicit EarthWidget(QQuickItem *parent = 0);
   ~EarthWidget();

// PROPERTY:
	QString display_mode();

   /** compel EarthWidget to load config from Server */
   Q_INVOKABLE QString request_config(QString address);

   /** use "auto"/"manual" string */
	void setDisplay_mode(const QString &dm);

	/** use non-negative number */
	void setManual_mode_timeout(int seconds);	

	/** from 0 to 1.0E+8 */
	void setMin_camera_altitude_manual(double metres);
	void setMax_camera_altitude_manual(double metres);


   /** 'ru', 'en', 'es' */
   void setLanguage(const QString &la); 

   /** Flight-infowindow properties. Use only one of com/web */
   void setRequest_flight_info_com(bool bRequest);
   void setRequest_flight_info_web(bool bRequest);

   /** load (or replace) airplane model */
   void setModel_path(const QString &fileName);

   /** airplane trajectory line color */
   void setTrace_color(const QString &color);

   /** set COM port, like COM3 */
   void setCom_port(const QString &comPort);

   /** set Web address, like ws://localhost:2345 */
   void setWeb_address(const QString &adr);

   // inside QML drawing
   virtual void viewerDrawsFrame(osgQtQuick::AerogisViewer *v) final;
   // after QML drawing
   virtual void viewerDrawnFrame() final;

   QString mCom_port;
   QString mWeb_address;
   QString mFlight_archive_request_address;

signals:
	/** we ask someone to call setModel_path! (with file name to load from) */
   void needAirplaneModel();

	/** we say we have error */
   void error(const QString &msg);

   /** It's time to show 'object info window'*/
   void newObjectInfo(const QString &objCaption, const QString &objDescr, const QString &objImgFile);

   /** new flight information from COM port */
   void newFlightInfo(double Lng, double Lat, double Alt, const QDateTime &MoscowDateTime, const QString &TimeToDest, double KmToDest, double Temp, double Speed);

   /** this signal says that airplane has arrived at target point (this means end of animation launched by moveTo(...) ) */
   void moveAirplaneAnimationDone();

 // PROPERTY:
	void display_modeChanged();
   void request_flight_info_comChanged();
   void request_flight_info_webChanged();

public slots:
	/** instantly move airplane to new coordinates */
   void setNewCoordinatesOnScene(double lon, double lat, double alt);
   void setNewCoordinatesOnScene(double lon, double lat, double alt, double azim);

	/** (re)set airplane path array. Direct request. Map (Earth) must be loaded. */
	void setArchivePath(const QVariantList &ar);

	/** slowly move airplane to new coordinates */
	void moveTo(double lon, double lat, double alt, int move_time_ms);

	/** slowly move camera to new height and radius */
	void moveCameraTo(double hat, double rad, int move_time_ms);

   /** try to find close object on map. If succeeds, emits signal newObjectInfo(...) */
   void searchForNearbyObject();

   /** try to get flight info from COM port. If succeeds, emits signal newFlightInfo(...) */
   void requestFlightInfo();

   /** change language [name_xx] fields and TRANSLATE labels on map. It doesn't use 'language' (mLanguage) property */
   void setLanguageOnMap(const QString &la, const QString &laOld);

   /** used when config loading */
   void setSave_images_top(int low, int med, int hi);

   void pCamera();
   void mCamera();

   void myClearPath();
   /** tell EarthWidget that config has done loading */
   Q_INVOKABLE void onConfigLoaded();


   
private slots:
	void onMoveTimer();
	void onRotTimer();
	void onHeightTimer();
   void onManualToAutoTimer();
   void onLastFlightInfoCheckTimer();

	void onManualEvent();
	void onFrameEventsProcessed();
   void onFrameDrawn(QOpenGLFramebufferObject *fb);

	// set airplane orientation and camera View matrix, from EarthWidget parameters
	void applyCoordinatesOnScene();

   void loadFeatureObjects();

   void requestFlightArchive();

protected:
	// inherited methods
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent *event);


private:
	osgEarth::MapNode* mapNode;

	// properties:
   QString mEarth_file;
   bool mRequest_earth;
   QString mEarth_request_address;

   QString mModel_path;
   QString mTrace_color;

   int mFixed_flight_height;  // if 0 means not used

   enum Display_mode { AUTO, MANUAL } mDisplay_mode;
	int mManual_mode_timeout;
	double mMin_camera_altitude_manual;
	double mMax_camera_altitude_manual;

   bool mNeedTakeScreenshot;  // take screenshot, at closest good moment
   bool mNeedTake3Screenshots;  // take 3 screenshots, at closest good moment
   bool mNeedTakeTopScreenshots;  // take 3 screenshots from top, at closest good moment
   bool mNeedTakeGUIScreenshot;  // take screenshot with GUI, at closest good moment
   QString mNeedTakeScreenshotLang;  // take screenshot at closest good moment

   //int mSave_images_interval;   // see corresponding Timer in AerogisWidget.qml
   double mSave_images_stack_size;	// bytes
   QString mSave_images_path;     // 'screens' directory
   struct { bool isSet; int low, med, hi; } mSave_images_h; // height (altitudes) to make screenshots
   QString mLanguage;

   bool mRequest_flight_info_com;
   bool mRequest_flight_info_web;
   bool mRequest_flight_archive;
   int mFlight_infowindow_interval;
   int mFlight_infowindow_showtime;

   QThread *mVSSThread;  // implementation is VSSThread class defined in EarthWidget.cpp. Synchronization is done using QThread object state (only one run() at a time).

   double mMax_object_distance;  // metres

   QString mObjects_features_name;
   QString mObjects_photo_dir;
   QString mObjects_text_dir;
   std::vector<osgEarth::Features::Feature*> featureObjects;

   // Dictionary[modellayer*][la_nguage][word] = feature*
   std::map < osgEarth::Features::FeatureModelGraph*, std::map < std::string, std::map< std::string, osgEarth::Features::Feature* > > > featureDict;

	// calc azimuth (radians) to new point
	double azimuthTo(double lon, double lat);

   osg::Vec3d fromGeo(osg::Vec3d lonLatHeight);
   osg::Vec3d toGeo(osg::Vec3d xyz);
   osg::Vec3d localUpFromGeo(osg::Vec3d lonLatHeight);

	// coordinates of airplane position
   double latitude, longitude, altitude, azimuth;
	bool airplaneSet;		// (in position on map)

   QTimer *mLastFlightInfoCheckTimer;

	// path of airplane
	std::vector<osg::Vec3d> pathPoints;
   osgEarth::Symbology::MultiGeometry *pathGeom;   // geometric presentation of path (blue stripe)
   osgEarth::Annotation::FeatureNode *pathNode;    // geometric presentation of path (blue stripe) Node

   osg::MatrixTransform *airplaneGroup;
	osg::PositionAttitudeTransform *airplaneTransform;
   double mAlt_scale;
   double mPath_width;

   void addPathPoint(osg::Vec3d lonLatHeight);

	// update animation timer period
	int updFrameTime_ms;

	// point-to-point move animation
	osg::Vec3d moveEnd;	// lon, lat, hat
	double moveEndAzim;
	QTime moveEndTime, moveLastUpdTime;
	QTimer *moveTimer;

	// rotate camera animation
	double cameraAzim, cameraRad, cameraHeight;
	bool cameraSet;
	double rotSpeed;		// radians/s
	QTime rotLastUpdTime;
	QTimer *rotTimer;

	// camera height and radius animation
	double cameraRadEnd, cameraHeightEnd;
	QTime heightEndTime, heightLastUpdTime;
	QTimer *heightTimer;

	// camera manual-to-auto timer switch
	QTime lastManualUseTime;
	QTimer *manualToAutoTimer;

};

#endif // GLOBEOSGEARTH_H
