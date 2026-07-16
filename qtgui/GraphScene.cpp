#include "GraphScene.hpp"
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QAction>
#include <QGraphicsView>
#include <QGraphicsSimpleTextItem>

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
        addItem(node);
        noeuds_[n.id] = node;
    }

    for (const auto& s : reseau_->synapses) {
        auto* src = noeuds_.value(s.source_id);
        auto* dst = noeuds_.value(s.target_id);
        if (src && dst) {
            auto* edge = new SynapseEdge(s.id, src, dst, s.poids);
            addItem(edge);
            aretes_[s.id] = edge;
        }
    }
    en_reconstruction_ = false;
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
                // Visual hint
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

    if (!menu.isEmpty())
        menu.exec(event->screenPos());
}
