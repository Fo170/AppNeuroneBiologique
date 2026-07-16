#include "PropertyPanel.hpp"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QFileDialog>
#include <QDebug>
#include <QPainterPath>
#include <QFrame>
#include <algorithm>
#include <cmath>

// ============================================================
//  GraphiqueErreur — widget custom pour la courbe d'erreur
// ============================================================

GraphiqueErreur::GraphiqueErreur(QWidget* parent) : QWidget(parent) {
    setMinimumSize(200, 100);
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::white);
    setPalette(pal);
}

void GraphiqueErreur::ajouter_point(double erreur) {
    points_.append(erreur);
    if (points_.size() > 500)
        points_.remove(0, points_.size() - 500);
    // Auto-scale: recompute max from visible points
    max_erreur_ = 0.01;
    for (double p : points_)
        if (p > max_erreur_) max_erreur_ = p;
    update();
}

void GraphiqueErreur::reinitialiser() {
    points_.clear();
    max_erreur_ = 1.0;
    update();
}

void GraphiqueErreur::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    int w = width();
    int h = height();
    if (w < 10 || h < 10) return;

    // Margins
    const int m = 8;
    double gx = m;
    double gy = m;
    double gw = w - 2 * m;
    double gh = h - 2 * m - 10; // leave room for bottom label

    if (gw < 10 || gh < 10 || points_.isEmpty()) return;

    double max_v = max_erreur_ > 0 ? max_erreur_ : 1.0;

    // Axes
    p.setPen(QPen(Qt::gray, 1));
    p.drawLine(QPointF(gx, gy), QPointF(gx, gy + gh));
    p.drawLine(QPointF(gx, gy + gh), QPointF(gx + gw, gy + gh));

    // Y-axis labels
    QFont f = p.font();
    f.setPointSize(7);
    p.setFont(f);
    p.setPen(Qt::darkGray);
    p.drawText(QPointF(gx + 2, gy + gh), "0");
    p.drawText(QPointF(gx + 2, gy + 8), QString::number(max_v, 'f', 3));

    // Courbe
    QPainterPath path;
    int n = points_.size();
    for (int i = 0; i < n; ++i) {
        double x = gx + (double)i / (n - 1) * gw;
        double y = gy + gh - (points_[i] / max_v) * gh;
        if (i == 0) path.moveTo(x, y);
        else path.lineTo(x, y);
    }

    p.setPen(QPen(QColor(200, 50, 50), 1.5));
    p.setBrush(Qt::NoBrush);
    p.drawPath(path);

    // Dernière valeur
    p.setPen(Qt::black);
    f.setPointSize(8);
    p.setFont(f);
    p.drawText(QPointF(gx, gy + gh + 12),
               QString("Époques: %1  ·  Erreur: %2")
                   .arg(n).arg(points_.last(), 0, 'f', 5));
}

static void lire_table_depuis_gui(Dataset* dataset, QTableWidget* table) {
    if (!dataset) return;
    dataset->exemples.clear();
    int nb = table->columnCount();
    if (nb < 2) return;
    dataset->noms_colonnes.clear();
    for (int c = 0; c < nb; ++c) {
        auto* h = table->horizontalHeaderItem(c);
        dataset->noms_colonnes << (h ? h->text() : QString("col%1").arg(c));
    }
    int ni = (dataset->nb_entrees > 0) ? dataset->nb_entrees : (nb - 1);
    int ns = dataset->nb_sorties();
    // Only read base columns (input + target), ignore sortie/err appended columns
    int data_end = (ns > 0) ? (ni + ns) : nb;
    for (int r = 0; r < table->rowCount(); ++r) {
        Exemple ex;
        int c = 0;
        for (; c < std::min(ni, data_end); ++c) {
            auto* item = table->item(r, c);
            ex.entrees.push_back(item ? item->text().toFloat() : 0.0f);
        }
        for (; c < data_end; ++c) {
            auto* item = table->item(r, c);
            ex.cibles.push_back(item ? item->text().toFloat() : 0.0f);
        }
        dataset->exemples.push_back(ex);
    }
}

PropertyPanel::PropertyPanel(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(4, 4, 4, 4);

    tabs_ = new QTabWidget;
    tabs_->addTab(onglet_dataset(), "Dataset");
    tabs_->addTab(onglet_neurone(), "Neurone");
    tabs_->addTab(onglet_synapse(), "Synapse");
    tabs_->setCurrentIndex(0);
    layout->addWidget(tabs_);
}

