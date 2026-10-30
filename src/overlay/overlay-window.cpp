#include "overlay-window.h"
#include <QLabel>
#include <QScreen>
#include <QGuiApplication>
#include <QVBoxLayout>
OverlayWindow::OverlayWindow(QWidget*p):QWidget(p,Qt::Tool|Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::WindowDoesNotAcceptFocus|Qt::WindowTransparentForInput){setAttribute(Qt::WA_TranslucentBackground);setAttribute(Qt::WA_ShowWithoutActivating);auto *box=new QVBoxLayout(this);box->setContentsMargins(10,6,10,6);m_label=new QLabel("Tayyor",this);m_label->setStyleSheet("color:#000000;background:transparent;");box->addWidget(m_label);setStyleSheet("background:transparent;");}
void OverlayWindow::setText(const QString&s){m_label->setText(s);adjustSize();reposition();}
void OverlayWindow::setFontPixels(int n){auto f=m_label->font();f.setPixelSize(n);m_label->setFont(f);adjustSize();}
void OverlayWindow::setMargin(int n){m_margin=n;reposition();}
void OverlayWindow::showEvent(QShowEvent*e){QWidget::showEvent(e);reposition();}
void OverlayWindow::reposition(){auto*s=screen();if(!s)s=QGuiApplication::primaryScreen();if(s)move(s->availableGeometry().right()-width()-m_margin,s->availableGeometry().bottom()-height()-m_margin);}
