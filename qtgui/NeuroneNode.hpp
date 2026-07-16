#ifndef NEURONE_NODE_HPP
#define NEURONE_NODE_HPP

#include <QGraphicsObject>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

class NeuroneNode : public QGraphicsObject {
    Q_OBJECT
public:
    NeuroneNode(int id, const QString& nom, bool est_entree = false);

    int neurone_id() const { return id_; }
    void set_nom(const QString& n);
    void set_est_entree(bool e);
    void set_sortie(double s);
    double sortie() const { return sortie_; }
    QPointF centre() const;

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;
    QPainterPath shape() const override;

signals:
    void position_changed(int id, QPointF pos);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

private:
    int id_;
    QString nom_;
    bool est_entree_;
    double sortie_ = 0.0;
    static constexpr double RAYON = 28.0;
};

#endif
