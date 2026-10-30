#pragma once
#include <QImage>
#include <QString>

struct AnalysisRequest { QImage image; QString questionId; QString context; };
struct AnalysisResult {
    QString questionId;
    QString selectedLabel;
    QString status = "error";
    double confidence = 0.0;
    QString error;
    bool isSuccess() const { return status == "success" && !selectedLabel.isEmpty(); }
};
