/****************************************************************************
** Meta object code from reading C++ file 'GraphScene.hpp'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../GraphScene.hpp"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'GraphScene.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.4.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
namespace {
struct qt_meta_stringdata_GraphScene_t {
    uint offsetsAndSizes[28];
    char stringdata0[11];
    char stringdata1[20];
    char stringdata2[1];
    char stringdata3[11];
    char stringdata4[20];
    char stringdata5[11];
    char stringdata6[15];
    char stringdata7[20];
    char stringdata8[6];
    char stringdata9[10];
    char stringdata10[21];
    char stringdata11[14];
    char stringdata12[3];
    char stringdata13[4];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_GraphScene_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_GraphScene_t qt_meta_stringdata_GraphScene = {
    {
        QT_MOC_LITERAL(0, 10),  // "GraphScene"
        QT_MOC_LITERAL(11, 19),  // "neurone_selectionne"
        QT_MOC_LITERAL(31, 0),  // ""
        QT_MOC_LITERAL(32, 10),  // "neurone_id"
        QT_MOC_LITERAL(43, 19),  // "synapse_selectionne"
        QT_MOC_LITERAL(63, 10),  // "synapse_id"
        QT_MOC_LITERAL(74, 14),  // "selection_vide"
        QT_MOC_LITERAL(89, 19),  // "mode_liaison_change"
        QT_MOC_LITERAL(109, 5),  // "actif"
        QT_MOC_LITERAL(115, 9),  // "source_id"
        QT_MOC_LITERAL(125, 20),  // "on_selection_changed"
        QT_MOC_LITERAL(146, 13),  // "on_node_moved"
        QT_MOC_LITERAL(160, 2),  // "id"
        QT_MOC_LITERAL(163, 3)   // "pos"
    },
    "GraphScene",
    "neurone_selectionne",
    "",
    "neurone_id",
    "synapse_selectionne",
    "synapse_id",
    "selection_vide",
    "mode_liaison_change",
    "actif",
    "source_id",
    "on_selection_changed",
    "on_node_moved",
    "id",
    "pos"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_GraphScene[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   50,    2, 0x06,    1 /* Public */,
       4,    1,   53,    2, 0x06,    3 /* Public */,
       6,    0,   56,    2, 0x06,    5 /* Public */,
       7,    2,   57,    2, 0x06,    6 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      10,    0,   62,    2, 0x08,    9 /* Private */,
      11,    2,   63,    2, 0x08,   10 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool, QMetaType::Int,    8,    9,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::QPointF,   12,   13,

       0        // eod
};

Q_CONSTINIT const QMetaObject GraphScene::staticMetaObject = { {
    QMetaObject::SuperData::link<QGraphicsScene::staticMetaObject>(),
    qt_meta_stringdata_GraphScene.offsetsAndSizes,
    qt_meta_data_GraphScene,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_GraphScene_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<GraphScene, std::true_type>,
        // method 'neurone_selectionne'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'synapse_selectionne'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'selection_vide'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'mode_liaison_change'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_selection_changed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_node_moved'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<QPointF, std::false_type>
    >,
    nullptr
} };

void GraphScene::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<GraphScene *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->neurone_selectionne((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 1: _t->synapse_selectionne((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->selection_vide(); break;
        case 3: _t->mode_liaison_change((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 4: _t->on_selection_changed(); break;
        case 5: _t->on_node_moved((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QPointF>>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (GraphScene::*)(int );
            if (_t _q_method = &GraphScene::neurone_selectionne; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (GraphScene::*)(int );
            if (_t _q_method = &GraphScene::synapse_selectionne; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (GraphScene::*)();
            if (_t _q_method = &GraphScene::selection_vide; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (GraphScene::*)(bool , int );
            if (_t _q_method = &GraphScene::mode_liaison_change; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject *GraphScene::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GraphScene::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_GraphScene.stringdata0))
        return static_cast<void*>(this);
    return QGraphicsScene::qt_metacast(_clname);
}

int GraphScene::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsScene::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void GraphScene::neurone_selectionne(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void GraphScene::synapse_selectionne(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void GraphScene::selection_vide()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void GraphScene::mode_liaison_change(bool _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
