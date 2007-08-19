//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//


#ifndef MAINWINDOW_H
#define MAINWINDOW_H
 

#include <KXmlGuiWindow>
#include <KAction>

#include "ControlView.h"


class QLabel;

class ControlView;
class MarbleWidget;

 
class MainWindow : public KXmlGuiWindow
{
    Q_OBJECT

 public:
    explicit MainWindow(const QString& marbleDataPath = QString(), 
                        QWidget *parent=0);
    ~MainWindow();

    ControlView  *controlView()  const { return m_controlView; }
    MarbleWidget *marbleWidget() const { return m_controlView->marbleWidget();}

 private:
    void  setupActions();
    void  createStatusBar();

 public Q_SLOTS:
    void  showZoom(int zoom);

 private Q_SLOTS:
    void  exportMapScreenShot();
    void  printMapScreenShot();
    void  copyMap();
    void  showFullScreen( bool );
    void  showSideBar( bool );
    void  openFile();

 protected:
    virtual void saveProperties( KConfigGroup &group );
    virtual void readProperties( const KConfigGroup &group );

 private:
    QString  readMarbleDataPath();
    void  readSettings();
    void  writeSettings();

 private:
    // All the functionality is provided by this widget.
    ControlView  *m_controlView;

    // Zoom label for the statusbar.
    QLabel       *m_zoomLabel;

    // Actions for the GUI.
    KAction      *m_exportMapAction;
    KAction      *m_printMapAction;
    KAction      *m_copyMapAction;
    KAction      *m_sideBarAct;
    KAction      *m_fullScreenAct;
    KAction      *m_openAct;
};

 
#endif
