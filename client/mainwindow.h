#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTcpServer>
#include <QMessageBox>
#include <QFileInfo>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QTcpSocket* clientSocket;
public slots:
    void reveiveData();
    void sendFile();
private slots:
    void on_sendMsgButton_clicked();
    void on_loginButton_clicked();

    void on_openFileButton_clicked();
    void on_sendFileButton_clicked();

private:
    Ui::MainWindow *ui;

    QString userName;

    bool isHead = false;
    bool isFile = false;
    QFile file;
    QString fileName;
    qint64 fileSize;
    qint64 sendSize;
    qint64 recSize;
};

#endif // MAINWINDOW_H
