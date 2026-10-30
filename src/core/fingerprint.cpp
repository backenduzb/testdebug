#include "fingerprint.h"
#include <QCryptographicHash>
#include <QBuffer>
QString Fingerprint::image(const QImage &input, const QString &context) { QImage scaled = input.scaled(512,512,Qt::KeepAspectRatio,Qt::SmoothTransformation).convertToFormat(QImage::Format_Grayscale8); QByteArray bytes; QBuffer b(&bytes); b.open(QIODevice::WriteOnly); scaled.save(&b,"PNG",0); return QString::fromLatin1(QCryptographicHash::hash(bytes + context.toUtf8(),QCryptographicHash::Sha256).toHex()); }
