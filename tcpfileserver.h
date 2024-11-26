#ifndef TCPFILESERVER_H
#define TCPFILESERVER_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QFile>
#include <QProgressBar>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>

class TcpFileServer : public QWidget {
    Q_OBJECT

public:
    explicit TcpFileServer(QWidget *parent = nullptr);
    ~TcpFileServer();

private slots:
    void start();
    void acceptConnection();
    void updateServerProgress();
    void displayError(QAbstractSocket::SocketError socketError);

private:
    QTcpServer tcpServer;
    QTcpSocket *tcpServerConnection;
    QFile *localFile;

    QProgressBar *serverProgressBar;
    QLabel *serverStatusLabel;
    QPushButton *startButton;
    QPushButton *quitButton;
    QLineEdit *ipLineEdit;
    QLineEdit *portLineEdit;
    QDialogButtonBox *buttonBox;

    qint64 totalBytes;
    qint64 byteReceived;
    qint64 fileNameSize;
    QString fileName;
    QByteArray inBlock;
};

#endif // TCPFILESERVER_H
