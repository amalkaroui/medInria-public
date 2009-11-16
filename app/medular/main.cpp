/* main.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu Sep 17 08:29:18 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Nov  3 14:54:44 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 89
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include <QtGui>

#include "medMainWindow.h"

#include <dtkScript/dtkScriptInterpreter.h>
#include <dtkScript/dtkScriptInterpreterPool.h>
#include <dtkScript/dtkScriptManager.h>

#include <dtkGui/dtkInterpreter.h>

#include <medCore/medPluginManager.h>

#include <medGui/medStyle.h>

// Helper functions

QString readFile(const QString& path)
{
    QFile file(path);
    if(!file.open(QFile::ReadOnly))
        qDebug() << "Unable to read file" << path;

    QString contents = file.readAll();

    file.close();

    qDebug() << contents;

    return contents;
}

// main

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    application.setApplicationName("medular");
    application.setApplicationVersion("0.0.1");
    application.setOrganizationName("inria");
    application.setOrganizationDomain("fr");
    application.setStyleSheet(readFile(":/medular.qss"));
    
    // Initialize managers

    medPluginManager::instance()->initialize();
    dtkScriptManager::instance()->initialize();

    // Setting up main window

    medMainWindow mainwindow;
    if (application.arguments().contains("--fullscreen"))
        mainwindow.showFullScreen();
    else
        mainwindow.show();

    // Setting up interpreter

    dtkScriptInterpreter *interpreter = dtkScriptInterpreterPool::instance()->python();
    
    QObject::connect(interpreter, SIGNAL(stopped()), &application, SLOT(quit()));
    QObject::connect(&application, SIGNAL(aboutToQuit()), interpreter, SLOT(stop()));

#if defined(Q_OS_WIN)
    dtkInterpreter *ginterpreter = new dtkInterpreter;
    ginterpreter->registerInterpreter(interpreter);
    ginterpreter->readSettings();
    ginterpreter->show();

    QObject::connect(&application, SIGNAL(aboutToQuit()), ginterpreter, SLOT(writeSettings()));
#else
    interpreter->start();
#endif

    // Run event loop

    int status = application.exec();
    
    // Uninitialize managers

    //dtkPluginManager::instance()->uninitialize();
    //dtkScriptManager::instance()->uninitialize();
    
    return status;
}
