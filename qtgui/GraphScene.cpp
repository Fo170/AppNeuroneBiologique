#include "GraphScene.hpp"
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QAction>
#include <QGraphicsView>
#include <QGraphicsSimpleTextItem>
#include <QKeyEvent>
#include <QApplication>
#include <algorithm>

GraphScene::GraphScene(ReseauNeural* reseau, QObject* parent)
    : QGraphicsScene(parent), reseau_(reseau)
{
    connect(this, &QGraphicsScene::selectionChanged,
            this, &GraphScene::on_selection_changed);
}

void GraphScene::reconstruire() {
    en_reconstruction_ = true;
    clear();
    noeuds_.clear();
    aretes_.clear();
    source_liaison_ = -1;
    mode_liaison_ = false;
    texte_liaison_ = nullptr;

    for (const auto& n : reseau_->neurones) {
        auto* node = new NeuroneNode(n.id, n.nom, n.est_entree);
        node->setPos(n.pos_x, n.pos_y);
        node->set_sortie(n.sortie);
        connect(node, &NeuroneNode::position_changed,
                this, &GraphScene::on_node_moved);
        connect(node, &NeuroneNode::connection_demarree, this, [this](int id, QPointF) {
            pin_connection_source_ = id;
            auto* src = noeuds_.value(id);
            if (src) {
                QPen pen(QColor(60, 130, 210), 2.5, Qt::DashLine);
                pen.setDashPattern({6, 4});
                pin_line_ = addLine(QLineF(src->pin_pos(NeuroneNode::PIN_SORTIE),
                                           src->pin_pos(NeuroneNode::PIN_SORTIE)), pen);
                pin_line_->setZValue(100);
            }
        });
        addItem(node);
        noeuds_[n.id] = node;
    }

    for (const auto& s : reseau_->synapses) {
        auto* src = noeuds_.value(s.source_id);
        auto* dst = noeuds_.value(s.target_id);
        if (src && dst) {
            auto* edge = new SynapseEdge(s.id, src, dst, s.poids);
            connect(edge, &SynapseEdge::poids_change, this, [this](int sid, float p) {
                auto* syn = reseau_->trouver_synapse(sid);
                if (syn) syn->poids = p;
                emit synapse_poids_change(sid, p);
            });
            addItem(edge);
            aretes_[s.id] = edge;
        }
    }
    en_reconstruction_ = false;
    emit scene_reconstruite();
}

NeuroneNode* GraphScene::trouver_noeud(int id) const {
    return noeuds_.value(id, nullptr);
}

void GraphScene::mettre_a_jour_synapses() {
    for (auto* e : qAsConst(aretes_))
        e->ajuster();
}

void GraphScene::mettre_a_jour_poids() {
    for (const auto& s : reseau_->synapses) {
        auto* edge = aretes_.value(s.id);
        if (edge) edge->set_poids(s.poids);
    }
}

void GraphScene::sync_sorties_depuis_reseau() {
    for (const auto& n : reseau_->neurones) {
        auto* node = noeuds_.value(n.id);
        if (node) node->set_sortie(n.sortie);
    }
}

