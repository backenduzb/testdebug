# Gemini Vision Practice Assistant

Qt 6/C++23 Windows desktop assistant for explicitly authorized practice interfaces. It captures an authorized screen, sends the image to a configurable Gemini vision model, validates the JSON result, and shows a compact bottom-right overlay.

## Build locally

Install Qt 6 with MSVC 2022, CMake, and Visual Studio 2022. Then from a **Developer PowerShell**:

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
windeployqt --release build/GeminiVisionPracticeAssistant.exe
```

The GitHub workflow in `.github/workflows/windows.yml` performs the same Windows build, tests, Qt deployment, and uploads a portable artifact. Push this repository yourself; no secret is needed at build time.

## Configure Gemini

Start the app, open Settings, enter a Gemini API key and a currently available image-capable model, then restart practice. The key is stored via `QSettings` in the current user's application settings and is never committed to source. Do not put it in GitHub Actions variables unless you intentionally add a runtime integration.

The API request uses HTTPS and certificate validation. The response is accepted only for the supported statuses and, for success, a visible answer label. Model confidence is displayed only as data from the model and is not a calibrated probability.

## Authorized integration and limitations

The current baseline uses a Windows screenshot adapter and mockable interfaces. Event-driven website integration and native Windows Graphics Capture window selection still need Windows-specific implementation/testing; `WindowsScreenshotProvider` currently uses the supported Qt screen-grab path as a safe fallback. The controller deduplicates identical frames and rejects stale question IDs, but periodic sampling/UI event wiring should be extended before production use.

Linux can run the core unit tests if Qt 6 development packages are installed, but Windows capture, DPI placement, MSVC ABI, `windeployqt`, and Windows 10/11 behavior are verified only by the GitHub Actions Windows runner or a local Windows machine.