void PropertyPanel::afficher_dataset(Dataset* dataset) {
    dataset_courant_ = dataset;
    if (!dataset) return;
    tabs_->setCurrentIndex(0);

    QString infos = dataset_courant_->resume();
    label_info_dataset_->setText(infos);

    qDebug() << "[afficher_dataset]" << infos
             << "| examples:" << dataset->exemples.size()
             << "nb_entrees:" << dataset->nb_entrees
             << "noms:" << dataset->noms_colonnes;

    int ni = dataset->nb_entrees_reel();
    int ns = dataset->nb_sorties();
    int total = ni + ns;
    if (total == 0 || dataset->exemples.empty()) {
        table_dataset_->setRowCount(0);
        table_dataset_->setColumnCount(0);
        return;
    }

    // Reset to base columns only (remove sortie/err columns if any)
    nettoyer_sorties_dataset();

    table_dataset_->setRowCount(dataset->exemples.size());
    table_dataset_->setColumnCount(total);

    auto headers = dataset->noms_colonnes;
    if (headers.size() < total) {
        headers.clear();
        for (int i = 0; i < ni; ++i)
            headers << QString("e%1").arg(i + 1);
        if (ns == 1) headers << "cible";
        else for (int i = 0; i < ns; ++i)
            headers << QString("cible%1").arg(i + 1);
    }
    table_dataset_->setHorizontalHeaderLabels(headers);

    for (int r = 0; r < (int)dataset->exemples.size(); ++r) {
        const auto& ex = dataset->exemples[r];
        int c = 0;
        for (float v : ex.entrees)
            table_dataset_->setItem(r, c++, new QTableWidgetItem(QString::number(v, 'f', 4)));
        for (float v : ex.cibles)
            table_dataset_->setItem(r, c++, new QTableWidgetItem(QString::number(v, 'f', 4)));
    }
    table_dataset_->resizeColumnsToContents();
    table_dataset_->updateGeometry();

    // Color input/output columns
    for (int c = 0; c < total; ++c) {
        auto* h = table_dataset_->horizontalHeaderItem(c);
        if (!h) continue;
        if (c < ni)
            h->setBackground(QColor(220, 240, 255));
        else
            h->setBackground(QColor(255, 240, 220));
    }
}

void PropertyPanel::mettre_a_jour_sorties_dataset(const std::vector<std::vector<float>>& sorties) {
    if (!dataset_courant_) return;
    int ni = dataset_courant_->nb_entrees_reel();
    int ns = dataset_courant_->nb_sorties();
    int nr = dataset_courant_->nb_exemples();
    if (ni <= 0 || ns <= 0 || nr <= 0) return;

    int base_cols = ni + ns;
    int expected_cols = ni + 3 * ns;

    // Add sortie + erreur columns if not present
    if (table_dataset_->columnCount() < expected_cols) {
        table_dataset_->setColumnCount(expected_cols);
        table_dataset_->setHorizontalHeaderItem(ni + ns, new QTableWidgetItem("sortie"));
        for (int k = 1; k < ns; ++k)
            table_dataset_->setHorizontalHeaderItem(ni + ns + k, new QTableWidgetItem(QString("sortie%1").arg(k + 1)));
        table_dataset_->setHorizontalHeaderItem(ni + 2 * ns, new QTableWidgetItem("erreur"));
        for (int k = 1; k < ns; ++k)
            table_dataset_->setHorizontalHeaderItem(ni + 2 * ns + k, new QTableWidgetItem(QString("err%1").arg(k + 1)));

        // Color headers
        for (int c = ni + ns; c < ni + 2 * ns; ++c) {
            auto* h = table_dataset_->horizontalHeaderItem(c);
            if (h) h->setBackground(QColor(200, 255, 200));
        }
        for (int c = ni + 2 * ns; c < expected_cols; ++c) {
            auto* h = table_dataset_->horizontalHeaderItem(c);
            if (h) h->setBackground(QColor(255, 200, 200));
        }
    }

    // Fill values
    for (int r = 0; r < nr && r < table_dataset_->rowCount(); ++r) {
        for (int k = 0; k < ns; ++k) {
            float s = (r < (int)sorties.size() && k < (int)sorties[r].size())
                ? sorties[r][k] : 0.0f;
            float cible = (r < (int)dataset_courant_->exemples.size()
                && k < (int)dataset_courant_->exemples[r].cibles.size())
                ? dataset_courant_->exemples[r].cibles[k] : 0.0f;
            float err = std::abs(s - cible);

            auto* item_s = new QTableWidgetItem(QString::number(s, 'f', 4));
            item_s->setFlags(item_s->flags() & ~Qt::ItemIsEditable);
            table_dataset_->setItem(r, ni + ns + k, item_s);

            auto* item_e = new QTableWidgetItem(QString::number(err, 'f', 4));
            item_e->setFlags(item_e->flags() & ~Qt::ItemIsEditable);
            table_dataset_->setItem(r, ni + 2 * ns + k, item_e);
        }
    }
    table_dataset_->resizeColumnsToContents();
}

