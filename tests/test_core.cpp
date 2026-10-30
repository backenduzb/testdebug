#include <QtTest>
#include "core/responsevalidator.h"
#include "core/fingerprint.h"
#include "config/appsettings.h"
class CoreTests:public QObject{Q_OBJECT private slots:void parsesValid(){auto r=ResponseValidator::parse(R"({"questionId":"q1","selectedLabel":"B","status":"success","confidence":.9})",{"A","B"},"q1");QCOMPARE(r.status,QString("success"));QCOMPARE(r.selectedLabel,QString("B"));}void rejectsLabel(){auto r=ResponseValidator::parse(R"({"selectedLabel":"Z","status":"success"})",{"A","B"},{});QCOMPARE(r.status,QString("error"));}void rejectsInvalid(){auto r=ResponseValidator::parse("no",{"A"},{});QCOMPARE(r.status,QString("error"));}void fingerprintChanges(){QImage a(20,20,QImage::Format_RGB32);a.fill(Qt::red);auto x=Fingerprint::image(a);a.fill(Qt::blue);QVERIFY(x!=Fingerprint::image(a));}void settings(){AppSettings s;s.apiKey="x";QString e;QVERIFY(s.valid(&e));s.sampleIntervalMs=1;QVERIFY(!s.valid(&e));}};
QTEST_MAIN(CoreTests)
#include "test_core.moc"
