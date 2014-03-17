/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <v3dViewAnnotationInteractor.h>

#include <medAbstractData.h>
#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkLog/dtkLog.h>

#include <medAnnotationData.h>
#include <medSeedPointAnnotationData.h>
#include <medImageMaskAnnotationData.h>

#include <medImageViewFactory.h>

#include <v3dViewAnnIntSeedPointHelper.h>
#include <v3dViewAnnIntImageMaskHelper.h>

#include <vtkProperty2D.h>
#include <vtkRenderer.h>
#include <vtkSeedWidget.h>
#include <vtkSeedRepresentation.h>
#include <vtkSphereHandleRepresentation.h>
#include <vtkSmartPointer.h>

#include <medVtkViewBackend.h>
#include <vtkImageView2D.h>
#include <vtkImageView3D.h>


// pImpl
class v3dViewAnnotationInteractorPrivate
{
public:
    typedef QVector<v3dViewAnnIntHelper *> HelperVecType;
    HelperVecType helpers;

    typedef std::map< medAnnotationData*, HelperVecType::size_type> HelperMap;
    HelperMap  dataToHelperIdMap;

    typedef QSet< dtkSmartPointer<medAnnotationData> > AnnotationHash;
    AnnotationHash installedAnnotations;


    medAbstractImageView *medVtkView;
    vtkImageView2D *view2d;
    vtkImageView3D *view3d;
    vtkRenderWindow *render;

    medAbstractData *imageData;

    QWidget* toolbox;
    QWidget* toolbar;
    QImage thumbnail;
};

// Implementation
v3dViewAnnotationInteractor::v3dViewAnnotationInteractor(medAbstractImageView* parent):
    medAbstractImageViewInteractor(parent), d(new v3dViewAnnotationInteractorPrivate)
{
    d->helpers.push_back(new v3dViewAnnIntSeedPointHelper(this));
    d->helpers.push_back(new v3dViewAnnIntImageMaskHelper(this));

    d->medVtkView = parent;
    medVtkViewBackend* backend = static_cast<medVtkViewBackend*>(parent->backend());
    d->view2d = backend->view2D;
    d->view3d = backend->view3D;

    d->imageData = NULL;

    d->toolbox = NULL;
    d->toolbar = NULL;

    connect(parent, SIGNAL(currentLayerChanged()), this, SLOT(enableWindowLevelInteraction()));
}

v3dViewAnnotationInteractor::~v3dViewAnnotationInteractor()
{
    qDeleteAll(d->helpers);
    delete d;
    d = NULL;
}

QStringList v3dViewAnnotationInteractor::dataHandled()
{
    QStringList d = QStringList() << "medImageMaskAnnotationData";
    return  d;
}

QStringList v3dViewAnnotationInteractor::handled() const
{
    return v3dViewAnnotationInteractor::dataHandled();
}

QString v3dViewAnnotationInteractor::description() const
{
    return tr("Interactor displaying annotations");
}

QString v3dViewAnnotationInteractor::identifier() const
{
    return s_identifier();
}

bool v3dViewAnnotationInteractor::registered()
{
    medImageViewFactory *factory = medImageViewFactory::instance();
    return factory->registerInteractor<v3dViewAnnotationInteractor>("v3dViewAnnotationInteractor",
                                                                  QStringList () << "medVtkView" <<
                                                                  v3dViewAnnotationInteractor::dataHandled());
}

QString v3dViewAnnotationInteractor::s_identifier()
{
    return "v3dViewAnnotationInteractor";
}


medAbstractImageView * v3dViewAnnotationInteractor::getView()
{
    return d->medVtkView;
}

void v3dViewAnnotationInteractor::setData(medAbstractData *data)
{
    if ( d->imageData ) {
        disconnect(d->imageData, SIGNAL(dataModified(medAbstractData*)), this, SLOT(onDataModified(medAbstractData*)) );
        // Remove annotations
        foreach( dtkSmartPointer<medAnnotationData> key,  d->installedAnnotations ) {
            if ( key->parentData() == data ) {
                this->removeAnnotation( key );
            }
        }
    }
    
    d->imageData = data;

    if (d->imageData)
    {
        connect( d->imageData, SIGNAL(attachedDataAdded(medAttachedData*)), this,  SLOT(onAttachedDataAdded(medAttachedData*)) );
        connect( d->imageData, SIGNAL(attachedDataRemoved(medAttachedData*)), this,  SLOT(onAttachedDataRemoved(medAttachedData*)) );
        
        typedef itk::Image<unsigned char, 3> MaskType;
        medImageMaskAnnotationData *maskAnnData = dynamic_cast<medImageMaskAnnotationData*>(data);
        if (MaskType* image = dynamic_cast<MaskType*>((itk::Object*)(maskAnnData->maskData()->data())))
        {
            d->view2d->SetITKInput(image, d->medVtkView->layer(d->imageData));
            d->view3d->SetITKInput(image, d->medVtkView->layer(d->imageData));
        }

        medAnnotationData *annItem = qobject_cast<medAnnotationData*>(data);
        onAttachedDataAdded(annItem);

        // Add any existing annotations on the data to our list.
        QList<medAttachedData*> attached = d->imageData->attachedData();
        foreach( medAttachedData* item,  attached ) {
            medAnnotationData *annItem = qobject_cast<medAnnotationData*>(item);
            if (annItem) {
                this->onAttachedDataAdded(annItem);
            }
        }
    }
}

