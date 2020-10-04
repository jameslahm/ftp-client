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
    localDirPathLabel=new QLabel("");

    QPushButton *changeLocalDirButton=new QPushButton(tr("change"));
    localFileList = new QTreeWidget;
    localFileList->setRootIsDecorated(false);
    localFileList->setHeaderLabels(QStringList()<<tr("Name")<<tr("Size")<<tr("Owner")<<tr("Group")<<tr("Time"));
    //    localFileList->header()->setStretchLastSection(false);
    localFileList->header()->setSectionResizeMode(QHeaderView::Interactive);

    QLabel *remoteFileListLabel=new QLabel(tr("Remote Files"));
    remoteDirPathLabel=new QLabel("");
    returnToParentButton=new QPushButton(tr("return"));

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

    QHBoxLayout *localFileLabelLayout=new QHBoxLayout;
    localFileLabelLayout->addWidget(localFileListLabel);
    localFileLabelLayout->addWidget(localDirPathLabel);
    localFileLabelLayout->addStretch(1);
    localFileLabelLayout->addWidget(changeLocalDirButton);

    localFileLayout->addLayout(localFileLabelLayout);
    localFileLayout->addWidget(localFileList);

    QVBoxLayout *remoteFileLayout = new QVBoxLayout;
    QHBoxLayout *remoteFileLabelLayout=new QHBoxLayout;

    remoteFileLabelLayout->addWidget(remoteFileListLabel);
    remoteFileLabelLayout->addWidget(remoteDirPathLabel);
    remoteFileLabelLayout->addStretch(1);
    remoteFileLabelLayout->addWidget(returnToParentButton);
    remoteFileLayout->addLayout(remoteFileLabelLayout);
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

    remoteFileList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(remoteFileList,&QTreeWidget::customContextMenuRequested,[=](QPoint pos){this->handleContextMenu(pos,"REMOTE");});

    localFileList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(localFileList,&QTreeWidget::customContextMenuRequested,[=](QPoint pos){this->handleContextMenu(pos,"LOCAL");});

    connect(connectButton,&QPushButton::clicked,[=](){this->connectToServer();});
    connect(localFileList,&QTreeWidget::itemDoubleClicked,[=](QTreeWidgetItem* item,int){if(item){
            this->changeLocalDir(item->text(0));
        }});
    connect(remoteFileList,&QTreeWidget::itemDoubleClicked,[=](QTreeWidgetItem* item,int){if(item){
            this->changeRemoteDir(item->text(0));
        }});

    connect(changeLocalDirButton,&QPushButton::clicked,[=](){
        QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                        QDir::homePath(),
                                                        QFileDialog::ShowDirsOnly
                                                        | QFileDialog::DontResolveSymlinks);
        if(dir.isEmpty()){
            return;
        }
        else{
            this->changeLocalDir(dir);
        }
    });
    connect(returnToParentButton,&QPushButton::clicked,[=](){
        this->changeRemoteDir("..");
    });

    returnToParentButton->setEnabled(false);
    changeLocalDir(QDir::currentPath());
}

void MainWindow::changeLocalDir(QString path){
    localFileList->clear();
    isLocalDirectory.clear();

    localDir.cd(path);
    if(path=="."){
        localDir.refresh();
    }

    qDebug()<<"Set local dir: "<<localDir.absolutePath();
    localDirPathLabel->setText(localDir.absolutePath());

    foreach(QFileInfo fileInfo,localDir.entryInfoList()){
        QTreeWidgetItem* item=new QTreeWidgetItem;
        item->setText(0,fileInfo.fileName());
        item->setText(1,QString::number(fileInfo.size()));
        item->setText(2,fileInfo.owner());
        item->setText(3,fileInfo.group());
        QLocale locale{QLocale::English};
        item->setText(4,locale.toString(fileInfo.lastModified(),"MMM dd yyyy"));
        isLocalDirectory[fileInfo.fileName()]=(fileInfo.isDir());

        QPixmap pixmap(isLocalDirectory[fileInfo.fileName()]? ":/images/dir.png":":/images/file.png");
        item->setIcon(0,pixmap);

        localFileList->addTopLevelItem(item);
    }
}

