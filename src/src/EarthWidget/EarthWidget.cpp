#include "EarthWidget.h"

#include <osg/Notify>
#include <osg/DisplaySettings>
#include <osgEarthUtil/ExampleResources>
#include <osgEarthUtil/EarthManipulator>
#include <osgEarthUtil/Controls>
#include <osgEarth/Registry>
#include <osgEarth/Capabilities>
#include <osgEarth/MapNode>
#include <QMessageBox>
#include <osgEarth/SpatialReference>
#include <osgGA/TrackballManipulator>
#include <osgEarthUtil/Sky>
#include <osgEarthSymbology/Geometry>
#include <osgEarthSymbology/StyleSheet>
#include <osgEarthFeatures/FeatureModelGraph>
#include <QOpenGLFramebufferObject>
#include <thread>
#include <qdir.h>
#include <QTimer>
#include <qtextstream.h>
#include <osgEarthAnnotation/PlaceNode>
#include <osgEarthAnnotation/AnnotationUtils>
#include <osg/Image>
#include <QThread>
#include <qquickwindow.h>
#include <osg/Depth>
#include <qapplication.h>

#include "../AerogisPlugin/VSSWebReader.h"

#include "ShaderFix.h"
#include "ShaderGeneratorFix.h"
#include <iostream>



#ifdef _MSC_VER
#include <VSSRoutines.h>
#endif

extern Q_GUI_EXPORT QImage qt_gl_read_framebuffer(const QSize &size, bool alpha_format, bool include_alpha);

EarthWidget::EarthWidget(QQuickItem *parent)
   : osgQtQuick::OSGViewport(parent)
   , mapNode(nullptr)
   , airplaneGroup(nullptr)
   , airplaneTransform(nullptr)
   , airplaneSet(false)
   , pathNode(0)
   , mTrace_color("#502a7aab")
   , altitude(0.0)
   , mFixed_flight_height(0)
   , updFrameTime_ms(40)
   , cameraAzim(0)
   , cameraRad(2000000)	// 2000 km
   , cameraHeight(5000000)	// 5000 km
   , cameraSet(false)
   , rotSpeed(0.01)
   , mDisplay_mode(AUTO)
   , mManual_mode_timeout(10)
   , mMin_camera_altitude_manual(1000)
   , mMax_camera_altitude_manual(10000000)
   , mAlt_scale(8.0)
   , mPath_width(10000)
   , mNeedTakeScreenshot(false)
   , mNeedTake3Screenshots(false)
   , mNeedTakeGUIScreenshot(false)
   , mNeedTakeTopScreenshots(false)
   , mRequest_flight_info_com(false)
   , mRequest_flight_info_web(false)
   , mVSSThread(0)
{
   mSave_images_h.isSet = false;
   //qDebug()<<"Construct EarthWidget";
   try {
      if (osg::getNotifyLevel() == osg::NotifySeverity::NOTICE) // (if default value, then do minimum notifies)
      {
#ifdef NDEBUG
         osg::setNotifyLevel(osg::FATAL);
#else
         osg::setNotifyLevel(osg::WARN);  // by default, show console messages in debug version
#endif
      }

      if ( !ShaderFix::isSetGL2Support )
         ShaderFix::setGL2Support();

      // controls to manipulate Earth
      view()->setCameraManipulator(new osgEarth::Util::EarthManipulator());


      // Airplane positioner - height, Quat-orientation (we will need this for setModel_path(...) )
      airplaneTransform = new osg::PositionAttitudeTransform;

      // trajectory (we will need this later)
      pathGeom = new osgEarth::Symbology::MultiGeometry();



		// timer for point-to-point move animation
		moveTimer = new QTimer(this);
		connect(moveTimer, SIGNAL(timeout()), this, SLOT(onMoveTimer()));

		// timer for rotate camera animation
		rotTimer = new QTimer(this);
		connect(rotTimer, SIGNAL(timeout()), this, SLOT(onRotTimer()));
		rotTimer->start(updFrameTime_ms);

		// timer for change camera height and radius
		heightTimer = new QTimer(this);
		connect(heightTimer, SIGNAL(timeout()), this, SLOT(onHeightTimer()));

      manualToAutoTimer = new QTimer(this);
      connect(manualToAutoTimer, SIGNAL(timeout()), this, SLOT(onManualToAutoTimer()));

      mLastFlightInfoCheckTimer = new QTimer(this);
      connect(mLastFlightInfoCheckTimer, SIGNAL(timeout()), this, SLOT(onLastFlightInfoCheckTimer()));

		// manual min/max height corrections:
		connect(this, SIGNAL(frameEventsProcessed()), this, SLOT(onFrameEventsProcessed()));

      // frame is drawn (save frame moment)
      connect(this, SIGNAL(frameDrawn(QOpenGLFramebufferObject*)), this, SLOT(onFrameDrawn(QOpenGLFramebufferObject*)), Qt::DirectConnection);

	}
	catch (...) {
		std::cerr << "unhandled exception in EarthWidget ctor\n";
	}
}

EarthWidget::~EarthWidget()
{
  //qDebug()<<"Detstruct EarthWidget";
   if (mVSSThread)
   {
      dynamic_cast<VSSThread*>(mVSSThread)->mIsExiting = true;
      mVSSThread->wait();
   }
   qDeleteAll(featureObjects);
}

QString EarthWidget::request_config(QString address)
{
   try {
      // get data from server
      VSSWebReader wr(address);
      wr.waitDone(10);  // wait 10 seconds

      if (wr.mText == "")
      {
         std::cerr << "Could not load Config from Server in EarthWidget::request_config(...).\nStarting using local config...\n";
      }

      return wr.mText;
   }
   catch (...)	{
      std::cerr << "unhandled exception in EarthWidget::request_config(...)\n";
   }
}

QString EarthWidget::display_mode()
{
	if (mDisplay_mode == AUTO)
		return "auto";
	else if (mDisplay_mode == MANUAL)
		return "manual";
	return "";
}

void EarthWidget::setDisplay_mode(const QString &dm)
{
	try {
		if (dm == "auto")
		{
			mDisplay_mode = AUTO;
			emit display_modeChanged();
		}
		else if (dm == "manual")
		{
			mDisplay_mode = MANUAL;
			emit display_modeChanged();
		}
		else emit error("EarthWidget::setDisplay_mode(...) must get 'auto' or 'manual'");
	}
	catch (...)	{
		std::cerr << "unhandled exception in EarthWidget::setDisplay_mode\n";
	}
}

void EarthWidget::setManual_mode_timeout(int to)
{
	try {
		if (to >= 0)
			mManual_mode_timeout = to;
		else emit error("EarthWidget::setManual_mode_timeout(...) must get integer > 0");
	}
	catch (...)	{
		std::cerr << "unhandled exception in EarthWidget::setManual_mode_timeout\n";
	}
}

void EarthWidget::setMin_camera_altitude_manual(double ma)
{
	try {
		if (ma >= 0 && ma <= 1e8)
			mMin_camera_altitude_manual = ma;
		else emit error("EarthWidget::setMin_camera_altitude_manual(...) must get positive double <= 1.0E+8 (metres)");
	}
	catch (...)	{
		std::cerr << "unhandled exception in EarthWidget::setMin_camera_altitude_manual\n";
	}
}

