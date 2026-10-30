#include "main-window.h"
#include "capture/windows-screenshot-provider.h"
#include "vision/local-agent-provider.h"
#include "core/practice-controller.h"
#include "overlay/overlay-window.h"
#include "settings-dialog.h"
#include <QPushButton>
#include <QLabel>
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
MainWindow::MainWindow(QWidget*p):QMainWindow(p){m_settings.load();auto*w=new QWidget(this);auto*layout=new QVBoxLayout(w);auto*buttons=new QHBoxLayout;auto*st=new QPushButton("Start Practice",w);auto*sp=new QPushButton("Stop",w);auto*set=new QPushButton("Settings",w);buttons->addWidget(st);buttons->addWidget(sp);buttons->addWidget(set);layout->addLayout(buttons);m_status=new QLabel("Stopped",w);layout->addWidget(m_status);m_diag=new QPlainTextEdit(w);m_diag->setReadOnly(true);m_diag->setPlaceholderText("Diagnostics");layout->addWidget(m_diag);setCentralWidget(w);setWindowTitle("Gemini Vision Practice Assistant");auto*c=new WindowsScreenshotProvider(this);auto*a=new LocalAgentProvider({},this);m_controller=new PracticeController(c,a,this);m_overlay=new OverlayWindow; m_overlay->setFontPixels(m_settings.fontSize);m_overlay->setMargin(m_settings.margin);connect(st,&QPushButton::clicked,this,&MainWindow::start);connect(sp,&QPushButton::clicked,this,&MainWindow::stop);connect(set,&QPushButton::clicked,this,&MainWindow::settings);connect(m_controller,&PracticeController::diagnostic,this,[this](const QString&s){m_diag->appendPlainText(s);m_status->setText("Error");});connect(m_controller,&PracticeController::resultReady,this,[this](const AnalysisResult&r){QString t=r.isSuccess()?"Answer: "+r.selectedLabel:(r.status+": "+r.error);m_overlay->setText(t);m_status->setText(t);});}
void MainWindow::start(){m_overlay->show();m_controller->start();m_status->setText("Running");}
void MainWindow::stop(){m_controller->stop();m_overlay->hide();m_status->setText("Stopped");}
void MainWindow::settings(){SettingsDialog d(m_settings,this);if(d.exec()==QDialog::Accepted)m_diag->appendPlainText("Settings saved; restart practice to apply API changes.");}
