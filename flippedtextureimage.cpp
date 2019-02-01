#include "flippedtextureimage.h"

#include <QPainter>
#include <QImage>
#include <QPaintDevice>

FlippedTextureImage::FlippedTextureImage(Qt3DCore::QNode *parent) : Qt3DRender::QPaintedTextureImage(parent) {

}

void FlippedTextureImage::paint(QPainter *painter) {
    qDebug() << "Updating texture image.";
    int w = painter->device()->width();
    int h = painter->device()->height();
    painter->drawImage(0, 0, QImage(":/test.png"));
}