void v3dViewAnnotationInteractor::removeData()
{
//    d->view2d->RemoveLayer(d->medVtkView->layer(d->imageData));
//    d->view3d->RemoveLayer(d->medVtkView->layer(d->imageData));
//    if(d->medVtkView->is2D())
//        d->view2d->Render();
//    else
//        d->view3d->Render();
}


bool v3dViewAnnotationInteractor::showIn2dView() const
{
    return true;
}

bool v3dViewAnnotationInteractor::showIn3dView() const
{
    return true;
}

bool v3dViewAnnotationInteractor::isInSlice( const QVector3D & slicePoint, const QVector3D & sliceNormal,
                                            qreal thickness ) const
{
    return true;
}


void v3dViewAnnotationInteractor::onDataModified( medAbstractData* data )
{
    medAnnotationData * annData = qobject_cast<medAnnotationData*>(data);
    if (!annData)
        return;

    v3dViewAnnotationInteractorPrivate::HelperMap::iterator it = d->dataToHelperIdMap.find(annData);

    if ( it == d->dataToHelperIdMap.end() )
        return;

    v3dViewAnnIntHelper * helper = d->helpers.at( it->second );
    helper->annotationModified(annData);
}

bool v3dViewAnnotationInteractor::onAddAnnotation( medAnnotationData * annItem )
{
    typedef v3dViewAnnotationInteractorPrivate::HelperVecType::size_type IndexType;
    bool isAdded = false;
    for (IndexType i(0), end(d->helpers.size()); i<end; ++i){
        v3dViewAnnIntHelper * helper = d->helpers[i];
        if ( helper->addAnnotation(annItem) ) {
            isAdded = true;
            d->dataToHelperIdMap[annItem] = i;
            break;
        }
    }
    return isAdded;
}

void v3dViewAnnotationInteractor::onRemoveAnnotation( medAnnotationData * annData )
{
    v3dViewAnnotationInteractorPrivate::HelperMap::iterator it = d->dataToHelperIdMap.find(annData);
    if ( it == d->dataToHelperIdMap.end() )
        return;

    typedef v3dViewAnnotationInteractorPrivate::HelperVecType::size_type IndexType;
    IndexType iHelper = it->second;

    d->helpers[iHelper]->removeAnnotation( annData );
    d->dataToHelperIdMap.erase(annData);
}

void v3dViewAnnotationInteractor::addAnnotation( medAnnotationData * annData )
{
    if ( d->installedAnnotations.contains(annData) ) {
        return;
    }
    
    bool isAdded = this->onAddAnnotation(annData);
    
    if ( isAdded ) {
        d->installedAnnotations.insert( annData );
        
        connect(annData, SIGNAL(dataModified(medAbstractData*)), this, SLOT(onDataModified(medAbstractData*)) );
    }
}

void v3dViewAnnotationInteractor::removeAnnotation( medAnnotationData * annData )
{
    
}

void v3dViewAnnotationInteractor::onAttachedDataAdded( medAttachedData* data )
{
    medAnnotationData * annData = qobject_cast<medAnnotationData *>(data);
    if ( annData ) {
        this->addAnnotation(annData);
    }
}

void v3dViewAnnotationInteractor::onAttachedDataRemoved( medAttachedData* data )
{
    medAnnotationData * annData = qobject_cast<medAnnotationData *>(data);
    if ( annData ) {
        this->removeAnnotation(annData);
    }
}

QPointF v3dViewAnnotationInteractor::worldToScene( const QVector3D & worldVec ) const
{
    return d->medVtkView->mapWorldToDisplayCoordinates( worldVec );
}

QVector3D v3dViewAnnotationInteractor::sceneToWorld( const QPointF & sceneVec ) const
{
    return d->medVtkView->mapDisplayToWorldCoordinates( sceneVec );
}

QVector3D v3dViewAnnotationInteractor::viewUp() const
{
    return d->medVtkView->viewUp( );
}

