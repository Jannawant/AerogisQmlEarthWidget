/****************************************************************************
** Meta object code from reading C++ file 'OSGGroup.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../EarthWidget/OSGGroup.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'OSGGroup.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_osgQtQuick__OSGGroup_t {
    QByteArrayData data[4];
    char stringdata0[81];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_osgQtQuick__OSGGroup_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_osgQtQuick__OSGGroup_t qt_meta_stringdata_osgQtQuick__OSGGroup = {
    {
QT_MOC_LITERAL(0, 0, 20), // "osgQtQuick::OSGGroup"
QT_MOC_LITERAL(1, 21, 15), // "DefaultProperty"
QT_MOC_LITERAL(2, 37, 5), // "child"
QT_MOC_LITERAL(3, 43, 37) // "QQmlListProperty<osgQtQuick::..."

    },
    "osgQtQuick::OSGGroup\0DefaultProperty\0"
    "child\0QQmlListProperty<osgQtQuick::OSGNode>"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_osgQtQuick__OSGGroup[] = {

 // content:
       7,       // revision
       0,       // classname
       1,   14, // classinfo
       0,    0, // methods
       1,   16, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // classinfo: key, value
       1,    2,

 // properties: name, type, flags
       2, 0x80000000 | 3, 0x00095009,

       0        // eod
};

void osgQtQuick::OSGGroup::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{

#ifndef QT_NO_PROPERTIES
    if (_c == QMetaObject::ReadProperty) {
        OSGGroup *_t = static_cast<OSGGroup *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QQmlListProperty<osgQtQuick::OSGNode>*>(_v) = _t->child(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject osgQtQuick::OSGGroup::staticMetaObject = {
    { &OSGNode::staticMetaObject, qt_meta_stringdata_osgQtQuick__OSGGroup.data,
      qt_meta_data_osgQtQuick__OSGGroup,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *osgQtQuick::OSGGroup::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *osgQtQuick::OSGGroup::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_osgQtQuick__OSGGroup.stringdata0))
        return static_cast<void*>(const_cast< OSGGroup*>(this));
    return OSGNode::qt_metacast(_clname);
}

int osgQtQuick::OSGGroup::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = OSGNode::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
   if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
