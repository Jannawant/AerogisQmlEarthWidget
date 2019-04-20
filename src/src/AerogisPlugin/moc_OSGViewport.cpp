/****************************************************************************
** Meta object code from reading C++ file 'OSGViewport.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../EarthWidget/OSGViewport.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'OSGViewport.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_osgQtQuick__OSGViewport_t {
    QByteArrayData data[20];
    char stringdata0[241];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_osgQtQuick__OSGViewport_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_osgQtQuick__OSGViewport_t qt_meta_stringdata_osgQtQuick__OSGViewport = {
    {
QT_MOC_LITERAL(0, 0, 23), // "osgQtQuick::OSGViewport"
QT_MOC_LITERAL(1, 24, 12), // "colorChanged"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 5), // "color"
QT_MOC_LITERAL(4, 44, 11), // "modeChanged"
QT_MOC_LITERAL(5, 56, 11), // "DrawingMode"
QT_MOC_LITERAL(6, 68, 4), // "mode"
QT_MOC_LITERAL(7, 73, 20), // "frameEventsProcessed"
QT_MOC_LITERAL(8, 94, 10), // "frameDrawn"
QT_MOC_LITERAL(9, 105, 25), // "QOpenGLFramebufferObject*"
QT_MOC_LITERAL(10, 131, 2), // "fb"
QT_MOC_LITERAL(11, 134, 14), // "ownCameraEvent"
QT_MOC_LITERAL(12, 149, 16), // "viewerDrawsFrame"
QT_MOC_LITERAL(13, 166, 14), // "AerogisViewer*"
QT_MOC_LITERAL(14, 181, 1), // "v"
QT_MOC_LITERAL(15, 183, 16), // "viewerDrawnFrame"
QT_MOC_LITERAL(16, 200, 12), // "myplusCamera"
QT_MOC_LITERAL(17, 213, 13), // "myminusCamera"
QT_MOC_LITERAL(18, 227, 6), // "Native"
QT_MOC_LITERAL(19, 234, 6) // "Buffer"

    },
    "osgQtQuick::OSGViewport\0colorChanged\0"
    "\0color\0modeChanged\0DrawingMode\0mode\0"
    "frameEventsProcessed\0frameDrawn\0"
    "QOpenGLFramebufferObject*\0fb\0"
    "ownCameraEvent\0viewerDrawsFrame\0"
    "AerogisViewer*\0v\0viewerDrawnFrame\0"
    "myplusCamera\0myminusCamera\0Native\0"
    "Buffer"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_osgQtQuick__OSGViewport[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       2,   76, // properties
       1,   84, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   59,    2, 0x06 /* Public */,
       4,    1,   62,    2, 0x06 /* Public */,
       7,    0,   65,    2, 0x06 /* Public */,
       8,    1,   66,    2, 0x06 /* Public */,
      11,    0,   69,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      12,    1,   70,    2, 0x0a /* Public */,
      15,    0,   73,    2, 0x0a /* Public */,
      16,    0,   74,    2, 0x0a /* Public */,
      17,    0,   75,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QColor,    3,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 9,   10,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 13,   14,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // properties: name, type, flags
       3, QMetaType::QColor, 0x00495103,
       6, 0x80000000 | 5, 0x0049510b,

 // properties: notify_signal_id
       0,
       1,

 // enums: name, flags, count, data
       5, 0x0,    2,   88,

 // enum data: key, value
      18, uint(osgQtQuick::OSGViewport::Native),
      19, uint(osgQtQuick::OSGViewport::Buffer),

       0        // eod
};

void osgQtQuick::OSGViewport::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        OSGViewport *_t = static_cast<OSGViewport *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->colorChanged((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 1: _t->modeChanged((*reinterpret_cast< DrawingMode(*)>(_a[1]))); break;
        case 2: _t->frameEventsProcessed(); break;
        case 3: _t->frameDrawn((*reinterpret_cast< QOpenGLFramebufferObject*(*)>(_a[1]))); break;
        case 4: _t->ownCameraEvent(); break;
        case 5: _t->viewerDrawsFrame((*reinterpret_cast< AerogisViewer*(*)>(_a[1]))); break;
        case 6: _t->viewerDrawnFrame(); break;
        case 7: _t->myplusCamera(); break;
        case 8: _t->myminusCamera(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (OSGViewport::*_t)(const QColor & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&OSGViewport::colorChanged)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (OSGViewport::*_t)(DrawingMode );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&OSGViewport::modeChanged)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (OSGViewport::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&OSGViewport::frameEventsProcessed)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (OSGViewport::*_t)(QOpenGLFramebufferObject * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&OSGViewport::frameDrawn)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (OSGViewport::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&OSGViewport::ownCameraEvent)) {
                *result = 4;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        OSGViewport *_t = static_cast<OSGViewport *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QColor*>(_v) = _t->color(); break;
        case 1: *reinterpret_cast< DrawingMode*>(_v) = _t->mode(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        OSGViewport *_t = static_cast<OSGViewport *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setColor(*reinterpret_cast< QColor*>(_v)); break;
        case 1: _t->setMode(*reinterpret_cast< DrawingMode*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject osgQtQuick::OSGViewport::staticMetaObject = {
    { &QQuickItem::staticMetaObject, qt_meta_stringdata_osgQtQuick__OSGViewport.data,
      qt_meta_data_osgQtQuick__OSGViewport,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *osgQtQuick::OSGViewport::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *osgQtQuick::OSGViewport::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_osgQtQuick__OSGViewport.stringdata0))
        return static_cast<void*>(const_cast< OSGViewport*>(this));
    return QQuickItem::qt_metacast(_clname);
}

int osgQtQuick::OSGViewport::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QQuickItem::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
#ifndef QT_NO_PROPERTIES
   else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void osgQtQuick::OSGViewport::colorChanged(const QColor & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void osgQtQuick::OSGViewport::modeChanged(DrawingMode _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void osgQtQuick::OSGViewport::frameEventsProcessed()
{
    QMetaObject::activate(this, &staticMetaObject, 2, Q_NULLPTR);
}

// SIGNAL 3
void osgQtQuick::OSGViewport::frameDrawn(QOpenGLFramebufferObject * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void osgQtQuick::OSGViewport::ownCameraEvent()
{
    QMetaObject::activate(this, &staticMetaObject, 4, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