bool v3dViewAnnotationInteractor::isPointInSlice(const QVector3D & testPoint, const QVector3D & slicePoint,
                                                 const QVector3D & sliceNormal, qreal thickness ) const
{
    qreal distanceToPlane = QVector3D::dotProduct( (testPoint - slicePoint), sliceNormal );
    
    distanceToPlane = distanceToPlane / sliceNormal.length();
    
    return ( distanceToPlane >= -thickness ) && ( distanceToPlane < thickness );
}

bool v3dViewAnnotationInteractor::isPointInCurrentSlice( const QVector3D & testPoint ) const
{
    return this->isPointInSlice(testPoint, d->medVtkView->viewCenter(), d->medVtkView->viewPlaneNormal(),
                                0.5*d->medVtkView->sliceThickness());
}

void v3dViewAnnotationInteractor::setVisibility(bool /*visible*/)
{
    //TODO
}

bool v3dViewAnnotationInteractor::visibility() const
{
    //TODO
    return true;
}

QWidget* v3dViewAnnotationInteractor::toolBarWidget()
{
    if(!d->toolbar)
    {
        d->toolbar = new QWidget;
    }
    return d->toolbar;
}

QWidget* v3dViewAnnotationInteractor::toolBoxWidget()
{
    if(!d->toolbox)
    {
        d->toolbox = new QWidget;
    }
    return d->toolbox;
}

QWidget* v3dViewAnnotationInteractor::layerWidget()
{
    return new QWidget();
}

void v3dViewAnnotationInteractor::moveToSlice(int slice)
{
    int zslice = d->view2d->GetSlice();

    if(slice!=zslice)
      d->view2d->SetSlice ( slice );

    if(d->medVtkView->is2D())
        d->view2d->Render();
    else
        d->view3d->Render();
}

void v3dViewAnnotationInteractor::moveToSliceAtPosition(const QVector3D &position)
{
    double pos[3];
    pos[0] = position.x();
    pos[1] = position.y();
    pos[2] = position.z();

    d->view2d->SetCurrentPoint ( pos );
    d->view2d->UpdateCursorPosition(pos);

    if(d->medVtkView->is2D())
    {
        unsigned int zslice = d->view2d->GetSlice();
        //d->slicingParameter->setValue ( zslice );
    }
}

void v3dViewAnnotationInteractor::setWindowLevel (double &window, double &level)
{

}

void v3dViewAnnotationInteractor::windowLevel(double &window, double &level)
{

}

void v3dViewAnnotationInteractor::setOpacity(double opacity)
{
    d->view3d->SetOpacity (opacity, d->medVtkView->layer(d->imageData));
    d->view2d->SetOpacity (opacity, d->medVtkView->layer(d->imageData));
}

double v3dViewAnnotationInteractor::opacity() const
{
     //double opacity = static_cast<double>(d->opacityParam->value()) / 100.0;
     //return opacity;
    return 100;
}

QImage v3dViewAnnotationInteractor::generateThumbnail(const QSize &size)
{
  /*  int w(size.width()), h(size.height());

    d->view2d->SetBackground(0.0, 0.0, 0.0);
    d->view2d->CursorFollowMouseOff();
    d->view2d->ShowImageAxisOff();
    d->view2d->ShowScalarBarOff();
    d->view2d->ShowAnnotationsOff();
    d->view2d->ShowRulerWidgetOff();

    d->render->SetOffScreenRendering(1);

    d->medVtkView->viewWidget()->resize(w,h);
    d->render->vtkRenderWindow::SetSize(w,h);
    d->view2d->Reset();
    d->view2d->Render();

    QGLWidget *glWidget = dynamic_cast<QGLWidget *>(d->medVtkView->viewWidget());
    d->thumbnail = glWidget->grabFrameBuffer();

    d->render->SetOffScreenRendering(0);

    return d->thumbnail;*/
    QImage thumnbail;
    return thumnbail;
}

QList<medAbstractParameter*> v3dViewAnnotationInteractor::parameters()
{
    return QList<medAbstractParameter*>();
}


void v3dViewAnnotationInteractor::enableWindowLevelInteraction()
{
    if(d->medVtkView->currentLayer() == d->medVtkView->layer(d->imageData))
    {
        d->view2d->SetLeftButtonInteractionStyle ( vtkInteractorStyleImageView2D::InteractionTypeNull );
    }
    else
    {
        d->view2d->SetLeftButtonInteractionStyle ( vtkInteractorStyleImageView2D::InteractionTypeWindowLevel );
    }
}

////////////////////////////
// v3dViewAnnIntHelper
////////////////////////////

v3dViewAnnIntHelper::v3dViewAnnIntHelper( v3dViewAnnotationInteractor * annInt)
    : m_v3dViewAnnInt( annInt )
{

}

v3dViewAnnIntHelper::~v3dViewAnnIntHelper()
{

}


