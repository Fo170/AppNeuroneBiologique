#ifndef NEURONE_NODE_HPP
#define NEURONE_NODE_HPP

#include <QGraphicsObject>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

class NeuroneNode : public QGraphicsObject {
    Q_OBJECT
public:
    enum PinType { PIN_AUCUN, PIN_ENTREE, PIN_SORTIE };

    NeuroneNode(int id, const QString& nom, bool est_entree = false);

    int neurone_id() const { return id_; }
    bool est_entree() const { return est_entree_; }
    void set_nom(const QString& n);
    void set_est_entree(bool e);
    void set_sortie(double s);
    double sortie() const { return sortie_; }
    QPointF centre() const;

    QPointF pin_pos(PinType pin) const;
    PinType hit_test_pin(QPointF local_pos) const;

    void set_pin_actif(bool actif);
    void set_pin_cible(bool cible);
    void set_info_colonne(const QString& info);

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;
    QPainterPath shape() const override;

    static constexpr double RAYON = 28.0;
    static constexpr double PIN_R = 5.0;
    static constexpr double INFO_H = 14.0;

signals:
    void position_changed(int id, QPointF pos);
    void connection_demarree(int id, QPointF scene_pos);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

private:
    int id_;
    QString nom_;
    bool est_entree_;
    double sortie_ = 0.0;
    bool drag_pin_ = false;
    bool pin_survol_entree_ = false;
    bool pin_survol_sortie_ = false;
    bool pin_actif_ = false;
    bool pin_cible_ = false;
    QString info_colonne_;
};

#endif
