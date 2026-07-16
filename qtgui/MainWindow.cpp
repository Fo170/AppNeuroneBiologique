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
#include <QInputDialog>
#include <QFileInfo>
#include <QMouseEvent>

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
    view_->setDragMode(QGraphicsView::RubberBandDrag);
    view_->setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
    view_->viewport()->installEventFilter(this);
    view_->setMouseTracking(true);

    panel_ = new PropertyPanel;
    panel_->afficher_dataset(&dataset_);

    palette_ = new ComponentPalette;
    palette_->setMinimumWidth(160);
    palette_->setMaximumWidth(220);

    auto* splitter = new QSplitter;
    splitter->addWidget(palette_);
    splitter->addWidget(view_);
    splitter->addWidget(panel_);
    splitter->setSizes({180, 720, 300});
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    splitter->setStretchFactor(2, 0);
    setCentralWidget(splitter);

    connect(scene_, &GraphScene::neurone_selectionne, this, &MainWindow::on_neurone_selectionne);
    connect(scene_, &GraphScene::synapse_selectionne, this, &MainWindow::on_synapse_selectionne);
    connect(scene_, &GraphScene::selection_vide, this, &MainWindow::on_selection_vide);
    connect(panel_, &PropertyPanel::neurone_modifie, this, &MainWindow::on_neurone_modifie);
    connect(panel_, &PropertyPanel::synapse_modifie, this, &MainWindow::on_synapse_modifie);
    connect(panel_, &PropertyPanel::stop_simulation, this, &MainWindow::arreter_simulation);

    scene_->set_dataset(&dataset_);
    connect(scene_, &GraphScene::scene_reconstruite, this, [this]() {
        if (!dataset_.exemples.empty())
            scene_->sync_colonnes_dataset(dataset_);
    });
    connect(scene_, &GraphScene::mapping_colonne_change, this, [this](int id, bool entree, int col) {
        auto* n = reseau_.trouver_neurone(id);
        if (!n) return;
        if (entree)  n->colonne_entree = col;
        else         n->colonne_sortie = col;
        scene_->sync_colonnes_dataset(dataset_);
    });
    connect(palette_, &ComponentPalette::component_selected, this, [this](const QVariantMap& data) {
        scene_->set_placement_mode(data);
    });
    connect(scene_, &GraphScene::component_place, this, &MainWindow::on_component_place);
    connect(scene_, &GraphScene::exporter_selection, this, &MainWindow::exporter_selection_module);

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
    mf->addSeparator();
    auto* a_im = mf->addAction("📦 Importer un module...");
    connect(a_im, &QAction::triggered, this, [this]() {
        QString p = QFileDialog::getOpenFileName(this, "Importer un module",
            exemples_dir("modules"), "Module (*.neuron *.json);;Tous (*)");
        if (p.isEmpty()) return;
        QString mod_dir = exemples_dir("modules");
        QDir().mkpath(mod_dir);
        QFile::copy(p, mod_dir + "/" + QFileInfo(p).fileName());
        palette_->scanner_modules(mod_dir);
    });

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
    auto* tb_sim_w = tb->widgetForAction(a_tb_sim);
    if (tb_sim_w) tb_sim_w->setStyleSheet(
        "QToolButton { color: #2e7d32; font-weight: bold; }");

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

    // Scan existing modules
    QString mod_dir = exemples_dir("modules");
    QDir().mkpath(mod_dir);
    palette_->scanner_modules(mod_dir);
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
    stop_demande_ = false;
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
        if (stop_demande_) { fini = true; break; }
        if (epoch_actuel_ >= spin_epochs_->value()) { fini = true; break; }
        erreur = reseau_.simuler_un_epoch(dataset_, 1.0f, true);
        ++epoch_actuel_;
        if (erreur < meilleure_erreur_)
            meilleure_erreur_ = erreur;
        if (erreur < panel_->seuil_arret()) { fini = true; break; }
    }

    if (!stop_demande_) {
        scene_->mettre_a_jour_poids();
        scene_->sync_sorties_depuis_reseau();
        panel_->ajouter_point_erreur(epoch_actuel_, erreur);
        panel_->set_meilleure_erreur(meilleure_erreur_);
        auto sorties = reseau_.calculer_sorties(dataset_, 1.0f);
        panel_->mettre_a_jour_sorties_dataset(sorties);
    }

    if (!fini) return;
    if (stop_demande_)
        statusBar()->showMessage(QString("Arrêté (%1 itérations, err=%2)")
            .arg(epoch_actuel_).arg(meilleure_erreur_, 0, 'f', 5));
    else if (epoch_actuel_ >= spin_epochs_->value())
        statusBar()->showMessage(QString("Terminé (%1 itérations, err=%2)")
            .arg(epoch_actuel_).arg(meilleure_erreur_, 0, 'f', 5));
    else
        statusBar()->showMessage(QString("Seuil atteint (err=%1)").arg(erreur, 0, 'f', 5));
    arreter_simulation();
}

