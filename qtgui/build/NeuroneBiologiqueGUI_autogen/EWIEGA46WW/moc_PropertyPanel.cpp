/****************************************************************************
** Meta object code from reading C++ file 'PropertyPanel.hpp'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../PropertyPanel.hpp"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PropertyPanel.hpp' doesn't include <QObject>."
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
struct qt_meta_stringdata_GraphiqueErreur_t {
    uint offsetsAndSizes[2];
    char stringdata0[16];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_GraphiqueErreur_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_GraphiqueErreur_t qt_meta_stringdata_GraphiqueErreur = {
    {
        QT_MOC_LITERAL(0, 15)   // "GraphiqueErreur"
    },
    "GraphiqueErreur"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_GraphiqueErreur[] = {

 // content:
      10,       // revision
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

Q_CONSTINIT const QMetaObject GraphiqueErreur::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_GraphiqueErreur.offsetsAndSizes,
    qt_meta_data_GraphiqueErreur,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_GraphiqueErreur_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<GraphiqueErreur, std::true_type>
    >,
    nullptr
} };

void GraphiqueErreur::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *GraphiqueErreur::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GraphiqueErreur::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_GraphiqueErreur.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int GraphiqueErreur::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_PropertyPanel_t {
    uint offsetsAndSizes[24];
    char stringdata0[14];
    char stringdata1[16];
    char stringdata2[1];
    char stringdata3[16];
    char stringdata4[16];
    char stringdata5[16];
    char stringdata6[21];
    char stringdata7[6];
    char stringdata8[7];
    char stringdata9[21];
    char stringdata10[2];
    char stringdata11[28];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_PropertyPanel_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_PropertyPanel_t qt_meta_stringdata_PropertyPanel = {
    {
        QT_MOC_LITERAL(0, 13),  // "PropertyPanel"
        QT_MOC_LITERAL(14, 15),  // "neurone_modifie"
        QT_MOC_LITERAL(30, 0),  // ""
        QT_MOC_LITERAL(31, 15),  // "synapse_modifie"
        QT_MOC_LITERAL(47, 15),  // "dataset_modifie"
        QT_MOC_LITERAL(63, 15),  // "stop_simulation"
        QT_MOC_LITERAL(79, 20),  // "ajouter_point_erreur"
        QT_MOC_LITERAL(100, 5),  // "epoch"
        QT_MOC_LITERAL(106, 6),  // "erreur"
        QT_MOC_LITERAL(113, 20),  // "set_meilleure_erreur"
        QT_MOC_LITERAL(134, 1),  // "v"
        QT_MOC_LITERAL(136, 27)   // "reinitialiser_apprentissage"
    },
    "PropertyPanel",
    "neurone_modifie",
    "",
    "synapse_modifie",
    "dataset_modifie",
    "stop_simulation",
    "ajouter_point_erreur",
    "epoch",
    "erreur",
    "set_meilleure_erreur",
    "v",
    "reinitialiser_apprentissage"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_PropertyPanel[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   56,    2, 0x06,    1 /* Public */,
       3,    0,   57,    2, 0x06,    2 /* Public */,
       4,    0,   58,    2, 0x06,    3 /* Public */,
       5,    0,   59,    2, 0x06,    4 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       6,    2,   60,    2, 0x0a,    5 /* Public */,
       9,    1,   65,    2, 0x0a,    8 /* Public */,
      11,    0,   68,    2, 0x0a,   10 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Double,    7,    8,
    QMetaType::Void, QMetaType::Double,   10,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject PropertyPanel::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_PropertyPanel.offsetsAndSizes,
    qt_meta_data_PropertyPanel,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_PropertyPanel_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<PropertyPanel, std::true_type>,
        // method 'neurone_modifie'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'synapse_modifie'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'dataset_modifie'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'stop_simulation'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'ajouter_point_erreur'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        // method 'set_meilleure_erreur'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        // method 'reinitialiser_apprentissage'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void PropertyPanel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PropertyPanel *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->neurone_modifie(); break;
        case 1: _t->synapse_modifie(); break;
        case 2: _t->dataset_modifie(); break;
        case 3: _t->stop_simulation(); break;
        case 4: _t->ajouter_point_erreur((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2]))); break;
        case 5: _t->set_meilleure_erreur((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 6: _t->reinitialiser_apprentissage(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (PropertyPanel::*)();
            if (_t _q_method = &PropertyPanel::neurone_modifie; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (PropertyPanel::*)();
            if (_t _q_method = &PropertyPanel::synapse_modifie; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (PropertyPanel::*)();
            if (_t _q_method = &PropertyPanel::dataset_modifie; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (PropertyPanel::*)();
            if (_t _q_method = &PropertyPanel::stop_simulation; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject *PropertyPanel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PropertyPanel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PropertyPanel.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int PropertyPanel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void PropertyPanel::neurone_modifie()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void PropertyPanel::synapse_modifie()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void PropertyPanel::dataset_modifie()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void PropertyPanel::stop_simulation()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
