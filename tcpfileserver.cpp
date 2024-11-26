#include "tcpfileserver.h"
#include <QIntValidator>


TcpFileServer::TcpFileServer(QWidget *parent)
    : QWidget(parent)
{
    totalBytes = 0;
    byteReceived = 0;
    fileNameSize = 0;

    serverProgressBar = new QProgressBar;
    serverStatusLabel = new QLabel(QStringLiteral("伺服器端就緒"));

    QLabel *ipLabel = new QLabel(QStringLiteral("IP:"));
    ipLineEdit = new QLineEdit;
    ipLineEdit->setPlaceholderText(QStringLiteral("輸入伺服器 IP"));

    QLabel *portLabel = new QLabel(QStringLiteral("Port:"));
    portLineEdit = new QLineEdit;
    portLineEdit->setPlaceholderText(QStringLiteral("輸入伺服器埠號"));
    portLineEdit->setValidator(new QIntValidator(0, 65535, this));

    startButton = new QPushButton(QStringLiteral("接收"));
    quitButton = new QPushButton(QStringLiteral("退出"));

    buttonBox = new QDialogButtonBox;
    buttonBox->addButton(startButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);

    QGridLayout *inputLayout = new QGridLayout;
    inputLayout->addWidget(ipLabel, 0, 0);
    inputLayout->addWidget(ipLineEdit, 0, 1);
    inputLayout->addWidget(portLabel, 1, 0);
    inputLayout->addWidget(portLineEdit, 1, 1);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(serverProgressBar);
    mainLayout->addWidget(serverStatusLabel);
    mainLayout->addLayout(inputLayout);
    mainLayout->addStretch();
    mainLayout->addWidget(buttonBox);

    setLayout(mainLayout);
    setWindowTitle(QStringLiteral("接收檔案"));

    connect(startButton, SIGNAL(clicked()), this, SLOT(start()));
    connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(&tcpServer, SIGNAL(newConnection()), this, SLOT(acceptConnection()));
    connect(&tcpServer, SIGNAL(acceptError(QAbstractSocket::SocketError)), this,
            SLOT(displayError(QAbstractSocket::SocketError)));
}

TcpFileServer::~TcpFileServer()
{

}
void TcpFileServer::start()
{
    startButton->setEnabled(false);
    byteReceived = 0;
    fileNameSize = 0;

    QString serverAddress = ipLineEdit->text();
    QString portText = portLineEdit->text();
    int port = portText.toInt();

    if (serverAddress.isEmpty() || port == 0)
    {
        QMessageBox::warning(this, QStringLiteral("錯誤"), QStringLiteral("IP 或 Port 不能為空"));
        startButton->setEnabled(true);
        return;
    }

    QHostAddress address(serverAddress);
    while (!tcpServer.isListening() &&
           !tcpServer.listen(address, port))
    {
        QMessageBox::StandardButton ret = QMessageBox::critical(this,
                                                                QStringLiteral("錯誤"),
                                                                QStringLiteral("無法啟動伺服器: %1.")
                                                                    .arg(tcpServer.errorString()),
                                                                QMessageBox::Retry | QMessageBox::Cancel);
        if (ret == QMessageBox::Cancel)
        {
            startButton->setEnabled(true);
            return;
        }
    }

    serverStatusLabel->setText(QStringLiteral("監聽中，IP: %1, Port: %2").arg(serverAddress).arg(port));
}

void TcpFileServer::acceptConnection()
{
    tcpServerConnection = tcpServer.nextPendingConnection();
    connect(tcpServerConnection, SIGNAL(readyRead()),
            this, SLOT(updateServerProgress()));
    connect(tcpServerConnection, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(displayError(QAbstractSocket::SocketError)));
    serverStatusLabel->setText(QStringLiteral("接受連線"));
    tcpServer.close();
}

void TcpFileServer::updateServerProgress()
{
    QDataStream in(tcpServerConnection);
    in.setVersion(QDataStream::Qt_4_6);
    if(byteReceived <= sizeof(qint64)*2)
    {
        if((fileNameSize == 0) && (tcpServerConnection->bytesAvailable() >=
                                    sizeof(qint64)*2))
        {
            in >> totalBytes >> fileNameSize;
            byteReceived += sizeof(qint64)*2;
        }
        if((fileNameSize != 0) && (tcpServerConnection->bytesAvailable() >=
                                    fileNameSize)  )
        {
            in >> fileName;
            byteReceived += fileNameSize;
            localFile = new QFile(fileName);
            if(!localFile->open(QFile::WriteOnly))
            {
                QMessageBox::warning(this, QStringLiteral("應用程式"),
                                     QStringLiteral("無法讀取檔案 %1：\n%2.").arg(fileName)
                                         .arg(localFile->errorString()));
                return;
            }
        }else{
            return;
        }
    }

    if (byteReceived < totalBytes)
    {
        byteReceived += tcpServerConnection->bytesAvailable();
        inBlock = tcpServerConnection->readAll();
        localFile->write(inBlock);
        inBlock.resize(0);
    }

    serverProgressBar->setMaximum(totalBytes);
    serverProgressBar->setValue(byteReceived);
    qDebug() << byteReceived;
    serverStatusLabel->setText(QStringLiteral("已接收 %1 Bytes")
                                   .arg(byteReceived));

    if (byteReceived == totalBytes)
    {
        tcpServerConnection->close();
        startButton->setEnabled(true);
        localFile->fileName();
        localFile->close();
        start();
    }



}

void TcpFileServer::displayError(QAbstractSocket::SocketError socketError)
{
    QObject * server = qobject_cast<QObject *>(sender());
    if (server == tcpServerConnection) qDebug() << "Hi I am QTcpSocket";
    if (server == &tcpServer) qDebug() << "Hi I am QTcpServer";
    // if (socketError == QTcpSocket::RemoteHostClosedError)
    //     return;

    QMessageBox::information(this, QStringLiteral("網絡錯誤"),
                             QStringLiteral("產生如下錯誤: %1.")
                                 .arg(tcpServerConnection->errorString()));
    tcpServerConnection->close();
    serverProgressBar->reset();
    serverStatusLabel->setText(QStringLiteral("伺服器就緒"));
    startButton->setEnabled(true);

}
