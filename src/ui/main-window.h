#pragma once
#include <QMainWindow>
#include "config/appsettings.h"
class PracticeController; class OverlayWindow; class TriggerServer; class QLabel; class QPlainTextEdit; class QSystemTrayIcon;
class MainWindow:public QMainWindow{Q_OBJECT public:explicit MainWindow(QWidget *p=nullptr);protected:void closeEvent(QCloseEvent*) override;private slots:void start();void stop();void settings();void showMain();void quitApp();private:AppSettings m_settings;PracticeController*m_controller=nullptr;OverlayWindow*m_overlay=nullptr;TriggerServer*m_trigger=nullptr;QSystemTrayIcon*m_tray=nullptr;QLabel*m_status;QPlainTextEdit*m_diag;};
