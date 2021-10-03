#include "qscreenmapscene.h"
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QScreen>
#include <QGraphicsSceneMouseEvent>

QScreenMapScene::QScreenMapScene(QScreen *screen)
    : QGraphicsScene{}, screen{screen} {
    QTimer *timer;

    setSceneRect(screen->virtualGeometry());

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
    selectionRect = addRect(screen->virtualGeometry());
    selectionRect->setBrush(QBrush{QColor{255, 0, 0, 50}});
}

void QScreenMapScene::updateScreenMapPreview() {
    QPixmap pixmap = screen->grabWindow(0, -screen->geometry().x(), -screen->geometry().y(), screen->virtualSize().width(), screen->virtualSize().height()).scaled(sceneRect().size().toSize(),
                                                  Qt::KeepAspectRatio,
                                                  Qt::FastTransformation);
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
