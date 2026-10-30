#pragma once
#include "iscreenshotprovider.h"
class MockScreenshotProvider final : public IScreenshotProvider { Q_OBJECT public: using IScreenshotProvider::IScreenshotProvider; QImage nextImage; QString nextError; QImage capture(QString *error=nullptr) override { if(!nextError.isEmpty()){if(error)*error=nextError;return{};} return nextImage; } };
