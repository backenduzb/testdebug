#pragma once
#include "iscreenshotprovider.h"
class WindowsScreenshotProvider final : public IScreenshotProvider { Q_OBJECT public: using IScreenshotProvider::IScreenshotProvider; QImage capture(QString *error=nullptr) override; };
