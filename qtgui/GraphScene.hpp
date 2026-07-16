#ifndef GRAPH_SCENE_HPP
#define GRAPH_SCENE_HPP

#include <QGraphicsScene>
#include <QMap>
#include "ReseauNeural.hpp"
#include "NeuroneNode.hpp"
#include "SynapseEdge.hpp"

class GraphScene : public QGraphicsScene {
    Q_OBJECT
public:
    explicit GraphScene(ReseauNeural* reseau, QObject* parent = nullptr);

    void reconstruire();
    NeuroneNode* trouver_noeud(int id) const;
    void mettre_a_jour_synapses();
    void mettre_a_jour_poids();
    void sync_sorties_depuis_reseau();
    bool est_en_liaison() const { return mode_liaison_; }
    int source_liaison() const { return source_liaison_; }

signals:
    void neurone_selectionne(int neurone_id);
    void synapse_selectionne(int synapse_id);
    void selection_vide();
    void mode_liaison_change(bool actif, int source_id);

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;

private slots:
    void on_selection_changed();
    void on_node_moved(int id, QPointF pos);

private:
    ReseauNeural* reseau_;
    QMap<int, NeuroneNode*> noeuds_;
    QMap<int, SynapseEdge*> aretes_;
    int source_liaison_ = -1;
    bool mode_liaison_ = false;
    bool en_reconstruction_ = false;
    QGraphicsSimpleTextItem* texte_liaison_ = nullptr;
};

#endif
