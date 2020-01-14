#ifndef QPRESSURECURVESCENE_H
#define QPRESSURECURVESCENE_H

#include "qpressurecurvepoint.h"
#include <QGraphicsScene>

class QPressureCurveScene : public QGraphicsScene {
    Q_OBJECT

public:
    QPressureCurveScene();

private:
    QPressureCurvePoint *points[4];
    QGraphicsPathItem *curve;
    qint16 curveCoefs[4];

    qreal curveFn(qreal x);
    void findCoefs();

private slots:
    void updatePressureCurve();

signals:
    void updatePressureForm(qint16 *);
};

#endif // QPRESSURECURVESCENE_H
