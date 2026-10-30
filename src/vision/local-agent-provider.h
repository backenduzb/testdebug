#pragma once
#include "igemini-provider.h"
#include <QNetworkAccessManager>
class LocalAgentProvider final : public IGeminiProvider { Q_OBJECT public: explicit LocalAgentProvider(QString endpoint = "http://127.0.0.1:8765/v1/analyze", QString token = {}, QObject *parent = nullptr); void analyze(const AnalysisRequest &) override; void cancel() override; private: QString m_endpoint, m_token; QNetworkAccessManager m_net; QNetworkReply *m_reply = nullptr; };
