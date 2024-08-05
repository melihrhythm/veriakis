#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "serialmodel.h"
#include <QTextEdit>
#include <QFile>
#include <QTextStream>
#include <QTextDocument>
#include <QTextCursor>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr); // Constructor
    ~MainWindow(); // Destructor

public slots:
    void refreshSerialPorts(); // Serial portları yenilemek için slot
    void saveData(); // Verileri kaydetmek için slot

private slots:
    void handleConnectButton(); // Bağlan/Diskonekt butonu için slot
    void handleSendButton(); // Gönder butonu için slot
    void displayData(const QByteArray &data); // Gelen veriyi göstermek için slot
    void clearTextEdit(); // Metin alanını temizlemek için slot
    void on_comboBox_currentIndexChanged(int index); // ComboBox öğesi değiştiğinde tetiklenen slot
    void on_lineEdit_returnPressed(); // LineEdit'te Enter tuşuna basıldığında tetiklenen slot

private:
    Ui::MainWindow *ui; // UI nesnesi
    SerialModel &serialModel; // SerialModel referansı
    void listSerialPorts(); // Serial portları listelemek için fonksiyon
    void appendToTextEditWithTimestamp(const QString &message); // Zaman damgası ile metin eklemek için fonksiyon
    void logSentData(const QString &data); // Gönderilen verileri loglamak için fonksiyon
    void logReceivedData(const QString &data); // Alınan verileri loglamak için fonksiyon
    void saveDataToTxtFile(const QString &fileName); // Verileri .txt dosyasına kaydetmek için fonksiyon
    void saveDataToCsvFile(const QString &fileName); // Verileri .csv dosyasına kaydetmek için fonksiyon
    QString sentDataLog; // Gönderilen verilerin logu
    QString receivedDataLog; // Alınan verilerin logu
};

#endif // MAINWINDOW_H
