#ifndef GRAPH_SCENE_HPP
#define GRAPH_SCENE_HPP

#include <QGraphicsScene>
#include <QMap>
#include <QGraphicsLineItem>
#include "ReseauNeural.hpp"
#include "NeuroneNode.hpp"
#include "SynapseEdge.hpp"
#include "Dataset.hpp"

class GraphScene : public QGraphicsScene {
    Q_OBJECT
public:
    explicit GraphScene(ReseauNeural* reseau, QObject* parent = nullptr);

    void reconstruire();
    NeuroneNode* trouver_noeud(int id) const;
    void mettre_a_jour_synapses();
    void mettre_a_jour_poids();
    void sync_sorties_depuis_reseau();
    void sync_colonnes_dataset(const Dataset& dataset);
    void set_dataset(const Dataset* ds) { dataset_ptr_ = ds; }
    bool est_en_liaison() const { return mode_liaison_; }
    int source_liaison() const { return source_liaison_; }

    void set_placement_mode(const QVariantMap& data);
    void quitter_placement_mode();
    bool est_en_placement() const { return !placement_data_.isEmpty(); }
    const QVariantMap& placement_data() const { return placement_data_; }

signals:
    void neurone_selectionne(int neurone_id);
    void synapse_selectionne(int synapse_id);
    void selection_vide();
    void mode_liaison_change(bool actif, int source_id);
    void synapse_poids_change(int synapse_id, float nouveau_poids);
    void component_place(const QVariantMap& data, QPointF scene_pos);
    void exporter_selection();
    void scene_reconstruite();
    void mapping_colonne_change(int neurone_id, bool est_entree, int colonne);

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

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

    int drag_source_ = -1;
    QPointF drag_start_pos_;
    bool drag_active_ = false;
    QGraphicsLineItem* drag_line_ = nullptr;

    QVariantMap placement_data_;

    int pin_connection_source_ = -1;
    QGraphicsLineItem* pin_line_ = nullptr;
    const Dataset* dataset_ptr_ = nullptr;

    void nettoyer_etats_pins();
};

#endif
