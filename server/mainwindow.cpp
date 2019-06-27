#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QNetworkInterface>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->progressBar->setValue(0);

    connect(ui->msgLineEdit, SIGNAL(returnPressed()), this, SLOT(on_sendMsgButton_clicked()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::receiveData(){

    if(!isHead&&!isFile){

        QByteArray text = serverSocket -> readAll();
        QString strText = QVariant(text).toString();

        if(strText == "Incoming file"){
            isHead = true;
        }

        if(text != "Starting file transfer..."){
            ui->mainTextArea->append(text);
        }else{
            sendFile();
        }

        }else if(isHead){

        isHead = false;
        isFile = true;
        QByteArray head = serverSocket -> readAll();

        qDebug()<<head;

        fileName = QVariant(head).toString().section("##",0,0);
        fileSize = QVariant(head).toString().section("##",1,1).toInt();

        ui->progressBar->setMaximum(fileSize);
        ui->progressBar->setMinimum(0);

        recSize = 0;
        file.setFileName(fileName);
        file.open(QIODevice::WriteOnly);
        serverSocket->write("Starting file transfer...");

    }else if(isFile){

        QByteArray buf = serverSocket -> readAll();
        qint64 len = file.write((buf));
        recSize += len;
        ui->progressBar->setValue(recSize);
        if(recSize == fileSize){
            file.close();
            serverSocket->write("File transfer succeed.");
            QMessageBox::information(this,"Complete", "File was recived.");
            isFile = false;
        }
    }

}


void MainWindow::newConnect(){
    serverSocket = tcpServer ->nextPendingConnection();
    QString clientIP = serverSocket->peerAddress().toString();
    qDebug()<<clientIP;
    ui -> mainTextArea ->append("IP: " + clientIP.mid(7) + " sucesfully connected to server!");
    connect(serverSocket,SIGNAL(readyRead()),this,SLOT(receiveData()));
    qDebug()<<"New connection!";
}
void MainWindow::on_startServerButton_clicked()
{
    ui->mainTextArea->setText("Waiting for clients...");
    tcpServer = new QTcpServer();

    qint16 serverPort = ui->portLineEdit->text().toInt();
    tcpServer->listen(QHostAddress::Any, serverPort);

    // Getting local IP adresss
    foreach (const QHostAddress &address, QNetworkInterface::allAddresses()) {
                if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost))
                     ui->ipLabel->setText(address.toString());
    }

    connect(tcpServer,SIGNAL(newConnection()),this,SLOT(newConnect()));
    qDebug()<<"Server started!";
}

void MainWindow::on_sendMsgButton_clicked()
{
    QString text = ui->msgLineEdit->text();
    serverSocket->write(QString(text).toUtf8());
    ui->mainTextArea->append("<b>Server:</b> " + text);
    ui -> msgLineEdit->clear();
}

void MainWindow::on_openFileButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,tr("Open File"),"../");
    if(!filePath.isEmpty()){
        fileName.clear();
        fileSize = 0;

        QFileInfo info(filePath);
        fileName = info.fileName();
        fileSize = info.size();
        sendSize = 0;
        file.setFileName(filePath);
        file.open(QIODevice::ReadOnly);
        ui->filePathLabel->setText(filePath);
        serverSocket->write("Incoming file");

    }else{
        ui->filePathLabel->setText("Opening file filed.");
    }
}

void MainWindow::on_sendFileButton_clicked()
{
    QString head = QString("%1##%2").arg(fileName).arg(fileSize);
    qint64 len = serverSocket->write(head.toUtf8());
    if(len < 0){
        QMessageBox::warning(this,"Warning","File header was not send.");
        file.close();
    }
}

void MainWindow::sendFile(){
    qint64 len = 0;
    do{
        char buf[4*1024];
        len = 0;
        len = file.read(buf,4*1024);
        len = serverSocket->write(buf,len);
    }while(len>0);
}
