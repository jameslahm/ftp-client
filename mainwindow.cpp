#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QLabel *serverAdressLabel = new QLabel(tr("IP/Host"));
    serverAdressInput =new QLineEdit("127.0.0.1");
    QLabel *serverPortLabel = new QLabel(tr("Port"));
    serverPortInput = new QLineEdit("21");

    QLabel *userNameLabel=new QLabel(tr("Username"));
    userNameInput = new QLineEdit("anonymous");
    QLabel *passwordLabel = new QLabel(tr("Password"));
    passwordInput = new QLineEdit;
    passwordInput->setEchoMode(QLineEdit::Password);

    QGroupBox *groupRadioBox = new QGroupBox(tr("Data Connection Mode"));

    QRadioButton *pasvRadio = new QRadioButton(tr("PASV"));
    QRadioButton *portRadio = new QRadioButton(tr("PORT"));
    portRadio->setChecked(true);

    connect(pasvRadio,&QRadioButton::clicked,[=](){this->changeDataConnMode(DataConnMode::PASV);});
    connect(pasvRadio,&QRadioButton::clicked,[=](){this->changeDataConnMode(DataConnMode::PORT);});


    QHBoxLayout *groupRadioLayout = new QHBoxLayout;
    groupRadioLayout->addWidget(pasvRadio);
    groupRadioLayout->addWidget(portRadio);
    groupRadioBox->setLayout(groupRadioLayout);

    connectButton = new QPushButton(tr("connect"));
    quitButton = new QPushButton(tr("quit"));

    QLabel *logPanelLabel =new QLabel(tr("Command Logs"));
    logPanel=new QPlainTextEdit;
    logPanel->setReadOnly(true);

    QLabel *localFileListLabel=new QLabel(tr("Local Files"));
    localFileList = new QTreeWidget;
    localFileList->setRootIsDecorated(false);
    localFileList->setHeaderLabels(QStringList()<<tr("Name")<<tr("Size")<<tr("Owner")<<tr("Group")<<tr("Time"));
    //    localFileList->header()->setStretchLastSection(false);
    localFileList->header()->setSectionResizeMode(QHeaderView::Stretch);

    QLabel *remoteFileListLabel=new QLabel(tr("Remote Files"));
    remoteFileList =new QTreeWidget;
    remoteFileList->setRootIsDecorated(false);
    remoteFileList->setHeaderLabels(QStringList()<<tr("Name")<<tr("Size")<<tr("Owner")<<tr("Group")<<tr("Time"));
    //    remoteFileList->header()->setStretchLastSection(false);
    remoteFileList->header()->setSectionResizeMode(QHeaderView::Stretch);

    QLabel* taskListLabel = new QLabel(tr("Task List"));
    taskList=new QTreeWidget;
    taskList->setRootIsDecorated(false);
    taskList->setHeaderLabels(QStringList()<<tr("Name")<<tr("Progress"));
    //    taskList->header()->setStretchLastSection(false);
    taskList->header()->setSectionResizeMode(QHeaderView::Stretch);

    statusLabel = new QLabel("Please enter the name of an FTP　");

    QVBoxLayout *mainLayout = new QVBoxLayout;

    QVBoxLayout *inputLayout=new QVBoxLayout;

    QHBoxLayout *inputHLayout = new QHBoxLayout;
    inputHLayout->addWidget(serverAdressLabel);
    inputHLayout->addWidget(serverAdressInput);
    inputHLayout->addWidget(serverPortLabel);
    inputHLayout->addWidget(serverPortInput);
    inputHLayout->addWidget(userNameLabel);
    inputHLayout->addWidget(userNameInput);
    inputHLayout->addWidget(passwordLabel);
    inputHLayout->addWidget(passwordInput);


    QHBoxLayout *inputLLayout=new QHBoxLayout;
    inputLLayout->addWidget(groupRadioBox);
    inputLLayout->addStretch(1);
    inputLLayout->addWidget(connectButton);
    inputLLayout->addWidget(quitButton);

    inputLayout->addLayout(inputHLayout);
    inputLayout->addLayout(inputLLayout);

    mainLayout->addLayout(inputLayout);
    mainLayout->addWidget(logPanelLabel);
    mainLayout->addWidget(logPanel);

    QHBoxLayout *fileLayout=new QHBoxLayout;
    QVBoxLayout *localFileLayout = new QVBoxLayout;
    localFileLayout->addWidget(localFileListLabel);
    localFileLayout->addWidget(localFileList);

    QVBoxLayout *remoteFileLayout = new QVBoxLayout;
    remoteFileLayout->addWidget(remoteFileListLabel);
    remoteFileLayout->addWidget(remoteFileList);

    fileLayout->addLayout(localFileLayout);
    fileLayout->addLayout(remoteFileLayout);

    mainLayout->addLayout(fileLayout);

    mainLayout->addWidget(taskListLabel);
    mainLayout->addWidget(taskList);
    mainLayout->addWidget(statusLabel);

    QWidget *centralWidget=new QWidget;
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    setWindowTitle(tr("FTP"));

    connect(connectButton,&QPushButton::clicked,[=](){this->connectToServer();});

}

