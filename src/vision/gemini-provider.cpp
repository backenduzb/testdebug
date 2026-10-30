#include "gemini-provider.h"
#include "core/responsevalidator.h"
#include <QBuffer>
#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QTimer>
#include <QUrl>

static const char *instruction = "Analyze this image of an authorized practice question. Identify the question and all visible answer choices. Select the best-supported answer only when sufficient. Return JSON only: {questionId:string,selectedLabel:string,status:'success|ambiguous|unreadable|unsupported|error',confidence:number}. Do not provide reasoning.";
GeminiProvider::GeminiProvider(QString key, QString model, QObject *parent) : IGeminiProvider(parent), m_key(std::move(key)), m_model(std::move(model)) {}
void GeminiProvider::cancel() { if (m_reply) { m_reply->abort(); m_reply->deleteLater(); m_reply = nullptr; } }
void GeminiProvider::analyze(const AnalysisRequest &request) {
    cancel();
    if (request.image.isNull()) { AnalysisResult r; r.error = "Rasm bo'sh"; emit finished(r); return; }
    QByteArray imageData; QBuffer buffer(&imageData); buffer.open(QIODevice::WriteOnly); request.image.save(&buffer, "JPEG", 80);
    QJsonObject imagePart{{"inline_data", QJsonObject{{"mime_type", "image/jpeg"}, {"data", QString::fromLatin1(imageData.toBase64())}}}};
    QJsonObject textPart{{"text", QString::fromUtf8(instruction) + " Context: " + request.context}};
    QJsonObject contents{{"parts", QJsonArray{textPart, imagePart}}};
    QJsonObject body{{"contents", QJsonArray{contents}}, {"generationConfig", QJsonObject{{"responseMimeType", "application/json"}}}};
    QUrl url("https://generativelanguage.googleapis.com/v1beta/models/" + m_model + ":generateContent?key=" + QUrl::toPercentEncoding(m_key));
    QNetworkRequest networkRequest(url); networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    emit statusChanged("Gemini so'rovi yuborilmoqda..."); m_reply = m_net.post(networkRequest, QJsonDocument(body).toJson(QJsonDocument::Compact));
    connect(m_reply, &QNetworkReply::finished, this, [this, request] {
        AnalysisResult result;
        if (m_reply->error() != QNetworkReply::NoError) result.error = m_reply->error() == QNetworkReply::TimeoutError ? "Gemini timeout" : "Network xatosi: " + m_reply->errorString();
        else { const auto outer = QJsonDocument::fromJson(m_reply->readAll()).object(); const auto candidates = outer.value("candidates").toArray();
            if (!candidates.isEmpty()) { const auto parts = candidates.first().toObject().value("content").toObject().value("parts").toArray(); if (!parts.isEmpty()) result = ResponseValidator::parse(parts.first().toObject().value("text").toString().toUtf8(), {}, request.questionId); else result.error = "Gemini bo'sh javob qaytardi"; }
            else result.error = "Gemini candidates qaytarmadi"; }
        emit finished(result); m_reply->deleteLater(); m_reply = nullptr;
    });
    QTimer::singleShot(30000, m_reply, [this] { if (m_reply) m_reply->abort(); });
}
