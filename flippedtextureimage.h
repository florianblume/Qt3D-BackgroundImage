#ifndef FLIPPEDTEXTUREIMAGE_H
#define FLIPPEDTEXTUREIMAGE_H

#include <Qt3DRender/QPaintedTextureImage>

class FlippedTextureImage : public Qt3DRender::QPaintedTextureImage
{
public:
    FlippedTextureImage(Qt3DCore::QNode *parent = Q_NULLPTR);
    void paint(QPainter *painter) override;
};

#endif // FLIPPEDTEXTUREIMAGE_H
