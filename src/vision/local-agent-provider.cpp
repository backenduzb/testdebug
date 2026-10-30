#include "local-agent-provider.h"
#include "core/responsevalidator.h"
#include <QBuffer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QTimer>
LocalAgentProvider::LocalAgentProvider(QString endpoint, QString token, QObject *parent):IGeminiProvider(parent),m_endpoint(std::move(endpoint)),m_token(std::move(token)){}
void LocalAgentProvider::cancel(){if(m_reply){m_reply->abort();m_reply->deleteLater();m_reply=nullptr;}}
void LocalAgentProvider::analyze(const AnalysisRequest &request){cancel();if(request.image.isNull()){AnalysisResult r;r.error="Rasm bo'sh";emit finished(r);return;}QByteArray bytes;QBuffer b(&bytes);b.open(QIODevice::WriteOnly);request.image.save(&b,"PNG");QJsonObject body{{"imageBase64",QString::fromLatin1(bytes.toBase64())},{"questionId",request.questionId},{"context",request.context}};QNetworkRequest net{QUrl(m_endpoint)};net.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");if(!m_token.isEmpty())net.setRawHeader("Authorization",("Bearer "+m_token).toUtf8());emit statusChanged("Local Codex/OpenCode service so'rovi yuborilmoqda...");m_reply=m_net.post(net,QJsonDocument(body).toJson(QJsonDocument::Compact));connect(m_reply,&QNetworkReply::finished,this,[this,request]{AnalysisResult r;if(m_reply->error()!=QNetworkReply::NoError)r.error="Local service: "+m_reply->errorString();else r=ResponseValidator::parse(m_reply->readAll(),{},request.questionId);emit finished(r);m_reply->deleteLater();m_reply=nullptr;});QTimer::singleShot(130000,m_reply,[this]{if(m_reply)m_reply->abort();});}
