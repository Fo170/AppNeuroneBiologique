#include "SynapseEdge.hpp"
#include <QPainterPath>
#include <QPainterPathStroker>
#include <QPen>
#include <QFont>
#include <QtMath>

SynapseEdge::SynapseEdge(int id, NeuroneNode* source, NeuroneNode* target, float poids)
    : id_(id), source_(source), cible_(target), poids_(poids)
{
    setFlags(ItemIsSelectable);
    setAcceptHoverEvents(true);
    setZValue(0);
    ajuster();
}

void SynapseEdge::set_poids(float p) { poids_ = p; ajuster(); update(); }

void SynapseEdge::ajuster() {
    prepareGeometryChange();
    debut_ = source_->centre();
    fin_ = cible_->centre();
    update();
}

QRectF SynapseEdge::boundingRect() const {
    return QRectF(debut_, fin_).normalized().adjusted(-16, -16, 16, 16);
}

QPainterPath SynapseEdge::shape() const {
    QPainterPath p;
    p.moveTo(debut_);
    p.lineTo(fin_);
    QPainterPathStroker stroker;
    stroker.setWidth(10);
    return stroker.createStroke(p);
}

void SynapseEdge::paint(QPainter* painter, const QStyleOptionGraphicsItem*,
                        QWidget*) {
    painter->setRenderHint(QPainter::Antialiasing);

    QLineF ligne(debut_, fin_);
    double angle = std::atan2(-ligne.dy(), ligne.dx());
    float epaisseur = std::clamp(std::abs(poids_) * 3.0f, 1.0f, 6.0f);

    QColor couleur = (poids_ >= 0) ? QColor(40, 120, 40) : QColor(180, 40, 40);
    if (isSelected()) couleur = Qt::blue;

    painter->setPen(QPen(couleur, epaisseur, Qt::SolidLine, Qt::RoundCap));
    painter->drawLine(ligne);

    // Arrowhead
    double tete = 12.0;
    QPointF arrowP1 = fin_ - QPointF(
        std::cos(angle - M_PI / 6) * tete,
        -std::sin(angle - M_PI / 6) * tete);
    QPointF arrowP2 = fin_ - QPointF(
        std::cos(angle + M_PI / 6) * tete,
        -std::sin(angle + M_PI / 6) * tete);

    painter->setBrush(couleur);
    painter->setPen(Qt::NoPen);
    QPolygonF fleche;
    fleche << fin_ << arrowP1 << arrowP2;
    painter->drawPolygon(fleche);

    // Weight text at midpoint
    QPointF milieu = (debut_ + fin_) / 2.0;
    painter->setPen(Qt::black);
    QFont f = painter->font();
    f.setPointSize(9);
    f.setBold(true);
    painter->setFont(f);
    QString txt = QString::number(poids_, 'f', 3);
    QRectF tr = painter->boundingRect(QRectF(), Qt::AlignCenter, txt);
    // Background rectangle for readability
    painter->setBrush(QColor(255, 255, 255, 200));
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(milieu.x() - tr.width()/2 - 3,
                             milieu.y() - tr.height()/2 - 2,
                             tr.width() + 6, tr.height() + 4, 3, 3);
    painter->setPen(Qt::black);
    painter->drawText(QRectF(milieu.x() - tr.width()/2 - 3,
                             milieu.y() - tr.height()/2 - 2,
                             tr.width() + 6, tr.height() + 4),
                      Qt::AlignCenter, txt);
}