void MainWindow::changeRemoteDir(QString path){
    QString destPath=remoteDirPath;
    if(path==".."){
        QList<QString> destPathList=destPath.split("/");
        destPathList.pop_back();
        qDebug()<<destPathList<<endl;
        if(destPathList.length()==1){
            destPath="/";
        }
        else{
            destPath=destPathList.join("/");
        }
    }
    else{
        if(remoteDirPath!="/"){
            destPath=destPath+"/"+path;
        }
        else{
            destPath=destPath+path;
        }
    }
    sendCommandRequest(RequestType::CWD,destPath+"\r\n");
    remoteDirPathLabel->setText(destPath);

    // TODO: refactor
    if(destPath==remoteRootDirPath){
        returnToParentButton->setEnabled(false);
    }
    else{
        returnToParentButton->setEnabled(true);
    }
    remoteDirPath=destPath;
    qDebug()<<"RemoteDirPath: "<<remoteDirPath<<endl;

    changeServerStatus(ServerStatus::CWD_START);
}


void MainWindow::handleContextMenu(QPoint pos,QString fileListType){
    if(fileListType=="REMOTE"){
        QTreeWidgetItem* item=remoteFileList->itemAt(pos);

        if(item!=nullptr){
            QAction *downloadAction=new QAction(tr("Download"));
            QMenu menu(this);
            connect(downloadAction,&QAction::triggered,[=](){
                this->currentFileName=item->text(0);
                this->addTask(this->currentFileName,item->text(1).toInt());
                this->changeServerStatus(ServerStatus::RETR_START);
            });

            menu.addAction(downloadAction);
            menu.exec(remoteFileList->mapToGlobal(pos));
        }
        else{
            //TODO: make dir
        }

    }
    if(fileListType=="LOCAL"){
        QTreeWidgetItem* item=localFileList->itemAt(pos);

        if(item!=nullptr){
            QAction *uploadAction=new QAction(tr("Upload"));
            QMenu menu(this);
            connect(uploadAction,&QAction::triggered,[=](){
                this->currentFileName=item->text(0);
                this->addTask(this->currentFileName,item->text(1).toInt());
                this->changeServerStatus(ServerStatus::STOR_START);
            });

            menu.addAction(uploadAction);
            menu.exec(localFileList->mapToGlobal(pos));
        }
    }
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

    qDebug()<<"Start parse response: ";
    bool isResponseEnd=false;
    QString responseBuf;
    int responseCode=-1;
    while(isResponseEnd==false){
        QString tmpBuf(commandSocket->readLine());
        if(tmpBuf.isEmpty()){
            break;
        }
        logPanel->appendPlainText(tmpBuf);
        qDebug().noquote()<<"Response: "<<tmpBuf;
        if(tmpBuf.indexOf("-")==-1){
            isResponseEnd=true;
        }
        tmpBuf=tmpBuf.replace("-"," ");
        QPair<int,QString> parseRes=parseCommandResponse(tmpBuf);
        responseCode=parseRes.first;
        responseBuf+=parseRes.second;
    }
    qDebug()<<"Parse response successfully"<<endl;

    if(responseCode==150){
        if(commandSocket->bytesAvailable()!=0){
            handleCommandResponse();
            return;
        }
    }

    if(responseCode==220){
        // command connect success
        if(serverStatus==ServerStatus::COMMAND_CONNECT_START){
            changeServerStatus(ServerStatus::COMMAND_CONNECT_END);
            return;
        }
    }

    if(responseCode==331){
        if(serverStatus==ServerStatus::USER_START){
            changeServerStatus(ServerStatus::USER_END);
            return;
        }
    }

    if(responseCode==230){
        if(serverStatus==ServerStatus::PASS_START){
            changeServerStatus(ServerStatus::PASS_END);
            return;
        }
    }

    if(responseCode==215){
        if(serverStatus==ServerStatus::SYST_START){
            changeServerStatus(ServerStatus::SYST_END);
            return;
        }
    }

    if(responseCode == 200){
        if(serverStatus==ServerStatus::TYPE_START){
            changeServerStatus(ServerStatus::TYPE_END);
            return;
        }
        if(serverStatus==ServerStatus::PORT_START){
            changeServerStatus(ServerStatus::PORT_END);
            return;
        }
    }

    if(responseCode==226){
        if(serverStatus==ServerStatus::LIST_START){
            return;
        }
        if(serverStatus==ServerStatus::RETR_START){
            return;
        }
        if(serverStatus==ServerStatus::STOR_START){
            return;
        }
    }

    if(responseCode==257){
        if(serverStatus==ServerStatus::PWD_START){
            QTextStream tmpStream(&responseBuf);
            tmpStream>>remoteDirPath;
            remoteDirPath=remoteDirPath.section('"',1,1);
            qDebug().noquote()<<"Remote dir path: "<<remoteDirPath<<endl;
            if(remoteRootDirPath==nullptr){
                remoteRootDirPath=remoteDirPath;
            }
            remoteDirPathLabel->setText(remoteDirPath);
            changeServerStatus(ServerStatus::PWD_END);
            return;
        }
    }
    if(responseCode==250){
        if(serverStatus==ServerStatus::CWD_START){
            changeServerStatus(ServerStatus::CWD_END);
        }
    }

}

