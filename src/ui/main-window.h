#pragma once
#include <QMainWindow>
#include "config/appsettings.h"
class PracticeController; class OverlayWindow; class QLabel; class QPlainTextEdit;
class MainWindow:public QMainWindow{Q_OBJECT public:explicit MainWindow(QWidget *p=nullptr);private slots:void start();void stop();void settings();private:AppSettings m_settings;PracticeController*m_controller=nullptr;OverlayWindow*m_overlay=nullptr;QLabel*m_status;QPlainTextEdit*m_diag;};
