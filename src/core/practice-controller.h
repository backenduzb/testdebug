#pragma once
#include "models.h"
#include "capture/iscreenshotprovider.h"
#include "vision/igemini-provider.h"
#include "core/fingerprint.h"
class PracticeController : public QObject { Q_OBJECT public: PracticeController(IScreenshotProvider*,IGeminiProvider*,QObject* p=nullptr); void start(); void stop(); public slots: void questionChanged(const QString &id={},const QString &context={}); void analyzeNow(); signals: void resultReady(const AnalysisResult&); void diagnostic(const QString&); void runningChanged(bool); private: IScreenshotProvider *m_capture; IGeminiProvider *m_ai; QString m_fingerprint,m_questionId,m_context; quint64 m_revision=0; bool m_running=false; };
