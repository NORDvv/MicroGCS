#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QByteArray>

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


private:
    Ui::MainWindow *ui;
    void handleReadyRead();
    void handleSerialError(QSerialPort::SerialPortError error);
};
#endif // MAINWINDOW_H
