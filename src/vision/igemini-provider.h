#pragma once
#include "core/models.h"
#include <QObject>
class IGeminiProvider : public QObject { Q_OBJECT public: using QObject::QObject; virtual void analyze(const AnalysisRequest &request)=0; virtual void cancel()=0; signals: void finished(const AnalysisResult &result); void statusChanged(const QString &status); };