void EarthWidget::setMax_camera_altitude_manual(double ma)
{
   try {
      if (ma >= 0 && ma <= 1e8)
         mMax_camera_altitude_manual = ma;
      else emit error("EarthWidget::setMax_camera_altitude_manual(...) must get positive double <= 1.0E+8 (metres)");
   }
   catch (...)	{
      std::cerr << "unhandled exception in EarthWidget::setMax_camera_altitude_manual\n";
   }
}

void EarthWidget::setRequest_flight_info_com(bool r)
{
   mRequest_flight_info_com = r;

#ifdef _MSC_VER
   if (mRequest_flight_info_com)
   {
      if (mCom_port != "")
      {
         mLastFlightInfoCheckTimer->start(2000);
      }
   }
   else
   {
      mLastFlightInfoCheckTimer->stop();
   }
#endif
   emit request_flight_info_comChanged();
}

void EarthWidget::setRequest_flight_info_web(bool r)
{
   mRequest_flight_info_web = r;

   if (mRequest_flight_info_web)
   {
      if (mWeb_address != "")
      {
         mLastFlightInfoCheckTimer->start(2000);
      }
   }
   else
   {
      mLastFlightInfoCheckTimer->stop();
   }
   emit request_flight_info_webChanged();
}

void EarthWidget::onRotTimer()
{
	try {
		if (!airplaneSet)
			return;

		auto curT = QTime::currentTime();
		int time_old_to_cur_ms = rotLastUpdTime.msecsTo(curT);	// time from last frame to current

		double dAz = double(time_old_to_cur_ms) * 0.001 * rotSpeed;	// fraction of time that passed
		cameraAzim += dAz;
		applyCoordinatesOnScene();
		rotLastUpdTime = curT;
	}
	catch (...) {
		std::cerr << "unhandled exception in EarthWidget::onMoveTimer\n";
	}
}

void EarthWidget::onMoveTimer()
{
	try {
		auto curT = QTime::currentTime();
		int time_old_to_end_ms = moveLastUpdTime.msecsTo(moveEndTime);
		int time_cur_to_end_ms = curT.msecsTo(moveEndTime);	// will be < 0 if moveEndTime passed
		int time_old_to_cur_ms = moveLastUpdTime.msecsTo(curT);	// time from last frame to current
		if (time_cur_to_end_ms > 0)
		{
			double t = double(time_old_to_cur_ms) / time_old_to_end_ms;	// fraction of time that passed
			auto lerp = [t](double a, double b) { return a + t*(b - a); };
			setNewCoordinatesOnScene(lerp(longitude, moveEnd.x()), lerp(latitude, moveEnd.y()), lerp(altitude, moveEnd.z()), lerp(azimuth, moveEndAzim));
			moveLastUpdTime = curT;
		}
		else
		{
			moveTimer->stop();
         setNewCoordinatesOnScene(moveEnd.x(), moveEnd.y(), moveEnd.z(), moveEndAzim);
         pathNode->init(); // make visible the-last-spline geometry we created earlier
         emit moveAirplaneAnimationDone();
		}
	}
	catch (...) {
		std::cerr << "unhandled exception in EarthWidget::onMoveTimer\n";
	}
}

void EarthWidget::onHeightTimer()
{
	try {
		auto curT = QTime::currentTime();
		int time_old_to_end_ms = heightLastUpdTime.msecsTo(heightEndTime);
		int time_cur_to_end_ms = curT.msecsTo(heightEndTime);	// will be < 0 if moveEndTime passed
		int time_old_to_cur_ms = heightLastUpdTime.msecsTo(curT);	// time from last frame to current
		if (time_cur_to_end_ms > 0)
		{
			double t = double(time_old_to_cur_ms) / time_old_to_end_ms;	// fraction of time that passed
			auto lerp = [t](double a, double b) { return a + t*(b - a); };

			cameraHeight = lerp(cameraHeight, cameraHeightEnd);
			cameraRad = lerp(cameraRad, cameraRadEnd);
			applyCoordinatesOnScene();

			heightLastUpdTime = curT;
		}
		else
		{
			heightTimer->stop();
			cameraHeight = cameraHeightEnd;
			cameraRad = cameraRadEnd;
			applyCoordinatesOnScene();
		}
	}
	catch (...) {
		std::cerr << "unhandled exception in EarthWidget::onHeightTimer\n";
	}
}

void EarthWidget::onManualToAutoTimer()
{
   try {
      auto curT = QTime::currentTime();
      int time_old_to_cur_ms = lastManualUseTime.msecsTo(curT);	// time from last manual use to current
      if ((mManual_mode_timeout * 1000 - time_old_to_cur_ms) < 250)		// accuracy 250 ms
      {
         setDisplay_mode("auto");
         manualToAutoTimer->stop();
      }
   }
   catch (...) {
      std::cerr << "unhandled exception in EarthWidget::onManualToAutoTimer\n";
   }
}

void EarthWidget::onLastFlightInfoCheckTimer()
{
   if (mVSSThread)
   {
      auto &lfi = dynamic_cast<VSSThread*>(mVSSThread)->mLastFlightInfo;
      if (lfi.isNew)
      {
         lfi.isNew = false;
         emit newFlightInfo(lfi.Lng, lfi.Lat, lfi.Alt,
            lfi.MoscowDateTime, lfi.TimeToDest, lfi.KmToDest,
            lfi.Temp, lfi.Speed);
      }
   }
}

void EarthWidget::searchForNearbyObject()
{
   try {

      // find & show closest Object

      if (mObjects_features_name.isEmpty())
         return;

      // airplane geo point
      const auto* latLong = osgEarth::SpatialReference::get("wgs84");	// coordinate system wsg84
      osgEarth::GeoPoint geoPoint(latLong, longitude, latitude, 0, osgEarth::ALTMODE_ABSOLUTE);


      // Look at all Objects on Map - do we need to show picture for them?
      if (featureObjects.empty())
         loadFeatureObjects();

      // check all features, find closest (but inside mMax_object_distance)
      double minDist = mMax_object_distance;
      osgEarth::Features::Feature *feature = nullptr;
      for (auto *f : featureObjects)
      {
         osg::Vec3d gp = f->getGeometry()->asVector().front();

         // get the closest
         double dist = geoPoint.distanceTo(osgEarth::GeoPoint(latLong, gp, osgEarth::ALTMODE_ABSOLUTE));
         if (dist < minDist)
         {
            // must have text description or photo
            if (!f->getString("text_" + mLanguage.toStdString()).empty() || !f->getString("photo").empty())
            {
               minDist = dist;
               feature = f;
            }
         }
      }

      if (feature)
      {
		  if (!mLanguage.isEmpty())
		  {

			  // set to display the feature
			  QString objCaption, objDescr, objImgFile;

			  objCaption = feature->getString("name_" + mLanguage.toStdString()).c_str();
			  objDescr = feature->getString("text_" + mLanguage.toStdString()).c_str();
			  if (!objDescr.isEmpty())
			  {
				  objDescr = mObjects_text_dir + (mObjects_text_dir.isEmpty() ? "" : "/") + mLanguage + "/" + objDescr;
				  // read text file
				  QFile f(objDescr);
				  if (f.open(QFile::ReadOnly | QFile::Text))
				  {
					  QTextStream in(&f);
                 in.setCodec("utf-8");
					  objDescr = in.readAll();
				  }
				  else
				  {
					  std::cerr << "EarthWidget::searchForNearbyObject - '" << objDescr.toStdString() <<"' no text file\n";
					  objDescr.clear();
				  }
			  }
			  objImgFile = feature->getString("photo").c_str();
			  if (!objImgFile.isEmpty())
				  objImgFile = mObjects_photo_dir + (mObjects_photo_dir.isEmpty() ? "" : "/") + objImgFile;
			  emit newObjectInfo(objCaption, objDescr, objImgFile);
		  }
		  else
		  {
			  std::cerr << "EarthWidget::searchForNearbyObject - 'language' not set\n";
		  }
      }

   }
   catch (...) {
      std::cerr << "unhandled exception in EarthWidget::searchForNearbyObject\n";
   }
}

