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

    void handleReadyRead();
    void handleSerialError(QSerialPort::SerialPortError error);
    void updateTelemetryDisplay(const TelemetryPacket& packet);
    void setControlPanelEnabled(bool enabled);
};
#endif // MAINWINDOW_H
