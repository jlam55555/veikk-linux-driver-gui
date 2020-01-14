#include "qpressurecurvescene.h"
#include <QPainter>
#include <QDebug>
#include <cmath>

QPressureCurveScene::QPressureCurveScene()
    : QGraphicsScene{} {
    QGraphicsRectItem *border;
    qint8 i;

    setSceneRect(QRectF{-0.1, -0.1, 1.2, 1.2});

    // set up points
    for(i=0; i<4; i++) {
        points[i] = new QPressureCurvePoint{QPointF{i/3.0, i/3.0}, 0.05};
        points[i]->setBrush(QBrush{Qt::red});
        points[i]->setPen(QPen{Qt::transparent});
        connect(points[i], SIGNAL(updatePressureCurve(qint16 *)),
                this, SLOT(updatePressureCurve(qint16 *)));
        addItem(points[i]);
    }

    // set up curve
    curve = addPath(QPainterPath{});
    curve->setPen(QPen{QBrush{Qt::blue}, 0.01});
    updatePressureCurve(nullptr);

    // set up border rect
    border = addRect(QRectF{0, 0, 1, 1});
    border->setPen(QPen{QBrush{Qt::green}, 0.01});
}

qreal QPressureCurveScene::curveFn(qreal x) {
    return (curveCoefs[0]
          + curveCoefs[1]*x
          + curveCoefs[2]*x*x
          + curveCoefs[3]*x*x*x)/100;
}

// called by form on update with newCoefs, called by moving points with nullptr
void QPressureCurveScene::updatePressureCurve(qint16 *newCoefs) {
    qreal x, y, granularity=0.02;
    QPainterPath path;

    if(newCoefs == nullptr)
        findCoefs();
    else {
        memcpy(curveCoefs, newCoefs, 4*sizeof(qint16));
        repositionControlPoints();
    }

    path.clear();
    for(x=0; x<1; x+=granularity) {
        y = curveFn(x);
        y = y>1?1:y<0?0:y;
        if(x<granularity)
            path.moveTo(x, y);
        else
            path.lineTo(x, y);
    }
    curve->setPath(path);
}

// solve the matrix equation a*x=b; c is the cofactor matrix
void QPressureCurveScene::findCoefs() {
    qreal a[4][4], c[4][4], b[4], x[4], tmp33[9], det;
    qint8 i, j, k, l, m;

    // generate coefficient matrix a
    for(i=0; i<4; i++) {
        for(j=0; j<4; j++)
            a[i][j] = pow(points[i]->rect().center().x(), j);
        b[i] = points[i]->rect().center().y();
    }

    // calculate cofactor matrix c
    for(i=0; i<4; i++)
        for(j=0; j<4; j++) {
            for(k=0, m=0; k<4; k++)
                for(l=0; l<4; l++)
                    if(k!=i && l!=j)
                        tmp33[m++] = a[k][l];
            c[i][j] = ((i+j)%2?-1:1) *
                    (tmp33[0]*(tmp33[4]*tmp33[8]-tmp33[5]*tmp33[7])
                    -tmp33[1]*(tmp33[3]*tmp33[8]-tmp33[5]*tmp33[6])
                    +tmp33[2]*(tmp33[3]*tmp33[7]-tmp33[4]*tmp33[6]));
        }

    // calculate determinant; exit if (approx.) zero
    for(det=0, i=0; i<4; i++)
        det += a[0][i] * c[0][i];
    if(fabs(det) < 0.00001)
        return;

    // calculate inverse (x=c.T*B), save as qint16-ready in curveCoefs
    for(i=0; i<4; i++) {
        for(j=0, x[i]=0; j<4; j++)
            x[i] += c[j][i] * b[j];
        curveCoefs[i] = qint16(x[i]/det*100);
    }

    emit updatePressureForm(curveCoefs);
}

void QPressureCurveScene::repositionControlPoints() {
    qint8 i;
    for(i=0; i<4; i++)
        points[i]->moveCenter(QPointF{i/3.0, curveFn(i/3.0)});
}
