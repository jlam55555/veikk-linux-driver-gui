/**
 * Draggable control point for cubic presssure mapping function.
 */

#ifndef QPRESSURECURVEPOINT_H
#define QPRESSURECURVEPOINT_H

#include <QGraphicsRectItem>

class QPressureCurvePoint
    : public QObject, public QGraphicsRectItem {
    Q_OBJECT

public:
    QPressureCurvePoint(QPointF center, qreal size);
    void moveCenter(QPointF newCenter);
    QRectF boundingRect() const override;

private:
    bool isDragging;
    const qreal size;

    void mousePressEvent(QGraphicsSceneMouseEvent *evt) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *evt) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *evt) override;

signals:
    void updatePressureCurve(qint16 *);
};

#endif // QPRESSURECURVEPOINT_H
