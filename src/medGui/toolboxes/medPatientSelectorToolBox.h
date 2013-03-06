/* medPatientSelectorToolBox.h ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Feb 19 09:02:09 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Sun Jun 27 18:16:49 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 28
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#pragma once

#include "medGuiExport.h"
#include "medToolBox.h"

class medDataIndex;
class medPatientSelectorToolBoxPrivate;

class MEDGUI_EXPORT medPatientSelectorToolBox : public medToolBox
{
    Q_OBJECT

public:
     medPatientSelectorToolBox(QWidget *parent = 0);
    ~medPatientSelectorToolBox(void);


    QSet<medDataIndex> patientIndex(void) const;
    QSet<medDataIndex> patientIndex(QString patient) const;
    QSet<medDataIndex> patientIndex(int itemId) const;

signals:
    /** Emitted when the selected patient changes.
     *  \param Patients currently selected */
    void patientIndexChanged(const medDataIndex&);

public slots:
    void setPatientIndex(const medDataIndex& index);
    void setupDatabase(void);

protected slots:
    void onCurrentIndexChanged(int);
    //! Attaches new connections to the registered controller, and updates if needed.
    void onDbControllerRegistered(const QString&);
protected:
    int addChooseItem();
    void removeChooseItem();

    int addItem(const QString& item, const medDataIndex& data);

    void clear(void);

private:
    medPatientSelectorToolBoxPrivate *d;
};


