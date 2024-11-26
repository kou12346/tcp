#include "mainwindow.h"
#include "tcpfileserver.h"
#include "tcpfilesender.h"
#include <QTabWidget>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    tabWidget = new QTabWidget(this);

    fileServer = new TcpFileServer(this);
    fileSender = new TcpFileSender(this);

    tabWidget->addTab(fileServer, QStringLiteral("SERVER"));
    tabWidget->addTab(fileSender, QStringLiteral("SENDER"));

    setCentralWidget(tabWidget);

    setWindowTitle("檔案傳送與伺服器");
}

MainWindow::~MainWindow()
{
    delete fileServer;
    delete fileSender;
}
