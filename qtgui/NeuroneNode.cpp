#include "NeuroneNode.hpp"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QPen>
#include <QFont>

NeuroneNode::NeuroneNode(int id, const QString& nom, bool est_entree)
    : id_(id), nom_(nom), est_entree_(est_entree)
{
    setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsGeometryChanges);
    setAcceptHoverEvents(true);
    setCursor(Qt::OpenHandCursor);
    setZValue(1);
}

void NeuroneNode::set_nom(const QString& n) { nom_ = n; update(); }
void NeuroneNode::set_est_entree(bool e) { est_entree_ = e; update(); }
void NeuroneNode::set_sortie(double s) { sortie_ = s; update(); }

QPointF NeuroneNode::centre() const { return pos() + QPointF(RAYON, RAYON); }

QRectF NeuroneNode::boundingRect() const {
    return QRectF(0, 0, RAYON * 2, RAYON * 2) + QMarginsF(4, 4, 4, 4);
}

QPainterPath NeuroneNode::shape() const {
    QPainterPath p;
    p.addRoundedRect(QRectF(0, 0, RAYON * 2, RAYON * 2), 8, 8);
    return p;
}

void NeuroneNode::paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
                        QWidget*) {
    Q_UNUSED(option);
    painter->setRenderHint(QPainter::Antialiasing);

    // Couleur selon sortie : bleu (inactif) → blanc (50%) → rouge (actif)
    double v = std::clamp(sortie_, 0.0, 1.0);
    double R, G, B;
    if (v < 0.5) {
        double t = 2.0 * v;
        R = t; G = t; B = 1.0;
    } else {
        double t = 2.0 * (v - 0.5);
        R = 1.0; G = 1.0 - t; B = 1.0 - t;
    }
    QColor fond = QColor::fromRgbF(R, G, B);

    QRectF r(0, 0, RAYON * 2, RAYON * 2);
    painter->setBrush(isSelected() ? fond.lighter(130) : fond);
    painter->setPen(QPen(isSelected() ? Qt::blue : Qt::darkGray, 2));
    painter->drawRoundedRect(r, 8, 8);

    painter->setPen(Qt::black);
    QFont f = painter->font();
    f.setPointSize(9);
    f.setBold(false);
    painter->setFont(f);

    QString label = nom_;
    if (sortie_ > 0.0 || est_entree_)
        label += QString("\n%1").arg(sortie_, 0, 'f', 3);

    painter->drawText(r, Qt::AlignCenter | Qt::TextWordWrap, label);
}

QVariant NeuroneNode::itemChange(GraphicsItemChange change, const QVariant& value) {
    if (change == ItemPositionHasChanged)
        emit position_changed(id_, value.toPointF());
    return QGraphicsObject::itemChange(change, value);
}

void NeuroneNode::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    setCursor(Qt::ClosedHandCursor);
    QGraphicsObject::mousePressEvent(event);
}