void MainWindow::arreter_simulation() {
    stop_demande_ = true;
    if (timer_simulation_) timer_simulation_->stop();
    if (!statusBar()->currentMessage().startsWith("Arrêté")
        && !statusBar()->currentMessage().startsWith("Terminé")
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
    scene_->sync_colonnes_dataset(dataset_);
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

    // Window title with filenames
    QString titre = "Éditeur de Réseau de Neurones Biologiques";
    if (!reseau_.chemin_fichier.isEmpty())
        titre = QFileInfo(reseau_.chemin_fichier).fileName() + " — " + titre;
    if (!dataset_.chemin_fichier.isEmpty())
        titre = QFileInfo(dataset_.chemin_fichier).fileName() + " — " + titre;
    setWindowTitle(titre);
    auto sel = scene_->selectedItems();
    if (scene_->est_en_placement())
        msg += "  |  Mode placement actif (clic sur la scène)";
    else if (!sel.isEmpty()) {
        if (auto* n = dynamic_cast<NeuroneNode*>(sel.first()))
            msg += QString("  |  Sélection: #%1").arg(n->neurone_id());
        else if (auto* e = dynamic_cast<SynapseEdge*>(sel.first()))
            msg += QString("  |  Synapse #%1").arg(e->synapse_id());
    }
    statusBar()->showMessage(msg);
}

void MainWindow::on_component_place(const QVariantMap& data, QPointF pos) {
    QString type = data.value("type").toString();
    if (type == "neurone_entree") {
        reseau_.ajouter_neurone(
            QString("e%1").arg(reseau_.ids_entree().size() + 1),
            pos.x(), pos.y(), true);
    } else if (type == "neurone_cache") {
        reseau_.ajouter_neurone(
            QString("n%1").arg(reseau_.neurones.size()),
            pos.x(), pos.y(), false);
    } else if (type == "neurone_sortie") {
        reseau_.ajouter_neurone(
            QString("s%1").arg(reseau_.neurones.size()),
            pos.x(), pos.y(), false);
    } else if (type == "module") {
        QString fichier = data.value("fichier").toString();
        if (!fichier.isEmpty())
            reseau_.importer_module(fichier, pos);
    }
    scene_->reconstruire();
    mettre_a_jour_status();
}

void MainWindow::exporter_selection_module() {
    auto items = scene_->selectedItems();
    if (items.isEmpty()) return;

    std::vector<int> ids_n, ids_s;
    for (auto* item : items) {
        if (auto* n = dynamic_cast<NeuroneNode*>(item))
            ids_n.push_back(n->neurone_id());
        else if (auto* e = dynamic_cast<SynapseEdge*>(item))
            ids_s.push_back(e->synapse_id());
    }

    QString nom = QInputDialog::getText(this, "Exporter le module",
        "Nom du module :", QLineEdit::Normal, "mon_module");
    if (nom.isEmpty()) return;

    QString mod_dir = exemples_dir("modules");
    QDir().mkpath(mod_dir);
    QString chemin = mod_dir + "/" + nom + ".neuron";
    if (reseau_.exporter_module(chemin, ids_n, ids_s)) {
        palette_->scanner_modules(mod_dir);
        statusBar()->showMessage(QString("Module exporté : %1").arg(chemin));
    } else {
        QMessageBox::warning(this, "Erreur", "Impossible d'exporter le module.");
    }
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::MouseButtonPress) {
        auto* me = static_cast<QMouseEvent*>(event);
        if (me->button() == Qt::MiddleButton) {
            view_->setDragMode(QGraphicsView::ScrollHandDrag);
            // Synthesize left press to start scroll
            QMouseEvent fake(QEvent::MouseButtonPress, me->position(), me->globalPosition(),
                             Qt::LeftButton, Qt::LeftButton, me->modifiers());
            QCoreApplication::sendEvent(view_->viewport(), &fake);
            return true;
        }
    }
    if (event->type() == QEvent::MouseButtonRelease) {
        auto* me = static_cast<QMouseEvent*>(event);
        if (me->button() == Qt::MiddleButton) {
            // Synthesize left release to end scroll
            QMouseEvent fake(QEvent::MouseButtonRelease, me->position(), me->globalPosition(),
                             Qt::LeftButton, Qt::LeftButton, me->modifiers());
            QCoreApplication::sendEvent(view_->viewport(), &fake);
            view_->setDragMode(QGraphicsView::RubberBandDrag);
            return true;
        }
    }
    return QMainWindow::eventFilter(obj, event);
}
