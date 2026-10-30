#include "trigger-server.h"
#include <QTcpSocket>
TriggerServer::TriggerServer(QObject*p):QTcpServer(p){}
bool TriggerServer::start(quint16 port){return listen(QHostAddress::LocalHost,port);}
void TriggerServer::incomingConnection(qintptr descriptor){auto*s=new QTcpSocket(this);s->setSocketDescriptor(descriptor);connect(s,&QTcpSocket::readyRead,this,&TriggerServer::readRequest);connect(s,&QTcpSocket::disconnected,s,&QObject::deleteLater);}
void TriggerServer::readRequest(){auto*s=qobject_cast<QTcpSocket*>(sender());if(!s)return;const auto request=s->readAll();if(request.startsWith("POST /capture ")){s->write("HTTP/1.1 204 No Content\r\nContent-Length: 0\r\nConnection: close\r\n\r\n");emit triggered();}else{s->write("HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\nConnection: close\r\n\r\n");}s->disconnectFromHost();}
