#pragma once
#include <QString>
struct AppSettings { QString apiKey; QString model = "gemini-3.8-flash"; QString localEndpoint = "https://archlinux.taile8e867.ts.net/v1/analyze"; QString localToken; int sampleIntervalMs = 1500; int fontSize = 13; int margin = 12; bool overlayEnabled = true; bool periodicSampling = false; void load(); void save() const; bool valid(QString *error=nullptr) const; };
