#pragma once

#include <QObject>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

// Receives right-button presses system-wide without consuming the mouse event.
class GlobalMouseTrigger final : public QObject {
    Q_OBJECT
public:
    explicit GlobalMouseTrigger(QObject *parent = nullptr);
    ~GlobalMouseTrigger() override;
    bool start();
    void stop();

signals:
    void triggered();

private slots:
    void emitTrigger();

#ifdef Q_OS_WIN
    static LRESULT CALLBACK mouseProc(int code, WPARAM message, LPARAM data);
    static GlobalMouseTrigger *s_instance;
    HHOOK m_hook = nullptr;
    ULONGLONG m_lastTrigger = 0;
#endif
};
