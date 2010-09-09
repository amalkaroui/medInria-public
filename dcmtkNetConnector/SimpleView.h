#ifndef SimpleView_H
#define SimpleView_H

// includes
#include <QMainWindow>
#include <iostream>
#include <vector>

// Forward Qt class declarations
class Ui_SimpleView;
class QTreeWidgetItem;

class ServerThread;

class dcmtkEchoScu;
class dcmtkMoveScu;
class dcmtkStoreScu;
class dcmtkFindDataset;
class dcmtkFindScu;

class LoggerConsoleOutput;
class LoggerFileOutput;
class LoggerWidgetOutput;
class LoggerWidget;

class SimpleView : public QMainWindow
{
  Q_OBJECT

public:

  // Constructor/Destructor
  SimpleView(); 
  ~SimpleView();

protected:

  
   
protected slots:

  void search();
  void move(QTreeWidgetItem * item, int column);
  void testConnection();
  void applySettingsSlot();
  void fileAppender(int state);
  void shellAppender(int state);
  void widgetAppender(int state);
  void changeLogLevel(int index);
  void startServer();
  void stopServer();

private:

  void setConnectionParams();

  void storeSettings(int type);
  void retrieveSettings();
    
  // Designer form
  Ui_SimpleView *ui;

  dcmtkEchoScu*                 m_echoScu;
  dcmtkFindScu*                 m_findScu;
  dcmtkMoveScu*                 m_moveScu;
  dcmtkStoreScu*                m_storeScu;

  ServerThread*                 m_serverThread;
  
  std::vector<dcmtkFindDataset*> m_resultSet;
  std::vector<dcmtkFindDataset*>::iterator iter;

  std::string m_peerIP, m_peerTitle;
  std::string m_ourIP, m_ourTitle;
  std::string m_patientName;
  int m_peerPort, m_ourPort;

  LoggerConsoleOutput*          m_shellOut;
  LoggerFileOutput*             m_fileOut;
  LoggerWidgetOutput*           m_widgetOut;
  LoggerWidget*                 m_loggerWidget;
};

#endif // SimpleView_H