void EarthWidget::loadFeatureObjects()
{
   try {
      if (mapNode == nullptr)
      {
         emit error("error in EarthWidget::loadFeatureObjects: map not yet loaded!\n");
         return;
      }

      // load features
      auto *layersGrp = mapNode->getModelLayerGroup();
      for (unsigned int ml = 0; ml < layersGrp->getNumChildren(); ml++)
      {
         auto *mn = layersGrp->getChild(ml);
         auto *chn = dynamic_cast<osg::Group*>(mn)->getChild(0);
         auto s = chn->getName();
         if (mObjects_features_name == s.c_str())    // "cities_labels"
         {
            std::cout <<"MY DEBUG:: " << s.c_str() << endl;
            auto *fn = dynamic_cast<osgEarth::Features::FeatureModelGraph*>(chn);   // features node
            if (fn)
            {
               auto *fs = fn->getSession()->getFeatureSource();
               int fN = fs->getFeatureCount();
               featureObjects.reserve(fN);
               // load all features
               for (int fi = 0; fi < fN; fi++)
               {
                  auto *f = fs->getFeature(fi);
                  featureObjects.push_back(f);
               }
            }
         }
      }
   }
   catch (...)
   {
      std::cerr << "unhandled exception in EarthWidget::loadFeatureObjects\n";
   }

}

double EarthWidget::azimuthTo(double lon, double lat)
{
   return -atan2((longitude - lon)*cos(latitude / 180 * osg::PI), latitude - lat);
}

osg::Vec3d EarthWidget::fromGeo(osg::Vec3d llh)
{
   osg::Vec3d w;
   osgEarth::GeoPoint p(mapNode->getMapSRS(), llh);
   p.toWorld(w);
   return w;
}

osg::Vec3d EarthWidget::toGeo(osg::Vec3d xyz)
{
   osg::Vec3d g;
   osgEarth::GeoPoint p;
   p.fromWorld(mapNode->getMapSRS(),xyz);
   return p.vec3d();
}

osg::Vec3d EarthWidget::localUpFromGeo(osg::Vec3d llh)
{
   osg::Vec3d lu = fromGeo(llh);
   lu.normalize();
   return lu;
}

void EarthWidget::moveTo(double lon, double lat, double alt, int move_time_ms)
{
	try {
      alt = mFixed_flight_height ? mFixed_flight_height/mAlt_scale : alt; // showAlt
      
      if (!airplaneSet)
		{
			setNewCoordinatesOnScene(lon, lat, alt, 0);
         addPathPoint(osg::Vec3d(lon, lat, alt));
         return;
		}

		if (lon == longitude && lat == latitude)
			return;

		moveEnd = osg::Vec3d(lon, lat, alt);
		moveEndAzim = azimuthTo(lon, lat);	// airplane end azimuth to be begin-to-end vector
		auto curT = QTime::currentTime();
		moveLastUpdTime = curT;
		moveEndTime = curT.addMSecs(move_time_ms);
		moveTimer->start(updFrameTime_ms);	// 40 ms timer

      addPathPoint(osg::Vec3d(lon, lat, alt));
	}
	catch (...) {
		std::cerr << "unhandled exception in EarthWidget::moveTo\n";
	}
}

void EarthWidget::moveCameraTo(double hat, double rad, int move_time_ms)
{
	try {
		if (!cameraSet)
		{
			cameraHeight = hat;
			cameraRad = rad;
			applyCoordinatesOnScene();
			return;
		}

		cameraHeightEnd = hat;
		cameraRadEnd = rad;

		auto curT = QTime::currentTime();
		heightLastUpdTime = curT;
		heightEndTime = curT.addMSecs(move_time_ms);
		heightTimer->start(updFrameTime_ms);	// 40 ms timer
	}
	catch (...) {
		std::cerr << "unhandled exception in EarthWidget::moveCameraTo\n";
	}
}

void EarthWidget::setNewCoordinatesOnScene(double lon, double lat, double alt)
{
	try {

		// airplane local orientation
		double azim = 0;
		if (airplaneSet)
			azim = azimuthTo(lon, lat);
		setNewCoordinatesOnScene(lon, lat, alt, azim);
	}
	catch (...) {
		std::cerr << "unhandled exception in EarthWidget::setNewCoordinatesOnScene(lon,lat,alt)\n";
	}
}

void EarthWidget::setNewCoordinatesOnScene(double lon, double lat, double alt, double azim)
{
	try {

		latitude = lat;
		longitude = lon;
        altitude = alt;
		azimuth = azim;
		applyCoordinatesOnScene();
	}
	catch (...) {
		std::cerr << "unhandled exception in EarthWidget::setNewCoordinatesOnScene(lon,lat,alt,azim)\n";
	}
}

void EarthWidget::setArchivePath(const QVariantList &ar)
{
	try {
      if (mapNode == nullptr)
      {
         emit error("error in EarthWidget::setArchivePath: map not yet loaded!\n");
         return;
      }
      
      pathPoints.clear();
      pathGeom->clear();
	   osg::Vec3d lastP;
		for (const auto &i : ar)
		{
         const auto &v = i.value<QVector3D>();
         double showAlt = mFixed_flight_height ? mFixed_flight_height/mAlt_scale : v.z();
         addPathPoint(lastP = osg::Vec3d(v.x(), v.y(), showAlt));
		}
		if (ar.size() > 0)
		{
			longitude = lastP.x();
			latitude = lastP.y();
			altitude = lastP.z();
		}
	}
	catch (...) {
		std::cerr << "unhandled exception in EarthWidget::setArchivePath(...)\n";
	}
}


void EarthWidget::myClearPath()
{
    pathPoints.clear();
    pathGeom->clear();
}