void MainWindow::changeServerStatus(ServerStatus status){
    qDebug().noquote()<<"Change serverStatus from "<<stringifyServerStatus(serverStatus)<<" to"<<stringifyServerStatus(status)<<endl;
    serverStatus=status;
    if(status==ServerStatus::COMMAND_CONNECT_END){
        changeServerStatus(ServerStatus::USER_START);
        return;
    }
    if(status==ServerStatus::USER_START){
        sendCommandRequest(RequestType::USER,userNameInput->text()+"\r\n");
        return;
    }
    if(status==ServerStatus::USER_END){
        changeServerStatus(ServerStatus::PASS_START);
        return;
    }
    if(status==ServerStatus::PASS_START){
        sendCommandRequest(RequestType::PASS,passwordInput->text()+"\r\n");
        return;
    }
    if(status==ServerStatus::PASS_END){
        changeServerStatus(ServerStatus::SYST_START);
        return;
    }
    if(status==ServerStatus::SYST_START){
        sendCommandRequest(RequestType::SYST,passwordInput->text()+"\r\n");
        return;
    }
    if(status==ServerStatus::SYST_END){
        changeServerStatus(ServerStatus::TYPE_START);
        return;
    }
    if(status==ServerStatus::TYPE_START){
        sendCommandRequest(RequestType::TYPE,"I\r\n");
        return;
    }
    if(status==ServerStatus::TYPE_END){
        if(remoteDirPath==nullptr){
            changeServerStatus(ServerStatus::PWD_START);
            return;
        }
    }
    if(status==ServerStatus::PORT_START){
        QString ipAddress;
        int port=dataServer->serverPort();
        ipAddress=commandSocket->localAddress().toString();
        sendCommandRequest(RequestType::PORT,QString("%1,%2,%3\r\n").arg(ipAddress).arg(port/256).arg(port%256).replace(".",","));
        statusLabel->setText(tr("The server is running on IP: %1 port: %2\n")
                             .arg(ipAddress).arg(dataServer->serverPort()));
        return;
    }
    if(status==ServerStatus::PORT_END){
        dataConnStatus=DataConnStatus::CONNECT;
        qDebug().noquote()<<"Resume status: "<<stringifyServerStatus(resumeServerStatus)<<endl;
        changeServerStatus(resumeServerStatus);
        return;
    }
    if(status==ServerStatus::PWD_START){
        sendCommandRequest(RequestType::PWD,"\r\n");
        return;
    }
    if(status==ServerStatus::PWD_END){
        changeServerStatus(ServerStatus::LIST_START);
        return ;
    }
    if(status==ServerStatus::CWD_END){
        changeServerStatus(ServerStatus::LIST_START);
        return;
    }
    if(status==ServerStatus::CWD_START){
        return;
    }
    if(status==ServerStatus::LIST_START){
        if(dataConnStatus==DataConnStatus::DISCONNECT){
            resumeServerStatus=ServerStatus::LIST_START;
            initDataConn();
        }
        else{
            sendCommandRequest(RequestType::LIST,"\r\n");
        }
        return;
    }
    if(status==ServerStatus::LIST_END){
        changeServerStatus(ServerStatus::IDLE);
        return;
    }
    if(status==ServerStatus::RETR_START){
        if(dataConnStatus==DataConnStatus::DISCONNECT){
            resumeServerStatus=ServerStatus::RETR_START;
            initDataConn();
        }
        else{
            currentFile.setFileName(currentFileName);
            if(!currentFile.open(QIODevice::WriteOnly)){

                qDebug().noquote()<<"can't open file"<<endl;
            }
            sendCommandRequest(RequestType::RETR,currentFileName+"\r\n");
            return;
        }
    }
    if(status==ServerStatus::RETR_END){
        changeServerStatus(ServerStatus::IDLE);
        return;
    }
    if(status==ServerStatus::STOR_START){
        if(dataConnStatus==DataConnStatus::DISCONNECT){
            resumeServerStatus=ServerStatus::STOR_START;
            initDataConn();
        }
        else{
            currentFile.setFileName(currentFileName);
            if(!currentFile.open(QIODevice::ReadOnly)){

                qDebug().noquote()<<"can't open file"<<endl;
            }
            sendCommandRequest(RequestType::STOR,currentFileName+"\r\n");
            return;
        }
    }
    if(status==ServerStatus::STOR_END){
        changeServerStatus(ServerStatus::LIST_START);
        return;
    }
}