void PropertyPanel::nettoyer_sorties_dataset() {
    if (!dataset_courant_ || table_dataset_->columnCount() == 0) return;
    int ni = dataset_courant_->nb_entrees_reel();
    int ns = dataset_courant_->nb_sorties();
    int base = ni + ns;
    if (table_dataset_->columnCount() > base)
        table_dataset_->setColumnCount(base);
}

void PropertyPanel::afficher_neurone(NeuroneInfo* info) {
    neurone_courant_ = info;
    synapse_courant_ = nullptr;
    tabs_->setCurrentIndex(1);
    if (!info) return;
    label_id_->setText(QString("Neurone #%1").arg(info->id));
    edit_nom_->setText(info->nom);

    // Bloque les signaux pour éviter la boucle setValue → neurone_modifie → reconstruire
    {
        const QSignalBlocker b1(spin_v_rest_);   spin_v_rest_->setValue(info->V_rest);
        const QSignalBlocker b2(spin_tau_);      spin_tau_->setValue(info->tau);
        const QSignalBlocker b3(spin_biais_);    spin_biais_->setValue(info->biais);
        const QSignalBlocker b4(spin_refr_);     spin_refr_->setValue(info->refractaire_ms);
        const QSignalBlocker b5(spin_eta_);      spin_eta_->setValue(info->eta);
        const QSignalBlocker b6(spin_oubli_);    spin_oubli_->setValue(info->oubli_lent);
        const QSignalBlocker b7(check_entree_);  check_entree_->setChecked(info->est_entree);
    }

    label_sortie_->setText(QString::number(info->sortie, 'f', 4));
    label_v_->setText(QString::number(info->V, 'f', 4));
}

void PropertyPanel::afficher_synapse(SynapseInfo* info) {
    neurone_courant_ = nullptr;
    synapse_courant_ = info;
    tabs_->setCurrentIndex(2);
    if (!info) return;
    s_label_id_->setText(QString("Synapse #%1").arg(info->id));
    s_label_source_->setText(QString::number(info->source_id));
    s_label_cible_->setText(QString::number(info->target_id));
    {
        const QSignalBlocker b1(s_spin_poids_);  s_spin_poids_->setValue(info->poids);
        const QSignalBlocker b2(s_combo_type_);  s_combo_type_->setCurrentIndex(static_cast<int>(info->type));
    }
}

