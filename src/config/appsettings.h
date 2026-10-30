#pragma once
#include <QString>
struct AppSettings { QString apiKey; QString model = "gemini-3.8-flash"; int sampleIntervalMs = 1500; int fontSize = 12; int margin = 12; bool overlayEnabled = true; bool periodicSampling = false; void load(); void save() const; bool valid(QString *error=nullptr) const; };
