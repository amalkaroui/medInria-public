/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medSegmentationPlugin.h>

#include <medAlgorithmInitializer.h>
#include <medAnnotationInteractor.h>
#include <msegAlgorithmPaintToolbox.h>

#include <dtkLog/dtkLog.h>

// /////////////////////////////////////////////////////////////////
// PluginPrivate
// /////////////////////////////////////////////////////////////////

class medSegmentationPluginPrivate
{
public:
    // Class variables go here.
    static const char *s_Name;
};
const char * medSegmentationPluginPrivate::s_Name = "segmentationPlugin";

// /////////////////////////////////////////////////////////////////
// Plugin
// /////////////////////////////////////////////////////////////////

medSegmentationPlugin::medSegmentationPlugin(QObject *parent) : dtkPlugin(parent), d(new medSegmentationPluginPrivate)
{

}

medSegmentationPlugin::~medSegmentationPlugin()
{
    delete d;
    d = NULL;
}

bool medSegmentationPlugin::initialize()
{
    if(!medAnnotationInteractor::registered())
        qDebug() << "Unable to register medAnnotationInteractor";
    return medAlgorithmInitializer::initialize();
    }
    if(!AlgorithmPaintToolbox::registered())
    {
        qDebug() << "Unable to register msegAlgorithmPaintToolbox";
}

bool medSegmentationPlugin::uninitialize()
{
    return true;
}

QString medSegmentationPlugin::name() const
{
    return medSegmentationPluginPrivate::s_Name;
}

QString medSegmentationPlugin::description() const
{
    return tr("Segmentation plugin<br/>"
              "Manual painting of regions: inside, and outside."
              "<br/> Exports masks with three values: inside (1), outside (2)"
              "and other (0). You may leave out the outside colour "
              "for simple binary masks.<br/>"
              "Ctrl+z         Undo <br/>"
              "Ctrl+y         Redo <br/>"
              "Ctrl+c         Copy <br/>"
              "Ctrl+v         Paste<br/>"
              "Ctrl+Backspace Remove seed<br/>"
              "Ctrl+Up        Add brush size<br/>"
              "Ctrl+Down      Reduce brush size");
    
}

QString medSegmentationPlugin::version() const
{
    return MEDSEGMENTATIONPLUGIN_VERSION;
}

QString medSegmentationPlugin::contact() const
{
    return "John.Stark@inria.fr";
}

QStringList medSegmentationPlugin::authors() const
{
    QStringList list;
    list << "John Stark";
    return list;
}

QStringList medSegmentationPlugin::contributors() const
{
    QStringList list;
    list <<  QString::fromUtf8("Benoît Bleuzé")
          << "Olivier Commowick"
             << "Mathilde Merle <mathilde.merle@ihu-liryc.fr>";
    return list;
}

QString medSegmentationPlugin::identifier() const
{
    return medSegmentationPluginPrivate::s_Name;
}

QStringList medSegmentationPlugin::tags() const
{
    return QStringList();
}

QStringList medSegmentationPlugin::types() const
{
    return QStringList();
}

Q_EXPORT_PLUGIN2(medSegmentationPlugin, medSegmentationPlugin)
