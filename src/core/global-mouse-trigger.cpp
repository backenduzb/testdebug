#include "global-mouse-trigger.h"

#ifdef Q_OS_WIN
#include <QMetaObject>
GlobalMouseTrigger *GlobalMouseTrigger::s_instance = nullptr;
#endif

GlobalMouseTrigger::GlobalMouseTrigger(QObject *parent) : QObject(parent) {}
GlobalMouseTrigger::~GlobalMouseTrigger() { stop(); }

bool GlobalMouseTrigger::start()
{
#ifdef Q_OS_WIN
    if (m_hook)
        return true;
    s_instance = this;
    m_hook = SetWindowsHookExW(WH_MOUSE_LL, &GlobalMouseTrigger::mouseProc,
                               GetModuleHandleW(nullptr), 0);
    if (!m_hook)
        s_instance = nullptr;
    return m_hook != nullptr;
#else
    return false;
#endif
}

void GlobalMouseTrigger::stop()
{
#ifdef Q_OS_WIN
    if (m_hook) {
        UnhookWindowsHookEx(m_hook);
        m_hook = nullptr;
    }
    if (s_instance == this)
        s_instance = nullptr;
#endif
}

void GlobalMouseTrigger::emitTrigger() { emit triggered(); }

#ifdef Q_OS_WIN
LRESULT CALLBACK GlobalMouseTrigger::mouseProc(int code, WPARAM message, LPARAM data)
{
    Q_UNUSED(data);
    if (code == HC_ACTION && message == WM_RBUTTONDOWN && s_instance) {
        const auto now = GetTickCount64();
        if (now - s_instance->m_lastTrigger >= 250) {
            s_instance->m_lastTrigger = now;
            QMetaObject::invokeMethod(s_instance, &GlobalMouseTrigger::emitTrigger,
                                      Qt::QueuedConnection);
        }
    }
    return CallNextHookEx(nullptr, code, message, data);
}
#endif
