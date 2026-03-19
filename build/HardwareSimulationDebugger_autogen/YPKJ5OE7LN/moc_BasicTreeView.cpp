/****************************************************************************
** Meta object code from reading C++ file 'BasicTreeView.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/ui/BasicTreeView.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'BasicTreeView.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSBasicTreeViewENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSBasicTreeViewENDCLASS = QtMocHelpers::stringData(
    "BasicTreeView",
    "mShowContextMenu",
    "",
    "xPosition",
    "mRunExpandLoaded",
    "mRunCollapseLoaded",
    "mRunFetchCurrentBranch",
    "mOnNodeExpanded",
    "QModelIndex",
    "xIndex"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSBasicTreeViewENDCLASS_t {
    uint offsetsAndSizes[20];
    char stringdata0[14];
    char stringdata1[17];
    char stringdata2[1];
    char stringdata3[10];
    char stringdata4[17];
    char stringdata5[19];
    char stringdata6[23];
    char stringdata7[16];
    char stringdata8[12];
    char stringdata9[7];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSBasicTreeViewENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSBasicTreeViewENDCLASS_t qt_meta_stringdata_CLASSBasicTreeViewENDCLASS = {
    {
        QT_MOC_LITERAL(0, 13),  // "BasicTreeView"
        QT_MOC_LITERAL(14, 16),  // "mShowContextMenu"
        QT_MOC_LITERAL(31, 0),  // ""
        QT_MOC_LITERAL(32, 9),  // "xPosition"
        QT_MOC_LITERAL(42, 16),  // "mRunExpandLoaded"
        QT_MOC_LITERAL(59, 18),  // "mRunCollapseLoaded"
        QT_MOC_LITERAL(78, 22),  // "mRunFetchCurrentBranch"
        QT_MOC_LITERAL(101, 15),  // "mOnNodeExpanded"
        QT_MOC_LITERAL(117, 11),  // "QModelIndex"
        QT_MOC_LITERAL(129, 6)   // "xIndex"
    },
    "BasicTreeView",
    "mShowContextMenu",
    "",
    "xPosition",
    "mRunExpandLoaded",
    "mRunCollapseLoaded",
    "mRunFetchCurrentBranch",
    "mOnNodeExpanded",
    "QModelIndex",
    "xIndex"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSBasicTreeViewENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   44,    2, 0x08,    1 /* Private */,
       4,    0,   47,    2, 0x08,    3 /* Private */,
       5,    0,   48,    2, 0x08,    4 /* Private */,
       6,    0,   49,    2, 0x08,    5 /* Private */,
       7,    1,   50,    2, 0x08,    6 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::QPoint,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 8,    9,

       0        // eod
};

Q_CONSTINIT const QMetaObject BasicTreeView::staticMetaObject = { {
    QMetaObject::SuperData::link<QTreeView::staticMetaObject>(),
    qt_meta_stringdata_CLASSBasicTreeViewENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSBasicTreeViewENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSBasicTreeViewENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<BasicTreeView, std::true_type>,
        // method 'mShowContextMenu'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QPoint &, std::false_type>,
        // method 'mRunExpandLoaded'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'mRunCollapseLoaded'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'mRunFetchCurrentBranch'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'mOnNodeExpanded'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QModelIndex &, std::false_type>
    >,
    nullptr
} };

void BasicTreeView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<BasicTreeView *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->mShowContextMenu((*reinterpret_cast< std::add_pointer_t<QPoint>>(_a[1]))); break;
        case 1: _t->mRunExpandLoaded(); break;
        case 2: _t->mRunCollapseLoaded(); break;
        case 3: _t->mRunFetchCurrentBranch(); break;
        case 4: _t->mOnNodeExpanded((*reinterpret_cast< std::add_pointer_t<QModelIndex>>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject *BasicTreeView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *BasicTreeView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSBasicTreeViewENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QTreeView::qt_metacast(_clname);
}

int BasicTreeView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTreeView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 5;
    }
    return _id;
}
QT_WARNING_POP
