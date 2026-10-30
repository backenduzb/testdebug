#include "practice-controller.h"
PracticeController::PracticeController(IScreenshotProvider *c,IGeminiProvider *a,QObject*p):QObject(p),m_capture(c),m_ai(a){connect(m_ai,&IGeminiProvider::finished,this,[this](const AnalysisResult&r){if(!m_running)return; if(r.questionId==m_questionId||r.questionId.isEmpty())emit resultReady(r);});}
void PracticeController::start(){m_running=true;emit runningChanged(true);}
void PracticeController::analyzeNow(){if(!m_running)return;m_fingerprint.clear();questionChanged(m_questionId,m_context);}
void PracticeController::stop(){m_running=false;++m_revision;m_ai->cancel();emit runningChanged(false);}
void PracticeController::questionChanged(const QString&id,const QString&ctx){if(!m_running)return;m_questionId=id;m_context=ctx;QString e;auto image=m_capture->capture(&e);if(image.isNull()){emit diagnostic(e.isEmpty()?"Screenshot capture failed":e);return;}auto fp=Fingerprint::image(image,ctx+id);if(fp==m_fingerprint)return;m_fingerprint=fp;AnalysisRequest r{image,id,ctx};m_ai->analyze(r);}
