#pragma once
#include <QWidget>
class QLabel;
class OverlayWindow : public QWidget { Q_OBJECT public: explicit OverlayWindow(QWidget* p=nullptr); void setText(const QString&); void setFontPixels(int); void setMargin(int); protected: void showEvent(QShowEvent*) override; private: void reposition(); QLabel *m_label; int m_margin=12; };
