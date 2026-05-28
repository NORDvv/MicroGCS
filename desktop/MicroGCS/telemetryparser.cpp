#include "telemetryparser.h"

bool parseTelemetryLine(const QString& line, TelemetryPacket& packet, QString& errorMessage)
{
    const QString trimmedLine = line.trimmed();

    if (!trimmedLine.startsWith("TEL;"))
    {
        errorMessage = "Line does not start with TEL;";
        return false;
    }

    const QStringList parts = trimmedLine.split(';', Qt::SkipEmptyParts);

    QMap<QString, QString> fields;

    for (int i = 1; i < parts.size(); ++i)
    {
        const QStringList keyValue = parts[i].split('=');

        if (keyValue.size() != 2)
        {
            errorMessage = QString("Invalid field: %1").arg(parts[i]);
            return false;
        }

        fields[keyValue[0].trimmed()] = keyValue[1].trimmed();
    }

    const QStringList requiredFields = {
        "armed", "mode", "batt", "alt", "x", "y", "wp", "status"
    };

    for (const QString& field : requiredFields)
    {
        if (!fields.contains(field))
        {
            errorMessage = QString("Missing field: %1").arg(field);
            return false;
        }
    }

    bool ok = false;

    packet.armed = fields["armed"].toInt(&ok) != 0;
    if (!ok)
    {
        errorMessage = "Invalid armed value";
        return false;
    }

    packet.mode = fields["mode"];

    packet.battery = fields["batt"].toDouble(&ok);
    if (!ok)
    {
        errorMessage = "Invalid battery value";
        return false;
    }

    packet.altitude = fields["alt"].toDouble(&ok);
    if (!ok)
    {
        errorMessage = "Invalid altitude value";
        return false;
    }

    packet.x = fields["x"].toDouble(&ok);
    if (!ok)
    {
        errorMessage = "Invalid x value";
        return false;
    }

    packet.y = fields["y"].toDouble(&ok);
    if (!ok)
    {
        errorMessage = "Invalid y value";
        return false;
    }

    packet.waypoint = fields["wp"].toInt(&ok);
    if (!ok)
    {
        errorMessage = "Invalid waypoint value";
        return false;
    }

    packet.status = fields["status"];

    return true;
}
