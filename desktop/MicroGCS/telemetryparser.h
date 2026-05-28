#ifndef TELEMETRYPARSER_H
#define TELEMETRYPARSER_H

#include <QString>
#include <QMap>
#include <QStringList>

struct TelemetryPacket
{
    bool armed = false;
    QString mode;
    double battery = 0.0;
    double altitude = 0.0;
    double x = 0.0;
    double y = 0.0;
    int waypoint = 0;
    QString status;
};

bool parseTelemetryLine(const QString& line, TelemetryPacket& packet, QString& errorMessage);

#endif // TELEMETRYPARSER_H
