#include "responsevalidator.h"
#include <QJsonDocument>

AnalysisResult ResponseValidator::parse(const QByteArray &body, const QStringList &visibleLabels, const QString &currentQuestionId) {
    AnalysisResult r;
    QJsonParseError e{}; const auto doc = QJsonDocument::fromJson(body, &e);
    if (e.error != QJsonParseError::NoError || !doc.isObject()) { r.error = "Gemini javobi valid JSON emas"; return r; }
    const auto o = doc.object(); r.questionId = o.value("questionId").toString(); r.selectedLabel = o.value("selectedLabel").toString().trimmed(); r.status = o.value("status").toString(); r.confidence = o.value("confidence").toDouble();
    if (!currentQuestionId.isEmpty() && !r.questionId.isEmpty() && r.questionId != currentQuestionId) { r.status = "error"; r.error = "Eskirgan savol javobi rad etildi"; return r; }
    if (!QStringList{"success","ambiguous","unreadable","unsupported","error"}.contains(r.status)) { r.status = "error"; r.error = "Noma'lum response status"; return r; }
    if (r.status == "success" && !visibleLabels.isEmpty() && !visibleLabels.contains(r.selectedLabel)) { r.status = "error"; r.error = "Tanlangan label ko'rinadigan variantlar ichida yo'q"; }
    if (r.status == "success" && r.selectedLabel.isEmpty()) { r.status = "error"; r.error = "Muvaffaqiyatli javobda selectedLabel yo'q"; }
    return r;
}
