#pragma once
#include "igemini-provider.h"
#include <QNetworkAccessManager>
class GeminiProvider final : public IGeminiProvider { Q_OBJECT public: GeminiProvider(QString key,QString model,QObject *parent=nullptr); void analyze(const AnalysisRequest&) override; void cancel() override; private: QString m_key,m_model; QNetworkAccessManager m_net; QNetworkReply *m_reply=nullptr; };
