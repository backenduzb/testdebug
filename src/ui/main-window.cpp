#include "main-window.h"
#include "capture/windows-screenshot-provider.h"
#include "vision/local-agent-provider.h"
#include "core/practice-controller.h"
#include "core/trigger-server.h"
#include "overlay/overlay-window.h"
#include "settings-dialog.h"
#include <QPushButton>
#include <QLabel>
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QApplication>
#include <QCloseEvent>
#include <QStyle>
MainWindow::MainWindow(QWidget*p):QMainWindow(p){m_settings.load();auto*w=new QWidget(this);auto*layout=new QVBoxLayout(w);auto*buttons=new QHBoxLayout;auto*st=new QPushButton("Arm Practice",w);auto*sp=new QPushButton("Stop",w);auto*set=new QPushButton("Settings",w);buttons->addWidget(st);buttons->addWidget(sp);buttons->addWidget(set);layout->addLayout(buttons);m_status=new QLabel("Stopped",w);layout->addWidget(m_status);m_diag=new QPlainTextEdit(w);m_diag->setReadOnly(true);m_diag->setPlaceholderText("Diagnostics");layout->addWidget(m_diag);setCentralWidget(w);setWindowTitle("Gemini Vision Practice Assistant");auto*c=new WindowsScreenshotProvider(this);auto*a=new LocalAgentProvider(m_settings.localEndpoint,m_settings.localToken,this);m_controller=new PracticeController(c,a,this);m_overlay=new OverlayWindow; m_overlay->setFontPixels(m_settings.fontSize);m_overlay->setMargin(m_settings.margin);m_trigger=new TriggerServer(this);if(!m_trigger->start())m_diag->appendPlainText("Right-click trigger port 8766 unavailable");connect(m_trigger,&TriggerServer::triggered,this,[this]{if(m_controller){m_overlay->setText("Loading...");m_status->setText("Loading...");m_controller->analyzeNow();}});connect(st,&QPushButton::clicked,this,&MainWindow::start);connect(sp,&QPushButton::clicked,this,&MainWindow::stop);connect(set,&QPushButton::clicked,this,&MainWindow::settings);connect(m_controller,&PracticeController::diagnostic,this,[this](const QString&s){m_diag->appendPlainText(s);m_status->setText("Error");});connect(m_controller,&PracticeController::resultReady,this,[this](const AnalysisResult&r){QString t=r.isSuccess()?"Answer: "+r.selectedLabel:(r.status+": "+r.error);m_overlay->setText(t);m_status->setText(t);});if(QSystemTrayIcon::isSystemTrayAvailable()){m_tray=new QSystemTrayIcon(style()->standardIcon(QStyle::SP_ComputerIcon),this);m_tray->setToolTip("Gemini Vision Practice Assistant");auto*menu=new QMenu(this);menu->addAction("Show",this,&MainWindow::showMain);menu->addAction("Arm Practice",this,&MainWindow::start);menu->addAction("Stop",this,&MainWindow::stop);menu->addAction("Settings",this,&MainWindow::settings);menu->addSeparator();menu->addAction("Quit",this,&MainWindow::quitApp);m_tray->setContextMenu(menu);connect(m_tray,&QSystemTrayIcon::activated,this,[this](QSystemTrayIcon::ActivationReason reason){if(reason==QSystemTrayIcon::DoubleClick)showMain();});m_tray->show();}}
void MainWindow::closeEvent(QCloseEvent*event){if(m_tray&&m_tray->isVisible()){hide();event->ignore();}else{event->accept();}}
void MainWindow::showMain(){showNormal();raise();activateWindow();}
void MainWindow::quitApp(){m_controller->stop();m_overlay->hide();qApp->quit();}
void MainWindow::start(){m_overlay->show();m_controller->start();m_overlay->setText("Ready - right click");m_status->setText("Ready - right click");}
void MainWindow::stop(){m_controller->stop();m_overlay->hide();m_status->setText("Stopped");}
void MainWindow::settings(){SettingsDialog d(m_settings,this);if(d.exec()==QDialog::Accepted)m_diag->appendPlainText("Settings saved; restart practice to apply API changes.");}
