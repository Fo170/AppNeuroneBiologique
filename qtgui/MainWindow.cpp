#include "MainWindow.hpp"
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QToolBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QPushButton>
#include <QLabel>
#include <QCoreApplication>
#include <QDir>

static QString exemples_dir(const char* sous) {
    QString dir = QCoreApplication::applicationDirPath();
    // Navigate from build/ to exemples/
    if (dir.endsWith('/')) dir.chop(1);
    if (dir.endsWith("build"))
        return dir + "/../exemples/" + sous;
    return dir + "/../exemples/" + sous;
}

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Éditeur de Réseau de Neurones Biologiques");
    resize(1200, 700);

    scene_ = new GraphScene(&reseau_, this);
    view_ = new QGraphicsView(scene_);
    view_->setRenderHint(QPainter::Antialiasing);
    view_->setDragMode(QGraphicsView::ScrollHandDrag);
    view_->setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);

    panel_ = new PropertyPanel;
    panel_->afficher_dataset(&dataset_);

    auto* splitter = new QSplitter;
    splitter->addWidget(view_);
    splitter->addWidget(panel_);
    splitter->setSizes({900, 300}); // 75% / 25%
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 1);
    setCentralWidget(splitter);

    connect(scene_, &GraphScene::neurone_selectionne, this, &MainWindow::on_neurone_selectionne);
    connect(scene_, &GraphScene::synapse_selectionne, this, &MainWindow::on_synapse_selectionne);
    connect(scene_, &GraphScene::selection_vide, this, &MainWindow::on_selection_vide);
    connect(panel_, &PropertyPanel::neurone_modifie, this, &MainWindow::on_neurone_modifie);
    connect(panel_, &PropertyPanel::synapse_modifie, this, &MainWindow::on_synapse_modifie);
    connect(panel_, &PropertyPanel::stop_simulation, this, &MainWindow::arreter_simulation);

    // Menu
    auto* mf = menuBar()->addMenu("&Fichier");
    auto* a_sn = mf->addAction("\U0001F4BE Sauvegarder le réseau");
    a_sn->setShortcut(QKeySequence("Ctrl+Shift+S"));
    connect(a_sn, &QAction::triggered, this, &MainWindow::sauvegarder_reseau);
    auto* a_ln = mf->addAction("\U0001F4C2 Charger un réseau");
    a_ln->setShortcut(QKeySequence("Ctrl+Shift+O"));
    connect(a_ln, &QAction::triggered, this, &MainWindow::charger_reseau);
    mf->addSeparator();
    auto* a_sd = mf->addAction("\U0001F4BE Sauvegarder le dataset");
    a_sd->setShortcut(QKeySequence::Save);
    connect(a_sd, &QAction::triggered, this, [this]() {
        QString p = QFileDialog::getSaveFileName(this, "Sauvegarder dataset",
            exemples_dir("datasets"), "CSV (*.csv)");
        if (!p.isEmpty()) { dataset_.sauvegarder_csv(p); mettre_a_jour_status(); }
    });
    auto* a_ld = mf->addAction("\U0001F4C2 Charger un dataset");
    a_ld->setShortcut(QKeySequence::Open);
    connect(a_ld, &QAction::triggered, this, &MainWindow::charger_dataset);
    mf->addSeparator();
    auto* a_q = mf->addAction("Quitter");
    a_q->setShortcut(QKeySequence::Quit);
    connect(a_q, &QAction::triggered, this, &QWidget::close);

    auto* ms = menuBar()->addMenu("&Simulation");
    auto* a_sim = ms->addAction("\u25B6 Lancer la simulation (F5)");
    a_sim->setShortcut(QKeySequence("F5"));
    connect(a_sim, &QAction::triggered, this, &MainWindow::simuler);

    auto* me = menuBar()->addMenu("&Édition");
    auto* a_r = me->addAction("\u21BA R\u00E9initialiser l'\u00E9tat");
    connect(a_r, &QAction::triggered, this, &MainWindow::reinitialiser);
    auto* a_rand_m = me->addAction("\U0001F3B2 Randomiser les poids synaptiques");
    connect(a_rand_m, &QAction::triggered, this, [this]() {
        reseau_.randomiser_poids(-1.0f, 1.0f);
        scene_->reconstruire();
        mettre_a_jour_status();
    });

    // Toolbar
    auto* tb = addToolBar("Principale");
    tb->setIconSize(QSize(16, 16));

    auto* a_tb_sim = tb->addAction("\u25B6 Simuler (F5)");
    connect(a_tb_sim, &QAction::triggered, this, &MainWindow::simuler);

    tb->addSeparator();

    auto* a_tb_sn = tb->addAction("\U0001F4BE Sauver r\u00E9seau");
    connect(a_tb_sn, &QAction::triggered, this, &MainWindow::sauvegarder_reseau);
    auto* a_tb_ln = tb->addAction("\U0001F4C2 Charger r\u00E9seau");
    connect(a_tb_ln, &QAction::triggered, this, &MainWindow::charger_reseau);

    tb->addSeparator();

    auto* a_tb_ld = tb->addAction("\U0001F4C2 Dataset CSV");
    connect(a_tb_ld, &QAction::triggered, this, &MainWindow::charger_dataset);

    tb->addSeparator();

    auto* a_tb_r = tb->addAction("\u21BA Reset \u00E9tat");
    connect(a_tb_r, &QAction::triggered, this, &MainWindow::reinitialiser);

    auto* a_rand = tb->addAction("\U0001F3B2 Randomiser poids");
    connect(a_rand, &QAction::triggered, this, [this]() {
        reseau_.randomiser_poids(-1.0f, 1.0f);
        scene_->reconstruire();
        mettre_a_jour_status();
    });

    tb->addSeparator();
    tb->addWidget(new QLabel(" Max itérations:"));
    spin_epochs_ = new QSpinBox;
    spin_epochs_->setRange(1, 1000000);
    spin_epochs_->setValue(100);
    spin_epochs_->setSingleStep(10);
    spin_epochs_->setToolTip("Nombre maximal d'itérations d'apprentissage.\n"
                             "La simulation s'arrête automatiquement à ce nombre.");
    tb->addWidget(spin_epochs_);

    scene_->reconstruire();
    mettre_a_jour_status();
}

