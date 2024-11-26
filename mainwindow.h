#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "tcpfileserver.h"
#include "tcpfilesender.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    QTabWidget *tabWidget;
    TcpFileServer *fileServer;
    TcpFileSender *fileSender;
};
#endif // MAINWINDOW_H
