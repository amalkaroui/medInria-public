#ifndef QTDCMDATASOURCESERIETOOLBOX_H
#define QTDCMDATASOURCESERIETOOLBOX_H

#include <medToolBox.h>

class QtDcmPreviewWidget;
class QtDcmSerieInfoWidget;
class QtDcmImportWidget;

class qtdcmDataSourceSerieToolBoxPrivate;

class qtdcmDataSourceSerieToolBox : public medToolBox
{
    Q_OBJECT
public:
    qtdcmDataSourceSerieToolBox ( QWidget* parent = 0 );
    ~qtdcmDataSourceSerieToolBox();

    QtDcmPreviewWidget * getPreviewWidget();
    QtDcmSerieInfoWidget * getSerieInfoWidget();
    QtDcmImportWidget * getImportWidget();

private:
    qtdcmDataSourceSerieToolBoxPrivate * d;
};

#endif // QTDCMDATASOURCEPREVIEWTOOLBOX_H
