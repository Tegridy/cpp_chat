#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    clientSocket = new QTcpSocket();

    ui->setupUi(this);
    ui->progressBar->setValue(0);

    connect(ui->msgEdit, SIGNAL(returnPressed()), this, SLOT(on_sendMsgButton_clicked()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::reveiveData(){

    if(!isHead&&!isFile){       

        QByteArray text = clientSocket -> readAll();
        QString strText = QVariant(text).toString();


        if(text == "Starting file transfer..."){
            sendFile();
        }

        if(strText == "Incoming file"){
            isHead = true;
        }else{
            ui ->mainTextArea->append("<b>Server:</b> " + strText);
        }

    }else if(isHead){
        isHead = false;
        isFile = true;
        QByteArray head = clientSocket -> readAll();

        qDebug()<<head;

        fileName = QVariant(head).toString().section("##",0,0);
        fileSize = QVariant(head).toString().section("##",1,1).toInt();

        ui->progressBar->setMaximum(fileSize);
        ui->progressBar->setMinimum(0);

        recSize = 0;
        file.setFileName(fileName);
        file.open(QIODevice::WriteOnly);
        clientSocket->write("Starting file transfer...");

    }else if(isFile){
        QByteArray buf = clientSocket -> readAll();
        qint64 len = file.write((buf));             
        recSize += len;

        ui->progressBar->setValue(recSize);

        if(recSize == fileSize){
            file.close();
            clientSocket->write("File transfer succeed.");
            QMessageBox::information(this,"Complete", "File recived.");
            isFile = false;
        }
    }

}

void MainWindow::sendFile(){
    qint64 len = 0;
    do{
        char buf[4*1024];
        len = 0;
        len = file.read(buf,4*1024);
        len = clientSocket->write(buf,len);
    }while(len>0);
}

void MainWindow::on_openFileButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,tr("Open File"),"../");
    if(!filePath.isEmpty()){
        fileName.clear();
        fileSize = 0;
        // Get file name and size
        QFileInfo info(filePath);
        fileName = info.fileName();
        fileSize = info.size();
        sendSize = 0;

        file.setFileName(filePath);
        file.open(QIODevice::ReadOnly);
        ui->filePathLabel->setText(filePath);

        clientSocket->write("Incoming file");

    }else{
        ui->filePathLabel->setText("Opening file filed.");
    }
}

void MainWindow::on_sendFileButton_clicked()
{
    QString head = QString("%1##%2").arg(fileName).arg(fileSize);
    qint64 len = clientSocket->write(head.toUtf8());
    if(len < 0){
        QMessageBox::warning(this,"Warning","File header was not send.");
        file.close();
    }
}

void MainWindow::on_sendMsgButton_clicked()
{
    QString text = ui -> msgEdit -> text();

    ui->mainTextArea->append(userName + ": " + text);

    clientSocket->write(QString(userName + ": " + text).toUtf8());

    ui -> msgEdit ->clear();
    ui -> msgEdit ->cursor();
}

void MainWindow::on_loginButton_clicked()
{
    QString serverAddress = ui -> ipLineEdit -> text();
    quint16 serverPort = ui->portLineEdit-> text().toInt();
    userName = ui->userLineEdit->text().trimmed();
    userName = "<b>" + userName + "</b>";

    clientSocket -> connectToHost(serverAddress,serverPort);

    connect(clientSocket,SIGNAL(readyRead()),this,SLOT(reveiveData()));

    QMessageBox::information(this,tr("OK"),tr("Connected to server!"));

}
