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
    QPressureCurvePoint(const QRectF &rect);
    QRectF boundingRect() const override;

private:
    bool isDragging;
    const qreal size;

    void mousePressEvent(QGraphicsSceneMouseEvent *evt) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *evt) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *evt) override;

signals:
    void updatePressureCurve();
};

#endif // QPRESSURECURVEPOINT_H
