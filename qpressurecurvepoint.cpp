#include "qpressurecurvepoint.h"
#include <QGraphicsSceneMouseEvent>

QPressureCurvePoint::QPressureCurvePoint(const QRectF &rect)
    : QGraphicsRectItem{rect}, isDragging{false}, size{rect.width()} { }

void QPressureCurvePoint::mousePressEvent(__attribute__((unused))
                                          QGraphicsSceneMouseEvent *evt) {
    isDragging = true;
}
void QPressureCurvePoint::mouseMoveEvent(QGraphicsSceneMouseEvent *evt) {
    if(!isDragging) return;
    setRect(QRectF{evt->pos().x()-size/2, evt->pos().y()-size/2, size, size});
    emit updatePressureCurve();
}
void QPressureCurvePoint::mouseReleaseEvent(__attribute__((unused))
                                            QGraphicsSceneMouseEvent *evt) {
    isDragging = false;
}

// default bounding rect is too large (1.05x1.05)
QRectF QPressureCurvePoint::boundingRect() const {
    return rect();
}
