#include "serialmodel.h"

SerialModel::SerialModel(QObject *parent)
    : QObject(parent), serialPort(new QSerialPort(this))
{
    connect(serialPort, &QSerialPort::readyRead, this, &SerialModel::handleReadyRead);
    // QSerialPort nesnesinin readyRead sinyali, handleReadyRead slot fonksiyonuna bağlanıyor
}

SerialModel::~SerialModel()
{
    if (serialPort->isOpen())
        serialPort->close();
    //eğer seri port açıksa kapatılıyor
    delete serialPort;
    //seri port bellekten siliniyor
}

void SerialModel::configureSerialPort(const QString &portName, qint32 baudRate)
{
    serialPort->setPortName(portName);

    //port adı ayarlanır

    serialPort->setBaudRate(baudRate);

    //iletişim hızı baund rate ayarlanır

    serialPort->setDataBits(QSerialPort::Data8);

    //veri bit sayısı 8 olarak ayarlanır

    serialPort->setParity(QSerialPort::NoParity);

    //parity biti kullanılmaz

    serialPort->setStopBits(QSerialPort::OneStop);

    //durma biti ayarlanır

    serialPort->setFlowControl(QSerialPort::NoFlowControl);

    //akış kontrolü kullanılmaz
}

void SerialModel::openSerialPort()//seri port açık değilse açar
{
    if (!serialPort->isOpen())
        serialPort->open(QIODevice::ReadWrite);
}

void SerialModel::closeSerialPort()//seri port açıksa kapatılır
{
    if (serialPort->isOpen())
        serialPort->close();
}

QByteArray SerialModel::readData()//seri porttan gelen veriyi okur ve döndürür
{
    return serialPort->readAll();
}

QStringList SerialModel::listSerialPorts()
{
    // Kullanılabilir tüm seri port bilgilerini alır
    QStringList portList;
    const auto infos = QSerialPortInfo::availablePorts();
    // Her bir seri port bilgisini portList'e ekler
    for (const QSerialPortInfo &info : infos)
        portList.append(info.portName());
    // Port adlarını içeren QStringList'i döndürür
    return portList;
}

void SerialModel::writeData(const QString &data)
{
    if (serialPort->isOpen() && !data.isEmpty()) {
        serialPort->write(data.toUtf8());
        // Eğer seri port açıksa ve veri boş değilse,
        // UTF-8 formatında veriyi yazmaya çalışır
    }
}

void SerialModel::handleReadyRead()
//seri porttan gelen veriyi okuma işlemi
{
    QByteArray data = readData();//veriyi okur
    emit dataReceived(data);
}
