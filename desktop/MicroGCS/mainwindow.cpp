#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // UI Widgets set up
    ui->setupUi(this);

    LeftMenuButton = ui->LeftMenuButton;
    RightMenuButton = ui->RightMenuButton;

    RefreshPortsPushButton = ui->RefreshPortsPushButton;
    ConnectToPortPushButton = ui->ConnectToPortPushButton;
    AvailablePortsComboBox = ui->AvailablePortsComboBox;
    PortConnectionStatusLabel = ui->PortConnectionStatusLabel;
    MainOutputTextEdit = ui->MainOutputTextEdit;

    ArmedCheckBox = ui->ArmedCheckBox;
    AutoRadioButton = ui->AutoRadioButton;
    ManualRadioButton = ui->ManualRadioButton;
    RTLRadioButton = ui->RTLRadioButton;
    ResetPushButton = ui->ResetPushButton;

    TelemetryHealthIndicatorLabel = ui->TelemetryHealthIndicatorLabel;
    LastUpdateIndicatorLabel = ui->LastUpdateIndicatorLabel;
    CommsAgeIndicatorLabel = ui->CommsAgeIndicatorLabel;
    PacketsNumIndicatorLabel = ui->PacketsNumIndicatorLabel;
    LastCommandIndicatorLabel = ui->LastCommandIndicatorLabel;



    // Widgets extra settings
    MainOutputTextEdit->setReadOnly(true);
    ConnectToPortPushButton->setText("Connect");
    PortConnectionStatusLabel->setText("Disconnected");
    ui->MainStackedWidget->setCurrentIndex(0);


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

    connect(
        RightMenuButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            int ind = ui->MainStackedWidget->currentIndex();
            if (ind < ui->MainStackedWidget->count()-1) {
                ind += 1;
            } else {
                ind = 0;
            }
            ui->MainStackedWidget->setCurrentIndex(ind);
        }
    );

    connect(
        LeftMenuButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            int ind = ui->MainStackedWidget->currentIndex();
            if (ind > 0) {
                ind -= 1;
            } else {
                ind = ui->MainStackedWidget->count()-1;
            }
            ui->MainStackedWidget->setCurrentIndex(ind);
        }
    );

    connect(ui->ArmedCheckBox, &QCheckBox::toggled, this, [this](bool checked)
            {
                sendCommand(checked ? "CMD;ARM" : "CMD;DISARM");
            });

    connect(ui->ManualRadioButton, &QRadioButton::clicked, this, [this]()
            {
                sendCommand("CMD;MODE=MANUAL");
                ui->ModeIndicatorLabel->setText("MANUAL");
            });

    connect(ui->AutoRadioButton, &QRadioButton::clicked, this, [this]()
            {
                sendCommand("CMD;MODE=AUTO");
                ui->ModeIndicatorLabel->setText("AUTO");
            });

    connect(ui->RTLRadioButton, &QRadioButton::clicked, this, [this]()
            {
                sendCommand("CMD;MODE=RTL");
                ui->ModeIndicatorLabel->setText("RTL");
            });

    connect(ui->ResetPushButton, &QPushButton::clicked, this, [this]()
            {
                sendCommand("CMD;RESET");
            });

    connect(&telemetryHealthTimer,
            &QTimer::timeout,
            this,
            &MainWindow::updateTelemetryHealth);


    // Proper boot up
    refreshPorts();
    setControlPanelEnabled(false);
    telemetryHealthTimer.start(250);
    startSessionLog();
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
        MainOutputTextEdit->append("[ERROR] No serial port selected.");
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

        MainOutputTextEdit->append(
            QString("[ERROR] Failed to open %1: %2")
                .arg(selectedPortName)
                .arg(errorMessage)
            );

        PortConnectionStatusLabel->setText("Connection failed");
        logEvent("SYS", "ERROR", QString("Failed to open %1: %2").arg(selectedPortName).arg(errorMessage));
        return;
    }

    PortConnectionStatusLabel->setText(QString("Connected to %1").arg(selectedPortName));
    ConnectToPortPushButton->setText("Disconnect");
    AvailablePortsComboBox->setEnabled(false);
    RefreshPortsPushButton->setEnabled(false);

    MainOutputTextEdit->append(
        QString("[INFO] Connected to %1 at 115200 baud.").arg(selectedPortName)
        );
    setControlPanelEnabled(true);
    logEvent("SYS", "CONNECT", QString("Connected to %1 at 115200").arg(selectedPortName));
}