void EarthWidget::requestFlightArchive()
{
   try {
      if (mapNode == nullptr)
      {
         emit error("error in EarthWidget::requestFlightArchive: map not yet loaded!\n");
         return;
      }

      if (!mRequest_flight_archive || mFlight_archive_request_address == "")
      {
         emit error("error in EarthWidget::requestFlightArchive: parameters not set!\n");
         return;
      }

      // get data from server
      VSSWebReader wr(mFlight_archive_request_address);
      wr.waitDone(10);

      if (wr.mDatasGot == 2)
      {
         int dN;
         memcpy(&dN, wr.mData[0].data(), 4);
         if (wr.mData[0].size() == (dN + 1) * 4)
         {
            std::vector<int> szs(dN);
            int tSz = 0;
            for (int i = 0; i < dN; i++)
            {
               memcpy(&szs[i], wr.mData[0].data()+i*4+4, 4);
               tSz += szs[i];
            }
            if (tSz == wr.mData[1].size())
            {
               pathPoints.clear();
               pathGeom->clear();

               osg::Vec3d lastP;
               int ptr = 0;
               FlightInfo fi;
               for (int i = 0; i < dN; i += 2)
               {
                  QByteArray ba(wr.mData[1].data() + ptr, szs[i]);
                  fi.isNew = false;
                  if (!fi.initFromBlob(ba))
                  {
                     QByteArray ba2(wr.mData[1].data() + ptr + szs[i], szs[i + 1]);
                     fi.initFromBlob(ba2);
                  }
                  if (fi.isNew)
                  {
                     double showAlt = mFixed_flight_height ? mFixed_flight_height / mAlt_scale : fi.Alt;
                     addPathPoint(lastP = osg::Vec3d(fi.Lat, fi.Lng, showAlt));
                  }

                  ptr += szs[i] + szs[i + 1];
               }
               if (fi.isNew)
               {
                  longitude = lastP.x();
                  latitude = lastP.y();
                  altitude = lastP.z();
               }
                  
            }
         }
      }
      else
      {
         std::cerr << "EarthWidget::requestFlightArchive(...) failed to get flight Archive from server " << mFlight_archive_request_address.toStdString() << "\n";
      }
   }
   catch (...) {
      std::cerr << "unhandled exception in EarthWidget::requestFlightArchive(...)\n";
   }
}

void EarthWidget::addPathPoint(osg::Vec3d newP)
{
   try {

      if (mapNode == nullptr)
      {
         emit error("error in EarthWidget::addPathPoint: map not yet loaded!\n");
         return;
      }

      //-------------------- { spline functions -----------------------//

      // Bezier curve 3-order
      auto spln = [](const osg::Vec3d &p0, const osg::Vec3d &p1, const osg::Vec3d &p2, const osg::Vec3d &p3, double t)->osg::Vec3d
      {
         double q = 1.0 - t;
         double q2 = q*q, t2 = t*t;
         return p0*(q*q2) + p1*(3.0*t*q2) + p2*(3.0*t2*q) + p3*(t*t2);
      };

      // our Bezier curve 3-order built by v1,v2
      auto splnByV1V2 = [&spln](const osg::Vec3d &p1, const osg::Vec3d &p2, const osg::Vec3d &v1, const osg::Vec3d &v2, double t)->osg::Vec3d
      {
         // part = 0.5
         const double part = 0.333333;
         double len = (p2 - p1).length();
         auto x1 = p1 + v1*len*part;
         auto x2 = p2 - v2*len*part;

         return spln(p1, x1, x2, p2, t);
      };

      auto lerp = [](const double &v0, const double &v1, double t)->double
      {
         double q = 1.0 - t;
         return v0*q + v1*t;
      };

      auto lerpV = [](const osg::Vec3d &v0, const osg::Vec3d &v1, double t)->osg::Vec3d
      {
         double q = 1.0 - t;
         auto v = v0*q + v1*t;
         v.normalize();
         return v;
      };

      auto fillPoly = [this, &splnByV1V2, &lerp, &lerpV](osgEarth::Symbology::Polygon *poly, osg::Vec3d p1, osg::Vec3d p2, osg::Vec3d v1, osg::Vec3d v2)
      {
         auto left1 = p1 ^ v1;
         left1.normalize();
         auto left2 = p2 ^ v2;
         left2.normalize();

         // fill Polygon with points
         const int N = 4;  // spline will have 4 points
         poly->resize(N * 2 + 2);

         auto gp1 = toGeo(p1), gp2 = toGeo(p2);
         double h1 = gp1.z(), h2 = gp2.z();

         for (int n = 0; n <= N; n++)
         {
            double t = 1.0/N * n;
            auto pt = splnByV1V2(p1, p2, v1, v2, t);
            auto leftt = lerpV(left1, left2, t) * mPath_width*0.5;
            double h = lerp(h1, h2, t)*mAlt_scale;

            poly->at(n) = toGeo(pt + leftt);
            poly->at(n).z() = h;
            poly->at(N * 2 + 1 - n) = toGeo(pt - leftt);
            poly->at(N * 2 + 1 - n).z() = h;
         }
      };

      //-------------------- spline functions } -----------------------//

      pathPoints.push_back(newP);

      if (pathPoints.size() <= 1)
         return;

      // add new Polygon to MultiGeometry
      auto *pathPolygon = new osgEarth::Symbology::Polygon();

      auto p1 = fromGeo(*(pathPoints.end() - 2));  // from this point-
      auto p2 = fromGeo(newP);                     // -to this

      auto r12 = p2 - p1;
      auto v12 = r12;   v12.normalize();  // tangent vector p1->p2

      if (pathPoints.size() == 2)
      {
         // fill last polygon
         fillPoly(pathPolygon, p1, p2, v12, v12);
      }
      else // (pathPoints.size() > 2)
      {
         // fill last polygon
         auto p0 = fromGeo(*(pathPoints.end() - 3));  // p0 - starting point of previous (pre-last) polygon
         auto r01 = p1 - p0;
         auto v01 = r01; v01.normalize(); // tangent vector p0->p1
         auto v1 = v01 + v12;   v1.normalize();   // tangent vector at p1
         auto v2 = (v12 - v1) + v12;   // extrapolate tangent at p2
         v2.normalize();
         fillPoly(pathPolygon, p1, p2, v1, v2);

         // correct pre-last polygon
         // because - v1 changed
         auto *prevPoly = dynamic_cast<osgEarth::Symbology::Polygon*>(pathGeom->getComponents().back().get());
         if (pathPoints.size() == 3)
         {
            fillPoly(prevPoly, p0, p1, v01, v1);
         }
         else // (pathPoints.size() > 3)
         {
            auto p_ = fromGeo(*(pathPoints.end() - 4));  // p_ - starting point of pre-pre-last polygon
            auto r_0 = p0 - p_;
            auto v_0 = r_0; v_0.normalize(); // tangent vector p_->p0
            auto v0 = v_0 + v01;   v0.normalize();   // tangent vector at p0
            fillPoly(prevPoly, p0, p1, v0, v1);
         }
      }

      pathNode->init();

      pathGeom->add(pathPolygon);   // add, but yet not visible! make it visible when plane arrives.
   }
   catch (...) {
      std::cerr << "unhandled exception in EarthWidget::addPathPoint(...)\n";
   }
}

