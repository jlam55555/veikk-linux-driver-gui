#include "qpressurecurvepoint.h"
#include <QGraphicsSceneMouseEvent>

QPressureCurvePoint::QPressureCurvePoint(QPointF center, qreal size)
    : QGraphicsRectItem{QRectF{center.x()-size/2, center.y()-size/2,
                               size, size}},
      isDragging{false}, size{size} { }

void QPressureCurvePoint::moveCenter(QPointF newCenter) {
    QRectF newRect = rect();
    newRect.moveCenter(newCenter);
    setRect(newRect);
}

void QPressureCurvePoint::mousePressEvent(__attribute__((unused))
                                          QGraphicsSceneMouseEvent *evt) {
    isDragging = true;
}
void QPressureCurvePoint::mouseMoveEvent(QGraphicsSceneMouseEvent *evt) {
    if(!isDragging) return;
    setRect(QRectF{evt->pos().x()-size/2, evt->pos().y()-size/2, size, size});
    emit updatePressureCurve(nullptr);
}
void QPressureCurvePoint::mouseReleaseEvent(__attribute__((unused))
                                            QGraphicsSceneMouseEvent *evt) {
    isDragging = false;
}

// default bounding rect is too large (1.05x1.05)
QRectF QPressureCurvePoint::boundingRect() const {
    return rect();
}
