#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTcpServer>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QTcpSocket* serverSocket;
    QTcpServer* tcpServer;
    void sendFile();
private:

    bool isHead = false;
    bool isFile = false;

    Ui::MainWindow *ui;
    QFile file;
    QString fileName;
    qint64 fileSize;
    qint64 sendSize;
    qint64 recSize;

    QString clientName;

public slots:
    void receiveData();
    void newConnect();

private slots:
    void on_startServerButton_clicked();
    void on_sendMsgButton_clicked();
    void on_openFileButton_clicked();
    void on_sendFileButton_clicked();
};

#endif // MAINWINDOW_H
