#include <QApplication>
#include "ui/main-window.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    app.setOrganizationName("AuthorizedPractice");
    app.setApplicationName("GeminiVisionPracticeAssistant");
    app.setQuitOnLastWindowClosed(false);

    MainWindow window;
    window.resize(460, 300);
    // Show the control window on startup. It can be closed to the tray later.
    // This also makes a missing notification area visible instead of leaving
    // the user with a process that appears not to start.
    window.show();

    return app.exec();
}
