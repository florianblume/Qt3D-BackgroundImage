#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "flippedtextureimage.h"

#include <Qt3DExtras/QTorusMesh>
#include <Qt3DExtras/QPlaneMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QCamera>
#include <Qt3DCore/QTransform>
#include <QLabel>
#include <Qt3DRender/QLayerFilter>
#include <Qt3DRender/QFrameGraphNode>
#include <Qt3DRender/QViewport>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QLayer>
#include <Qt3DRender/QRenderSurfaceSelector>
#include <Qt3DRender/QClearBuffers>
#include <Qt3DExtras/QTextureMaterial>
#include <Qt3DRender/QTexture>
#include <Qt3DRender/QRenderSettings>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_3d_window = new Qt3DExtras::Qt3DWindow();
    m_3d_window->setGeometry(this->geometry());
    QRect rect = m_3d_window->geometry();

    Qt3DRender::QRenderSurfaceSelector *renderSurfaceSelector = new Qt3DRender::QRenderSurfaceSelector();
    renderSurfaceSelector->setSurface(m_3d_window);

    // First branch clearing the buffers
    Qt3DRender::QClearBuffers *clearBuffers = new Qt3DRender::QClearBuffers(renderSurfaceSelector);
    clearBuffers->setBuffers(Qt3DRender::QClearBuffers::ColorDepthBuffer);

    // Second branch
    // Custom framegraph for background image
    Qt3DRender::QLayerFilter *backgroundLayerFilter = new Qt3DRender::QLayerFilter(renderSurfaceSelector);
    Qt3DRender::QLayer *backgroundLayer = new Qt3DRender::QLayer(backgroundLayerFilter);
    backgroundLayerFilter->addLayer(backgroundLayer);
    Qt3DRender::QCameraSelector *cameraSelector = new Qt3DRender::QCameraSelector(backgroundLayer);
    Qt3DRender::QCamera *backgroundCamera = new Qt3DRender::QCamera(cameraSelector);
    backgroundCamera->lens()->setOrthographicProjection(-1, 1, -1, 1, 0.1f, 1000.f);
    backgroundCamera->setPosition(QVector3D(0, 0, 1));
    backgroundCamera->setViewCenter(QVector3D(0, 0, 0));
    backgroundCamera->setUpVector(QVector3D(0, 1, 0));
    cameraSelector->setCamera(backgroundCamera);

    // Third branch
    // Framegraph for objects
    Qt3DRender::QLayerFilter *objectsLayerFilter = new Qt3DRender::QLayerFilter(renderSurfaceSelector);
    Qt3DRender::QLayer *objectsLayer = new Qt3DRender::QLayer(objectsLayerFilter);
    objectsLayerFilter->addLayer(objectsLayer);
    Qt3DRender::QViewport *viewport = new Qt3DRender::QViewport(objectsLayer);
    Qt3DRender::QCameraSelector *objectsCameraSelector = new Qt3DRender::QCameraSelector(viewport);
    Qt3DRender::QCamera *objectsCamera = new Qt3DRender::QCamera(objectsCameraSelector);
    objectsCamera->lens()->setPerspectiveProjection(45.f, this->geometry().width() / (float) this->geometry().height(), 0.01f, 1000.f);
    objectsCamera->setPosition(QVector3D(0, 0, -10));
    objectsCamera->setViewCenter(QVector3D(0, 0, 0));
    objectsCamera->setUpVector(QVector3D(0, 1, 0));
    objectsCameraSelector->setCamera(objectsCamera);
    Qt3DRender::QClearBuffers *clearDepthBuffers = new Qt3DRender::QClearBuffers(objectsCamera);
    clearDepthBuffers->setBuffers(Qt3DRender::QClearBuffers::DepthBuffer);

    // Set the new framegraph
    m_3d_window->setActiveFrameGraph(renderSurfaceSelector);
    m_3d_window->renderSettings()->setRenderPolicy(Qt3DRender::QRenderSettings::Always);

    // Root
    Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity();
    m_3d_window->setRootEntity(rootEntity);

    // Background
    Qt3DCore::QEntity *planeEntity = new Qt3DCore::QEntity(rootEntity);
    Qt3DExtras::QPlaneMesh *planeMesh = new Qt3DExtras::QPlaneMesh(planeEntity);
    planeMesh->setHeight(2);
    planeMesh->setWidth(2);

    Qt3DExtras::QTextureMaterial *planeMaterial = new Qt3DExtras::QTextureMaterial(planeEntity);
    Qt3DRender::QTexture2D *planeTexture = new Qt3DRender::QTexture2D(planeMaterial);
    FlippedTextureImage *planeTextureImage = new FlippedTextureImage(planeTexture);
    planeTextureImage->setSize(QSize(this->geometry().width(), this->geometry().height()));
    planeTexture->addTextureImage(planeTextureImage);
    planeMaterial->setTexture(planeTexture);

    Qt3DCore::QTransform *planeTransform = new Qt3DCore::QTransform(planeEntity);
    planeTransform->setRotationX(90);
    planeTransform->setTranslation(QVector3D(0, 0, 0));

    planeEntity->addComponent(planeMesh);
    planeEntity->addComponent(planeMaterial);
    planeEntity->addComponent(planeTransform);
    planeEntity->addComponent(backgroundLayer);

    // Torus
    Qt3DCore::QEntity *torusEntity = new Qt3DCore::QEntity(rootEntity);
    Qt3DExtras::QTorusMesh *torusMesh = new Qt3DExtras::QTorusMesh(torusEntity);
    torusMesh->setSlices(50);
    torusMesh->setRings(50);
    torusMesh->setRadius(2.0f);
    Qt3DExtras::QPhongMaterial *torusMaterial = new Qt3DExtras::QPhongMaterial(torusEntity);
    torusMaterial->setAmbient(Qt::gray);
    Qt3DCore::QTransform *torusTransform = new Qt3DCore::QTransform(torusEntity);
    torusTransform->setTranslation(QVector3D(0, 0, 10));
    torusTransform->setRotationY(50);
    torusEntity->addComponent(torusTransform);
    torusEntity->addComponent(torusMesh);
    torusEntity->addComponent(torusMaterial);
    torusEntity->addComponent(objectsLayer);

    m_3d_window_container = QWidget::createWindowContainer(m_3d_window);
    centralWidget()->setGeometry(this->geometry());
    m_3d_window_container->setGeometry(this->geometry());
    m_3d_window_container->setParent(ui->centralWidget);
    planeTextureImage->update();

    timer.setInterval(100);
    connect(&timer, &QTimer::timeout, [planeTextureImage]() {
        planeTextureImage->update();
     });
    timer.start();
}

MainWindow::~MainWindow()
{
    delete ui;
}
