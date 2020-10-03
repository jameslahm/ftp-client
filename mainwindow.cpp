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
    localFileList->header()->setSectionResizeMode(QHeaderView::Interactive);

    QLabel *remoteFileListLabel=new QLabel(tr("Remote Files"));
    remoteFileList =new QTreeWidget;
    remoteFileList->setRootIsDecorated(false);
    remoteFileList->setHeaderLabels(QStringList()<<tr("Name")<<tr("Size")<<tr("Owner")<<tr("Group")<<tr("Time"));
    //    remoteFileList->header()->setStretchLastSection(false);
    remoteFileList->header()->setSectionResizeMode(QHeaderView::Interactive);

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
    qDebug().noquote()<<"Change mode ";
    if(mode==DataConnMode::PASV){
        qDebug().noquote()<<"PASV"<<endl;
    }
    else{
        qDebug().noquote()<<"PORT"<<endl;
    }
}

void MainWindow::connectToServer(){
    commandSocket=new QTcpSocket(this);
    connectButton->setEnabled(false);

    connect(commandSocket,&QIODevice::readyRead,[=](){this->handleCommandResponse();});

    connect(commandSocket,QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),this,&MainWindow::handleCommandError);
    commandSocket->connectToHost(serverAdressInput->text(),serverPortInput->text().toUShort());

    serverStatus=ServerStatus::COMMAND_CONNECT_START;
}

void MainWindow::handleCommandError(QAbstractSocket::SocketError socketError){
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        qDebug().noquote()<<"Host not found"<<endl;
        break;
    case QAbstractSocket::ConnectionRefusedError:
        qDebug().noquote()<<"Connection refused"<<endl;
        break;
    default:
        qDebug().noquote()<<"Some error happend"<<endl;
    }
}

void MainWindow::handleCommandResponse(){
    QString responseBuf(commandSocket->readAll());

    logPanel->appendPlainText(responseBuf);

    qDebug().noquote()<<responseBuf<<endl;

    QPair<int,QString> parseRes=parseCommandResponse(responseBuf);
    int responseCode=parseRes.first;
    QString responseMessage=parseRes.second;

    if(responseCode==220){
        // command connect success
        if(serverStatus==ServerStatus::COMMAND_CONNECT_START){
            changeServerStatus(ServerStatus::COMMAND_CONNECT_END);
        }
        if(serverStatus==ServerStatus::PORT_START){
            changeServerStatus(ServerStatus::PORT_END);
        }
    }

    if(responseCode==331){
        if(serverStatus==ServerStatus::USER_START){
            changeServerStatus(ServerStatus::USER_END);
        }
    }

    if(responseCode==230){
        if(serverStatus==ServerStatus::PASS_START){
            changeServerStatus(ServerStatus::PASS_END);
        }
    }

    if(responseCode==215){
        if(serverStatus==ServerStatus::SYST_START){
            changeServerStatus(ServerStatus::SYST_END);
        }
    }

    if(responseCode == 200){
        if(serverStatus==ServerStatus::TYPE_START){
            changeServerStatus(ServerStatus::TYPE_END);
        }
        if(serverStatus==ServerStatus::PORT_START){
            changeServerStatus(ServerStatus::PORT_END);
        }
    }

    if(responseCode==226){
        if(serverStatus==ServerStatus::LIST_START){
            changeServerStatus(ServerStatus::LIST_END);
        }
    }

}

void MainWindow::changeServerStatus(ServerStatus status){
    if(status==ServerStatus::COMMAND_CONNECT_END){
        serverStatus=ServerStatus::USER_START;
        sendCommandRequest(RequestType::USER,userNameInput->text()+"\r\n");
    }
    if(status==ServerStatus::USER_END){
        serverStatus=ServerStatus::PASS_START;
        sendCommandRequest(RequestType::PASS,passwordInput->text()+"\r\n");
    }
    if(status==ServerStatus::PASS_END){
        serverStatus=ServerStatus::SYST_START;
        sendCommandRequest(RequestType::SYST,"\r\n");
    }
    if(status==ServerStatus::SYST_END){
        serverStatus=ServerStatus::TYPE_START;
        sendCommandRequest(RequestType::TYPE,"I\r\n");
    }
    if(status==ServerStatus::TYPE_END){
        if(dataConnStatus==DataConnStatus::DISCONNECT){
            initDataConn();
            if(dataConnMode==DataConnMode::PORT){
                serverStatus=ServerStatus::PORT_START;
                QString ipAddress;
                int port=dataServer->serverPort();
                ipAddress=commandSocket->localAddress().toString();
                sendCommandRequest(RequestType::PORT,QString("%1,%2,%3\r\n").arg(ipAddress).arg(port/256).arg(port%256).replace(".",","));
                statusLabel->setText(tr("The server is running on IP: %1 port: %2\n")
                                             .arg(ipAddress).arg(dataServer->serverPort()));
                dataConnStatus=DataConnStatus::CONNECT;
            }
        }
    }
    if(status==ServerStatus::PORT_END){
        if(remoteDirPath==nullptr){
            serverStatus=ServerStatus::LIST_START;
            sendCommandRequest(RequestType::LIST,"\r\n");
        }
    }
    if(status==ServerStatus::LIST_END){
        status=ServerStatus::IDLE;
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
        qDebug().noquote()<<"Port server listening"<<endl;
    }
}

void MainWindow::handlePortDataConn(){
    dataSocket=dataServer->nextPendingConnection();
    qDebug().noquote()<<"New data connection "<<dataSocket<<endl;
    connect(dataSocket,&QTcpSocket::readyRead,this,&MainWindow::handleDataConnResponse);
    connect(dataSocket, &QAbstractSocket::disconnected,dataSocket, &QObject::deleteLater);
}

void MainWindow::handleDataConnResponse(){
    if(serverStatus==ServerStatus::LIST_START){
        QString responseBuf(dataSocket->readAll());
        qDebug().noquote()<<responseBuf<<endl;
        QString permission;
        QString owner;
        QString group;
        QString size;
        QString month;
        QString day;
        QString year;
        QString name;
        QString time;
        QString tmp;
        QTextStream responseBufStream(&responseBuf);
        while(!responseBufStream.atEnd()){
            QString lineBuf=responseBufStream.readLine();
            QTextStream lineBufStream(&lineBuf);

            lineBufStream>>permission>>tmp>>owner>>group>>size>>month>>day>>year>>name;

            time=month+" "+day+" "+year;

            QTreeWidgetItem *item=new QTreeWidgetItem;
            item->setText(0,name);
            item->setText(1,size);
            item->setText(2,owner);
            item->setText(3,group);
            item->setText(4,time);
            isRemoteDirectory[name]=(permission[0]=='d');

            QPixmap pixmap(isRemoteDirectory[name]? ":/images/dir.png":":/images/file.png");
            item->setIcon(0,pixmap);

            remoteFileList->addTopLevelItem(item);
        }
    }
}

MainWindow::~MainWindow()
{

}

void MainWindow::centerAndResize(){
    // get the dimension available on this screen
    QSize availableSize = qApp->primaryScreen()->geometry().size();
    int width = availableSize.width();
    int height = availableSize.height();
    qDebug().noquote() << "Available dimensions " << width << "x" << height;
    width *= 0.8; // 60% of the screen size
    height *= 0.8; // 60% of the screen size
    qDebug().noquote() << "Computed dimensions " << width << "x" << height;
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