void MainWindow::addTask(QString taskName, int maximum){
    QTreeWidgetItem *item=new QTreeWidgetItem;
    item->setText(0,taskName);
    taskList->addTopLevelItem(item);
    QProgressBar *progressBar=new QProgressBar();
    progressBar->setMinimum(0);
    progressBar->setMaximum(maximum);
    progressBar->setValue(0);
    progressBar->setMaximumWidth(200);
    progressBar->setAlignment(Qt::AlignVCenter | Qt::AlignCenter);
    taskList->setItemWidget(item,1,progressBar);
}

void MainWindow::sendCommandRequest(RequestType requestType, QString requestArgs){
    QByteArray block;
    QString requestBuf = QString("%1 %2").arg(RequestTypeMap[requestType]).arg(requestArgs);
    block.append(requestBuf);
    commandSocket->write(block);
    commandSocket->flush();
    logPanel->appendPlainText(requestBuf);
    qDebug().noquote()<<"Request: "<<requestBuf;
}

QPair<int,QString> MainWindow::parseCommandResponse(QString responseBuf){
    QTextStream responseBufStream(&responseBuf);
    int responseCode;
    QString responseMessage;
    responseBufStream>>responseCode>>responseMessage;
    return qMakePair(responseCode,responseMessage);
}

void MainWindow::initDataConn(){
    if(dataConnMode==DataConnMode::PORT){
        dataServer = new QTcpServer(this);
        if (!dataServer->listen()) {
            QMessageBox::critical(this, tr("Data Conn Server"),
                                  tr("Unable to start the server: %1.")
                                  .arg(dataServer->errorString()));
            close();
            return;
        }

        connect(dataServer,&QTcpServer::newConnection,this,&MainWindow::handlePortDataConn);
        changeServerStatus(ServerStatus::PORT_START);
    }
}