void EarthWidget::applyCoordinatesOnScene()
{
	try {

      if (mapNode == nullptr)
      {
         emit error("error in EarthWidget::applyCoordinatesOnScene: map not yet loaded!\n");
         return;
      }

		//osgEarth::ModelLayer* layer = mapNode->getMap()->getModelLayerByName("airplane");
		//osg::Node* node = mapNode->getModelLayerNode(layer);

		// check if we have Airplane model?
		if (airplaneTransform->getNumChildren() == 0)
		{
			// try to load...
			emit needAirplaneModel();
		}

		// Geo-coordinates
		const auto* latLong = osgEarth::SpatialReference::get("wgs84");	// coordinate system wsg84
		osgEarth::GeoPoint point(latLong, longitude, latitude, 0, osgEarth::ALTMODE_ABSOLUTE);
		osg::Matrix matrix;
		point.createLocalToWorld(matrix);
		airplaneGroup->setMatrix(matrix);

		// airplane local orientation
		airplaneTransform->setAttitude(osg::Quat(azimuth, osg::Z_AXIS));
		double sceneAltitude = altitude*mAlt_scale;
      airplaneTransform->setPosition(osg::Vec3d(0, 0, sceneAltitude));

		airplaneSet = true;

		// camera coordinates
		osg::Vec3d groupPos, locUp;
		point.toWorld(groupPos);
		locUp = groupPos;
		locUp.normalize();

		osg::Matrix rotMatrix(osg::Quat(cameraAzim, osg::Z_AXIS));

		// We can't use Camera as regular Node, need to reset View matrix
		auto c4 = osg::Vec4d(cameraRad, 0, cameraHeight, 1) * rotMatrix * matrix;	// OSG uses postfix transform matrices!
		auto camPos = osg::Vec3d(c4.x(), c4.y(), c4.z());
		auto *camManip = view()->getCameraManipulator();
		if (camManip && mDisplay_mode==AUTO)
		{
			auto camMatrix = osg::Matrix::lookAt(camPos, groupPos + locUp*sceneAltitude, locUp);
			camManip->setByInverseMatrix(camMatrix);
		}

		cameraSet = true;
	}
	catch (...) {
		std::cerr << "unhandled exception in EarthWidget::applyCoordinatesOnScene\n";
	}
}

void EarthWidget::setModel_path(const QString &url)
{
   try {

      // load airplane model
      osg::Node *node = ShaderFix::createShaderCompositionScene(url.toStdString().c_str());
      if (node == nullptr)
      {
         emit error("failed to load file " + url);
         return;
      }

      // attach Airplane to scene
	   airplaneTransform->addChild(node);
      auto *ss = node->getOrCreateStateSet();
      ss->setRenderBinDetails(100, "RenderBin");

      mModel_path = url;
   }
   catch (...) {
      std::cerr << "unhandled exception in EarthWidget::setModel_path\n";
   }
}

void EarthWidget::setTrace_color(const QString &clr)
{
   try {

      mTrace_color = clr;

      auto *fn = dynamic_cast<osgEarth::Annotation::FeatureNode*>(pathNode);
      if (fn)
      {
         auto &st = fn->getFeature()->style();
         st->getOrCreate<osgEarth::Symbology::PolygonSymbol>()->fill()->color() = osgEarth::Symbology::Color(mTrace_color.toStdString());
      }
   }
   catch (...) {
      std::cerr << "unhandled exception in EarthWidget::setModel_path\n";
   }
}

void EarthWidget::setCom_port(const QString &comPort)
{
   try {

      mCom_port = comPort;

      if (mRequest_flight_info_com && mCom_port != "")
      {
         mLastFlightInfoCheckTimer->start(2000);
      }

   }
   catch (...) {
      std::cerr << "unhandled exception in EarthWidget::setCom_port\n";
   }
}

void EarthWidget::setWeb_address(const QString &adr)
{
   try {

      mWeb_address = adr;

      if (mRequest_flight_info_web && mWeb_address != "")
      {
         mLastFlightInfoCheckTimer->start(2000);
      }

   }
   catch (...) {
      std::cerr << "unhandled exception in EarthWidget::setWeb_address\n";
   }
}

void EarthWidget::onManualEvent()
{
	setDisplay_mode("manual");
	lastManualUseTime = QTime::currentTime();
	if (!manualToAutoTimer->isActive())
		manualToAutoTimer->start(500);
}

void EarthWidget::mousePressEvent(QMouseEvent *event)
{
	onManualEvent();
	OSGViewport::mousePressEvent(event);
}

void EarthWidget::mouseMoveEvent(QMouseEvent *event)
{
	onManualEvent();
	OSGViewport::mouseMoveEvent(event);
}

void EarthWidget::mouseReleaseEvent(QMouseEvent *event)
{
	onManualEvent();
	OSGViewport::mouseReleaseEvent(event);
}

void EarthWidget::wheelEvent(QWheelEvent *event)
{
	onManualEvent();
	OSGViewport::wheelEvent(event);
}

void EarthWidget::pCamera(){
    onManualEvent();
    myplusCamera();

}
void EarthWidget:: mCamera(){
    onManualEvent();
    myminusCamera();
}


void EarthWidget::onFrameEventsProcessed()
{
   // here we limit user movements by fixing View matrix, so Camera will be rendered in correct height
   try {
      if (mDisplay_mode != MANUAL)
         return;

      if (mapNode == nullptr)
      {
         emit error("error in EarthWidget::onFrameEventsProcessed: map not yet loaded!\n");
         return;
      }

      auto *camManip = view()->getCameraManipulator();
      osg::Vec3d eye, target, up;
      camManip->getInverseMatrix().getLookAt(eye, target, up);
      osgEarth::GeoPoint eyeGp;
      auto *srs = mapNode->getMapSRS();
      bool ok = eyeGp.fromWorld(srs, eye);
      if (!ok)
         return;

      double h = eyeGp.z();
      auto targetV = target - eye;
      double targetVpr = eye*targetV / eye.length();

      // limit by Top
      double dh = h - mMax_camera_altitude_manual;
      if (dh > 0)
      {
         eye -= targetV * (dh / targetVpr);
         camManip->setByInverseMatrix(osg::Matrixd::lookAt(eye, eye + targetV, up));
      }

      // limit by Bottom
      dh = h - mMin_camera_altitude_manual;
      if (dh < 0)
      {
         eye -= targetV * (dh / targetVpr);
         camManip->setByInverseMatrix(osg::Matrixd::lookAt(eye, eye + targetV, up));
      }
   }
   catch (...)
   {
      std::cerr << "unhandled exception in EarthWidget::onFrameEventsProcessed\n";
   }
}

void EarthWidget::setLanguage(const QString &_la)
{
   try {
      auto la = _la.toLower();
      auto laOld = mLanguage;
      mLanguage = la;
      mNeedTakeScreenshotLang = la;

      if (mapNode)
      {
         setLanguageOnMap(la, laOld);
      }
   }
   catch (...)
   {
      std::cerr << "unhandled exception in EarthWidget::setLanguage\n";
   }
}

