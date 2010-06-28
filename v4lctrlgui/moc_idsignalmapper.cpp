/****************************************************************************
** Meta object code from reading C++ file 'idsignalmapper.h'
**
** Created: Fri Apr 17 03:15:41 2009
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "idsignalmapper.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'idsignalmapper.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SliderWrapper[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      25,   15,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      77,   14,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_SliderWrapper[] = {
    "SliderWrapper\0\0id,newVal\0"
    "valueChangedId(unsigned long int,unsigned long int)\0"
    "valueChangedSlot(int)\0"
};

const QMetaObject SliderWrapper::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_SliderWrapper,
      qt_meta_data_SliderWrapper, 0 }
};

const QMetaObject *SliderWrapper::metaObject() const
{
    return &staticMetaObject;
}

void *SliderWrapper::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SliderWrapper))
        return static_cast<void*>(const_cast< SliderWrapper*>(this));
    return QObject::qt_metacast(_clname);
}

int SliderWrapper::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: valueChangedId((*reinterpret_cast< unsigned long int(*)>(_a[1])),(*reinterpret_cast< unsigned long int(*)>(_a[2]))); break;
        case 1: valueChangedSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void SliderWrapper::valueChangedId(unsigned long int _t1, unsigned long int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_CheckBoxWrapper[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      27,   17,   16,   16, 0x05,

 // slots: signature, parameters, type, tag, flags
      79,   16,   16,   16, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_CheckBoxWrapper[] = {
    "CheckBoxWrapper\0\0id,newVal\0"
    "valueChangedId(unsigned long int,unsigned long int)\0"
    "valueChangedSlot(int)\0"
};

const QMetaObject CheckBoxWrapper::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_CheckBoxWrapper,
      qt_meta_data_CheckBoxWrapper, 0 }
};

const QMetaObject *CheckBoxWrapper::metaObject() const
{
    return &staticMetaObject;
}

void *CheckBoxWrapper::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CheckBoxWrapper))
        return static_cast<void*>(const_cast< CheckBoxWrapper*>(this));
    return QObject::qt_metacast(_clname);
}

int CheckBoxWrapper::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: valueChangedId((*reinterpret_cast< unsigned long int(*)>(_a[1])),(*reinterpret_cast< unsigned long int(*)>(_a[2]))); break;
        case 1: valueChangedSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void CheckBoxWrapper::valueChangedId(unsigned long int _t1, unsigned long int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
