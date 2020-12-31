#include "qscreenmapscene.h"
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QScreen>
#include <QGraphicsSceneMouseEvent>

QScreenMapScene::QScreenMapScene(QScreen *screen)
    : QGraphicsScene{}, screen{screen} {
    QTimer *timer;

    setSceneRect(screen->geometry());

    // initialize screen pixmap
    pixmapItem = new QGraphicsPixmapItem{QPixmap{0, 0}};
    addItem(pixmapItem);

    // update screen every 2000ms
    updateScreenMapPreview();
    timer = new QTimer{this};
    connect(timer, &QTimer::timeout,
            this, &QScreenMapScene::updateScreenMapPreview);
    timer->start(2000);

    // set up selection rectangle
    isSelecting = false;
    selectionRect = addRect(screen->geometry());
    selectionRect->setBrush(QBrush{QColor{255, 0, 0, 50}});
}

void QScreenMapScene::updateScreenMapPreview() {
    QPixmap pixmap = screen->grabWindow(0).scaled(sceneRect().size().toSize(),
                                                  Qt::KeepAspectRatio,
                                                  Qt::FastTransformation);
    int32_t x1, y1, x2, y2;
    screen->geometry().getCoords(&x1, &y1, &x2, &y2);
    pixmapItem->setOffset(x1, y1);
    pixmapItem->setPixmap(pixmap);
}

void QScreenMapScene::updateScreenMapRect(QRect newScreenMap) {
    selectionRect->setRect(newScreenMap);
}

void QScreenMapScene::mousePressEvent(QGraphicsSceneMouseEvent *evt) {
    selectionStart = evt->scenePos();
    selectionRect->setRect(QRectF{selectionStart, selectionStart});
    isSelecting = true;
}
void QScreenMapScene::mouseMoveEvent(QGraphicsSceneMouseEvent *evt) {
    if(!isSelecting)
        return;
    selectionRect->setRect(QRectF{evt->scenePos(), selectionStart}
                           .normalized());
}
void QScreenMapScene::mouseReleaseEvent(__attribute__((unused))
                                        QGraphicsSceneMouseEvent *evt) {
    isSelecting = false;
    emit updateScreenMapForm(selectionRect->rect().toRect());
}
