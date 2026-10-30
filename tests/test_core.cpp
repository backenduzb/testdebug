#include <QtTest>
#include "core/responsevalidator.h"
#include "core/fingerprint.h"
#include "config/appsettings.h"
class CoreTests : public QObject {
    Q_OBJECT

private slots:
    void parsesValid()
    {
        auto result = ResponseValidator::parse(
            R"({"questionId":"q1","selectedLabel":"B","status":"success","confidence":.9})",
            {"A", "B"}, "q1");
        QCOMPARE(result.status, QString("success"));
        QCOMPARE(result.selectedLabel, QString("B"));
    }

    void rejectsLabel()
    {
        auto result = ResponseValidator::parse(
            R"({"selectedLabel":"Z","status":"success"})", {"A", "B"}, {});
        QCOMPARE(result.status, QString("error"));
    }

    void rejectsInvalid()
    {
        auto result = ResponseValidator::parse("no", {"A"}, {});
        QCOMPARE(result.status, QString("error"));
    }

    void fingerprintChanges()
    {
        QImage image(20, 20, QImage::Format_RGB32);
        image.fill(Qt::red);
        const auto redFingerprint = Fingerprint::image(image);
        image.fill(Qt::blue);
        QVERIFY(redFingerprint != Fingerprint::image(image));
    }

    void settings()
    {
        AppSettings settings;
        settings.apiKey = "x";
        QString error;
        QVERIFY(settings.valid(&error));
        settings.sampleIntervalMs = 1;
        QVERIFY(!settings.valid(&error));
    }
};
QTEST_MAIN(CoreTests)
#include "test_core.moc"