void GraphScene::sync_colonnes_dataset(const Dataset& dataset) {
    auto ids_ent = reseau_->ids_entree();
    auto ids_sor = reseau_->ids_sortie();
    for (auto* node : qAsConst(noeuds_)) {
        int id = node->neurone_id();
        auto* ni = reseau_->trouver_neurone(id);
        if (!ni) continue;

        auto it = std::find(ids_ent.begin(), ids_ent.end(), id);
        if (it != ids_ent.end()) {
            int def_col = static_cast<int>(it - ids_ent.begin());
            if (def_col >= dataset.nb_entrees_reel()) {
                node->set_info_colonne(QString());
                continue;
            }
            int col = (ni->colonne_entree >= 0) ? ni->colonne_entree : def_col;
            QString nom = (col < dataset.noms_colonnes.size())
                ? dataset.noms_colonnes[col] : QString::number(col);
            QString label = QString("→ %1").arg(nom);
            if (ni->colonne_entree >= 0)
                label += QString(" [%1]").arg(def_col);
            node->set_info_colonne(label);
            continue;
        }

        auto it2 = std::find(ids_sor.begin(), ids_sor.end(), id);
        if (it2 != ids_sor.end()) {
            int def_col = static_cast<int>(it2 - ids_sor.begin());
            if (def_col >= dataset.nb_sorties()) {
                node->set_info_colonne(QString());
                continue;
            }
            int col = (ni->colonne_sortie >= 0) ? ni->colonne_sortie : def_col;
            int idx = dataset.nb_entrees_reel() + col;
            QString nom = (idx < dataset.noms_colonnes.size())
                ? dataset.noms_colonnes[idx] : QString("s%1").arg(col);
            QString label = QString("← %1").arg(nom);
            if (ni->colonne_sortie >= 0)
                label += QString(" [défaut %1]").arg(def_col);
            node->set_info_colonne(label);
        }
    }
}

void GraphScene::on_selection_changed() {
    auto items = selectedItems();
    if (items.isEmpty()) {
        emit selection_vide();
    } else if (auto* node = dynamic_cast<NeuroneNode*>(items.first())) {
        emit neurone_selectionne(node->neurone_id());
    } else if (auto* edge = dynamic_cast<SynapseEdge*>(items.first())) {
        emit synapse_selectionne(edge->synapse_id());
    }
}

void GraphScene::on_node_moved(int id, QPointF pos) {
    if (en_reconstruction_) return;
    auto* n = reseau_->trouver_neurone(id);
    if (n) {
        n->pos_x = pos.x();
        n->pos_y = pos.y();
    }
    mettre_a_jour_synapses();
}

