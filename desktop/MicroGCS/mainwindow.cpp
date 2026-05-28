#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // UI Widgets set up
    ui->setupUi(this);
    RefreshPortsPushButton = ui->RefreshPortsPushButton;
    ConnectToPortPushButton = ui->ConnectToPortPushButton;
    AvailablePortsComboBox = ui->AvailablePortsComboBox;
    PortConnectionStatusLabel = ui->PortConnectionStatusLabel;
    MainOutputTextEdit = ui->MainOutputTextEdit;


    // Widgets extra settings
    MainOutputTextEdit->setReadOnly(true);
    ConnectToPortPushButton->setText("Connect");
    PortConnectionStatusLabel->setText("Disconnected");


    // Signals management
    connect(
        RefreshPortsPushButton,
        &QPushButton::clicked,
        this,
        &MainWindow::refreshPorts
        );

    connect(
        ConnectToPortPushButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            if (serialPort.isOpen())
            {
                disconnectSerial();
            }
            else
            {
                connectSerial();
            }
        }
        );

    connect(
        &serialPort,
        &QSerialPort::readyRead,
        this,
        &MainWindow::handleReadyRead
        );

    connect(
        &serialPort,
        &QSerialPort::errorOccurred,
        this,
        &MainWindow::handleSerialError
        );

    refreshPorts();


}

void MainWindow::refreshPorts() {
    AvailablePortsComboBox->clear();

    const QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();

    for (const QSerialPortInfo& portInfo : ports) {
        const QString displayName = QString("%1 - %2")
                                        .arg(portInfo.portName())
                                        .arg(portInfo.description().isEmpty() ? "Unknown device" : portInfo.portName());

        AvailablePortsComboBox->addItem(displayName,portInfo.portName());
    }

    if (ports.isEmpty()) {
        PortConnectionStatusLabel->setText("No serial ports found");
        MainOutputTextEdit->append("[WARN] No serial ports found");
    } else {
        PortConnectionStatusLabel->setText("Ports refreshed");
        MainOutputTextEdit->append(QString("[INFO] Found %1 serial port(s).").arg(ports.size()));
    }
}

void MainWindow::connectSerial() {
    if (serialPort.isOpen())
    {
        return;
    }

    const QString selectedPortName =
        AvailablePortsComboBox->currentData().toString();

    if (selectedPortName.isEmpty())
    {
        ui->MainOutputTextEdit->append("[ERROR] No serial port selected.");
        PortConnectionStatusLabel->setText("No port selected");
        return;
    }

    receiveBuffer.clear();

    serialPort.setPortName(selectedPortName);
    serialPort.setBaudRate(QSerialPort::Baud115200);
    serialPort.setDataBits(QSerialPort::Data8);
    serialPort.setParity(QSerialPort::NoParity);
    serialPort.setStopBits(QSerialPort::OneStop);
    serialPort.setFlowControl(QSerialPort::NoFlowControl);

    if (!serialPort.open(QIODevice::ReadWrite))
    {
        const QString errorMessage = serialPort.errorString();

        ui->MainOutputTextEdit->append(
            QString("[ERROR] Failed to open %1: %2")
                .arg(selectedPortName)
                .arg(errorMessage)
            );

        PortConnectionStatusLabel->setText("Connection failed");
        return;
    }

    PortConnectionStatusLabel->setText(QString("Connected to %1").arg(selectedPortName));
    ConnectToPortPushButton->setText("Disconnect");
    AvailablePortsComboBox->setEnabled(false);
    RefreshPortsPushButton->setEnabled(false);

    ui->MainOutputTextEdit->append(
        QString("[INFO] Connected to %1 at 115200 baud.").arg(selectedPortName)
        );
}

void MainWindow::disconnectSerial() {
    if (serialPort.isOpen())
    {
        const QString portName = serialPort.portName();
        serialPort.close();

        ui->MainOutputTextEdit->append(
            QString("[INFO] Disconnected from %1.").arg(portName)
            );
    }

    receiveBuffer.clear();

    PortConnectionStatusLabel->setText("Disconnected");
    ConnectToPortPushButton->setText("Connect");
    AvailablePortsComboBox->setEnabled(true);
    RefreshPortsPushButton->setEnabled(true);
}

void MainWindow::handleReadyRead() {
    receiveBuffer += serialPort.readAll();

    while (true)
    {
        const int newlineIndex = receiveBuffer.indexOf('\n');

        if (newlineIndex < 0)
        {
            break;
        }

        QByteArray lineBytes = receiveBuffer.left(newlineIndex);
        receiveBuffer.remove(0, newlineIndex + 1);

        lineBytes = lineBytes.trimmed();

        if (lineBytes.isEmpty())
        {
            continue;
        }

        const QString line = QString::fromUtf8(lineBytes);

        ui->MainOutputTextEdit->append(QString("[RX] %1").arg(line));

        TelemetryPacket packet;
        QString parseError;

        if (parseTelemetryLine(line, packet, parseError))
        {
            // Temporary proof that parsing works.
            ui->MainOutputTextEdit->append(
                QString("[PARSED] mode=%1 batt=%2 alt=%3 x=%4 y=%5")
                    .arg(packet.mode)
                    .arg(packet.battery)
                    .arg(packet.altitude)
                    .arg(packet.x)
                    .arg(packet.y)
                );

            // Later: update labels here.
        }
        else
        {
            ui->MainOutputTextEdit->append(
                QString("[PARSE ERROR] %1").arg(parseError)
                );
        }
    }
}

void MainWindow::handleSerialError(QSerialPort::SerialPortError error) {
    if (error == QSerialPort::NoError)
    {
        return;
    }

    ui->MainOutputTextEdit->append(
        QString("[SERIAL ERROR] %1").arg(serialPort.errorString())
        );

    if (error == QSerialPort::ResourceError ||
        error == QSerialPort::PermissionError)
    {
        disconnectSerial();
    }
}

MainWindow::~MainWindow()
{
    if (serialPort.isOpen())
    {
        serialPort.close();
    }

    delete ui;
}
