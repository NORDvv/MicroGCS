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



    void handleReadyRead();
    void handleSerialError(QSerialPort::SerialPortError error);
};
#endif // MAINWINDOW_H
