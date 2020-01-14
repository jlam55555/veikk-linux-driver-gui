#ifndef QSCREENMAPSCENE_H
#define QSCREENMAPSCENE_H

#include <QGraphicsScene>

class QScreenMapScene : public QGraphicsScene {
    Q_OBJECT

public:
    QScreenMapScene();

private:
    QGraphicsPixmapItem *pixmapItem;

    // selection rectangle
    QGraphicsRectItem *selectionRect;
    QPointF selectionStart;
    bool isSelecting;

    void mousePressEvent(QGraphicsSceneMouseEvent *evt) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *evt) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *evt) override;

private slots:
    void updateScreenMapPreview();

signals:
    void updateScreenSize(QSize newScreenSize);
    void updateScreenMap(QRect newScreenMap);
};

#endif // QSCREENMAPSCENE_H