QWidget* PropertyPanel::onglet_dataset() {
    auto* page = new QWidget;
    auto* lay = new QVBoxLayout(page);
    lay->setContentsMargins(4, 4, 4, 4);

    label_info_dataset_ = new QLabel("Aucun dataset chargé");
    QFont f = label_info_dataset_->font(); f.setBold(true); label_info_dataset_->setFont(f);
    label_info_dataset_->setWordWrap(true);
    lay->addWidget(label_info_dataset_);

    table_dataset_ = new QTableWidget;
    table_dataset_->setSelectionBehavior(QAbstractItemView::SelectItems);
    table_dataset_->horizontalHeader()->setStretchLastSection(true);
    lay->addWidget(table_dataset_, 1); // stretch = 1

    auto* bl = new QHBoxLayout;
    btn_add_row_ = new QPushButton("+ Ajouter ligne");
    btn_add_row_->setToolTip("Ajoute une nouvelle ligne d'exemple au dataset.");
    connect(btn_add_row_, &QPushButton::clicked, this, [this]() {
        if (!dataset_courant_) return;
        int r = table_dataset_->rowCount();
        int nb = table_dataset_->columnCount();
        if (nb == 0) {
            nb = 3;
            table_dataset_->setColumnCount(3);
            table_dataset_->setHorizontalHeaderLabels({"e1","e2","cible"});
        }
        table_dataset_->setRowCount(r + 1);
        for (int c = 0; c < nb; ++c)
            if (!table_dataset_->item(r, c))
                table_dataset_->setItem(r, c, new QTableWidgetItem("0"));
        ::lire_table_depuis_gui(dataset_courant_, table_dataset_);
        emit dataset_modifie();
    });
    bl->addWidget(btn_add_row_);

    btn_del_row_ = new QPushButton("- Supprimer ligne");
    btn_del_row_->setToolTip("Supprime la ligne sélectionnée du dataset.");
    connect(btn_del_row_, &QPushButton::clicked, this, [this]() {
        int r = table_dataset_->currentRow();
        if (r >= 0 && dataset_courant_) {
            table_dataset_->removeRow(r);
            ::lire_table_depuis_gui(dataset_courant_, table_dataset_);
            emit dataset_modifie();
        }
    });
    bl->addWidget(btn_del_row_);
    lay->addLayout(bl);

    auto* cl = new QHBoxLayout;
    btn_save_csv_ = new QPushButton("\U0001F4BE Sauvegarder CSV");
    btn_save_csv_->setToolTip("Sauvegarde le dataset courant dans un fichier CSV.");
    connect(btn_save_csv_, &QPushButton::clicked, this, [this]() {
        if (!dataset_courant_) return;
        ::lire_table_depuis_gui(dataset_courant_, table_dataset_);
        QString p = QFileDialog::getSaveFileName(this, "Sauvegarder dataset", "", "CSV (*.csv)");
        if (!p.isEmpty()) {
            dataset_courant_->sauvegarder_csv(p);
            label_info_dataset_->setText(dataset_courant_->resume());
        }
    });
    cl->addWidget(btn_save_csv_);

    btn_load_csv_ = new QPushButton("\U0001F4C2 Charger CSV");
    btn_load_csv_->setToolTip("Charge un dataset depuis un fichier CSV.");
    connect(btn_load_csv_, &QPushButton::clicked, this, [this]() {
        if (!dataset_courant_) return;
        QString p = QFileDialog::getOpenFileName(this, "Charger dataset", "", "CSV (*.csv)");
        if (!p.isEmpty() && dataset_courant_->charger_csv(p)) {
            afficher_dataset(dataset_courant_);
            emit dataset_modifie();
        }
    });
    cl->addWidget(btn_load_csv_);
    lay->addLayout(cl);

    // --- Separator / Apprentissage section ---
    auto* sep = new QFrame;
    sep->setFrameShape(QFrame::HLine);
    sep->setFrameShadow(QFrame::Sunken);
    lay->addWidget(sep);

    // Graphique
    graph_erreur_ = new GraphiqueErreur;
    lay->addWidget(graph_erreur_);

    // Infos
    auto* il = new QFormLayout;
    label_epoch_ = new QLabel("—");
    label_erreur_ = new QLabel("—");
    label_meilleure_ = new QLabel("—");
    il->addRow("Époque", label_epoch_);
    il->addRow("Erreur", label_erreur_);
    il->addRow("Meilleure", label_meilleure_);
    lay->addLayout(il);

    // Contrôles
    auto* ctrl = new QFormLayout;
    spin_seuil_ = new QDoubleSpinBox;
    spin_seuil_->setRange(0.0, 1.0);
    spin_seuil_->setDecimals(5);
    spin_seuil_->setSingleStep(0.001);
    spin_seuil_->setValue(0.001);
    spin_seuil_->setToolTip("Si l'erreur descend en dessous de ce seuil, la simulation s'arrête automatiquement.");
    ctrl->addRow("Seuil erreur", spin_seuil_);

    spin_batch_ = new QSpinBox;
    spin_batch_->setRange(1, 10000);
    spin_batch_->setValue(10);
    spin_batch_->setToolTip("Nombre d'itérations d'apprentissage entre deux rafraîchissements de l'interface.\n"
                            "Une valeur plus grande = mise à jour moins fréquente mais convergence plus rapide.");
    ctrl->addRow("Batch", spin_batch_);
    lay->addLayout(ctrl);

    btn_stop_ = new QPushButton("\u23F9 Arr\u00EAter");
    btn_stop_->setToolTip("Arrête la simulation en cours.");
    connect(btn_stop_, &QPushButton::clicked, this, &PropertyPanel::stop_simulation);
    lay->addWidget(btn_stop_);

    return page;
}

