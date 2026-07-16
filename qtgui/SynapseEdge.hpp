#ifndef SYNAPSE_EDGE_HPP
#define SYNAPSE_EDGE_HPP

#include <QGraphicsObject>
#include "NeuroneNode.hpp"

class SynapseEdge : public QGraphicsObject {
    Q_OBJECT
public:
    SynapseEdge(int id, NeuroneNode* source, NeuroneNode* target, float poids);

    int synapse_id() const { return id_; }
    void set_poids(float p);
    float poids() const { return poids_; }
    void ajuster();
    NeuroneNode* noeud_source() const { return source_; }
    NeuroneNode* noeud_cible() const { return cible_; }

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;
    QPainterPath shape() const override;

private:
    int id_;
    NeuroneNode* source_;
    NeuroneNode* cible_;
    float poids_;
    QPointF debut_, fin_;
};

#endif
