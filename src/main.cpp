#include <QApplication>
#include "ui/main-window.h"
int main(int argc,char**argv){QApplication app(argc,argv);app.setOrganizationName("AuthorizedPractice");app.setApplicationName("GeminiVisionPracticeAssistant");MainWindow w;w.resize(460,300);w.show();return app.exec();}
