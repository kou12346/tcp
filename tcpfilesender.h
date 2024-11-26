#ifndef TCPFILESENDER_H
#define TCPFILESENDER_H

#include <QWidget>
#include <QTcpSocket>
#include <QFile>
#include <QProgressBar>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QMessageBox>

class TcpFileSender : public QWidget
{
    Q_OBJECT

public:
    explicit TcpFileSender(QWidget *parent = nullptr);
    ~TcpFileSender();

private slots:
    void openFile();
    void start();
    void startTransfer();
    void updateClientProgress(qint64 numBytes);

private:
    QTcpSocket tcpClient;
    QFile *localFile;
    QString fileName;

    qint64 totalBytes;
    qint64 bytesWritten;
    qint64 bytesToWrite;
     const int loadSize = 1024 * 4;

    QProgressBar *clientProgressBar;
    QLabel *clientStatusLabel;
    QLineEdit *ipLineEdit;
    QLineEdit *portLineEdit;
    QPushButton *startButton;
    QPushButton *quitButton;
    QPushButton *openButton;
    QDialogButtonBox *buttonBox;

    QByteArray outBlock;
};

#endif // TCPFILESENDER_H
