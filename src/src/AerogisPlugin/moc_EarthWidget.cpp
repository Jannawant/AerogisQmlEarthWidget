/****************************************************************************
** Meta object code from reading C++ file 'EarthWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../EarthWidget/EarthWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'EarthWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_VSSThread_t {
    QByteArrayData data[1];
    char stringdata0[10];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_VSSThread_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_VSSThread_t qt_meta_stringdata_VSSThread = {
    {
QT_MOC_LITERAL(0, 0, 9) // "VSSThread"

    },
    "VSSThread"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_VSSThread[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void VSSThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject VSSThread::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_VSSThread.data,
      qt_meta_data_VSSThread,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *VSSThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *VSSThread::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_VSSThread.stringdata0))
        return static_cast<void*>(const_cast< VSSThread*>(this));
    return QThread::qt_metacast(_clname);
}

int VSSThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_EarthWidget_t {
    QByteArrayData data[91];
    char stringdata0[1321];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_EarthWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_EarthWidget_t qt_meta_stringdata_EarthWidget = {
    {
QT_MOC_LITERAL(0, 0, 11), // "EarthWidget"
QT_MOC_LITERAL(1, 12, 17), // "needAirplaneModel"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 5), // "error"
QT_MOC_LITERAL(4, 37, 3), // "msg"
QT_MOC_LITERAL(5, 41, 13), // "newObjectInfo"
QT_MOC_LITERAL(6, 55, 10), // "objCaption"
QT_MOC_LITERAL(7, 66, 8), // "objDescr"
QT_MOC_LITERAL(8, 75, 10), // "objImgFile"
QT_MOC_LITERAL(9, 86, 13), // "newFlightInfo"
QT_MOC_LITERAL(10, 100, 3), // "Lng"
QT_MOC_LITERAL(11, 104, 3), // "Lat"
QT_MOC_LITERAL(12, 108, 3), // "Alt"
QT_MOC_LITERAL(13, 112, 14), // "MoscowDateTime"
QT_MOC_LITERAL(14, 127, 10), // "TimeToDest"
QT_MOC_LITERAL(15, 138, 8), // "KmToDest"
QT_MOC_LITERAL(16, 147, 4), // "Temp"
QT_MOC_LITERAL(17, 152, 5), // "Speed"
QT_MOC_LITERAL(18, 158, 25), // "moveAirplaneAnimationDone"
QT_MOC_LITERAL(19, 184, 19), // "display_modeChanged"
QT_MOC_LITERAL(20, 204, 30), // "request_flight_info_comChanged"
QT_MOC_LITERAL(21, 235, 30), // "request_flight_info_webChanged"
QT_MOC_LITERAL(22, 266, 24), // "setNewCoordinatesOnScene"
QT_MOC_LITERAL(23, 291, 3), // "lon"
QT_MOC_LITERAL(24, 295, 3), // "lat"
QT_MOC_LITERAL(25, 299, 3), // "alt"
QT_MOC_LITERAL(26, 303, 4), // "azim"
QT_MOC_LITERAL(27, 308, 14), // "setArchivePath"
QT_MOC_LITERAL(28, 323, 2), // "ar"
QT_MOC_LITERAL(29, 326, 6), // "moveTo"
QT_MOC_LITERAL(30, 333, 12), // "move_time_ms"
QT_MOC_LITERAL(31, 346, 12), // "moveCameraTo"
QT_MOC_LITERAL(32, 359, 3), // "hat"
QT_MOC_LITERAL(33, 363, 3), // "rad"
QT_MOC_LITERAL(34, 367, 21), // "searchForNearbyObject"
QT_MOC_LITERAL(35, 389, 17), // "requestFlightInfo"
QT_MOC_LITERAL(36, 407, 16), // "setLanguageOnMap"
QT_MOC_LITERAL(37, 424, 2), // "la"
QT_MOC_LITERAL(38, 427, 5), // "laOld"
QT_MOC_LITERAL(39, 433, 18), // "setSave_images_top"
QT_MOC_LITERAL(40, 452, 3), // "low"
QT_MOC_LITERAL(41, 456, 3), // "med"
QT_MOC_LITERAL(42, 460, 2), // "hi"
QT_MOC_LITERAL(43, 463, 7), // "pCamera"
QT_MOC_LITERAL(44, 471, 7), // "mCamera"
QT_MOC_LITERAL(45, 479, 11), // "myClearPath"
QT_MOC_LITERAL(46, 491, 14), // "onConfigLoaded"
QT_MOC_LITERAL(47, 506, 11), // "onMoveTimer"
QT_MOC_LITERAL(48, 518, 10), // "onRotTimer"
QT_MOC_LITERAL(49, 529, 13), // "onHeightTimer"
QT_MOC_LITERAL(50, 543, 19), // "onManualToAutoTimer"
QT_MOC_LITERAL(51, 563, 26), // "onLastFlightInfoCheckTimer"
QT_MOC_LITERAL(52, 590, 13), // "onManualEvent"
QT_MOC_LITERAL(53, 604, 22), // "onFrameEventsProcessed"
QT_MOC_LITERAL(54, 627, 12), // "onFrameDrawn"
QT_MOC_LITERAL(55, 640, 25), // "QOpenGLFramebufferObject*"
QT_MOC_LITERAL(56, 666, 2), // "fb"
QT_MOC_LITERAL(57, 669, 23), // "applyCoordinatesOnScene"
QT_MOC_LITERAL(58, 693, 18), // "loadFeatureObjects"
QT_MOC_LITERAL(59, 712, 20), // "requestFlightArchive"
QT_MOC_LITERAL(60, 733, 14), // "request_config"
QT_MOC_LITERAL(61, 748, 7), // "address"
QT_MOC_LITERAL(62, 756, 10), // "earth_file"
QT_MOC_LITERAL(63, 767, 13), // "request_earth"
QT_MOC_LITERAL(64, 781, 21), // "earth_request_address"
QT_MOC_LITERAL(65, 803, 12), // "display_mode"
QT_MOC_LITERAL(66, 816, 19), // "manual_mode_timeout"
QT_MOC_LITERAL(67, 836, 26), // "min_camera_altitude_manual"
QT_MOC_LITERAL(68, 863, 26), // "max_camera_altitude_manual"
QT_MOC_LITERAL(69, 890, 22), // "save_images_stack_size"
QT_MOC_LITERAL(70, 913, 18), // "needTakeScreenshot"
QT_MOC_LITERAL(71, 932, 20), // "needTake3Screenshots"
QT_MOC_LITERAL(72, 953, 21), // "needTakeGUIScreenshot"
QT_MOC_LITERAL(73, 975, 22), // "needTakeTopScreenshots"
QT_MOC_LITERAL(74, 998, 16), // "save_images_path"
QT_MOC_LITERAL(75, 1015, 8), // "language"
QT_MOC_LITERAL(76, 1024, 23), // "request_flight_info_com"
QT_MOC_LITERAL(77, 1048, 8), // "com_port"
QT_MOC_LITERAL(78, 1057, 23), // "request_flight_info_web"
QT_MOC_LITERAL(79, 1081, 11), // "web_address"
QT_MOC_LITERAL(80, 1093, 22), // "request_flight_archive"
QT_MOC_LITERAL(81, 1116, 30), // "flight_archive_request_address"
QT_MOC_LITERAL(82, 1147, 26), // "flight_infowindow_interval"
QT_MOC_LITERAL(83, 1174, 26), // "flight_infowindow_showtime"
QT_MOC_LITERAL(84, 1201, 19), // "max_object_distance"
QT_MOC_LITERAL(85, 1221, 21), // "objects_features_name"
QT_MOC_LITERAL(86, 1243, 17), // "objects_photo_dir"
QT_MOC_LITERAL(87, 1261, 16), // "objects_text_dir"
QT_MOC_LITERAL(88, 1278, 10), // "model_path"
QT_MOC_LITERAL(89, 1289, 11), // "trace_color"
QT_MOC_LITERAL(90, 1301, 19) // "fixed_flight_height"

    },
    "EarthWidget\0needAirplaneModel\0\0error\0"
    "msg\0newObjectInfo\0objCaption\0objDescr\0"
    "objImgFile\0newFlightInfo\0Lng\0Lat\0Alt\0"
    "MoscowDateTime\0TimeToDest\0KmToDest\0"
    "Temp\0Speed\0moveAirplaneAnimationDone\0"
    "display_modeChanged\0request_flight_info_comChanged\0"
    "request_flight_info_webChanged\0"
    "setNewCoordinatesOnScene\0lon\0lat\0alt\0"
    "azim\0setArchivePath\0ar\0moveTo\0"
    "move_time_ms\0moveCameraTo\0hat\0rad\0"
    "searchForNearbyObject\0requestFlightInfo\0"
    "setLanguageOnMap\0la\0laOld\0setSave_images_top\0"
    "low\0med\0hi\0pCamera\0mCamera\0myClearPath\0"
    "onConfigLoaded\0onMoveTimer\0onRotTimer\0"
    "onHeightTimer\0onManualToAutoTimer\0"
    "onLastFlightInfoCheckTimer\0onManualEvent\0"
    "onFrameEventsProcessed\0onFrameDrawn\0"
    "QOpenGLFramebufferObject*\0fb\0"
    "applyCoordinatesOnScene\0loadFeatureObjects\0"
    "requestFlightArchive\0request_config\0"
    "address\0earth_file\0request_earth\0"
    "earth_request_address\0display_mode\0"
    "manual_mode_timeout\0min_camera_altitude_manual\0"
    "max_camera_altitude_manual\0"
    "save_images_stack_size\0needTakeScreenshot\0"
    "needTake3Screenshots\0needTakeGUIScreenshot\0"
    "needTakeTopScreenshots\0save_images_path\0"
    "language\0request_flight_info_com\0"
    "com_port\0request_flight_info_web\0"
    "web_address\0request_flight_archive\0"
    "flight_archive_request_address\0"
    "flight_infowindow_interval\0"
    "flight_infowindow_showtime\0"
    "max_object_distance\0objects_features_name\0"
    "objects_photo_dir\0objects_text_dir\0"
    "model_path\0trace_color\0fixed_flight_height"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_EarthWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      33,   14, // methods
      29,  280, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,  179,    2, 0x06 /* Public */,
       3,    1,  180,    2, 0x06 /* Public */,
       5,    3,  183,    2, 0x06 /* Public */,
       9,    8,  190,    2, 0x06 /* Public */,
      18,    0,  207,    2, 0x06 /* Public */,
      19,    0,  208,    2, 0x06 /* Public */,
      20,    0,  209,    2, 0x06 /* Public */,
      21,    0,  210,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      22,    3,  211,    2, 0x0a /* Public */,
      22,    4,  218,    2, 0x0a /* Public */,
      27,    1,  227,    2, 0x0a /* Public */,
      29,    4,  230,    2, 0x0a /* Public */,
      31,    3,  239,    2, 0x0a /* Public */,
      34,    0,  246,    2, 0x0a /* Public */,
      35,    0,  247,    2, 0x0a /* Public */,
      36,    2,  248,    2, 0x0a /* Public */,
      39,    3,  253,    2, 0x0a /* Public */,
      43,    0,  260,    2, 0x0a /* Public */,
      44,    0,  261,    2, 0x0a /* Public */,
      45,    0,  262,    2, 0x0a /* Public */,
      46,    0,  263,    2, 0x0a /* Public */,
      47,    0,  264,    2, 0x08 /* Private */,
      48,    0,  265,    2, 0x08 /* Private */,
      49,    0,  266,    2, 0x08 /* Private */,
      50,    0,  267,    2, 0x08 /* Private */,
      51,    0,  268,    2, 0x08 /* Private */,
      52,    0,  269,    2, 0x08 /* Private */,
      53,    0,  270,    2, 0x08 /* Private */,
      54,    1,  271,    2, 0x08 /* Private */,
      57,    0,  274,    2, 0x08 /* Private */,
      58,    0,  275,    2, 0x08 /* Private */,
      59,    0,  276,    2, 0x08 /* Private */,

 // methods: name, argc, parameters, tag, flags
      60,    1,  277,    2, 0x02 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString,    6,    7,    8,
    QMetaType::Void, QMetaType::Double, QMetaType::Double, QMetaType::Double, QMetaType::QDateTime, QMetaType::QString, QMetaType::Double, QMetaType::Double, QMetaType::Double,   10,   11,   12,   13,   14,   15,   16,   17,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Double, QMetaType::Double, QMetaType::Double,   23,   24,   25,
    QMetaType::Void, QMetaType::Double, QMetaType::Double, QMetaType::Double, QMetaType::Double,   23,   24,   25,   26,
    QMetaType::Void, QMetaType::QVariantList,   28,
    QMetaType::Void, QMetaType::Double, QMetaType::Double, QMetaType::Double, QMetaType::Int,   23,   24,   25,   30,
    QMetaType::Void, QMetaType::Double, QMetaType::Double, QMetaType::Int,   32,   33,   30,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   37,   38,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int,   40,   41,   42,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 55,   56,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // methods: parameters
    QMetaType::QString, QMetaType::QString,   61,

 // properties: name, type, flags
      62, QMetaType::QString, 0x00095003,
      63, QMetaType::Bool, 0x00095003,
      64, QMetaType::QString, 0x00095003,
      65, QMetaType::QString, 0x00495103,
      66, QMetaType::Int, 0x00095103,
      67, QMetaType::Double, 0x00095103,
      68, QMetaType::Double, 0x00095103,
      69, QMetaType::Double, 0x00095003,
      70, QMetaType::Bool, 0x00095003,
      71, QMetaType::Bool, 0x00095003,
      72, QMetaType::Bool, 0x00095003,
      73, QMetaType::Bool, 0x00095003,
      74, QMetaType::QString, 0x00095003,
      75, QMetaType::QString, 0x00095103,
      76, QMetaType::Bool, 0x00495103,
      77, QMetaType::QString, 0x00095103,
      78, QMetaType::Bool, 0x00495103,
      79, QMetaType::QString, 0x00095103,
      80, QMetaType::Bool, 0x00095003,
      81, QMetaType::QString, 0x00095003,
      82, QMetaType::Int, 0x00095003,
      83, QMetaType::Int, 0x00095003,
      84, QMetaType::Double, 0x00095003,
      85, QMetaType::QString, 0x00095003,
      86, QMetaType::QString, 0x00095003,
      87, QMetaType::QString, 0x00095003,
      88, QMetaType::QString, 0x00095103,
      89, QMetaType::QString, 0x00095103,
      90, QMetaType::Int, 0x00095003,

 // properties: notify_signal_id
       0,
       0,
       0,
       5,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       6,
       0,
       7,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,

       0        // eod
};

void EarthWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        EarthWidget *_t = static_cast<EarthWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->needAirplaneModel(); break;
        case 1: _t->error((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->newObjectInfo((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 3: _t->newFlightInfo((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3])),(*reinterpret_cast< const QDateTime(*)>(_a[4])),(*reinterpret_cast< const QString(*)>(_a[5])),(*reinterpret_cast< double(*)>(_a[6])),(*reinterpret_cast< double(*)>(_a[7])),(*reinterpret_cast< double(*)>(_a[8]))); break;
        case 4: _t->moveAirplaneAnimationDone(); break;
        case 5: _t->display_modeChanged(); break;
        case 6: _t->request_flight_info_comChanged(); break;
        case 7: _t->request_flight_info_webChanged(); break;
        case 8: _t->setNewCoordinatesOnScene((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3]))); break;
        case 9: _t->setNewCoordinatesOnScene((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3])),(*reinterpret_cast< double(*)>(_a[4]))); break;
        case 10: _t->setArchivePath((*reinterpret_cast< const QVariantList(*)>(_a[1]))); break;
        case 11: _t->moveTo((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 12: _t->moveCameraTo((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 13: _t->searchForNearbyObject(); break;
        case 14: _t->requestFlightInfo(); break;
        case 15: _t->setLanguageOnMap((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 16: _t->setSave_images_top((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 17: _t->pCamera(); break;
        case 18: _t->mCamera(); break;
        case 19: _t->myClearPath(); break;
        case 20: _t->onConfigLoaded(); break;
        case 21: _t->onMoveTimer(); break;
        case 22: _t->onRotTimer(); break;
        case 23: _t->onHeightTimer(); break;
        case 24: _t->onManualToAutoTimer(); break;
        case 25: _t->onLastFlightInfoCheckTimer(); break;
        case 26: _t->onManualEvent(); break;
        case 27: _t->onFrameEventsProcessed(); break;
        case 28: _t->onFrameDrawn((*reinterpret_cast< QOpenGLFramebufferObject*(*)>(_a[1]))); break;
        case 29: _t->applyCoordinatesOnScene(); break;
        case 30: _t->loadFeatureObjects(); break;
        case 31: _t->requestFlightArchive(); break;
        case 32: { QString _r = _t->request_config((*reinterpret_cast< QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (EarthWidget::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&EarthWidget::needAirplaneModel)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (EarthWidget::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&EarthWidget::error)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (EarthWidget::*_t)(const QString & , const QString & , const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&EarthWidget::newObjectInfo)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (EarthWidget::*_t)(double , double , double , const QDateTime & , const QString & , double , double , double );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&EarthWidget::newFlightInfo)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (EarthWidget::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&EarthWidget::moveAirplaneAnimationDone)) {
                *result = 4;
                return;
            }
        }
        {
            typedef void (EarthWidget::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&EarthWidget::display_modeChanged)) {
                *result = 5;
                return;
            }
        }
        {
            typedef void (EarthWidget::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&EarthWidget::request_flight_info_comChanged)) {
                *result = 6;
                return;
            }
        }
        {
            typedef void (EarthWidget::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&EarthWidget::request_flight_info_webChanged)) {
                *result = 7;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        EarthWidget *_t = static_cast<EarthWidget *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = _t->mEarth_file; break;
        case 1: *reinterpret_cast< bool*>(_v) = _t->mRequest_earth; break;
        case 2: *reinterpret_cast< QString*>(_v) = _t->mEarth_request_address; break;
        case 3: *reinterpret_cast< QString*>(_v) = _t->display_mode(); break;
        case 4: *reinterpret_cast< int*>(_v) = _t->mManual_mode_timeout; break;
        case 5: *reinterpret_cast< double*>(_v) = _t->mMin_camera_altitude_manual; break;
        case 6: *reinterpret_cast< double*>(_v) = _t->mMax_camera_altitude_manual; break;
        case 7: *reinterpret_cast< double*>(_v) = _t->mSave_images_stack_size; break;
        case 8: *reinterpret_cast< bool*>(_v) = _t->mNeedTakeScreenshot; break;
        case 9: *reinterpret_cast< bool*>(_v) = _t->mNeedTake3Screenshots; break;
        case 10: *reinterpret_cast< bool*>(_v) = _t->mNeedTakeGUIScreenshot; break;
        case 11: *reinterpret_cast< bool*>(_v) = _t->mNeedTakeTopScreenshots; break;
        case 12: *reinterpret_cast< QString*>(_v) = _t->mSave_images_path; break;
        case 13: *reinterpret_cast< QString*>(_v) = _t->mLanguage; break;
        case 14: *reinterpret_cast< bool*>(_v) = _t->mRequest_flight_info_com; break;
        case 15: *reinterpret_cast< QString*>(_v) = _t->mCom_port; break;
        case 16: *reinterpret_cast< bool*>(_v) = _t->mRequest_flight_info_web; break;
        case 17: *reinterpret_cast< QString*>(_v) = _t->mWeb_address; break;
        case 18: *reinterpret_cast< bool*>(_v) = _t->mRequest_flight_archive; break;
        case 19: *reinterpret_cast< QString*>(_v) = _t->mFlight_archive_request_address; break;
        case 20: *reinterpret_cast< int*>(_v) = _t->mFlight_infowindow_interval; break;
        case 21: *reinterpret_cast< int*>(_v) = _t->mFlight_infowindow_showtime; break;
        case 22: *reinterpret_cast< double*>(_v) = _t->mMax_object_distance; break;
        case 23: *reinterpret_cast< QString*>(_v) = _t->mObjects_features_name; break;
        case 24: *reinterpret_cast< QString*>(_v) = _t->mObjects_photo_dir; break;
        case 25: *reinterpret_cast< QString*>(_v) = _t->mObjects_text_dir; break;
        case 26: *reinterpret_cast< QString*>(_v) = _t->mModel_path; break;
        case 27: *reinterpret_cast< QString*>(_v) = _t->mTrace_color; break;
        case 28: *reinterpret_cast< int*>(_v) = _t->mFixed_flight_height; break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        EarthWidget *_t = static_cast<EarthWidget *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0:
            if (_t->mEarth_file != *reinterpret_cast< QString*>(_v)) {
                _t->mEarth_file = *reinterpret_cast< QString*>(_v);
            }
            break;
        case 1:
            if (_t->mRequest_earth != *reinterpret_cast< bool*>(_v)) {
                _t->mRequest_earth = *reinterpret_cast< bool*>(_v);
            }
            break;
        case 2:
            if (_t->mEarth_request_address != *reinterpret_cast< QString*>(_v)) {
                _t->mEarth_request_address = *reinterpret_cast< QString*>(_v);
            }
            break;
        case 3: _t->setDisplay_mode(*reinterpret_cast< QString*>(_v)); break;
        case 4: _t->setManual_mode_timeout(*reinterpret_cast< int*>(_v)); break;
        case 5: _t->setMin_camera_altitude_manual(*reinterpret_cast< double*>(_v)); break;
        case 6: _t->setMax_camera_altitude_manual(*reinterpret_cast< double*>(_v)); break;
        case 7:
            if (_t->mSave_images_stack_size != *reinterpret_cast< double*>(_v)) {
                _t->mSave_images_stack_size = *reinterpret_cast< double*>(_v);
            }
            break;
        case 8:
            if (_t->mNeedTakeScreenshot != *reinterpret_cast< bool*>(_v)) {
                _t->mNeedTakeScreenshot = *reinterpret_cast< bool*>(_v);
            }
            break;
        case 9:
            if (_t->mNeedTake3Screenshots != *reinterpret_cast< bool*>(_v)) {
                _t->mNeedTake3Screenshots = *reinterpret_cast< bool*>(_v);
            }
            break;
        case 10:
            if (_t->mNeedTakeGUIScreenshot != *reinterpret_cast< bool*>(_v)) {
                _t->mNeedTakeGUIScreenshot = *reinterpret_cast< bool*>(_v);
            }
            break;
        case 11:
            if (_t->mNeedTakeTopScreenshots != *reinterpret_cast< bool*>(_v)) {
                _t->mNeedTakeTopScreenshots = *reinterpret_cast< bool*>(_v);
            }
            break;
        case 12:
            if (_t->mSave_images_path != *reinterpret_cast< QString*>(_v)) {
                _t->mSave_images_path = *reinterpret_cast< QString*>(_v);
            }
            break;
        case 13: _t->setLanguage(*reinterpret_cast< QString*>(_v)); break;
        case 14: _t->setRequest_flight_info_com(*reinterpret_cast< bool*>(_v)); break;
        case 15: _t->setCom_port(*reinterpret_cast< QString*>(_v)); break;
        case 16: _t->setRequest_flight_info_web(*reinterpret_cast< bool*>(_v)); break;
        case 17: _t->setWeb_address(*reinterpret_cast< QString*>(_v)); break;
        case 18:
            if (_t->mRequest_flight_archive != *reinterpret_cast< bool*>(_v)) {
                _t->mRequest_flight_archive = *reinterpret_cast< bool*>(_v);
            }
            break;
        case 19:
            if (_t->mFlight_archive_request_address != *reinterpret_cast< QString*>(_v)) {
                _t->mFlight_archive_request_address = *reinterpret_cast< QString*>(_v);
            }
            break;
        case 20:
            if (_t->mFlight_infowindow_interval != *reinterpret_cast< int*>(_v)) {
                _t->mFlight_infowindow_interval = *reinterpret_cast< int*>(_v);
            }
            break;
        case 21:
            if (_t->mFlight_infowindow_showtime != *reinterpret_cast< int*>(_v)) {
                _t->mFlight_infowindow_showtime = *reinterpret_cast< int*>(_v);
            }
            break;
        case 22:
            if (_t->mMax_object_distance != *reinterpret_cast< double*>(_v)) {
                _t->mMax_object_distance = *reinterpret_cast< double*>(_v);
            }
            break;
        case 23:
            if (_t->mObjects_features_name != *reinterpret_cast< QString*>(_v)) {
                _t->mObjects_features_name = *reinterpret_cast< QString*>(_v);
            }
            break;
        case 24:
            if (_t->mObjects_photo_dir != *reinterpret_cast< QString*>(_v)) {
                _t->mObjects_photo_dir = *reinterpret_cast< QString*>(_v);
            }
            break;
        case 25:
            if (_t->mObjects_text_dir != *reinterpret_cast< QString*>(_v)) {
                _t->mObjects_text_dir = *reinterpret_cast< QString*>(_v);
            }
            break;
        case 26: _t->setModel_path(*reinterpret_cast< QString*>(_v)); break;
        case 27: _t->setTrace_color(*reinterpret_cast< QString*>(_v)); break;
        case 28:
            if (_t->mFixed_flight_height != *reinterpret_cast< int*>(_v)) {
                _t->mFixed_flight_height = *reinterpret_cast< int*>(_v);
            }
            break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject EarthWidget::staticMetaObject = {
    { &osgQtQuick::OSGViewport::staticMetaObject, qt_meta_stringdata_EarthWidget.data,
      qt_meta_data_EarthWidget,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *EarthWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *EarthWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_EarthWidget.stringdata0))
        return static_cast<void*>(const_cast< EarthWidget*>(this));
    return osgQtQuick::OSGViewport::qt_metacast(_clname);
}

int EarthWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = osgQtQuick::OSGViewport::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 33)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 33;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 33)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 33;
    }
#ifndef QT_NO_PROPERTIES
   else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 29;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 29;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 29;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 29;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 29;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 29;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void EarthWidget::needAirplaneModel()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void EarthWidget::error(const QString & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void EarthWidget::newObjectInfo(const QString & _t1, const QString & _t2, const QString & _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void EarthWidget::newFlightInfo(double _t1, double _t2, double _t3, const QDateTime & _t4, const QString & _t5, double _t6, double _t7, double _t8)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)), const_cast<void*>(reinterpret_cast<const void*>(&_t6)), const_cast<void*>(reinterpret_cast<const void*>(&_t7)), const_cast<void*>(reinterpret_cast<const void*>(&_t8)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void EarthWidget::moveAirplaneAnimationDone()
{
    QMetaObject::activate(this, &staticMetaObject, 4, Q_NULLPTR);
}

// SIGNAL 5
void EarthWidget::display_modeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, Q_NULLPTR);
}

// SIGNAL 6
void EarthWidget::request_flight_info_comChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 6, Q_NULLPTR);
}

// SIGNAL 7
void EarthWidget::request_flight_info_webChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 7, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