void EarthWidget::setLanguageOnMap(const QString &la, const QString &laOld)
{
   try {
      if (mapNode == nullptr)
      {
         emit error("error in EarthWidget::setLanguageOnMap: map not yet loaded!\n");
         return;
      }

      if (osg::getNotifyLevel() > osg::NotifySeverity::FATAL)
         printf("switch language '%s'->'%s'\n", laOld.toStdString().c_str(), la.toStdString().c_str());

      // Change language on map:
      // Look at all 'model'-s on Map (layers) - and change lanuguage

      auto *layersGrp = mapNode->getModelLayerGroup();
      for (unsigned int ml = 0; ml < layersGrp->getNumChildren(); ml++)
      {
         auto *mn = layersGrp->getChild(ml);
         auto *chn = dynamic_cast<osg::Group*>(mn)->getChild(0);

         /*
         // Method: show/hide layers with names _ru/_en/_es etc
         // does the same as dirty() the layer - so works pretty slow :(
         auto s = chn->getName();   // boundaries_100, cities_labels etc

         int L = s.length();
         if (L > 3 && *(s.end() - 3) == '_' && la.toStdString() == s.substr(L - 2, 2))
         {
         // looks like name we want. Enable it:
         chn->setNodeMask(0xffffffff);
         // and disable all other languages
         for (unsigned int ml2 = 0; ml2 < layersGrp->getNumChildren(); ml2++)
         {
         auto *mn2 = layersGrp->getChild(ml2);
         auto *chn2 = dynamic_cast<osg::Group*>(mn2)->getChild(0);
         auto s2 = chn2->getName();   // boundaries_100, cities_labels etc

         if (s2.length() == L && s2.substr(0, L - 2) == s.substr(0, L - 2) && s2 != s)
         {
         // same layer but other language
         // make invisible
         chn2->setNodeMask(0);
         }
         }
         }
         */



         // Method: rewrite text's "[name_ru]" to other language
         // is updated city-by-city (not all together), so it needs dirty() the whole layer; better use setNodeMask(0 / 0xffffffff) for the whole layer.

         auto *fn = dynamic_cast<osgEarth::Features::FeatureModelGraph*>(chn);   // features node
         bool containText = false;
         if (fn)
         {
            auto *styleSheet = fn->getStyles();
            const auto &sSelList = styleSheet->selectors();
            for (auto & sel : sSelList)
            {
               const auto &sNameRef = sel.styleName();
               if (sNameRef.isSet())
               {
                  auto styleName = sNameRef.get();
                  auto *style = styleSheet->getStyle(styleName, false);
                  auto *ts = style->getSymbol<osgEarth::Symbology::TextSymbol>();
                  if (ts)
                  {
                     // we found TextSymbol
                     auto &content = ts->content();
                     auto str = content.get().expr();
                     if (str.substr(0, 6) == "[name_" && str.back() == ']' && str.length() == 9)
                     {
                        // change 2 language letters
                        *(str.end() - 3) = la[0].toLatin1();
                        *(str.end() - 2) = la[1].toLatin1();
                        // change 'text-content'
                        content = str;

                        containText = true;
                     }
                  }
               }

            }
         }


         if (containText && laOld != "")
         {
            auto loadDict = [this, fn](std::map< std::string, osgEarth::Features::Feature* > &mp_, std::string la_)->void
            {
               // load part of dictionary

               auto *fs = fn->getSession()->getFeatureSource();
               int fN = fs->getFeatureCount();
               // load all features of this layer (fn)
               for (int fi = 0; fi < fN; fi++)
               {
                  auto *f = fs->getFeature(fi);
                  auto objCaption = f->getString("name_" + la_);
                  mp_[objCaption] = f;
               }
            };

            auto &laMapOld = featureDict[fn][laOld.toStdString()];
            if (laMapOld.empty())
               loadDict(laMapOld, laOld.toStdString());

            // reload whole layer

            std::function<void(osg::Group*)> goNodeChilds;
            goNodeChilds = [&goNodeChilds, &laMapOld, &la,&laOld](osg::Group* grp)
            {
               for (unsigned int chil = 0; chil < grp->getNumChildren(); chil++)
               {
                  auto *ch = grp->getChild(chil);

                  //auto *txt = dynamic_cast<osgText::Text*>(ch);
                  auto *txt = dynamic_cast<osgEarth::Annotation::PlaceNode*>(ch);
                  if (txt)
                  {
                     auto str = txt->getText();
                     if (!str.empty())
                     {
                        // change 'text-content'
                        auto fi = laMapOld.find(str);
                        if (fi != laMapOld.end())
                        {
                           auto newObjCaption = fi->second->getString("name_" + la.toStdString());
                           txt->setText(newObjCaption);
                        }
                        else
                        {
                           if (osg::getNotifyLevel() > osg::NotifySeverity::FATAL)
                           {
                              printf("no translation '%s'->'%s' for '%s'\n", laOld.toStdString().c_str(), la.toStdString().c_str(), str.c_str());
                           }
                        }

                        continue;   // don't go in inner cycle!
                     }
                  }

                  auto *gr2 = dynamic_cast<osg::Group*>(ch);
                  if (gr2)
                     goNodeChilds(gr2);   // go in inner cycle
               }
            };

            goNodeChilds(fn);
         }

      }


   }
   catch (...)
   {
      std::cerr << "unhandled exception in EarthWidget::setLanguageOnMap\n";
   }
}

void EarthWidget::setSave_images_top(int low, int med, int hi)
{
   try
   {
      auto &s = mSave_images_h;
      s.isSet = true;
      s.low = low;
      s.med = med;
      s.hi = hi;
   }
   catch (...)
   {
      std::cerr << "unhandled exception in EarthWidget::setSave_images_top\n";
   }
}

void EarthWidget::onConfigLoaded()
{
   try {

      // LOAD EARTH
      osg::Node* earthNode = nullptr;
      if (mRequest_earth)
      {
         if (mEarth_request_address != "")
         {
            // get data from server
            VSSWebReader wr(mEarth_request_address);
            wr.waitDone(10);  // wait 10 seconds

            if (wr.mText != "")
            {
               QFile f("aero_tmp.earth");
               if (f.open(QFile::WriteOnly | QFile::Text))
               {
                  QTextStream of(&f);
                  of << wr.mText;
                  of.flush();
                  f.flush();
                  earthNode = osgDB::readNodeFile("aero_tmp.earth");
               }
            }
            if (earthNode == nullptr)
            {
               std::cerr << "Could not load Earth file from Server in EarthWidget::onConfigLoaded().\nStarting using local earth file '"<< mEarth_file.toStdString()<< "'...\n";
            }
         }
      }
      if (!earthNode)
      {
         // Load an earth file
         earthNode = osgDB::readNodeFile(mEarth_file.toStdString());
         if (!earthNode)
         {
            auto msg = "Could not load Earth file '" + mEarth_file + "'\n";
            std::cerr << msg.toStdString();
            emit error(msg);
            return;
         }
      }
      mapNode = osgEarth::MapNode::get(earthNode);

      // Sky
      osgEarth::Config conf;
      conf.fromJSON("{\"driver\": \"gl\", \"hours\" : 0.0,  \"ambient\": 0.05, \"atmospheric_lighting\": true, \"exposure\": 3.0, \"color\": \"#ffffff\"}");
      osgEarth::Util::SkyOptions options(conf);
      auto *sky = osgEarth::Util::SkyNode::create(options, mapNode);
      if (!sky)
         QMessageBox::critical(0, "Error", "Failed to create sky.");
      // use Sky as scene Root
      view()->setSceneData(sky);
      sky->addChild(earthNode);


      // airplane
      // Geo-coordinates
      airplaneGroup = new osg::MatrixTransform;
      sky->addChild(airplaneGroup);
      // Airplane positioner - height, Quat-orientation
      airplaneGroup->addChild(airplaneTransform);


      // trajectory
      osgEarth::Symbology::Style flightLineStyle;
      flightLineStyle.getOrCreate<osgEarth::Symbology::PolygonSymbol>()->fill()->color() = osgEarth::Symbology::Color(mTrace_color.toStdString());
      flightLineStyle.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->clamping() = osgEarth::Symbology::AltitudeSymbol::CLAMP_ABSOLUTE;
      auto* connectionFeature = new osgEarth::Features::Feature(pathGeom, mapNode->getMapSRS(), flightLineStyle);
      pathNode = new osgEarth::Annotation::FeatureNode(mapNode, connectionFeature);
      pathNode->setClusterCulling(false);
      mapNode->addChild(pathNode);

      // fix earth shaders
      ShaderFix::fixEarth(mapNode);

      setLanguageOnMap(mLanguage, "");

      if (mRequest_flight_archive && mFlight_archive_request_address != "")
      {
         requestFlightArchive();
      }

   }
   catch (...) {
      std::cerr << "unhandled exception in EarthWidget::onConfigLoaded\n";
   }

}

