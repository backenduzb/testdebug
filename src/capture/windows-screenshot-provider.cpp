#include "windows-screenshot-provider.h"
#ifdef _WIN32
#include <QScreen>
#include <QGuiApplication>
#endif
QImage WindowsScreenshotProvider::capture(QString *error) {
#ifdef _WIN32
    auto *screen=QGuiApplication::primaryScreen(); if(!screen){if(error)*error="Ekran topilmadi";return{};} return screen->grabWindow(0).toImage();
#else
    if(error)*error="Windows Graphics Capture faqat Windows buildida mavjud"; return{};
#endif
}
