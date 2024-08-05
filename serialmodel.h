
#ifndef SERIALMODEL_H
#define SERIALMODEL_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QByteArray>
#include <QStringList>

class SerialModel : public QObject
{
    Q_OBJECT

public:
    static SerialModel& getInstance() // Singleton tasarım deseni
    {
        static SerialModel instance;
        return instance;
    }

    QSerialPort* getSerialPort() const { return serialPort; } // SerialPort nesnesini döndürür
    void configureSerialPort(const QString &portName, qint32 baudRate); // Serial portu yapılandırır
    void openSerialPort(); // Serial portu açar
    void closeSerialPort(); // Serial portu kapatır
    QByteArray readData(); // Serial porttan veri okur
    QStringList listSerialPorts(); // Kullanılabilir serial portları listeler
    void writeData(const QString &data); // Veriyi serial porta yazar

signals:
    void dataReceived(const QByteArray &data); // Veri alındığında sinyal

private slots:
    void handleReadyRead(); // Veri okunduğunda tetiklenen slot

private:
    SerialModel(QObject *parent = nullptr);
    ~SerialModel();
    SerialModel(const SerialModel&) = delete; // Kopyalama işlemi yasak
    SerialModel& operator=(const SerialModel&) = delete; // Atama işlemi yasak

    QSerialPort *serialPort; // QSerialPort nesnesi
};

#endif // SERIALMODEL_H