void MainWindow::on_neurone_selectionne(int id) {
    auto* n = reseau_.trouver_neurone(id);
    panel_->afficher_neurone(n);
    mettre_a_jour_status();
}

void MainWindow::on_synapse_selectionne(int id) {
    for (auto& s : reseau_.synapses)
        if (s.id == id) { panel_->afficher_synapse(&s); mettre_a_jour_status(); return; }
}

void MainWindow::on_selection_vide() {
    panel_->afficher_dataset(&dataset_);
    mettre_a_jour_status();
}

void MainWindow::on_neurone_modifie() { scene_->reconstruire(); mettre_a_jour_status(); }
void MainWindow::on_synapse_modifie() { scene_->reconstruire(); mettre_a_jour_status(); }

void MainWindow::simuler() {
    if (timer_simulation_ && timer_simulation_->isActive()) {
        arreter_simulation();
        return;
    }

    if (reseau_.neurones.empty()) {
        QMessageBox::information(this, "Simulation", "Construisez d'abord un réseau.");
        return;
    }
    if (dataset_.exemples.empty()) {
        QMessageBox::information(this, "Simulation", "Chargez d'abord un dataset.");
        return;
    }

    demarrer_simulation();
}

void MainWindow::demarrer_simulation() {
    epoch_actuel_ = 0;
    meilleure_erreur_ = 1e10f;
    panel_->reinitialiser_apprentissage();
    panel_->activer_onglet_apprentissage();

    if (!timer_simulation_) {
        timer_simulation_ = new QTimer(this);
        connect(timer_simulation_, &QTimer::timeout, this, &MainWindow::on_tick_simulation);
    }
    timer_simulation_->start(20);

    statusBar()->showMessage("Simulation en cours...");
}