QWidget* PropertyPanel::onglet_neurone() {
    auto* page = new QWidget;
    auto* lay = new QVBoxLayout(page);
    lay->setContentsMargins(4, 4, 4, 4);

    label_id_ = new QLabel("(aucun neurone sélectionné)");
    QFont f = label_id_->font(); f.setBold(true); label_id_->setFont(f);
    lay->addWidget(label_id_);

    auto* form = new QFormLayout;
    edit_nom_ = new QLineEdit; edit_nom_->setPlaceholderText("ex: neurone_0");
    edit_nom_->setToolTip("Nom libre du neurone pour l'identifier dans le réseau.");
    form->addRow("Nom", edit_nom_);
    connect(edit_nom_, &QLineEdit::editingFinished, this, [this]() {
        if (neurone_courant_) { neurone_courant_->nom = edit_nom_->text(); emit neurone_modifie(); }
    });
    spin_v_rest_ = new QDoubleSpinBox; spin_v_rest_->setRange(-10, 10); spin_v_rest_->setSingleStep(0.1); spin_v_rest_->setValue(0.0);
    spin_v_rest_->setToolTip("Potentiel de repos du neurone (mV).\nSans stimulation, le potentiel tend vers cette valeur.");
    form->addRow("V_rest", spin_v_rest_);
    connect(spin_v_rest_, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double v) {
        if (neurone_courant_) { neurone_courant_->V_rest = float(v); emit neurone_modifie(); }
    });
    spin_tau_ = new QDoubleSpinBox; spin_tau_->setRange(0.1, 100); spin_tau_->setSingleStep(1); spin_tau_->setValue(10.0);
    spin_tau_->setToolTip("Constante de temps de fuite (ms).\nPlus τ est grand, plus le neurone met du temps à revenir au repos.");
    form->addRow("τ (ms)", spin_tau_);
    connect(spin_tau_, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double v) {
        if (neurone_courant_) { neurone_courant_->tau = float(v); emit neurone_modifie(); }
    });
    spin_biais_ = new QDoubleSpinBox; spin_biais_->setRange(-10, 10); spin_biais_->setSingleStep(0.1); spin_biais_->setValue(0.0);
    spin_biais_->setToolTip("Biais du neurone — décale le seuil d'activation.\n"
                            "Un biais positif rend le neurone plus excitable, négatif le rend plus silencieux.\n"
                            "Pendant l'apprentissage, le biais est automatiquement ajusté (clampé à [-5, +5]).");
    form->addRow("Biais", spin_biais_);
    connect(spin_biais_, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double v) {
        if (neurone_courant_) { neurone_courant_->biais = float(v); emit neurone_modifie(); }
    });
    spin_refr_ = new QDoubleSpinBox; spin_refr_->setRange(0, 100); spin_refr_->setSingleStep(0.5); spin_refr_->setValue(2.0);
    spin_refr_->setToolTip("Période réfractaire (ms) après un potentiel d'action.\n"
                            "Pendant cette période, le neurone ignore les entrées.");
    form->addRow("Réfractaire (ms)", spin_refr_);
    connect(spin_refr_, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double v) {
        if (neurone_courant_) { neurone_courant_->refractaire_ms = float(v); emit neurone_modifie(); }
    });
    spin_eta_ = new QDoubleSpinBox; spin_eta_->setRange(0, 1); spin_eta_->setSingleStep(0.001); spin_eta_->setDecimals(4); spin_eta_->setValue(0.01);
    spin_eta_->setToolTip("Taux d'apprentissage (η).\n"
                          "Contrôle la vitesse d'adaptation des poids et du biais.\n"
                          "η trop grand = instable, η trop petit = convergence lente.");
    form->addRow("η (apprentissage)", spin_eta_);
    connect(spin_eta_, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double v) {
        if (neurone_courant_) { neurone_courant_->eta = float(v); emit neurone_modifie(); }
    });
    spin_oubli_ = new QDoubleSpinBox; spin_oubli_->setRange(0, 1); spin_oubli_->setSingleStep(0.0001); spin_oubli_->setDecimals(4); spin_oubli_->setValue(0.001);
    spin_oubli_->setToolTip("Taux d'oubli synaptique (non utilisé dans la règle delta actuelle).");
    form->addRow("Oubli", spin_oubli_);
    connect(spin_oubli_, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double v) {
        if (neurone_courant_) { neurone_courant_->oubli_lent = float(v); emit neurone_modifie(); }
    });
    check_entree_ = new QCheckBox("Neurone d'entrée");
    form->addRow("", check_entree_);
    connect(check_entree_, &QCheckBox::toggled, this, [this](bool ch) {
        if (neurone_courant_) { neurone_courant_->est_entree = ch; emit neurone_modifie(); }
    });
    lay->addLayout(form);

    auto* grp = new QGroupBox("État (runtime)");
    auto* gl = new QFormLayout;
    label_v_ = new QLabel("0.0"); gl->addRow("Potentiel V", label_v_);
    label_sortie_ = new QLabel("0.0"); gl->addRow("Sortie σ(V)", label_sortie_);
    grp->setLayout(gl); lay->addWidget(grp);
    lay->addStretch();
    return page;
}

