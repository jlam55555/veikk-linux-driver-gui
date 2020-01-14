#include "qscreenmapscene.h"
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QScreen>
#include <QGuiApplication>
#include <QGraphicsSceneMouseEvent>

QScreenMapScene::QScreenMapScene()
    : QGraphicsScene{} {
    QTimer *timer;

    // TODO: make dynamic and resize on screen resize
    setSceneRect(QRectF{0, 0, 1920, 1080});

    // initialize screen pixmap
    pixmapItem = new QGraphicsPixmapItem{QPixmap{0, 0}};
    addItem(pixmapItem);

    // update screen every 2000ms
    updateScreenMapPreview();
    timer = new QTimer{this};
    connect(timer, SIGNAL(timeout()), this, SLOT(updateScreenMapPreview()));
    timer->start(2000);

    // set up selection rectangle
    isSelecting = false;
    selectionRect = nullptr;
}

void QScreenMapScene::updateScreenMapPreview() {
    QScreen *screen;
    QPixmap pixmap;

    // TODO: make sure first screen is always correct
    screen = QGuiApplication::screens().first();
    pixmap = screen->grabWindow(0).scaled(sceneRect().size().toSize(),
                                          Qt::KeepAspectRatio,
                                          Qt::FastTransformation);
    pixmapItem->setPixmap(pixmap);
}

void QScreenMapScene::mousePressEvent(QGraphicsSceneMouseEvent *evt) {
    if(selectionRect)
        delete selectionRect;
    selectionStart = evt->scenePos();
    selectionRect = addRect(QRectF{selectionStart, selectionStart});
    selectionRect->setBrush(QBrush{QColor{255, 0, 0, 50}});
    isSelecting = true;
}
void QScreenMapScene::mouseMoveEvent(QGraphicsSceneMouseEvent *evt) {
    if(!isSelecting || !selectionRect)
        return;
    selectionRect->setRect(QRectF{evt->scenePos(), selectionStart}
                           .normalized());
}
void QScreenMapScene::mouseReleaseEvent(__attribute__((unused))
                                        QGraphicsSceneMouseEvent *evt) {
    isSelecting = false;
    emit updateScreenMap(selectionRect->rect().toRect());
}
