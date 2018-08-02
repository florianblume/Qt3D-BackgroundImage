# Qt3D-BackgroundImage

Demonstration of how to add a background image to a Qt3D scene that stretches over the whole surface.

## Framegraph

                QRenderSurfaceSelector
                          |
                          |
        ------------------------------------
        |                 |                |
    QClearBuffers    QLayerFilter      QLayerFilter
                          |                |
                    QCameraSelector   QCameraSelector
                                            |
                                   QClearBuffers::Depth
                
The second branch is the branch of the background image. It uses a QCamera with orthographic projection mode ranging from -1 to 1 in width an height and thus draws a plane of size (2, 2). The texture is drawn on the plane using the FlippedTextureImage, which uses a QPainter to draw the texture. A better solution would probably be to use a QTextureMaterial and mirror the image and rotate the plane. It is important to clear the depth buffers after drawing the plane because otherwise no objects will be drawn. Any object that is to be drawn needs to get the QLayer attached that was also set on the QLayerFilter of the objects framegraph branch.

Please do not hestitate to ask questions (via Issues) or if you have any suggestions or improvements (documentation etc) create a pull request.