/** helper function - to run in separate thread */
void saveImage(const uchar *pbits, int w, int h, QString path, QString fn, double sz_limit)
{
   try
   {
      // 1 // estimate free space
      QTime ct = QTime::currentTime();

      QDir dr(path);
      if (dr.path().isEmpty())
      {
         QString msg = "Error saving image to '" + path + "'\n";
         std::cerr << msg.toStdString();
         return;
      }

      auto infos = dr.entryInfoList();
      double all_sz = 0;
      if (!infos.isEmpty())
      {
         auto findOld = [&infos]()->QFileInfo*
         {
            QFileInfo *oldest = &infos.first();
            for (auto &entry : infos)
            {
               if (entry.created() < oldest->created())
                  oldest = &entry;
            }
            return oldest;
         };

         for (auto &entry : infos)
         {
            all_sz += entry.size();
         }

         while (sz_limit - all_sz < 500000 && !infos.isEmpty())  // 500K approx max screenshot file size
         {
            auto *i = findOld();
            all_sz -= i->size();
            if (osg::getNotifyLevel() > osg::NotifySeverity::FATAL)
            {
               std::cout << "deleting " << i->fileName().toStdString() << std::endl;
            }
            // securely delete file (only jpeg)
            if ( i->fileName().endsWith(".jpeg") )
               dr.remove(i->fileName());
            infos.removeOne(*i);
         }
      }

      // 2 // save image
      QImage image(pbits, w, h, QImage::Format_ARGB32);

      image.save(path + "/" + fn);
      delete[] pbits;

      QTime st = QTime::currentTime();
      if (osg::getNotifyLevel() > osg::NotifySeverity::FATAL)
      {
         std::cout << std::flush << " save time " << ct.msecsTo(st) << " ms\n" << std::flush;
      }
   }
   catch (...)
   {
      std::cerr << "unhandled exception in EarthWidget.cpp saveImage()\n";
   }
}

void EarthWidget::onFrameDrawn(QOpenGLFramebufferObject *fb)
{
   
   // here we limit user movements by fixing View matrix, so Camera will be rendered in correct height
   try {
      if (!mNeedTakeScreenshot)
         return;
      if (!fb)
         return;
      mNeedTakeScreenshot = false;

      QTime ct = QTime::currentTime();
      QString fn = mNeedTakeScreenshotLang + (mNeedTakeScreenshotLang.isEmpty() ? "" : "_") + QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss") + ".jpeg";

      QImage img(fb->toImage());
      int sz = img.byteCount();
      uchar* pbits = new uchar[sz];
      memcpy(pbits, img.constBits(), sz);
      std::thread saveThr(saveImage, pbits,img.width(),img.height(), mSave_images_path, fn, mSave_images_stack_size);   // save image, in a separate thread
      saveThr.detach();

      QTime gt = QTime::currentTime();
      if (osg::getNotifyLevel() > osg::NotifySeverity::FATAL)
      {
         std::cout << "grab time " << ct.msecsTo(gt) << " ms," << std::flush;
      }
   }
   catch (...)
   {
      std::cerr << "unhandled exception in EarthWidget::onFrameDrawn\n";
   }
   
}


void EarthWidget::viewerDrawsFrame(osgQtQuick::AerogisViewer *v)
{
   try {
      // this makes clear state of render:
      bool takeScrs = (mNeedTake3Screenshots || mNeedTakeTopScreenshots);
      bool drawnTopScrs = false;
      bool drawn3Scrs = false;
      QTime ct;
      osgDB::DatabasePager *pager = nullptr;
      if (takeScrs)
      {
         ct = QTime::currentTime(); // take render start time
         pager = view()->getDatabasePager();
         if (pager)
         {
            pager->setDatabasePagerThreadPause(true);
         }
      }

      // top-down screenshots (from over the airplane)
      if (mNeedTakeTopScreenshots)
      {
         mNeedTakeTopScreenshots = false;

         auto *camManip = view()->getCameraManipulator();
         if (mSave_images_h.isSet && camManip)
         {
            drawnTopScrs = true;

            // save camera
            auto oldCam = camManip->getInverseMatrix();

            // Geo-coordinates
            const auto* latLong = osgEarth::SpatialReference::get("wgs84");	// coordinate system wsg84
            osgEarth::GeoPoint point(latLong, longitude, latitude, 0, osgEarth::ALTMODE_ABSOLUTE);
            // camera coordinates
            osg::Vec3d groupPos, locUp;
            point.toWorld(groupPos);
            locUp = groupPos;
            locUp.normalize();

            QStringList prefs = { "low", "med", "hi" };
            auto prefi = prefs.begin();
            // 3 heights
            for (int h : {mSave_images_h.low, mSave_images_h.med, mSave_images_h.hi})
            {
               mNeedTakeScreenshotLang = *prefi++;   // prefix for screenshot name
               if (h == 0)
                  continue;
               auto camMatrix = osg::Matrix::lookAt(groupPos + locUp*h, groupPos, osg::Vec3d(osg::Z_AXIS));
               camManip->setByInverseMatrix(camMatrix);
               mNeedTakeScreenshot = true;
               if (osg::getNotifyLevel() > osg::NotifySeverity::FATAL)
                  printf("take srceenshot '%s'\n", mNeedTakeScreenshotLang.toStdString().c_str());

               v->frameUpdates();
               v->renderingTraversals();
            }

            //restore camera
            camManip->setByInverseMatrix(oldCam);

            // regular frame - draw later
         }
      }
      if (mNeedTake3Screenshots)
      {
         mNeedTake3Screenshots = false;
         drawn3Scrs = true;

         QStringList langs = {"ru","en","es"};
         langs.removeOne(mLanguage);   // first, draw 2 other languages

         mNeedTakeScreenshotLang = mLanguage;
         for (QString &la : langs)
         {
            setLanguageOnMap(la, mNeedTakeScreenshotLang);
            mNeedTakeScreenshotLang = la;
            mNeedTakeScreenshot = true;
            if (osg::getNotifyLevel() > osg::NotifySeverity::FATAL)
               printf("take srceenshot lang '%s'\n", mNeedTakeScreenshotLang.toStdString().c_str());

            v->frameUpdates();
            v->renderingTraversals();
         }

         // regular frame - draw later
      }

      // draw final (regular) frame
      if (takeScrs)
      {
         v->frameUpdates();
         if (drawn3Scrs)
         {
            // ok, need to draw and save 3rd frame in current language
            setLanguageOnMap(mLanguage, mNeedTakeScreenshotLang);
            mNeedTakeScreenshotLang = mLanguage;
            mNeedTakeScreenshot = true;
            if (osg::getNotifyLevel() > osg::NotifySeverity::FATAL)
               printf("take srceenshot lang '%s'\n", mNeedTakeScreenshotLang.toStdString().c_str());

         }
         // render
         v->renderingTraversals();

         if (pager)
         {
            pager->setDatabasePagerThreadPause(false);
         }

         // screenshots render time
         QTime gt = QTime::currentTime();
         if (osg::getNotifyLevel() > osg::NotifySeverity::FATAL)
         {
            std::cout << "grab time " << ct.msecsTo(gt) << " ms," << std::flush;
         }
      }
      else
      {
         v->frame();
      }
   }
   catch (...)
   {
      std::cerr << "unhandled exception in EarthWidget::viewerDrawsFrame\n";
   }
}

