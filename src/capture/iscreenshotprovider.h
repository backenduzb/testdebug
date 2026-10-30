#pragma once
#include <QObject>
#include <QImage>
class IScreenshotProvider : public QObject { Q_OBJECT public: using QObject::QObject; virtual ~IScreenshotProvider()=default; virtual QImage capture(QString *error=nullptr)=0; };