void MainWindow::changeDataConnMode(DataConnMode mode){
    this->dataConnMode = mode;
    qDebug()<<"Change mode ";
    if(mode==DataConnMode::PASV){
        qDebug()<<"PASV"<<endl;
    }
    else{
        qDebug()<<"PORT"<<endl;
    }
}

void MainWindow::connectToServer(){
    commandSocket=new QTcpSocket(this);
    connectButton->setEnabled(false);

    connect(commandSocket,&QIODevice::readyRead,[=](){this->handleCommandResponse();});

    connect(commandSocket,QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),this,&MainWindow::handleCommandError);
    commandSocket->connectToHost(serverAdressInput->text(),serverPortInput->text().toUShort());
}

void MainWindow::handleCommandError(QAbstractSocket::SocketError socketError){
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        qDebug()<<"Host not found"<<endl;
        break;
    case QAbstractSocket::ConnectionRefusedError:
        qDebug()<<"Connection refused"<<endl;
        break;
    default:
        qDebug()<<"Some error happend"<<endl;
    }
}

void MainWindow::handleCommandResponse(){
    QString responseBuf(commandSocket->readAll());

    logPanel->appendPlainText(responseBuf);

    qDebug()<<responseBuf<<endl;

    QPair<int,QString> parseRes=parseCommandResponse(responseBuf);
    int responseCode=parseRes.first;
    QString responseMessage=parseRes.second;

    qDebug()<<responseCode<<endl;

    if(responseCode==220){
        sendCommandRequest(RequestType::USER,userNameInput->text()+"\r\n");
    }

    if(responseCode==331){
        sendCommandRequest(RequestType::PASS,passwordInput->text()+"\r\n");
    }

    if(responseCode==230){
        sendCommandRequest(RequestType::SYST,"\r\n");
    }

    if(responseCode==215){
        sendCommandRequest(RequestType::TYPE,"I\r\n");
        dataConnStatus=DataConnStatus::IDLE;
    }

    if(responseCode == 200){
        if(dataConnStatus==DataConnStatus::IDLE){
            initDataConn();
            if(dataConnMode==DataConnMode::PORT){
                QString ipAddress;
                int port=dataServer->serverPort();
                QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
                for (int i = 0; i < ipAddressesList.size(); ++i) {
                    if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
                            ipAddressesList.at(i).toIPv4Address()) {
                        ipAddress = ipAddressesList.at(i).toString();
                        break;
                    }
                }
                if (ipAddress.isEmpty())
                    ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
                sendCommandRequest(RequestType::PORT,QString("%1,%2,%3\r\n").arg(ipAddress).arg(port/256).arg(port%256).replace(".",","));
                statusLabel->setText(tr("The server is running on IP: %1 port: %2\n")
                                             .arg(ipAddress).arg(dataServer->serverPort()));
            }
            dataConnStatus=DataConnStatus::CONNECT;
        }
    }

}

void MainWindow::sendCommandRequest(RequestType requestType, QString requestArgs){
    QByteArray block;
    QString requestBuf = QString("%1 %2").arg(RequestTypeMap[requestType]).arg(requestArgs);
    block.append(requestBuf);
    commandSocket->write(block);
    commandSocket->flush();
    logPanel->appendPlainText(requestBuf);
}

QPair<int,QString> MainWindow::parseCommandResponse(QString responseBuf){
    QTextStream responseBufStream(&responseBuf);
    int responseCode;
    QString responseMessage;
    while(!responseBufStream.atEnd()){
        QString tmpBuf = responseBufStream.readLine();
        QTextStream tmpBufStream(&tmpBuf);
        tmpBufStream>>responseCode>>responseMessage;
    }
    return qMakePair(responseCode,responseMessage);
}

void MainWindow::initDataConn(){
    if(dataConnMode==DataConnMode::PORT){
        if(dataSocket!=nullptr){
            dataSocket->close();
            dataSocket->deleteLater();
            dataSocket=nullptr;
        }

        dataServer = new QTcpServer(this);
        if (!dataServer->listen()) {
            QMessageBox::critical(this, tr("Data Conn Server"),
                                  tr("Unable to start the server: %1.")
                                  .arg(dataServer->errorString()));
            close();
            return;
        }
        connect(dataServer,&QTcpServer::newConnection,this,&MainWindow::handlePortDataConn);
    }
}

void MainWindow::handlePortDataConn(){
    dataSocket=dataServer->nextPendingConnection();
    qDebug()<<"New data connection "<<dataSocket<<endl;
    connect(dataSocket, &QAbstractSocket::disconnected,dataSocket, &QObject::deleteLater);
}

MainWindow::~MainWindow()
{

}

void MainWindow::centerAndResize(){
    // get the dimension available on this screen
    QSize availableSize = qApp->primaryScreen()->geometry().size();
    int width = availableSize.width();
    int height = availableSize.height();
    qDebug() << "Available dimensions " << width << "x" << height;
    width *= 0.8; // 60% of the screen size
    height *= 0.8; // 60% of the screen size
    qDebug() << "Computed dimensions " << width << "x" << height;
    QSize newSize( width, height );

    setGeometry(
                QStyle::alignedRect(
                    Qt::LeftToRight,
                    Qt::AlignCenter,
                    newSize,
                    qApp->primaryScreen()->geometry()
                    )
                );
}
