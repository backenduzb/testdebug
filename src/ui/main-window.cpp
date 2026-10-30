#include "main-window.h"
#include "capture/windows-screenshot-provider.h"
#include "vision/local-agent-provider.h"
#include "core/practice-controller.h"
#include "core/trigger-server.h"
#include "core/global-mouse-trigger.h"
#include "overlay/overlay-window.h"
#include "settings-dialog.h"

#include <QApplication>
#include <QCloseEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QStyle>
#include <QSystemTrayIcon>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    m_settings.load();
    auto *central = new QWidget(this);
    auto *layout = new QVBoxLayout(central);
    auto *buttons = new QHBoxLayout;
    auto *startButton = new QPushButton("Arm Practice", central);
    auto *stopButton = new QPushButton("Stop", central);
    auto *settingsButton = new QPushButton("Settings", central);
    buttons->addWidget(startButton);
    buttons->addWidget(stopButton);
    buttons->addWidget(settingsButton);
    layout->addLayout(buttons);
    m_status = new QLabel("Stopped", central);
    layout->addWidget(m_status);
    m_diag = new QPlainTextEdit(central);
    m_diag->setReadOnly(true);
    m_diag->setPlaceholderText("Diagnostics");
    layout->addWidget(m_diag);
    setCentralWidget(central);
    setWindowTitle("Gemini Vision Practice Assistant");

    auto *capture = new WindowsScreenshotProvider(this);
    auto *agent = new LocalAgentProvider(m_settings.localEndpoint, m_settings.localToken, this);
    m_controller = new PracticeController(capture, agent, this);
    m_overlay = new OverlayWindow;
    m_overlay->setFontPixels(m_settings.fontSize);
    m_overlay->setMargin(m_settings.margin);

    m_trigger = new TriggerServer(this);
    if (!m_trigger->start())
        m_diag->appendPlainText("Right-click trigger port 8766 unavailable");
    connect(m_trigger, &TriggerServer::triggered, this, &MainWindow::handleCaptureTrigger);

#ifdef Q_OS_WIN
    m_globalMouseTrigger = new GlobalMouseTrigger(this);
    if (!m_globalMouseTrigger->start())
        m_diag->appendPlainText("Global right-click hook unavailable");
    connect(m_globalMouseTrigger, &GlobalMouseTrigger::triggered,
            this, &MainWindow::handleCaptureTrigger);
#endif

    connect(startButton, &QPushButton::clicked, this, &MainWindow::start);
    connect(stopButton, &QPushButton::clicked, this, &MainWindow::stop);
    connect(settingsButton, &QPushButton::clicked, this, &MainWindow::settings);
    connect(m_controller, &PracticeController::diagnostic, this,
            [this](const QString &message) {
                m_diag->appendPlainText(message);
                m_status->setText("Error");
            });
    connect(m_controller, &PracticeController::resultReady, this,
            [this](const AnalysisResult &result) {
                const QString text = result.isSuccess()
                    ? "Answer: " + result.selectedLabel
                    : result.status + ": " + result.error;
                m_overlay->setText(text);
                m_status->setText(text);
            });

    if (QSystemTrayIcon::isSystemTrayAvailable()) {
        m_tray = new QSystemTrayIcon(style()->standardIcon(QStyle::SP_ComputerIcon), this);
        m_tray->setToolTip("Gemini Vision Practice Assistant");
        auto *menu = new QMenu(this);
        menu->addAction("Show", this, &MainWindow::showMain);
        menu->addAction("Arm Practice", this, &MainWindow::start);
        menu->addAction("Stop", this, &MainWindow::stop);
        menu->addAction("Settings", this, &MainWindow::settings);
        menu->addSeparator();
        menu->addAction("Quit", this, &MainWindow::quitApp);
        m_tray->setContextMenu(menu);
        connect(m_tray, &QSystemTrayIcon::activated, this,
                [this](QSystemTrayIcon::ActivationReason reason) {
                    if (reason == QSystemTrayIcon::DoubleClick)
                        showMain();
                });
        m_tray->show();
    } else {
        m_diag->appendPlainText(
            "System tray unavailable; the main window will remain visible");
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_tray && m_tray->isVisible()) {
        hide();
        event->ignore();
    } else {
        event->accept();
    }
}

void MainWindow::showMain()
{
    showNormal();
    raise();
    activateWindow();
}

void MainWindow::quitApp()
{
    m_controller->stop();
    m_overlay->hide();
    qApp->quit();
}

void MainWindow::handleCaptureTrigger()
{
    if (m_controller) {
        m_overlay->setText("Loading...");
        m_status->setText("Loading...");
        m_controller->analyzeNow();
    }
}

void MainWindow::start()
{
    m_overlay->show();
    m_controller->start();
    m_overlay->setText("Ready - right click");
    m_status->setText("Ready - right click");
}

void MainWindow::stop()
{
    m_controller->stop();
    m_overlay->hide();
    m_status->setText("Stopped");
}

void MainWindow::settings()
{
    SettingsDialog dialog(m_settings, this);
    if (dialog.exec() == QDialog::Accepted)
        m_diag->appendPlainText("Settings saved; restart practice to apply API changes.");
}
