#pragma once
#include "models.h"
#include <QJsonObject>
namespace ResponseValidator { AnalysisResult parse(const QByteArray &body, const QStringList &visibleLabels, const QString &currentQuestionId); }
