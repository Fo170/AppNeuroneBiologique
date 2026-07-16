/****************************************************************************
** Meta object code from reading C++ file 'MainWindow.hpp'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../MainWindow.hpp"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MainWindow.hpp' doesn't include <QObject>."
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
struct qt_meta_stringdata_MainWindow_t {
    uint offsetsAndSizes[28];
    char stringdata0[11];
    char stringdata1[23];
    char stringdata2[1];
    char stringdata3[3];
    char stringdata4[23];
    char stringdata5[18];
    char stringdata6[19];
    char stringdata7[19];
    char stringdata8[8];
    char stringdata9[19];
    char stringdata10[19];
    char stringdata11[15];
    char stringdata12[16];
    char stringdata13[14];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_MainWindow_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
        QT_MOC_LITERAL(0, 10),  // "MainWindow"
        QT_MOC_LITERAL(11, 22),  // "on_neurone_selectionne"
        QT_MOC_LITERAL(34, 0),  // ""
        QT_MOC_LITERAL(35, 2),  // "id"
        QT_MOC_LITERAL(38, 22),  // "on_synapse_selectionne"
        QT_MOC_LITERAL(61, 17),  // "on_selection_vide"
        QT_MOC_LITERAL(79, 18),  // "on_neurone_modifie"
        QT_MOC_LITERAL(98, 18),  // "on_synapse_modifie"
        QT_MOC_LITERAL(117, 7),  // "simuler"
        QT_MOC_LITERAL(125, 18),  // "on_tick_simulation"
        QT_MOC_LITERAL(144, 18),  // "sauvegarder_reseau"
        QT_MOC_LITERAL(163, 14),  // "charger_reseau"
        QT_MOC_LITERAL(178, 15),  // "charger_dataset"
        QT_MOC_LITERAL(194, 13)   // "reinitialiser"
    },
    "MainWindow",
    "on_neurone_selectionne",
    "",
    "id",
    "on_synapse_selectionne",
    "on_selection_vide",
    "on_neurone_modifie",
    "on_synapse_modifie",
    "simuler",
    "on_tick_simulation",
    "sauvegarder_reseau",
    "charger_reseau",
    "charger_dataset",
    "reinitialiser"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_MainWindow[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   80,    2, 0x08,    1 /* Private */,
       4,    1,   83,    2, 0x08,    3 /* Private */,
       5,    0,   86,    2, 0x08,    5 /* Private */,
       6,    0,   87,    2, 0x08,    6 /* Private */,
       7,    0,   88,    2, 0x08,    7 /* Private */,
       8,    0,   89,    2, 0x08,    8 /* Private */,
       9,    0,   90,    2, 0x08,    9 /* Private */,
      10,    0,   91,    2, 0x08,   10 /* Private */,
      11,    0,   92,    2, 0x08,   11 /* Private */,
      12,    0,   93,    2, 0x08,   12 /* Private */,
      13,    0,   94,    2, 0x08,   13 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_MainWindow.offsetsAndSizes,
    qt_meta_data_MainWindow,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_MainWindow_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MainWindow, std::true_type>,
        // method 'on_neurone_selectionne'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_synapse_selectionne'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_selection_vide'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_neurone_modifie'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_synapse_modifie'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'simuler'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_tick_simulation'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'sauvegarder_reseau'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'charger_reseau'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'charger_dataset'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'reinitialiser'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->on_neurone_selectionne((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 1: _t->on_synapse_selectionne((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->on_selection_vide(); break;
        case 3: _t->on_neurone_modifie(); break;
        case 4: _t->on_synapse_modifie(); break;
        case 5: _t->simuler(); break;
        case 6: _t->on_tick_simulation(); break;
        case 7: _t->sauvegarder_reseau(); break;
        case 8: _t->charger_reseau(); break;
        case 9: _t->charger_dataset(); break;
        case 10: _t->reinitialiser(); break;
        default: ;
        }
    }
}

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 11;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