void GraphScene::contextMenuEvent(QGraphicsSceneContextMenuEvent* event) {
    if (drag_active_) return;

    // If in placement mode, offer cancel
    if (!placement_data_.isEmpty()) {
        QMenu menu;
        auto* a_annuler = menu.addAction("❌ Annuler le placement");
        connect(a_annuler, &QAction::triggered, this, [this]() { quitter_placement_mode(); });
        menu.exec(event->screenPos());
        return;
    }

    QGraphicsItem* item = itemAt(event->scenePos(), QTransform());
    QMenu menu;

    auto ajouter_liaison_menu = [&](QMenu& m, NeuroneNode* node) {
        if (mode_liaison_ && source_liaison_ >= 0
            && source_liaison_ != node->neurone_id()) {
            auto* a = m.addAction("🔗 Lier à ce neurone");
            connect(a, &QAction::triggered, this, [this, node]() {
                reseau_->ajouter_synapse(source_liaison_, node->neurone_id(), 0.5f);
                reconstruire();
            });
            return true;
        }
        return false;
    };

    if (mode_liaison_) {
        menu.addAction(QString("🔗 Liaison depuis neurone #%1...").arg(source_liaison_));
    }

    if (auto* node = dynamic_cast<NeuroneNode*>(item)) {
        if (!ajouter_liaison_menu(menu, node)) {
            auto* a_relier = menu.addAction("🔗 Relier depuis ce neurone...");
            connect(a_relier, &QAction::triggered, this, [this, node]() {
                source_liaison_ = node->neurone_id();
                mode_liaison_ = true;
                emit mode_liaison_change(true, source_liaison_);
                texte_liaison_ = addSimpleText(
                    QString("🔗 Clic droit sur le neurone cible pour lier (neurone #%1)")
                        .arg(source_liaison_));
                texte_liaison_->setPos(10, 10);
                texte_liaison_->setZValue(100);
                QFont f = texte_liaison_->font();
                f.setPointSize(12);
                f.setBold(true);
                texte_liaison_->setFont(f);
                texte_liaison_->setBrush(QColor(0, 80, 180));
            });
        }

        auto* a_suppr = menu.addAction("🗑  Supprimer ce neurone");
        connect(a_suppr, &QAction::triggered, this, [this, node]() {
            reseau_->supprimer_neurone(node->neurone_id());
            reconstruire();
            emit selection_vide();
        });

        // Column mapping for input neurons
        if (dataset_ptr_ && node->est_entree()) {
            auto* col_menu = menu.addMenu(QString("🔢 Colonne d'entrée"));
            auto* def = col_menu->addAction("Position par défaut");
            connect(def, &QAction::triggered, this, [this, node]() {
                emit mapping_colonne_change(node->neurone_id(), true, -1);
            });
            int nb = dataset_ptr_->nb_entrees_reel();
            for (int c = 0; c < nb; ++c) {
                QString nom = c < dataset_ptr_->noms_colonnes.size()
                    ? dataset_ptr_->noms_colonnes[c] : QString::number(c);
                auto* a = col_menu->addAction(nom);
                connect(a, &QAction::triggered, this, [this, node, c]() {
                    emit mapping_colonne_change(node->neurone_id(), true, c);
                });
            }
        }
        // Column mapping for output neurons
        if (dataset_ptr_ && !node->est_entree()) {
            auto* col_menu = menu.addMenu(QString("🔢 Colonne de sortie"));
            auto* def = col_menu->addAction("Position par défaut");
            connect(def, &QAction::triggered, this, [this, node]() {
                emit mapping_colonne_change(node->neurone_id(), false, -1);
            });
            int nb = dataset_ptr_->nb_sorties();
            for (int c = 0; c < nb; ++c) {
                int idx = dataset_ptr_->nb_entrees_reel() + c;
                QString nom = idx < dataset_ptr_->noms_colonnes.size()
                    ? dataset_ptr_->noms_colonnes[idx] : QString("s%1").arg(c);
                auto* a = col_menu->addAction(nom);
                connect(a, &QAction::triggered, this, [this, node, c]() {
                    emit mapping_colonne_change(node->neurone_id(), false, c);
                });
            }
        }

    } else if (auto* edge = dynamic_cast<SynapseEdge*>(item)) {
        auto* a_suppr = menu.addAction("🗑  Supprimer cette synapse");
        connect(a_suppr, &QAction::triggered, this, [this, edge]() {
            reseau_->supprimer_synapse(edge->synapse_id());
            reconstruire();
            emit selection_vide();
        });

    } else {
        if (mode_liaison_) {
            auto* a_annuler = menu.addAction("❌ Annuler la liaison");
            connect(a_annuler, &QAction::triggered, this, [this]() {
                mode_liaison_ = false;
                source_liaison_ = -1;
                if (texte_liaison_) { removeItem(texte_liaison_); texte_liaison_ = nullptr; }
                emit mode_liaison_change(false, -1);
            });
        }
        auto* a_entree = menu.addAction("➕ Neurone d'entrée");
        connect(a_entree, &QAction::triggered, this, [this, event]() {
            QPointF p = event->scenePos();
            int id = reseau_->ajouter_neurone(
                QString("e%1").arg(reseau_->ids_entree().size() + 1),
                p.x(), p.y(), true);
            reconstruire();
            emit neurone_selectionne(id);
        });
        auto* a_cache = menu.addAction("➕ Neurone caché");
        connect(a_cache, &QAction::triggered, this, [this, event]() {
            QPointF p = event->scenePos();
            int id = reseau_->ajouter_neurone(
                QString("n%1").arg(reseau_->neurones.size()), p.x(), p.y(), false);
            reconstruire();
            emit neurone_selectionne(id);
        });
    }

    // Add "Exporter la sélection en module" if items are selected
    if (!selectedItems().isEmpty()) {
        if (!menu.isEmpty()) menu.addSeparator();
        auto* a_export = menu.addAction("📦 Exporter la sélection en module...");
        connect(a_export, &QAction::triggered, this, [this]() {
            emit exporter_selection();
        });
    }

    if (!menu.isEmpty())
        menu.exec(event->screenPos());
}

