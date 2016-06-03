/*=========================================================================
 medInria
 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.
=========================================================================*/

#pragma once

#include <medAbstractData.h>
#include <itkMorphologicalFiltersProcessBase.h>

#include <itkFiltersPluginExport.h>

class itkMorphologicalFiltersProcessBasePrivate;
class medAbstractData;

class ITKFILTERSPLUGIN_EXPORT itkFiltersCloseProcess : public itkMorphologicalFiltersProcessBase
{
    Q_OBJECT

public:
    itkFiltersCloseProcess(itkFiltersCloseProcess * parent = 0);
    itkFiltersCloseProcess(const itkFiltersCloseProcess& other);
    virtual ~itkFiltersCloseProcess(void);

    static bool registered ( void );
};

dtkAbstractProcess * createitkFiltersCloseProcess(void);