void MainWindow::on_tick_simulation() {
    int batch = panel_->batch();
    float erreur = 0.0f;
    bool fini = false;
    for (int i = 0; i < batch; ++i) {
        if (epoch_actuel_ >= spin_epochs_->value()) { fini = true; break; }
        erreur = reseau_.simuler_un_epoch(dataset_, 1.0f, true);
        ++epoch_actuel_;
        if (erreur < meilleure_erreur_)
            meilleure_erreur_ = erreur;
        if (erreur < panel_->seuil_arret()) { fini = true; break; }
    }

    scene_->mettre_a_jour_poids();
    scene_->sync_sorties_depuis_reseau();
    panel_->ajouter_point_erreur(epoch_actuel_, erreur);
    panel_->set_meilleure_erreur(meilleure_erreur_);
    auto sorties = reseau_.calculer_sorties(dataset_, 1.0f);
    panel_->mettre_a_jour_sorties_dataset(sorties);

    if (!fini) return;
    if (epoch_actuel_ >= spin_epochs_->value())
        statusBar()->showMessage(QString("Terminé (%1 itérations, err=%2)")
            .arg(epoch_actuel_).arg(meilleure_erreur_, 0, 'f', 5));
    else
        statusBar()->showMessage(QString("Seuil atteint (err=%1)").arg(erreur, 0, 'f', 5));
    arreter_simulation();
}

void MainWindow::arreter_simulation() {
    if (timer_simulation_) timer_simulation_->stop();
    if (!statusBar()->currentMessage().startsWith("Terminé")
        && !statusBar()->currentMessage().startsWith("Seuil"))
        statusBar()->showMessage(
            QString("%1 itérations × %2 exemples. Meilleure erreur: %3")
                .arg(epoch_actuel_).arg(dataset_.exemples.size())
                .arg(meilleure_erreur_, 0, 'f', 5));
}

void MainWindow::sauvegarder_reseau() {
    QString p = QFileDialog::getSaveFileName(this, "Sauvegarder le réseau",
        exemples_dir("reseaux"), "CSV réseau (*.csv);;Tous (*)");
    if (p.isEmpty()) return;
    reseau_.sauvegarder_csv(p);
    mettre_a_jour_status();
}

void MainWindow::charger_reseau() {
    QString p = QFileDialog::getOpenFileName(this, "Charger un réseau",
        exemples_dir("reseaux"), "CSV réseau (*.csv);;Tous (*)");
    if (p.isEmpty()) return;
    reseau_.charger_csv(p);
    scene_->reconstruire();
    panel_->afficher_dataset(&dataset_);

    // Sync dataset nb_entrees with network
    int ni = reseau_.ids_entree().size();
    if (ni > 0) dataset_.nb_entrees = ni;

    mettre_a_jour_status();
}

void MainWindow::charger_dataset() {
    QString p = QFileDialog::getOpenFileName(this, "Charger un dataset",
        exemples_dir("datasets"), "CSV (*.csv);;Tous (*)");
    if (p.isEmpty()) return;
    if (!dataset_.charger_csv(p)) {
        QMessageBox::warning(this, "Erreur",
            "Impossible de charger le dataset.\n"
            "Vérifiez que le fichier est au format CSV valide (en-tête avec au moins 2 colonnes).");
        return;
    }

    // If network has inputs, sync
    int ni = reseau_.ids_entree().size();
    if (ni > 0 && dataset_.nb_entrees == 0)
        dataset_.nb_entrees = ni;

    panel_->afficher_dataset(&dataset_);
    mettre_a_jour_status();
}

void MainWindow::reinitialiser() {
    reseau_.reinitialiser_etat();
    for (auto& n : reseau_.neurones) {
        auto* node = scene_->trouver_noeud(n.id);
        if (node) node->set_sortie(0.0f);
    }
    scene_->mettre_a_jour_synapses();
    mettre_a_jour_status();
}

void MainWindow::mettre_a_jour_status() {
    QString msg = reseau_.resume();
    msg += "  |  " + dataset_.resume().replace('\n', ' ');
    auto sel = scene_->selectedItems();
    if (!sel.isEmpty()) {
        if (auto* n = dynamic_cast<NeuroneNode*>(sel.first()))
            msg += QString("  |  Sélection: #%1").arg(n->neurone_id());
        else if (auto* e = dynamic_cast<SynapseEdge*>(sel.first()))
            msg += QString("  |  Synapse #%1").arg(e->synapse_id());
    }
    statusBar()->showMessage(msg);
}
