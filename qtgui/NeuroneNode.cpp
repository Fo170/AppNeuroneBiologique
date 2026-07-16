#include "NeuroneNode.hpp"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsScene>
#include <QPen>
#include <QFont>
#include <cmath>

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

void NeuroneNode::set_pin_actif(bool actif) { pin_actif_ = actif; update(); }
void NeuroneNode::set_pin_cible(bool cible) { pin_cible_ = cible; update(); }
void NeuroneNode::set_info_colonne(const QString& info) { info_colonne_ = info; update(); }

QPointF NeuroneNode::centre() const { return pos() + QPointF(RAYON, RAYON); }

QPointF NeuroneNode::pin_pos(PinType pin) const {
    if (pin == PIN_ENTREE)
        return pos() + QPointF(0.0, RAYON);
    if (pin == PIN_SORTIE)
        return pos() + QPointF(RAYON * 2.0, RAYON);
    return centre();
}

NeuroneNode::PinType NeuroneNode::hit_test_pin(QPointF local_pos) const {
    double dx_e = local_pos.x() - 0.0;
    double dy_e = local_pos.y() - RAYON;
    double dx_s = local_pos.x() - RAYON * 2.0;
    double dy_s = local_pos.y() - RAYON;
    if (std::sqrt(dx_e * dx_e + dy_e * dy_e) <= PIN_R + 2.0)
        return PIN_ENTREE;
    if (std::sqrt(dx_s * dx_s + dy_s * dy_s) <= PIN_R + 2.0)
        return PIN_SORTIE;
    return PIN_AUCUN;
}

QRectF NeuroneNode::boundingRect() const {
    return QRectF(-PIN_R - 2, -PIN_R - 2,
                  RAYON * 2 + (PIN_R + 2) * 2,
                  RAYON * 2 + (PIN_R + 2) * 2 + INFO_H);
}

QPainterPath NeuroneNode::shape() const {
    QPainterPath p;
    QRectF body(0, 0, RAYON * 2, RAYON * 2);
    p.addRoundedRect(body, 8, 8);
    if (!info_colonne_.isEmpty())
        p.addRect(QRectF(0, RAYON * 2, RAYON * 2, INFO_H));
    p.addEllipse(QPointF(0.0, RAYON), PIN_R + 3, PIN_R + 3);
    p.addEllipse(QPointF(RAYON * 2, RAYON), PIN_R + 3, PIN_R + 3);
    return p;
}

void NeuroneNode::paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
                        QWidget*) {
    Q_UNUSED(option);
    painter->setRenderHint(QPainter::Antialiasing);

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

    // Output pin (right)
    double sortie_r = pin_actif_ ? PIN_R * 1.8 : (pin_survol_sortie_ ? PIN_R * 1.4 : PIN_R);
    QColor sc = pin_actif_ ? QColor(255, 160, 40) : (pin_survol_sortie_ ? QColor(255, 200, 100) : QColor(200, 80, 40));
    painter->setBrush(sc);
    painter->setPen(QPen(sc.darker(130), 1));
    painter->drawEllipse(QPointF(RAYON * 2.0, RAYON), sortie_r, sortie_r);

    // Input pin (left)
    double entree_r = pin_cible_ ? PIN_R * 1.8 : (pin_survol_entree_ ? PIN_R * 1.4 : PIN_R);
    QColor ec = pin_cible_ ? QColor(60, 220, 60) : (pin_survol_entree_ ? QColor(130, 200, 255) : QColor(40, 100, 200));
    painter->setBrush(ec);
    painter->setPen(QPen(ec.darker(130), 1));
    painter->drawEllipse(QPointF(0.0, RAYON), entree_r, entree_r);

    // Label
    painter->setPen(Qt::black);
    QFont f = painter->font();
    f.setPointSize(9);
    f.setBold(false);
    painter->setFont(f);

    QString label = nom_;
    if (sortie_ > 0.0 || est_entree_)
        label += QString("\n%1").arg(sortie_, 0, 'f', 3);

    painter->drawText(r, Qt::AlignCenter | Qt::TextWordWrap, label);

    // Column info banner below the body
    if (!info_colonne_.isEmpty()) {
        QRectF info_r(0, RAYON * 2, RAYON * 2, INFO_H);
        painter->setBrush(QColor(220, 220, 240));
        painter->setPen(Qt::NoPen);
        painter->drawRect(info_r);
        painter->setPen(QColor(60, 60, 80));
        f.setPointSize(8);
        painter->setFont(f);
        painter->drawText(info_r, Qt::AlignCenter, info_colonne_);
    }
}

QVariant NeuroneNode::itemChange(GraphicsItemChange change, const QVariant& value) {
    if (change == ItemPositionHasChanged)
        emit position_changed(id_, value.toPointF());
    return QGraphicsObject::itemChange(change, value);
}

void NeuroneNode::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (event->button() == Qt::LeftButton && hit_test_pin(event->pos()) == PIN_SORTIE) {
        drag_pin_ = true;
        set_pin_actif(true);
        emit connection_demarree(id_, mapToScene(event->pos()));
        event->accept();
        return;
    }
    setCursor(Qt::ClosedHandCursor);
    drag_pin_ = false;
    QGraphicsObject::mousePressEvent(event);
}

void NeuroneNode::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    if (drag_pin_) {
        event->accept();
        return;
    }
    QGraphicsObject::mouseMoveEvent(event);
}

void NeuroneNode::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    if (drag_pin_) {
        drag_pin_ = false;
        event->accept();
        return;
    }
    setCursor(Qt::OpenHandCursor);
    QGraphicsObject::mouseReleaseEvent(event);
}

void NeuroneNode::hoverMoveEvent(QGraphicsSceneHoverEvent* event) {
    PinType pin = hit_test_pin(event->pos());
    bool sur_entree = (pin == PIN_ENTREE);
    bool sur_sortie = (pin == PIN_SORTIE);

    if (sur_entree != pin_survol_entree_ || sur_sortie != pin_survol_sortie_) {
        pin_survol_entree_ = sur_entree;
        pin_survol_sortie_ = sur_sortie;
        setCursor((sur_entree || sur_sortie) ? Qt::CrossCursor : Qt::OpenHandCursor);
        update();
    }
    QGraphicsObject::hoverMoveEvent(event);
}

void NeuroneNode::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
    pin_survol_entree_ = false;
    pin_survol_sortie_ = false;
    setCursor(Qt::OpenHandCursor);
    update();
    QGraphicsObject::hoverLeaveEvent(event);
}
