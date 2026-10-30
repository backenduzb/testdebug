#include "windows-screenshot-provider.h"
#ifdef _WIN32
#include <QScreen>
#include <QGuiApplication>
#else
#include <QProcess>
#endif
QImage WindowsScreenshotProvider::capture(QString *error) {
#ifdef _WIN32
    auto *screen=QGuiApplication::primaryScreen(); if(!screen){if(error)*error="Ekran topilmadi";return{};} return screen->grabWindow(0).toImage();
#else
    QProcess grim;
    grim.start("grim", {"-"});
    if (!grim.waitForFinished(10000) || grim.exitStatus() != QProcess::NormalExit || grim.exitCode() != 0) {
        if (error) *error = "Hyprland screenshot failed: grim o'rnatilmagan yoki ishlamadi";
        return {};
    }
    const auto image = QImage::fromData(grim.readAllStandardOutput(), "PNG");
    if (image.isNull() && error) *error = "grim noto'g'ri PNG qaytardi";
    return image;
#endif
}