void MainWindow::disconnectSerial() {
    if (serialPort.isOpen())
    {
        const QString portName = serialPort.portName();
        serialPort.close();

        MainOutputTextEdit->append(
            QString("[INFO] Disconnected from %1.").arg(portName)
            );

        logEvent("SYS","DISCONNECT", QString("Disconnected from %1").arg(portName));
    }

    receiveBuffer.clear();

    PortConnectionStatusLabel->setText("Disconnected");
    ConnectToPortPushButton->setText("Connect");
    AvailablePortsComboBox->setEnabled(true);
    RefreshPortsPushButton->setEnabled(true);
    setControlPanelEnabled(false);

    hasReceivedTelemetry = false;
    telemetryPacketCount = 0;
    TelemetryHealthIndicatorLabel->setText("Disconnected");
    LastUpdateIndicatorLabel->setText("---");
    CommsAgeIndicatorLabel->setText("---");
    PacketsNumIndicatorLabel->setText("0");
    LastCommandIndicatorLabel->setText("---");
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

        if (line.startsWith("TEL;")) {
            TelemetryPacket packet;
            QString parseError;
            logEvent("RX", "TEL", line);

            if (parseTelemetryLine(line, packet, parseError)) {
                markTelemetryReceived();
                updateTelemetryDisplay(packet);
            } else {
                MainOutputTextEdit->append(QString("[PARSE ERROR] %1 - %2").arg(parseError).arg(line));
                logEvent("RX","PARSE_ERROR",QString("%1 | error=%2").arg(line).arg(parseError));
            }
        } else if (line.startsWith("ACK;")) {
            MainOutputTextEdit->append(QString("[ACK] %1").arg(line));
            updateLastCommandResult(line, false);
            logEvent("RX", "ACK", line);
        } else if (line.startsWith("ERR;")) {
            MainOutputTextEdit->append(QString("[ERR] %1").arg(line));
            updateLastCommandResult(line, true);
            logEvent("RX", "ERR", line);
        } else {
            MainOutputTextEdit->append(QString("[RX UNKNOWN] %1").arg(line));
            logEvent("RX", "UNKNOWN", line);
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

void MainWindow::sendCommand(const QString& command)
{
    if (!serialPort.isOpen())
    {
        MainOutputTextEdit->append("[ERROR] Cannot send command: serial port is not open.");
        logEvent("SYS", "ERROR", QString("Cannot send command while disconnected: %1").arg(command));
        return;
    }

    QByteArray data = command.toUtf8();
    data.append("\r\n");

    const qint64 bytesWritten = serialPort.write(data);

    if (bytesWritten == -1)
    {
        MainOutputTextEdit->append(QString("[ERROR] Failed to send command: %1").arg(serialPort.errorString()));
        logEvent("SYS", "ERROR", QString("Failed to send command %1: %2").arg(command).arg(serialPort.errorString()));
        return;
    }

    ui->MainOutputTextEdit->append(QString("[TX] %1").arg(command));

    logEvent("TX", "CMD", command);
}

void MainWindow::updateTelemetryDisplay(const TelemetryPacket& packet)
{
    // signal block applies to the scope
    /*{
        QSignalBlocker blocker(ui->ArmedCheckBox);
        ui->ArmedCheckBox->setChecked(packet.armed);
    }

    {
        QSignalBlocker blocker1(ui->ManualRadioButton);
        QSignalBlocker blocker2(ui->AutoRadioButton);
        QSignalBlocker blocker3(ui->RTLRadioButton);

        ui->ManualRadioButton->setChecked(packet.mode == "MANUAL");
        ui->AutoRadioButton->setChecked(packet.mode == "AUTO");
        ui->RTLRadioButton->setChecked(packet.mode == "RTL");
    }*/

    ui->ModeIndicatorLabel->setText(packet.mode);
    ui->BatteryIndicatorLabel->setText(QString("%1 %").arg(packet.battery, 0, 'f', 1));
    ui->AltitudeIndicatorLabel->setText(QString("%1 m").arg(packet.altitude, 0, 'f', 1));
    ui->PositionIndicatorLabel->setText(QString("x=%1, y=%2").arg(packet.x).arg(packet.y));
    ui->WaypointIndicatorLabel->setText(QString::number(packet.waypoint));
    ui->StatusIndicatorLabel->setText(packet.status);
}

void MainWindow::setControlPanelEnabled(bool enabled)
{
    ui->ArmedCheckBox->setEnabled(enabled);
    ui->ManualRadioButton->setEnabled(enabled);
    ui->AutoRadioButton->setEnabled(enabled);
    ui->RTLRadioButton->setEnabled(enabled);
    ui->ResetPushButton->setEnabled(enabled);
}

void MainWindow::updateTelemetryHealth() {
    if (!serialPort.isOpen()) {
        TelemetryHealthIndicatorLabel->setText("Port Disconnected");
        CommsAgeIndicatorLabel->setText("---");
        return;
    }

    if (!hasReceivedTelemetry) {
        TelemetryHealthIndicatorLabel->setText("Waiting");
        CommsAgeIndicatorLabel->setText("---");
        return;
    }

    const quint64 ageMs = lastTelemetryTime.msecsTo(QDateTime::currentDateTime());

    CommsAgeIndicatorLabel->setText(QString::number(ageMs));

    if (ageMs > 20000) {
        TelemetryHealthIndicatorLabel->setText("STALE");

        if (!telemetryWasStale) {
            telemetryWasStale = true;
            logEvent("SYS","WARN",QString("Telemetry stale, age_ms=%1").arg(ageMs));
        }
    } else {
        TelemetryHealthIndicatorLabel->setText("OK");
        telemetryWasStale = false;
    }
}

void MainWindow::markTelemetryReceived() {
    hasReceivedTelemetry = true;
    lastTelemetryTime = QDateTime::currentDateTime();
    telemetryPacketCount++;

    LastUpdateIndicatorLabel->setText(lastTelemetryTime.toString("HH:mm:ss.zzzz"));
    PacketsNumIndicatorLabel->setText(QString::number(telemetryPacketCount));

    if (telemetryWasStale){
        logEvent("SYS", "RECOVERED", "Telemetry recovered");
    }

    telemetryWasStale = false;
}

void MainWindow::updateLastCommandResult(const QString& line, bool isError) {
    const QString prefix = isError ? "ERROR " : "OK ";
    LastCommandIndicatorLabel->setText(prefix + line);
}

void MainWindow::startSessionLog() {
    const QString logDirectoryPath = QCoreApplication::applicationDirPath() + "/logs";
    QDir logDirectory(logDirectoryPath);

    if (!logDirectory.exists()) {
        logDirectory.mkpath(".");
    }

    const QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss");
    currentLogPath = logDirectory.filePath(QString("microgcs_%1.csv").arg(timestamp));

    logFile.setFileName(currentLogPath);

    if (!logFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        MainOutputTextEdit->append(QString("[LOG ERROR] Could not open log file: %1").arg(logFile.errorString()));
        return;
    }

    logStream.setDevice(&logFile);

    logStream << "timestamp,direction,type,payload\n";
    logStream.flush();

    MainOutputTextEdit->append(QString("[LOG] Writing session log to: %1").arg(currentLogPath));

    logEvent("SYS", "START", "MicroGCS session started");
}

void MainWindow::stopSessionLog() {
    if (!logFile.isOpen()) {
        return;
    }

    logEvent("SYS", "STOP", "MicroGCS session stopped");
    logStream.flush();
    logFile.close();
}

void MainWindow::logEvent(const QString& direction, const QString& type, const QString& payload) {
    if (!logFile.isOpen()) {
        return;
    }

    const QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODateWithMs);

    logStream << timestamp << ","
              << direction << ","
              << type << ","
              << escapeCsvField(payload)
              << "\n";

    logStream.flush();
}

// prevents stupid errors in CSV, like unintentionl devision into extra columns because the error description has a coma in it.
QString MainWindow::escapeCsvField(const QString& value) const {
    QString escaped = value;
    escaped.replace("\"", "\"\"");
    return QString("\"%1\"").arg(escaped);
}

MainWindow::~MainWindow()
{
    if (serialPort.isOpen())
    {
        serialPort.close();
    }

    stopSessionLog();
    delete ui;
}
