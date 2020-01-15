#ifndef QSCREENMAPSCENE_H
#define QSCREENMAPSCENE_H

#include <QGraphicsScene>
#include <QScreen>

class QScreenMapScene : public QGraphicsScene {
    Q_OBJECT

public:
    QScreenMapScene(QScreen *screen);

private:
    QScreen *screen;
    QGraphicsPixmapItem *pixmapItem;

    // selection rectangle
    // TODO: make this permanent, don't keep deleting it
    QGraphicsRectItem *selectionRect;
    QPointF selectionStart;
    bool isSelecting;

    void mousePressEvent(QGraphicsSceneMouseEvent *evt) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *evt) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *evt) override;

public slots:
    void updateScreenMapPreview();
    void updateScreenMapRect(QRect newScreenMap);

signals:
    void updateScreenMapForm(QRect newScreenMap);
};

#endif // QSCREENMAPSCENE_H
