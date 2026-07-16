#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QGraphicsView>
#include <QSplitter>
#include <QSpinBox>
#include <QTimer>
#include "ReseauNeural.hpp"
#include "Dataset.hpp"
#include "GraphScene.hpp"
#include "PropertyPanel.hpp"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void on_neurone_selectionne(int id);
    void on_synapse_selectionne(int id);
    void on_selection_vide();
    void on_neurone_modifie();
    void on_synapse_modifie();
    void simuler();
    void on_tick_simulation();
    void sauvegarder_reseau();
    void charger_reseau();
    void charger_dataset();
    void reinitialiser();

private:
    void mettre_a_jour_status();
    void demarrer_simulation();
    void arreter_simulation();

    ReseauNeural reseau_;
    Dataset dataset_;
    GraphScene* scene_;
    QGraphicsView* view_;
    PropertyPanel* panel_;
    QSpinBox* spin_epochs_;
    QTimer* timer_simulation_ = nullptr;
    int epoch_actuel_ = 0;
    float meilleure_erreur_ = 1e10f;
};

#endif
