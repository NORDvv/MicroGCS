#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QByteArray>
#include <QSerialPortInfo>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QCheckBox>
#include <QRadioButton>
#include <QTimer>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>
#include <QDir>

#include "telemetryparser.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    void refreshPorts();
    void connectSerial();
    void disconnectSerial();

    void sendCommand(const QString& command);

private:
    QSerialPort serialPort;
    QByteArray receiveBuffer;
    QFile logFile;
    QTextStream logStream;
    QString currentLogPath;
    bool telemetryWasStale = false;

    Ui::MainWindow *ui;

    QPushButton* LeftMenuButton;
    QPushButton* RightMenuButton;

    QPushButton* RefreshPortsPushButton;
    QPushButton* ConnectToPortPushButton;
    QComboBox* AvailablePortsComboBox;
    QLabel* PortConnectionStatusLabel;
    QTextEdit* MainOutputTextEdit;

    QCheckBox* ArmedCheckBox;
    QRadioButton* AutoRadioButton;
    QRadioButton* ManualRadioButton;
    QRadioButton* RTLRadioButton;
    QPushButton* ResetPushButton;

    QLabel* TelemetryHealthIndicatorLabel;
    QLabel* LastUpdateIndicatorLabel;
    QLabel* CommsAgeIndicatorLabel;
    QLabel* PacketsNumIndicatorLabel;
    QLabel* LastCommandIndicatorLabel;
    QTimer telemetryHealthTimer;
    QDateTime lastTelemetryTime;
    uint telemetryPacketCount = 0;
    bool hasReceivedTelemetry = false;

    void handleReadyRead();
    void handleSerialError(QSerialPort::SerialPortError error);
    void updateTelemetryDisplay(const TelemetryPacket& packet);
    void setControlPanelEnabled(bool enabled);

    void updateTelemetryHealth();
    void markTelemetryReceived();
    void updateLastCommandResult(const QString& line, bool isError);

    void startSessionLog();
    void stopSessionLog();
    void logEvent(const QString& direction, const QString& type, const QString& payload);
    QString escapeCsvField(const QString& value) const;
};
#endif // MAINWINDOW_H
