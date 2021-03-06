/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkSmartPointer.h>

#include "itkBinaryThresholdImageFilter.h"
#include <itkConnectedComponentImageFilter.h>
#include "itkCommand.h"
#include "itkFiltersProcessBase_p.h"
#include "itkImage.h"
#include <itkRelabelComponentImageFilter.h>

#include <medMetaDataKeys.h>
#include <medUtilities.h>

class itkFiltersComponentSizeThresholdProcess;

class itkFiltersComponentSizeThresholdProcessPrivate : public itkFiltersProcessBasePrivate
{
public:
    itkFiltersComponentSizeThresholdProcessPrivate(itkFiltersComponentSizeThresholdProcess *q = 0) : itkFiltersProcessBasePrivate(q) {}
    itkFiltersComponentSizeThresholdProcessPrivate(const itkFiltersComponentSizeThresholdProcessPrivate& other) : itkFiltersProcessBasePrivate(other) {}

    virtual ~itkFiltersComponentSizeThresholdProcessPrivate(void) {}

    double minimumSize;

    template <class PixelType> int castToUInt3 ( void )
    {
        //we will later label the image so we don't care about precision.
        typedef itk::Image< PixelType, 3 > InputImageType;
        typedef itk::Image< unsigned int, 3 > OutputImageType;
        typedef itk::CastImageFilter< InputImageType, OutputImageType > CastFilterType;

        typename CastFilterType::Pointer  caster = CastFilterType::New();
        typename InputImageType::Pointer im = dynamic_cast< InputImageType*>((itk::Object*)(input->data()));
        caster->SetInput(im);
        caster->Update();

        input = medAbstractDataFactory::instance()->createSmartPointer ( "itkDataImageUInt3" );
        input->setData(caster->GetOutput());

        return DTK_SUCCEED;
    }

    template <class PixelType> int update ( void )
    {
        typedef itk::Image< PixelType, 3 > ImageType;
        typedef itk::Image< unsigned short, 3 > OutputImageType;

        typedef itk::ConnectedComponentImageFilter <ImageType, OutputImageType> ConnectedComponentFilterType;
        typename ConnectedComponentFilterType::Pointer connectedComponentFilter = ConnectedComponentFilterType::New();
        connectedComponentFilter->SetInput ( dynamic_cast<ImageType *> ( ( itk::Object* ) ( input->data() ) ) );
        connectedComponentFilter->Update();

        // RELABEL COMPONENTS according to their sizes (0:largest(background))
        typedef itk::RelabelComponentImageFilter<OutputImageType, OutputImageType> FilterType;
        typename FilterType::Pointer relabelFilter = FilterType::New();
        relabelFilter->SetInput(connectedComponentFilter->GetOutput());
        relabelFilter->SetMinimumObjectSize(minimumSize);
        relabelFilter->Update();

        // BINARY FILTER
        typedef itk::BinaryThresholdImageFilter <OutputImageType, OutputImageType> BinaryThresholdImageFilterType;
        typename BinaryThresholdImageFilterType::Pointer thresholdFilter
                = BinaryThresholdImageFilterType::New();
        thresholdFilter->SetInput(relabelFilter->GetOutput());
        thresholdFilter->SetUpperThreshold(0);
        thresholdFilter->SetInsideValue(0);
        thresholdFilter->SetOutsideValue(1);

        thresholdFilter->Update();

        callback = itk::CStyleCommand::New();
        callback->SetClientData ( ( void * ) this );
        callback->SetCallback ( itkFiltersProcessBasePrivate::eventCallback );

        connectedComponentFilter->AddObserver ( itk::ProgressEvent(), callback );

        output = medAbstractDataFactory::instance()->createSmartPointer ( "itkDataImageUShort3" );
        output->setData ( thresholdFilter->GetOutput() );


        QString newSeriesDescription = "connectedComponent " + QString::number(minimumSize);
        medUtilities::setDerivedMetaData(output, input, newSeriesDescription);

        return DTK_SUCCEED;
    }
};

DTK_IMPLEMENT_PRIVATE(itkFiltersComponentSizeThresholdProcess, itkFiltersProcessBase)
