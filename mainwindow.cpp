#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <QFileDialog>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , serialModel(SerialModel::getInstance()) // SerialModel örneğini al
{
    ui->setupUi(this); // UI'ı kur
    refreshSerialPorts(); // Başlangıçta serial portları yenile

    // Sinyal-slot bağlantıları
    connect(ui->refreshPortsButton, &QPushButton::clicked, this, &MainWindow::refreshSerialPorts); // Yenile butonu
    connect(&serialModel, &SerialModel::dataReceived, this, &MainWindow::displayData); // Veri alındığında
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::handleConnectButton); // Bağlan/Diskonekt butonu
    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::handleSendButton); // Gönder butonu
    connect(ui->pushButton_3, &QPushButton::clicked, this, &MainWindow::clearTextEdit); // Temizle butonu
    connect(ui->comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::on_comboBox_currentIndexChanged); // ComboBox değişikliği
    connect(ui->saveButton, &QPushButton::clicked, this, &MainWindow::saveData); // Kaydet butonu
    connect(ui->lineEdit, &QLineEdit::returnPressed, this, &MainWindow::on_lineEdit_returnPressed); // Enter tuşu

    // Dosya formatı seçeneklerini ekle
    ui->fileFormatComboBox->addItem("Save as .txt");
    ui->fileFormatComboBox->addItem("Save as .csv");
}

MainWindow::~MainWindow()
{
    // Serial port açıksa kapat
    if (serialModel.getSerialPort()->isOpen()) {
        serialModel.closeSerialPort();
    }
    delete ui; // UI nesnesini sil
}

void MainWindow::refreshSerialPorts()
{
    ui->comboBox->clear(); // ComboBox'ı temizle
    QStringList portNames = serialModel.listSerialPorts(); // Port isimlerini al
    ui->comboBox->addItems(portNames); // ComboBox'a ekle
    qDebug() << "Serial ports refreshed: " << portNames;
}

void MainWindow::handleConnectButton()
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString timestamp = currentDateTime.toString("[yyyy-MM-dd hh:mm:ss] ");

    if (serialModel.getSerialPort()->isOpen()) {
        // Port açıksa kapat
        serialModel.closeSerialPort();
        ui->pushButton->setText("Connect");
        ui->textEdit->append(timestamp + "Disconnected");
        qDebug() << "Disconnected from serial port.";
    } else {
        // Port kapalıysa aç
        QString portName = ui->comboBox->currentText();
        serialModel.configureSerialPort(portName, QSerialPort::Baud115200);
        serialModel.openSerialPort();
        if (serialModel.getSerialPort()->isOpen()) {
            ui->pushButton->setText("Disconnect");
            ui->textEdit->append(timestamp + "Connected to " + portName);
            qDebug() << "Connected to serial port: " << portName;
        } else {
            ui->textEdit->append(timestamp + "Failed to open port!");
            qDebug() << "Failed to open serial port: " << portName;
        }
    }
}

void MainWindow::handleSendButton()
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString timestamp = currentDateTime.toString("[yyyy-MM-dd hh:mm:ss] ");

    QString data = ui->lineEdit->text();
    if (!data.isEmpty()) {
        QString message = timestamp + "Sending: " + data;
        appendToTextEditWithTimestamp(message); // Mesajı metin alanına ekle
        logSentData(message); // Gönderilen veriyi logla
        serialModel.writeData(data.toLatin1()); // Veriyi seri porta gönder
        ui->lineEdit->clear(); // LineEdit'i temizle
    } else {
        appendToTextEditWithTimestamp(timestamp + "Cannot send empty data.");
    }
}

void MainWindow::appendToTextEditWithTimestamp(const QString &message)
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString timestamp = currentDateTime.toString("[yyyy-MM-dd hh:mm:ss] ");
    ui->textEdit->append(timestamp + message); // Mesajı zaman damgası ile metin alanına ekle
}

void MainWindow::logSentData(const QString &data)
{
    sentDataLog += data + "\n"; // Gönderilen veriyi logla
}

void MainWindow::logReceivedData(const QString &data)
{
    receivedDataLog += data + "\n"; // Alınan veriyi logla
}

void MainWindow::displayData(const QByteArray &data)
{
    QString message = "Received: " + QString(data);
    appendToTextEditWithTimestamp(message); // Mesajı metin alanına ekle
    logReceivedData(message); // Alınan veriyi logla
    qDebug() << "Data displayed: " << message;
}

void MainWindow::clearTextEdit()
{
    ui->textEdit->clear(); // Metin alanını temizle
}

void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    if (serialModel.getSerialPort()->isOpen()) {
        serialModel.closeSerialPort(); // Portu kapat
        ui->pushButton->setText("Connect");
        ui->textEdit->append("Disconnected from " + ui->comboBox->itemText(index)); // Mesajı metin alanına ekle
    }
}

void MainWindow::on_lineEdit_returnPressed()
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString timestamp = currentDateTime.toString("[yyyy-MM-dd hh:mm:ss] ");

    QString data = ui->lineEdit->text();
    if (!data.isEmpty()) {
        QString message = timestamp + "Sending: " + data;
        appendToTextEditWithTimestamp(message); // Mesajı metin alanına ekle
        logSentData(message); // Gönderilen veriyi logla
        serialModel.writeData(data.toLatin1()); // Veriyi seri porta gönder
        ui->lineEdit->clear(); // LineEdit'i temizle
    } else {
        appendToTextEditWithTimestamp(timestamp + "Cannot send empty data.");
    }
}

void MainWindow::saveData()
{
    QString selectedFormat = ui->fileFormatComboBox->currentText(); // Seçilen dosya formatını al
    QString fileName = QFileDialog::getSaveFileName(this, "Save File", "", selectedFormat == "Save as .txt" ? "Text Files (*.txt)" : "CSV Files (*.csv)");
    if (fileName.isEmpty())
        return;

    if (selectedFormat == "Save as .txt") {
        saveDataToTxtFile(fileName); // .txt dosyasına kaydet
    } else if (selectedFormat == "Save as .csv") {
        saveDataToCsvFile(fileName); // .csv dosyasına kaydet
    }
}

void MainWindow::saveDataToTxtFile(const QString &fileName)
{
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << "Sent Data Log:\n" << sentDataLog << "\nReceived Data Log:\n" << receivedDataLog; // Gönderilen ve alınan verileri yaz
    }
}

void MainWindow::saveDataToCsvFile(const QString &fileName)
{
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << "Type,Data\n"; // CSV başlık satırı
        QStringList sentLines = sentDataLog.split("\n", Qt::SkipEmptyParts);
        for (const QString &line : sentLines) {
            out << "Sent," << line << "\n"; // Gönderilen veriyi CSV formatında yaz
        }
        QStringList receivedLines = receivedDataLog.split("\n", Qt::SkipEmptyParts);
        for (const QString &line : receivedLines) {
            out << "Received," << line << "\n"; // Alınan veriyi CSV formatında yaz
        }
    }
}
