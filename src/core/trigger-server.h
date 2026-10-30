#pragma once
#include <QTcpServer>
class TriggerServer final : public QTcpServer { Q_OBJECT public: explicit TriggerServer(QObject *parent=nullptr); bool start(quint16 port=8766); signals: void triggered(); protected: void incomingConnection(qintptr socketDescriptor) override; private slots: void readRequest(); };