void GraphScene::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape) {
        if (!placement_data_.isEmpty()) {
            quitter_placement_mode();
            event->accept();
            return;
        }
        if (pin_connection_source_ >= 0) {
            if (pin_line_) { removeItem(pin_line_); delete pin_line_; pin_line_ = nullptr; }
            nettoyer_etats_pins();
            pin_connection_source_ = -1;
            event->accept();
            return;
        }
    }
    if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
        auto items = selectedItems();
        if (!items.isEmpty()) {
            for (auto* item : items) {
                if (auto* edge = dynamic_cast<SynapseEdge*>(item))
                    reseau_->supprimer_synapse(edge->synapse_id());
                else if (auto* node = dynamic_cast<NeuroneNode*>(item))
                    reseau_->supprimer_neurone(node->neurone_id());
            }
            reconstruire();
            emit selection_vide();
            event->accept();
            return;
        }
    }
    QGraphicsScene::keyPressEvent(event);
}

void GraphScene::set_placement_mode(const QVariantMap& data) {
    placement_data_ = data;
    if (!data.isEmpty()) {
        QApplication::setOverrideCursor(Qt::CrossCursor);
        if (texte_liaison_) {
            removeItem(texte_liaison_);
            texte_liaison_ = nullptr;
        }
        texte_liaison_ = addSimpleText(
            QString("📌 Placer « %1 » — clic gauche sur la scène, Échap pour annuler")
                .arg(data.value("nom").toString()));
        texte_liaison_->setPos(10, 10);
        texte_liaison_->setZValue(100);
        QFont f = texte_liaison_->font();
        f.setPointSize(11);
        f.setBold(true);
        texte_liaison_->setFont(f);
        texte_liaison_->setBrush(QColor(0, 100, 0));
    }
}

void GraphScene::quitter_placement_mode() {
    placement_data_.clear();
    QApplication::restoreOverrideCursor();
    if (texte_liaison_) { removeItem(texte_liaison_); texte_liaison_ = nullptr; }
}

void GraphScene::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (!placement_data_.isEmpty() && event->button() == Qt::LeftButton) {
        QGraphicsItem* item = itemAt(event->scenePos(), QTransform());
        if (!item) {
            QVariantMap data = placement_data_;
            emit component_place(data, event->scenePos());
            event->accept();
            return;
        }
    }
    QGraphicsScene::mousePressEvent(event);

    if (event->button() == Qt::RightButton) {
        QGraphicsItem* item = itemAt(event->scenePos(), QTransform());
        if (dynamic_cast<NeuroneNode*>(item)) {
            drag_source_ = static_cast<NeuroneNode*>(item)->neurone_id();
            drag_start_pos_ = event->scenePos();
            drag_active_ = false;
        }
    }
}

void GraphScene::nettoyer_etats_pins() {
    for (auto* node : qAsConst(noeuds_)) {
        node->set_pin_actif(false);
        node->set_pin_cible(false);
    }
}

void GraphScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    if (pin_connection_source_ >= 0) {
        if (pin_line_) {
            auto* src = noeuds_.value(pin_connection_source_);
            if (src)
                pin_line_->setLine(QLineF(src->pin_pos(NeuroneNode::PIN_SORTIE),
                                          event->scenePos()));
        }

        // Highlight target neuron by checking input pins directly
        for (auto* node : qAsConst(noeuds_))
            node->set_pin_cible(false);
        for (auto* node : qAsConst(noeuds_)) {
            if (node->neurone_id() != pin_connection_source_) {
                QPointF local = node->mapFromScene(event->scenePos());
                if (node->hit_test_pin(local) == NeuroneNode::PIN_ENTREE) {
                    node->set_pin_cible(true);
                    break;
                }
            }
        }

        QGraphicsScene::mouseMoveEvent(event);
        event->accept();
        return;
    }

    if ((event->buttons() & Qt::RightButton) && drag_source_ >= 0) {
        QPointF delta = event->scenePos() - drag_start_pos_;
        if (delta.manhattanLength() > 10) {
            if (!drag_active_) {
                drag_active_ = true;
                auto* src = noeuds_.value(drag_source_);
                if (src) {
                    QPen pen(QColor(100, 100, 100), 2, Qt::DashLine);
                    drag_line_ = addLine(QLineF(src->centre(), event->scenePos()), pen);
                    drag_line_->setZValue(100);
                }
            } else if (drag_line_) {
                auto* src = noeuds_.value(drag_source_);
                if (src)
                    drag_line_->setLine(QLineF(src->centre(), event->scenePos()));
            }

            // Highlight target by checking input pins directly
            for (auto* node : qAsConst(noeuds_))
                node->set_pin_cible(false);
            for (auto* node : qAsConst(noeuds_)) {
                if (node->neurone_id() != drag_source_) {
                    QPointF local = node->mapFromScene(event->scenePos());
                    if (node->hit_test_pin(local) == NeuroneNode::PIN_ENTREE) {
                        node->set_pin_cible(true);
                        break;
                    }
                }
            }

            event->accept();
            return;
        }
    }
    QGraphicsScene::mouseMoveEvent(event);
}

void GraphScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    if (pin_connection_source_ >= 0) {
        if (pin_line_) {
            removeItem(pin_line_);
            delete pin_line_;
            pin_line_ = nullptr;
        }
        if (event->button() == Qt::LeftButton) {
            QGraphicsItem* item = itemAt(event->scenePos(), QTransform());
            if (auto* node = dynamic_cast<NeuroneNode*>(item)) {
                if (node->neurone_id() != pin_connection_source_) {
                    int sid = reseau_->ajouter_synapse(pin_connection_source_,
                                                       node->neurone_id(), 0.5f);
                    auto* src = noeuds_.value(pin_connection_source_);
                    auto* dst = noeuds_.value(node->neurone_id());
                    if (src && dst) {
                        auto* edge = new SynapseEdge(sid, src, dst, 0.5f);
                        connect(edge, &SynapseEdge::poids_change, this, [this](int id, float p) {
                            auto* syn = reseau_->trouver_synapse(id);
                            if (syn) syn->poids = p;
                            emit synapse_poids_change(id, p);
                        });
                        addItem(edge);
                        aretes_[sid] = edge;
                    }
                }
            }
        }
        nettoyer_etats_pins();
        pin_connection_source_ = -1;
        QGraphicsScene::mouseReleaseEvent(event);
        event->accept();
        return;
    }

    if (event->button() == Qt::RightButton && drag_source_ >= 0) {
        if (drag_active_) {
            if (drag_line_) {
                removeItem(drag_line_);
                delete drag_line_;
                drag_line_ = nullptr;
            }
            QGraphicsItem* item = itemAt(event->scenePos(), QTransform());
            if (auto* node = dynamic_cast<NeuroneNode*>(item)) {
                if (node->neurone_id() != drag_source_) {
                    int sid = reseau_->ajouter_synapse(drag_source_,
                                                       node->neurone_id(), 0.5f);
                    auto* src = noeuds_.value(drag_source_);
                    auto* dst = noeuds_.value(node->neurone_id());
                    if (src && dst) {
                        auto* edge = new SynapseEdge(sid, src, dst, 0.5f);
                        connect(edge, &SynapseEdge::poids_change, this, [this](int id, float p) {
                            auto* syn = reseau_->trouver_synapse(id);
                            if (syn) syn->poids = p;
                            emit synapse_poids_change(id, p);
                        });
                        addItem(edge);
                        aretes_[sid] = edge;
                    }
                }
            }
            nettoyer_etats_pins();
            drag_source_ = -1;
            drag_active_ = false;
            QGraphicsScene::mouseReleaseEvent(event);
            event->accept();
            return;
        }
        nettoyer_etats_pins();
        drag_source_ = -1;
    }
    if (drag_line_) {
        removeItem(drag_line_);
        delete drag_line_;
        drag_line_ = nullptr;
    }
    drag_source_ = -1;
    drag_active_ = false;
    QGraphicsScene::mouseReleaseEvent(event);
}