QWidget* PropertyPanel::onglet_synapse() {
    auto* page = new QWidget;
    auto* lay = new QVBoxLayout(page);
    lay->setContentsMargins(4, 4, 4, 4);
    s_label_id_ = new QLabel("(aucune synapse sélectionnée)");
    QFont f = s_label_id_->font(); f.setBold(true); s_label_id_->setFont(f);
    lay->addWidget(s_label_id_);
    auto* form = new QFormLayout;
    s_label_source_ = new QLabel; form->addRow("Neurone source", s_label_source_);
    s_label_cible_ = new QLabel; form->addRow("Neurone cible", s_label_cible_);
    s_spin_poids_ = new QDoubleSpinBox; s_spin_poids_->setRange(-10, 10); s_spin_poids_->setSingleStep(0.1); s_spin_poids_->setDecimals(4); s_spin_poids_->setValue(0.5);
    s_spin_poids_->setToolTip("Poids synaptique.\n"
                              "Un poids positif = excitateur, négatif = inhibiteur.\n"
                              "Pendant l'apprentissage, le poids est automatiquement ajusté.");
    form->addRow("Poids", s_spin_poids_);
    connect(s_spin_poids_, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double v) {
        if (synapse_courant_) { synapse_courant_->poids = float(v); emit synapse_modifie(); }
    });
    s_combo_type_ = new QComboBox;
    s_combo_type_->addItems({"AXO_DENDRITIQUE","AXO_AXONIQUE","DENDRO_DENDRITIQUE","DENDRO_AXONIQUE"});
    s_combo_type_->setToolTip("Type biologique de la synapse.\n"
                              "Proportions naturelles : axo-dendritique 66.6%, axo-axonique 25.8%,\n"
                              "dendro-dendritique 5.8%, dendro-axonique 1.8%.");
    form->addRow("Type", s_combo_type_);
    connect(s_combo_type_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int idx) {
        if (synapse_courant_) { synapse_courant_->type = static_cast<TypeSynapse>(idx); emit synapse_modifie(); }
    });
    lay->addLayout(form);
    lay->addStretch();
    return page;
}

void PropertyPanel::set_meilleure_erreur(double v) {
    label_meilleure_->setText(QString::number(v, 'f', 6));
}

void PropertyPanel::activer_onglet_apprentissage() {
    tabs_->setCurrentIndex(0);
}

void PropertyPanel::ajouter_point_erreur(int epoch, double erreur) {
    graph_erreur_->ajouter_point(erreur);
    label_epoch_->setText(QString::number(epoch));
    label_erreur_->setText(QString::number(erreur, 'f', 6));
}

void PropertyPanel::reinitialiser_apprentissage() {
    graph_erreur_->reinitialiser();
    label_epoch_->setText("—");
    label_erreur_->setText("—");
    label_meilleure_->setText("—");
    nettoyer_sorties_dataset();
}

double PropertyPanel::seuil_arret() const {
    return spin_seuil_->value();
}

int PropertyPanel::batch() const {
    return spin_batch_->value();
}