void EarthWidget::viewerDrawnFrame()
{
   try {
      if (!mNeedTakeGUIScreenshot)
         return;
      mNeedTakeGUIScreenshot = false;

      QTime ct = QTime::currentTime();
      mNeedTakeScreenshotLang = "gui";
      QString fn = mNeedTakeScreenshotLang + (mNeedTakeScreenshotLang.isEmpty() ? "" : "_") + QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss") + ".jpeg";

      //QImage img(window()->grabWindow());
      QImage img(qt_gl_read_framebuffer(window()->size() * window()->devicePixelRatio(), false, false));
      int sz = img.byteCount();
      uchar* pbits = new uchar[sz];
      memcpy(pbits, img.constBits(), sz);
      std::thread saveThr(saveImage, pbits, img.width(), img.height(), mSave_images_path, fn, mSave_images_stack_size);   // save image, in a separate thread
      saveThr.detach();

      QTime gt = QTime::currentTime();
      if (osg::getNotifyLevel() > osg::NotifySeverity::FATAL)
      {
         std::cout << "grab time " << ct.msecsTo(gt) << " ms," << std::flush;
      }
   }
   catch (...)
   {
      std::cerr << "unhandled exception in EarthWidget::onFrameDrawn\n";
   }

}


/// Helper class function

bool FlightInfo::initFromBlob(const QByteArray &ba)
{
   std::list<CVSSData> vssData;
   int par = 0, pak = 0;
   VSSRoutines::parseVSSData(vssData, ba, par, pak);
   
   return initFromVssData(vssData);
}

bool FlightInfo::initFromVssData(std::list<CVSSData> &vssData)
{
   // we just take Last valid point
   CVSSData *validD = nullptr;
   for (auto &dt : vssData)
   {
      if (!dt.DateIsValid())
         continue;
      if (!dt.HeightFIsValid())
         continue;
      if (!dt.LatitudeIsValid())
         continue;
      if (!dt.LeftDistanceMileIsValid())
         continue;
      if (!dt.LeftTimeMinIsValid())
         continue;
      if (!dt.LongitudeIsValid())
         continue;
      if (!dt.SpeedUIsValid())
         continue;
      if (!dt.TemperatureIsValid())
         continue;
      if (!dt.TimeGIsValid())
         continue;
      validD = &dt;
   }

   if (validD)
   {
      auto &dt = *validD;
      double Lon = dt.Longitude() * (dt.LongitudeDirection() == TLongitudeDirection::longd_East ? 1.0 : -1.0);
      double lat = dt.Latitude() * (dt.LatitudeDirection() == TLatitudeDirection::latd_Nord ? 1.0 : -1.0);
      double alt = dt.HeightF() * (dt.HeightFLevel() == THeightFLevel::hl_Up ? 1.0 : -1.0);
      QDateTime MoscowDT(QDate(dt.DateY(), dt.DateM(), dt.DateD()), QTime(dt.TimeGH(), dt.TimeGM(), dt.TimeGS()));   // Greenwich
      MoscowDT.addSecs(3600 * 3);   // +3 hours (Moscow)
      auto tmin = int(dt.LeftTimeMin());
      auto tLeft = QTime(tmin / 60, tmin % 60, 0);
      auto TimeLeft = tLeft.toString("h:mm");
      double temp = dt.Temperature() * (dt.TemperatureSign() == TTemperatureSign::ts_Plus ? 1.0 : -1.0);

      Lng = Lon;
      Lat = lat;
      Alt = alt*0.3048;
      MoscowDateTime = MoscowDT;
      TimeToDest = TimeLeft;
      KmToDest = dt.LeftDistanceMile()*1.852;
      Temp = temp;
      Speed = dt.SpeedU()*1.852;
      isNew = true;
      return true;
   }

   return false;
}

/// Helper class - a thread to load data from COM-port/Web-server

VSSThread::VSSThread(QObject *parent)
   : QThread(parent)
   , mIsExiting(false)
{
   mLastFlightInfo.isNew = false;
}

void VSSThread::run()
{
   if (mIsExiting)
      return;
   std::list<CVSSData> vssData;
   try
   {
      if (mCom_port != "")
      {
#ifdef _MSC_VER
         if (!VSSRoutines::ReceiveVSSDataCom(mCom_port, vssData))
         {
            std::cerr << "Error receiving data from port " << mCom_port.toStdString() << "\n";
            return;
         }
#endif // only Windows
      }
      else if (mWeb_adr != "")
      {
         if (!VSSRoutines::ReceiveVSSDataWeb(mWeb_adr, vssData))
         {
            std::cerr << "Error receiving data from WebSocket " << mWeb_adr.toStdString() << "\n";
            return;
         }
      }
      else
      {
         std::cerr << "Receive data not work: not set COM port or WebSocket\n";
         return;
      }
   }
   catch (...)
   {
      std::cerr << "Error receiving data from COM port\n";
      return;
   }

   mLastFlightInfo.initFromVssData(vssData);

}


void EarthWidget::requestFlightInfo()
{
   try {
      if (mRequest_flight_info_com && mCom_port != "")
      {
#ifdef _MSC_VER
         // only Windows
         // request COM port in a separate thread
         if (mVSSThread == nullptr)
            mVSSThread = new VSSThread(this);
         auto &thr = *dynamic_cast<VSSThread*>(mVSSThread);
         thr.mCom_port = mCom_port;
         mVSSThread->start();
#endif
      }
      else if (mRequest_flight_info_web && mWeb_address != "")
      {
         // request WebSockets server in a separate thread
         if (mVSSThread == nullptr)
            mVSSThread = new VSSThread(this);
         auto &thr = *dynamic_cast<VSSThread*>(mVSSThread);
         thr.mWeb_adr = mWeb_address;
         mVSSThread->start();

      }
      else
      {
         std::cerr << "error in EarthWidget::requestFlightInfo(): set 'request_flight_info_com'/'web' and 'com_port'/'web_address' for EarthWidget first!\n";
         return;
      }

   }
   catch (...)
   {
      std::cerr << "unhandled exception in EarthWidget::requestFlightInfoFromCOM()\n";
   }
}

//#include "EarthWidget.moc"
