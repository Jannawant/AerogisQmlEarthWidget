/****************************************************************************
** Meta object code from reading C++ file 'QuickWindowViewer.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/EarthWidget/QuickWindowViewer.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QuickWindowViewer.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_osgQtQuick__QuickWindowViewer_t {
    QByteArrayData data[5];
    char stringdata0[79];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_osgQtQuick__QuickWindowViewer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_osgQtQuick__QuickWindowViewer_t qt_meta_stringdata_osgQtQuick__QuickWindowViewer = {
    {
QT_MOC_LITERAL(0, 0, 29), // "osgQtQuick::QuickWindowViewer"
QT_MOC_LITERAL(1, 30, 15), // "eventsProcessed"
QT_MOC_LITERAL(2, 46, 0), // ""
QT_MOC_LITERAL(3, 47, 16), // "viewerDrawsFrame"
QT_MOC_LITERAL(4, 64, 14) // "AerogisViewer*"

    },
    "osgQtQuick::QuickWindowViewer\0"
    "eventsProcessed\0\0viewerDrawsFrame\0"
    "AerogisViewer*"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_osgQtQuick__QuickWindowViewer[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   24,    2, 0x06 /* Public */,
       3,    1,   25,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    2,

       0        // eod
};

void osgQtQuick::QuickWindowViewer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QuickWindowViewer *_t = static_cast<QuickWindowViewer *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->eventsProcessed(); break;
        case 1: _t->viewerDrawsFrame((*reinterpret_cast< AerogisViewer*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (QuickWindowViewer::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QuickWindowViewer::eventsProcessed)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (QuickWindowViewer::*_t)(AerogisViewer * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QuickWindowViewer::viewerDrawsFrame)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject osgQtQuick::QuickWindowViewer::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_osgQtQuick__QuickWindowViewer.data,
      qt_meta_data_osgQtQuick__QuickWindowViewer,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *osgQtQuick::QuickWindowViewer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *osgQtQuick::QuickWindowViewer::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_osgQtQuick__QuickWindowViewer.stringdata0))
        return static_cast<void*>(const_cast< QuickWindowViewer*>(this));
    return QObject::qt_metacast(_clname);
}

int osgQtQuick::QuickWindowViewer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void osgQtQuick::QuickWindowViewer::eventsProcessed()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void osgQtQuick::QuickWindowViewer::viewerDrawsFrame(AerogisViewer * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