void MainWindow::handlePortDataConn(){
    dataSocket=dataServer->nextPendingConnection();
    qDebug().noquote()<<"New data connection "<<dataSocket<<endl;
    connect(dataSocket, &QAbstractSocket::disconnected,this,&MainWindow::handleDataConnDisconnect);

    if(serverStatus==ServerStatus::STOR_START){
        QByteArray requestBuf=currentFile.readAll();
        QTreeWidgetItem *item=taskList->topLevelItem(taskList->topLevelItemCount()-1);
        QProgressBar* progressBar=qobject_cast<QProgressBar*>(taskList->itemWidget(item,1));
        progressBar->setValue(progressBar->value()+requestBuf.size());
        qDebug()<<"Read current file: "<<currentFile.fileName()<<" "<<requestBuf.size()<<" bytes";
        dataSocket->write(requestBuf);
        dataSocket->flush();
        dataSocket->disconnectFromHost();
        return;
    }

    connect(dataSocket,&QTcpSocket::readyRead,this,&MainWindow::handleDataConnResponse);

}

void MainWindow::handleDataConnDisconnect(){
    qDebug()<<"Data Conn disconnect"<<endl;
    dataSocket->close();
    dataSocket->deleteLater();
    dataSocket=nullptr;

    dataServer->close();
    dataServer->deleteLater();
    dataServer=nullptr;

    dataConnStatus=DataConnStatus::DISCONNECT;

    if(serverStatus==ServerStatus::LIST_START){
        changeServerStatus(ServerStatus::LIST_END);
        return;
    }
    if(serverStatus==ServerStatus::RETR_START){
        qDebug()<<"Close current file: "<<currentFile.fileName();
        currentFile.close();
        changeLocalDir(".");
        changeServerStatus(ServerStatus::RETR_END);
        return ;
    }
    if(serverStatus==ServerStatus::STOR_START){
        qDebug()<<"Close current file: "<<currentFile.fileName();
        currentFile.close();
        changeServerStatus(ServerStatus::STOR_END);
        return ;
    }
}

void MainWindow::handleDataConnResponse(){
    qDebug().noquote()<<"Current Status: "<<stringifyServerStatus(serverStatus);
    if(serverStatus==ServerStatus::RETR_START){
        QByteArray responseBuf=dataSocket->readAll();
        currentFile.write(responseBuf);
        QTreeWidgetItem *item=taskList->topLevelItem(taskList->topLevelItemCount()-1);
        QProgressBar* progressBar=qobject_cast<QProgressBar*>(taskList->itemWidget(item,1));
        progressBar->setValue(progressBar->value()+responseBuf.size());
        qDebug()<<"Write current file: "<<currentFile.fileName()<<" "<<responseBuf.size()<<" bytes";
        return;
    }
    if(resumeServerStatus==ServerStatus::LIST_START){
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

        remoteFileList->clear();

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
        return;
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

QString MainWindow::stringifyServerStatus(ServerStatus status){
    if(status==ServerStatus::COMMAND_CONNECT_START){
        return "COMMAND_CONNECT_START";
    }
    if(status==ServerStatus::COMMAND_CONNECT_END){
        return "COMMAND_CONNECT_END";
    }
    if(status==ServerStatus::IDLE){
        return "IDLE";
    }
    if(status==ServerStatus::USER_START){
        return "USER_START";
    }
    if(status==ServerStatus::USER_END){
        return "USER_END";
    }
    if(status==ServerStatus::PASS_START){
        return "PASS_START";
    }
    if(status==ServerStatus::PASS_END){
        return "PASS_END";
    }
    if(status==ServerStatus::SYST_START){
        return "SYST_START";
    }
    if(status==ServerStatus::SYST_END){
        return "SYST_END";
    }
    if(status==ServerStatus::TYPE_START){
        return "TYPE_START";
    }
    if(status==ServerStatus::TYPE_END){
        return "TYPE_END";
    }
    if(status==ServerStatus::PORT_START){
        return "PORT_START";
    }
    if(status==ServerStatus::PORT_END){
        return "PORT_END";
    }
    if(status==ServerStatus::PWD_START){
        return "PWD_START";
    }
    if(status==ServerStatus::PWD_END){
        return "PWD_END";
    }
    if(status==ServerStatus::LIST_START){
        return "LIST_START";
    }
    if(status==ServerStatus::LIST_END){
        return "LIST_END";
    }
    if(status==ServerStatus::RETR_START){
        return "RETR_START";
    }
    if(status==ServerStatus::RETR_END){
        return "RETR_END";
    }
    return "";
}
