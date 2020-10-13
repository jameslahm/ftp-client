#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    qSetMessagePattern("[%{type} %{time}] %{appname} (%{file}:%{line}) - %{message}");

    QLabel *serverAdressLabel = new QLabel(tr("IP/Host"));
    //    serverAdressInput =new QLineEdit("139.196.81.14");
    serverAdressInput =new QLineEdit("127.0.0.1");
    QLabel *serverPortLabel = new QLabel(tr("Port"));
    //    serverPortInput = new QLineEdit("21");
    serverPortInput = new QLineEdit("12345");

    QLabel *userNameLabel=new QLabel(tr("Username"));
    //    userNameInput = new QLineEdit("vsftp");
    userNameInput = new QLineEdit("anonymous");
    QLabel *passwordLabel = new QLabel(tr("Password"));
    passwordInput = new QLineEdit("vsftp123");
    passwordInput->setEchoMode(QLineEdit::Password);

    QGroupBox *groupRadioBox = new QGroupBox(tr("Data Connection Mode"));

    QRadioButton *pasvRadio = new QRadioButton(tr("PASV"));
    QRadioButton *portRadio = new QRadioButton(tr("PORT"));
    portRadio->setChecked(true);

    connect(pasvRadio,&QRadioButton::clicked,[=](){this->changeDataConnMode(DataConnMode::PASV);});
    connect(portRadio,&QRadioButton::clicked,[=](){this->changeDataConnMode(DataConnMode::PORT);});


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

    changeLocalDirButton=new QPushButton();
    changeLocalDirButton->setIcon(QIcon(":/images/open_dir.png"));

    localFileList = new QTreeWidget;
    localFileList->setRootIsDecorated(false);
    localFileList->setHeaderLabels(QStringList()<<tr("Name")<<tr("Size")<<tr("Owner")<<tr("Group")<<tr("Time"));
    //    localFileList->header()->setStretchLastSection(false);
    localFileList->header()->setSectionResizeMode(QHeaderView::Interactive);

    QLabel *remoteFileListLabel=new QLabel(tr("Remote Files"));
    remoteDirPathLabel=new QLabel("");
    returnToParentButton=new QPushButton();
    returnToParentButton->setIcon(QIcon(":/images/return.png"));

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

    statusLabel = new QLabel("Please enter the server address of an FTP　");

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

    connect(connectButton,&QPushButton::clicked,[=](){this->changeServerStatus(ServerStatus::COMMAND_CONNECT_START);});
    connect(quitButton,&QPushButton::clicked,[=](){
        this->changeServerStatus(ServerStatus::QUIT_START);
    });

    connect(localFileList,&QTreeWidget::itemDoubleClicked,[=](QTreeWidgetItem* item,int){if(item){
            this->changeLocalDir(item->text(0));
        }});
    connect(remoteFileList,&QTreeWidget::itemDoubleClicked,[=](QTreeWidgetItem* item,int){if(item && isRemoteDirectory[item->text(0)]){
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
//    changeLocalDirButton->setEnabled(false);

    changeLocalDir(QDir::currentPath());
}

void MainWindow::changeLocalDir(QString path){
    localFileList->clear();
    isLocalDirectory.clear();

    localDir.cd(path);
    if(path=="."){
        localDir.refresh();
    }

    QString logString=QString("Set local dir %1").arg(localDir.absolutePath());
    qInfo().noquote()<<logString;
    //    statusLabel->setText(logString);
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
        qInfo().noquote()<<destPathList;
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
    changeServerStatus(ServerStatus::CWD_START);
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
    QString logString=QString("Set remote dir %1").arg(remoteDirPath);
    qInfo().noquote()<<logString;
    //    statusLabel->setText(logString);
}


void MainWindow::handleContextMenu(QPoint pos,QString fileListType){
    if(fileListType=="REMOTE"){
        QTreeWidgetItem* item=remoteFileList->itemAt(pos);

        if(item!=nullptr){
            QMenu menu(this);

            if(isRemoteDirectory[item->text(0)]){
                QAction *rmdAction =new QAction(tr("Delete Dir"));
                rmdAction->setIcon(QIcon(":/images/delete.png"));
                connect(rmdAction,&QAction::triggered,[=](){
                    int ret = QMessageBox::warning(this, tr("Confirm"),
                                                   tr("Are you sure to delete the dir?"),
                                                   QMessageBox::Yes
                                                   | QMessageBox::No,
                                                   QMessageBox::No);
                    if(ret==QMessageBox::Yes){
                        this->changeServerStatus(ServerStatus::RMD_START);
                        this->sendCommandRequest(RequestType::RMD,item->text(0)+"\r\n");
                    }});
                menu.addAction(rmdAction);
            }

            if(!isRemoteDirectory[item->text(0)]){
                QAction *rmfAction =new QAction(tr("Delete file"));
                rmfAction->setIcon(QIcon(":/images/delete.png"));

                connect(rmfAction,&QAction::triggered,[=](){
                    int ret = QMessageBox::warning(this, tr("Confirm"),
                                                   tr("Are you sure to delete the file?"),
                                                   QMessageBox::Yes
                                                   | QMessageBox::No,
                                                   QMessageBox::No);
                    if(ret==QMessageBox::Yes){
                        this->changeServerStatus(ServerStatus::DELE_START);
                        this->sendCommandRequest(RequestType::DELE,item->text(0)+"\r\n");
                    }});

                menu.addAction(rmfAction);

                QAction *downloadAction=new QAction(tr("Download"));
                downloadAction->setIcon(QIcon(":/images/download.png"));
                connect(downloadAction,&QAction::triggered,[=](){
                    this->currentFileName=item->text(0);
                    this->addTask(this->currentFileName,item->text(1).toInt());
                    this->changeServerStatus(ServerStatus::RETR_START);
                });
                menu.addAction(downloadAction);
            }

            QAction *rnAction=new QAction(tr("Rename"));
            rnAction->setIcon(QIcon(":/images/rename.png"));
            connect(rnAction,&QAction::triggered,[=](){
                this->changeServerStatus(ServerStatus::RNFR_START);
                this->sendCommandRequest(RequestType::RNFR,item->text(0)+"\r\n");
            });
            menu.addAction(rnAction);

            menu.exec(remoteFileList->mapToGlobal(pos));
        }
        else{
            QAction *mkdAction=new QAction(tr("New Dir"));
            mkdAction->setIcon(QIcon(":/images/new_dir.png"));
            QMenu menu(this);
            connect(mkdAction,&QAction::triggered,[=](){
                bool ok;
                QInputDialog *dialog=new QInputDialog;
                dialog->setInputMode(QInputDialog::TextInput);
                dialog->setLabelText("Dir Name");
                dialog->setWindowTitle("Please input the new dir name");
                dialog->resize(400,200);
                ok=dialog->exec();
                QString text=dialog->textValue();
                qInfo().noquote()<<"Dialog result: "<<ok<<" "<<text;
                if(ok && !text.isEmpty()){
                    this->changeServerStatus(ServerStatus::MKD_START);
                    this->sendCommandRequest(RequestType::MKD,text+"\r\n");
                }
            });
            if(serverStatus==ServerStatus::COMMAND_DISCONNECT){
                mkdAction->setEnabled(false);
            }
            menu.addAction(mkdAction);

            menu.exec(remoteFileList->mapToGlobal(pos));
        }

    }
    if(fileListType=="LOCAL"){
        QTreeWidgetItem* item=localFileList->itemAt(pos);

        if(item!=nullptr){

            QMenu menu(this);
            if(!isLocalDirectory[item->text(0)]){
                QAction *uploadAction=new QAction(tr("Upload"));
                uploadAction->setIcon(QIcon(":/images/upload.png"));
                connect(uploadAction,&QAction::triggered,[=](){
                    this->currentFileName=item->text(0);
                    this->addTask(this->currentFileName,item->text(1).toInt());
                    this->changeServerStatus(ServerStatus::STOR_START);
                });
                if(serverStatus==ServerStatus::COMMAND_DISCONNECT){
                    uploadAction->setEnabled(false);
                }
                menu.addAction(uploadAction);
            }
            menu.exec(localFileList->mapToGlobal(pos));
        }
    }
}

void MainWindow::changeDataConnMode(DataConnMode mode){
    this->dataConnMode = mode;
    QString logString;
    if(mode==DataConnMode::PASV){
        logString=QString("Change mode to PASV");
    }
    else{
        logString=QString("Change mode to PORT");;
    }
    qInfo().noquote()<<logString;
    statusLabel->setText(logString);
}

void MainWindow::handleSocketError(QAbstractSocket::SocketError socketError,QTcpSocket* socket){
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        qCritical().noquote()<<"Host not found";
        QMessageBox::critical(this,tr("Error"),tr("Host not found"),QMessageBox::Ok,QMessageBox::Ok);
        if(socket==commandSocket){
            changeServerStatus(ServerStatus::QUIT_END);
        }
        else{
            clearDataConn();
            changeServerStatus(ServerStatus::IDLE);
        }
        break;
    case QAbstractSocket::ConnectionRefusedError:
        qCritical().noquote()<<"Connection refused";
        QMessageBox::critical(this,tr("Error"),tr("Connection refused"),QMessageBox::Ok,QMessageBox::Ok);
        if(socket==commandSocket){
            changeServerStatus(ServerStatus::QUIT_END);
        }
        else{
            clearDataConn();
            changeServerStatus(ServerStatus::IDLE);
        }
        break;
    default:
        qCritical().noquote()<<"Some error happend";
        QMessageBox::critical(this,tr("Error"),tr("Connection error"),QMessageBox::Ok,QMessageBox::Ok);
        if(socket==commandSocket){
            changeServerStatus(ServerStatus::QUIT_END);
        }
        else{
            clearDataConn();
            changeServerStatus(ServerStatus::IDLE);
        }
        return;
    }
}

void MainWindow::handleCommandResponse(){
    cmdResponseStatus=-1;
    qInfo().noquote()<<"Start parse response: ";
    bool isResponseEnd=false;
    QString responseBuf;
    int responseCode=-1;
    while(isResponseEnd==false){
        QString tmpBuf(commandSocket->readLine());
        if(tmpBuf.isEmpty()){
            break;
        }

        qInfo().noquote()<<"Response: "<<tmpBuf;
        if(tmpBuf.indexOf("-")==-1){
            isResponseEnd=true;
        }
        QString tmpBufToParse=tmpBuf;
        tmpBufToParse.replace("-"," ");
        QPair<int,QString> parseRes=parseCommandResponse(tmpBufToParse);
        responseCode=parseRes.first;

        addLogToPanel(responseCode,tmpBuf);

        responseBuf+=parseRes.second;
    }
    qInfo().noquote()<<"Parse response successfully";

    if(responseCode==150){
        if(commandSocket->bytesAvailable()!=0){
            handleCommandResponse();
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

    if(responseCode==215){
        if(serverStatus==ServerStatus::SYST_START){
            changeServerStatus(ServerStatus::SYST_END);
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
    if(responseCode==221){
        if(serverStatus==ServerStatus::QUIT_START){
            changeServerStatus(ServerStatus::QUIT_END);
            return;
        }
    }

    if(responseCode==226){
        if(serverStatus==ServerStatus::LIST_START){
            cmdResponseStatus=226;
            if(dataSocket==nullptr){
                changeServerStatus(ServerStatus::LIST_END);
            }
            return;
        }
        if(serverStatus==ServerStatus::RETR_START){
            cmdResponseStatus=226;
            if(dataSocket==nullptr){
                changeServerStatus(ServerStatus::RETR_END);
            }
            return;
        }
        if(serverStatus==ServerStatus::STOR_START){
            cmdResponseStatus=226;
            if(dataSocket==nullptr){changeServerStatus(ServerStatus::STOR_END);}
            return;
        }
        if(serverStatus==ServerStatus::APPE_START){
            cmdResponseStatus=226;
            if(dataSocket==nullptr){changeServerStatus(ServerStatus::APPE_END);}
            return;
        }
    }

    if(responseCode==227){
        if(serverStatus==ServerStatus::PASV_START){
            int leftBracketIndex=responseBuf.indexOf("(");
            int rightBracketIndex=responseBuf.indexOf(")");
            responseBuf=responseBuf.mid(leftBracketIndex+1,rightBracketIndex-leftBracketIndex-1);
            QString ip_1;
            QString ip_2;
            QString ip_3;
            QString ip_4;
            QString port_1;
            QString port_2;
            responseBuf.replace(","," ");
            QTextStream responseBufStream(&responseBuf);
            responseBufStream>>ip_1>>ip_2>>ip_3>>ip_4>>port_1>>port_2;
            QString ip=ip_1+"."+ip_2+"."+ip_3+"."+ip_4;
            int port=port_1.toInt()*256+port_2.toInt();
            QString logString=QString("Connect to: %1:%2").arg(serverAdressInput->text()).arg(port);
            qInfo().noquote()<<logString;
            statusLabel->setText(logString);
            // TODO: check ip
            dataSocket->connectToHost(serverAdressInput->text(),(unsigned short)port);
            changeServerStatus(ServerStatus::PASV_END);
            return;
        }

    }

    if(responseCode==230){
        if(serverStatus==ServerStatus::PASS_START){
            changeServerStatus(ServerStatus::PASS_END);
            return;
        }
    }

    if(responseCode==250){
        if(serverStatus==ServerStatus::CWD_START){
            changeServerStatus(ServerStatus::CWD_END);
            return;
        }
        if(serverStatus==ServerStatus::RMD_START){
            changeServerStatus(ServerStatus::RMD_END);
            return;
        }
        if(serverStatus==ServerStatus::DELE_START){
            changeServerStatus(ServerStatus::DELE_END);
            return;
        }
        if(serverStatus==ServerStatus::RNTO_START){
            changeServerStatus(ServerStatus::RNTO_END);
            return;
        }
    }

    if(responseCode==257){
        if(serverStatus==ServerStatus::PWD_START){
            QTextStream tmpStream(&responseBuf);
            tmpStream>>remoteDirPath;
            remoteDirPath=remoteDirPath.section('"',1,1);
            qInfo().noquote()<<"Remote dir path: "<<remoteDirPath;
            if(remoteRootDirPath==nullptr){
                remoteRootDirPath=remoteDirPath;
            }
            remoteDirPathLabel->setText(remoteDirPath);
            changeServerStatus(ServerStatus::PWD_END);
            return;
        }
        if(serverStatus==ServerStatus::MKD_START){
            changeServerStatus(ServerStatus::MKD_END);
            return;
        }
    }

    if(responseCode==331){
        if(serverStatus==ServerStatus::USER_START){
            changeServerStatus(ServerStatus::USER_END);
            return;
        }
    }

    if(responseCode==350){
        if(serverStatus==ServerStatus::RNFR_START){
            changeServerStatus(ServerStatus::RNFR_END);
            return;
        }
        if(serverStatus==ServerStatus::REST_START){
            changeServerStatus(ServerStatus::REST_END);
            return;
        }
    }

    if(responseCode==421){
        if(serverStatus==ServerStatus::IDLE){
            changeServerStatus(ServerStatus::QUIT_END);
            return;
        }
    }

    if(responseCode==530){
        if(serverStatus==ServerStatus::USER_START){
            QMessageBox::critical(this,tr("Error"),tr("Username not acceptable"),QMessageBox::Ok,QMessageBox::Ok);
            changeServerStatus(ServerStatus::IDLE);
        }
        if(serverStatus==ServerStatus::PASS_START){
            QMessageBox::critical(this,tr("Error"),tr("Password incorrect"),QMessageBox::Ok,QMessageBox::Ok);
            changeServerStatus(ServerStatus::IDLE);
        }
    }

    if(responseCode==550){
        if(serverStatus==ServerStatus::RMD_START){
            QMessageBox::critical(this,tr("Error"),tr("Dir is not empty"),QMessageBox::Ok,QMessageBox::Ok);
            changeServerStatus(ServerStatus::IDLE);
            return;
        }
        if(serverStatus==ServerStatus::MKD_START){
            QMessageBox::critical(this,tr("Error"),tr("Create failure"),QMessageBox::Ok,QMessageBox::Ok);
            changeServerStatus(ServerStatus::IDLE);
            return;
        }
        if(serverStatus==ServerStatus::DELE_START){
            QMessageBox::critical(this,tr("Error"),tr("Delete failure"),QMessageBox::Ok,QMessageBox::Ok);
            changeServerStatus(ServerStatus::IDLE);
            return;
        }
        if(serverStatus==ServerStatus::RETR_START){
            QMessageBox::critical(this,tr("Error"),tr("Download failure"),QMessageBox::Ok,QMessageBox::Ok);
            clearDataConn();
            changeServerStatus(ServerStatus::IDLE);
            return;
        }
    }
    if(responseCode==553){
        if(serverStatus==ServerStatus::STOR_START){
            QMessageBox::critical(this,tr("Error"),tr("Upload failure"),QMessageBox::Ok,QMessageBox::Ok);
            clearDataConn();
            changeServerStatus(ServerStatus::IDLE);
            return;
        }
        if(serverStatus==ServerStatus::APPE_START){
            QMessageBox::critical(this,tr("Error"),tr("Upload failure"),QMessageBox::Ok,QMessageBox::Ok);
            clearDataConn();
            changeServerStatus(ServerStatus::IDLE);
            return;
        }
    }

    // Unknown error
    if(responseCode>=500){
        QMessageBox::critical(this,tr("Error"),tr("Some error happend"),QMessageBox::Ok,QMessageBox::Ok);
        clearDataConn();
        changeServerStatus(ServerStatus::IDLE);
        return;
    }

}


void MainWindow::checkBreakPoint(RequestType requestType){
    qInfo().noquote()<<"Start check break point"<<currentFileName;
    QString a="123";
    QString b="123";
    if(a==b){
        qInfo().noquote()<<"Test Success"<<endl;
    }
    if(requestType==RequestType::RETR){
        for(int i=0;i<localFileList->topLevelItemCount();i++){
            QTreeWidgetItem *item=localFileList->topLevelItem(i);
            qInfo().noquote()<<item->text(0);
            if(item->text(0)==currentFileName){
                currentFileBreakPoint=item->text(1).toInt();
                qInfo().noquote()<<"Current breakpoint"<<currentFileBreakPoint;
                return;
            };
        }
    }
    if(requestType==RequestType::STOR){
        for(int i=0;i<remoteFileList->topLevelItemCount();i++){
            QTreeWidgetItem *item=remoteFileList->topLevelItem(i);
            if(item->text(0)==currentFileName){
                currentFileBreakPoint=item->text(1).toInt();
                qInfo().noquote()<<"Current breakpoint"<<currentFileBreakPoint;
                return;
            };
        }
    }
    qInfo().noquote()<<"No break point";
}


void MainWindow::closeEvent( QCloseEvent * event )
{
    int res=QMessageBox::information( this, tr("Exit Tip"), tr("Do you really want to exit?"), QMessageBox::Yes | QMessageBox::No,QMessageBox::No);
    if(res==QMessageBox::Yes){
        event->accept();
    }
    else{
        event->ignore();
    }

}

void MainWindow::changeServerStatus(ServerStatus status){
    qInfo().noquote()<<"Change serverStatus from "<<stringifyServerStatus(serverStatus)<<" to"<<stringifyServerStatus(status)<<endl;
    serverStatus=status;
    returnToParentButton->setEnabled(false);
//    changeLocalDirButton->setEnabled(false);

    if(status==ServerStatus::IDLE){
        returnToParentButton->setEnabled(true);
//        changeLocalDirButton->setEnabled(true);
        return;
    }
    if(status==ServerStatus::COMMAND_DISCONNECT){
        connectButton->setEnabled(true);
        return;
    }
    if(status==ServerStatus::COMMAND_CONNECT_START){
        connectButton->setEnabled(false);
        commandSocket=new QTcpSocket(this);

        connect(commandSocket,&QIODevice::readyRead,[=](){this->handleCommandResponse();});

        connect(commandSocket,QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),this,[=](QAbstractSocket::SocketError socketError){
            this->handleSocketError(socketError,commandSocket);
        });
        connect(commandSocket,&QTcpSocket::disconnected,[=](){
            QMessageBox::information(this,tr("Info"),tr("Connection has been disconnected"),QMessageBox::Ok,QMessageBox::Ok);
            this->changeServerStatus(ServerStatus::QUIT_END);
        });
        commandSocket->connectToHost(serverAdressInput->text(),serverPortInput->text().toUShort());
        return;
    }
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
        sendCommandRequest(RequestType::SYST,"\r\n");
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
        else{
            changeServerStatus(ServerStatus::IDLE);
        }
        return;
    }
    if(status==ServerStatus::PORT_START){
        QString ipAddress;
        int port=dataServer->serverPort();
        ipAddress=commandSocket->localAddress().toString();
        sendCommandRequest(RequestType::PORT,QString("%1,%2,%3\r\n").arg(ipAddress).arg(port/256).arg(port%256).replace(".",","));
        statusLabel->setText(tr("The server is running on IP: %1 port: %2")
                             .arg(ipAddress).arg(dataServer->serverPort()));
        return;
    }
    if(status==ServerStatus::PORT_END){
        dataConnStatus=DataConnStatus::CONNECT;
        qInfo().noquote()<<"Resume status: "<<stringifyServerStatus(resumeServerStatus);
        changeServerStatus(resumeServerStatus);
        return;
    }
    if(status==ServerStatus::PASV_START){
        sendCommandRequest(RequestType::PASV,"\r\n");
        return;
    }
    if(status==ServerStatus::PASV_END){
        dataConnStatus=DataConnStatus::CONNECT;
        qInfo().noquote()<<"Resume status: "<<stringifyServerStatus(resumeServerStatus);


        connect(dataSocket,QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),this,[=](QAbstractSocket::SocketError socketError){
            this->handleSocketError(socketError,dataSocket);
        });
        connect(dataSocket, &QAbstractSocket::disconnected,this,&MainWindow::handleDataConnDisconnect);
        connect(dataSocket,&QTcpSocket::readyRead,this,&MainWindow::handleDataConnResponse);

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
            return;
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
    if(status==ServerStatus::REST_START){
        sendCommandRequest(RequestType::REST,QString("%1\r\n").arg(currentFileBreakPoint));
        return;
    }
    if(status==ServerStatus::REST_END){
        currentProgressBar->setValue(currentProgressBar->value()+currentFileBreakPoint);
        changeServerStatus(ServerStatus::RETR_START);
        return;
    }
    if(status==ServerStatus::RETR_START){
        if(dataConnStatus==DataConnStatus::DISCONNECT){
            resumeServerStatus=ServerStatus::RETR_START;
            initDataConn();
            return;
        }
        else{
            if(currentFileBreakPoint==-1){
                checkBreakPoint(RequestType::RETR);
                if(currentFileBreakPoint!=-1){
                    int res=QMessageBox::information(this,tr("Resume Transmission"),tr("Detect there exists the same file in local. Do you want to resume transmission?"),
                                                     QMessageBox::Yes|QMessageBox::No,QMessageBox::No);
                    if(res==QMessageBox::Yes){
                        changeServerStatus(ServerStatus::REST_START);
                        return;
                    }
                    else{
                        currentFileBreakPoint=-1;
                    }
                }
            }
            currentFile.setFileName(localDir.filePath(currentFileName));
            QIODevice::OpenMode mode;
            if(currentFileBreakPoint==-1){
                mode=QIODevice::WriteOnly | QIODevice::Truncate;
            }else{
                mode=QIODevice::WriteOnly|QIODevice::Append;
            }
            if(!currentFile.open(mode)){

                qCritical().noquote()<<"Can't open file";
            }
            sendCommandRequest(RequestType::RETR,currentFileName+"\r\n");
            return;
        }
    }
    if(status==ServerStatus::RETR_END){
        changeLocalDir(".");
        changeServerStatus(ServerStatus::IDLE);
        return;
    }
    if(status==ServerStatus::APPE_START){
        if(dataConnStatus==DataConnStatus::DISCONNECT){
            resumeServerStatus=ServerStatus::APPE_START;
            initDataConn();
            return;
        }
        else{
            currentProgressBar->setValue(currentProgressBar->value()+currentFileBreakPoint);
            currentFile.setFileName(localDir.filePath(currentFileName));
            if(!currentFile.open(QIODevice::ReadOnly)){

                qCritical().noquote()<<"Can't open file";
            }

            sendCommandRequest(RequestType::APPE,currentFileName+"\r\n");

            handleDataConnRequest();

            return;
        }
    }
    if(status==ServerStatus::APPE_END){
        changeServerStatus(ServerStatus::LIST_START);
        return;
    }
    if(status==ServerStatus::STOR_START){
        if(dataConnStatus==DataConnStatus::DISCONNECT){
            resumeServerStatus=ServerStatus::STOR_START;
            initDataConn();
            return;
        }
        else{
            if(currentFileBreakPoint==-1){
                checkBreakPoint(RequestType::STOR);
                if(currentFileBreakPoint!=-1){
                    int res=QMessageBox::information(this,tr("Resume Transmission"),tr("Detect there exists the same file in server. Do you want to resume transmission?"),
                                                     QMessageBox::Yes|QMessageBox::No,QMessageBox::No);
                    if(res==QMessageBox::Yes){
                        changeServerStatus(ServerStatus::APPE_START);
                        return;
                    }
                    else{
                        currentFileBreakPoint=-1;
                    }
                }
            }

            currentFile.setFileName(localDir.filePath(currentFileName));
            if(!currentFile.open(QIODevice::ReadOnly)){

                qCritical().noquote()<<"Can't open file";
            }

            sendCommandRequest(RequestType::STOR,currentFileName+"\r\n");

            handleDataConnRequest();

            return;
        }
    }
    if(status==ServerStatus::STOR_END){
        changeServerStatus(ServerStatus::LIST_START);
        return;
    }
    if(status==ServerStatus::MKD_START){
        return;
    }
    if(status==ServerStatus::MKD_END){
        changeServerStatus(ServerStatus::LIST_START);
        return;
    }
    if(status==ServerStatus::RMD_START){
        return;
    }
    if(status==ServerStatus::RMD_END){
        changeServerStatus(ServerStatus::LIST_START);
        return;
    }
    if(status==ServerStatus::DELE_START){
        return;
    }
    if(status==ServerStatus::DELE_END){
        changeServerStatus(ServerStatus::LIST_START);
        return;
    }
    if(status==ServerStatus::RNFR_START){
        return;
    }
    if(status==ServerStatus::RNFR_END){
        changeServerStatus(ServerStatus::RNTO_START);
        return;
    }
    if(status==ServerStatus::RNTO_START){
        bool ok;
        QInputDialog *dialog=new QInputDialog;
        dialog->setInputMode(QInputDialog::TextInput);
        dialog->setLabelText("File Name");
        dialog->setWindowTitle("Please input the file name");
        dialog->resize(400,200);
        ok=dialog->exec();
        QString text=dialog->textValue();
        qInfo().noquote()<<"Dialog result: "<<ok<<" "<<text;
        if(ok && !text.isEmpty()){
            this->sendCommandRequest(RequestType::RNTO,text+"\r\n");
        }
        return;
    }
    if(status==ServerStatus::RNTO_END){
        changeServerStatus(ServerStatus::LIST_START);
        return;
    }
    if(status==ServerStatus::QUIT_START){
        sendCommandRequest(RequestType::QUIT,"\r\n");
        return;
    }
    if(status==ServerStatus::QUIT_END){
        remoteDirPath=nullptr;
        commandSocket->close();
        commandSocket->deleteLater();
        commandSocket=nullptr;

        clearDataConn();

        remoteFileList->clear();
        isRemoteDirectory.clear();
        changeServerStatus(ServerStatus::COMMAND_DISCONNECT);
        return;
    }
}

void MainWindow::addTask(QString taskName, int maximum){
    QTreeWidgetItem *item=new QTreeWidgetItem;
    item->setText(0,taskName);
    taskList->addTopLevelItem(item);
    QProgressBar *progressBar=new QProgressBar();
    progressBar->setMinimum(0);
    progressBar->setMaximum(maximum+1);
    progressBar->setValue(1);
    progressBar->setMaximumWidth(200);
    progressBar->setAlignment(Qt::AlignVCenter | Qt::AlignCenter);
    currentProgressBar=progressBar;
    taskList->setItemWidget(item,1,progressBar);
}

void MainWindow::sendCommandRequest(RequestType requestType, QString requestArgs){
    QByteArray block;
    QString requestBuf = QString("%1 %2").arg(RequestTypeMap[requestType]).arg(requestArgs);
    block.append(requestBuf);
    commandSocket->write(block);
    commandSocket->flush();
    addLogToPanel(requestBuf);
    qInfo().noquote()<<"Request: "<<requestBuf;
}

QPair<int,QString> MainWindow::parseCommandResponse(QString responseBuf){
    int responseCode;
    QString responseMessage;
    int spaceIndex=responseBuf.indexOf(" ");
    responseCode=responseBuf.mid(0,spaceIndex).toInt();
    responseMessage=responseBuf.mid(spaceIndex+1);
    return qMakePair(responseCode,responseMessage);
}

void MainWindow::initDataConn(){
    qInfo().noquote()<<"Init data conn...";
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
    if(dataConnMode==DataConnMode::PASV){
        dataSocket=new QTcpSocket();
        changeServerStatus(ServerStatus::PASV_START);
    }
}

void MainWindow::handlePortDataConn(){
    dataSocket=dataServer->nextPendingConnection();
    qInfo().noquote()<<"New data connection "<<dataSocket;
    connect(dataSocket, &QAbstractSocket::disconnected,this,&MainWindow::handleDataConnDisconnect);
    connect(dataSocket,&QTcpSocket::readyRead,this,&MainWindow::handleDataConnResponse);

    if(serverStatus==ServerStatus::STOR_START || serverStatus==ServerStatus::APPE_START){
        handleDataConnRequest();
    }
}

void MainWindow::clearDataConn(){
    if(dataSocket!=nullptr){
        dataSocket->close();
        dataSocket->deleteLater();
        dataSocket=nullptr;
    }
    if(dataServer!=nullptr){
        dataServer->close();
        dataServer->deleteLater();
        dataServer=nullptr;
    }
}

void MainWindow::handleDataConnDisconnect(){
    QString logString="Data Conn disconnect";
    qInfo().noquote()<<logString;
    //    statusLabel->setText(logString);

    qInfo().noquote()<<"Empty response: "<<isEmptyDataConnResponse;
    if(isEmptyDataConnResponse){
        handleDataConnEmptyResponse();
    }
    isEmptyDataConnResponse=true;

    clearDataConn();

    dataConnStatus=DataConnStatus::DISCONNECT;

    if(serverStatus==ServerStatus::LIST_START){
        if(cmdResponseStatus==226)changeServerStatus(ServerStatus::LIST_END);
        return;
    }
    if(serverStatus==ServerStatus::RETR_START){
        qInfo().noquote()<<"Close current file: "<<currentFile.fileName();
        currentFile.close();
        //        statusLabel->setText(logString);
        currentFileBreakPoint=-1;
        if(cmdResponseStatus==226){
            changeServerStatus(ServerStatus::RETR_END);
        }
        return ;
    }
    if(serverStatus==ServerStatus::STOR_START){
        qInfo().noquote()<<"Close current file: "<<currentFile.fileName();
        currentFile.close();
        currentFileBreakPoint=-1;
        if(cmdResponseStatus==226)changeServerStatus(ServerStatus::STOR_END);
        return ;
    }
    if(serverStatus==ServerStatus::APPE_START){
        qInfo().noquote()<<"Close current file: "<<currentFile.fileName();
        currentFile.close();
        currentFileBreakPoint=-1;
        if(cmdResponseStatus==226)changeServerStatus(ServerStatus::APPE_END);
        return ;
    }
}

void MainWindow::handleDataConnRequest(){
    if(dataConnMode==DataConnMode::PASV){
        if(dataSocket->state()==QTcpSocket::ConnectingState){
            qInfo().noquote()<<"Still connecting...";
            connect(dataSocket,&QTcpSocket::connected,this,&MainWindow::handleDataConnRequest);
            return;
        }
    }
    if(dataSocket==nullptr){
        return;
    }

    qInfo().noquote()<<"Current Status: "<<stringifyServerStatus(serverStatus);
    isEmptyDataConnResponse=false;
    if(serverStatus==ServerStatus::STOR_START || serverStatus==ServerStatus::APPE_START){
        ReadWorker* readWorker=new ReadWorker;
        readWorker->currentFile=&this->currentFile;
        readWorker->offset=currentFileBreakPoint;

        connect(readWorker,&ReadWorker::resultReady,this,[=](QByteArray requestBuf){
            qInfo().noquote()<<"Current Thread: "<<QThread::currentThread();
            currentProgressBar->setValue(currentProgressBar->value()+requestBuf.size());
            QString logString=QString("Upload current file: %1 %2 bytes").arg(this->currentFile.fileName()).arg(requestBuf.size());
            qInfo().noquote()<<logString;
            //            statusLabel->setText(logString);
            qInfo().noquote()<<this->dataSocket;
            this->dataSocket->write(requestBuf);
            this->dataSocket->flush();
        });

        connect(readWorker,&ReadWorker::finished,this,[=](){
            QString logString=QString("Upload current file %1 successfully").arg(this->currentFile.fileName());
            qInfo().noquote()<<logString;
            currentProgressBar->setValue(currentProgressBar->value());
            //            statusLabel->setText(logString);
            this->dataSocket->disconnectFromHost();
        });

        qInfo().noquote()<<"Start New Thread";
        readWorker->start();

        return;
    }
}

void MainWindow::handleDataConnEmptyResponse(){
    qInfo().noquote()<<"Current Status: "<<stringifyServerStatus(serverStatus);
    if(serverStatus==ServerStatus::LIST_START){
        isRemoteDirectory.clear();
        remoteFileList->clear();
    }
}

void MainWindow::handleDataConnResponse(){
    qInfo().noquote()<<"Current Status: "<<stringifyServerStatus(serverStatus);
    isEmptyDataConnResponse=false;
    if(serverStatus==ServerStatus::RETR_START){
        QByteArray responseBuf=dataSocket->readAll();
        currentFile.write(responseBuf);
        currentProgressBar->setValue(currentProgressBar->value()+responseBuf.size());
        QString logString=QString("Download current file %1 %2 bytes").arg(currentFile.fileName()).arg(responseBuf.size());
        qInfo().noquote()<<logString;
        //        statusLabel->setText(logString);
        return;
    }
    if(serverStatus==ServerStatus::LIST_START){
        QString responseBuf(dataSocket->readAll());
        qInfo().noquote()<<"";
        qInfo().noquote()<<responseBuf;
        QString permission;
        QString owner;
        QString group;
        QString size;
        QString name;
        QString time;
        QString tmp;
        QTextStream responseBufStream(&responseBuf);

        isRemoteDirectory.clear();
        remoteFileList->clear();

        while(!responseBufStream.atEnd()){
            QString lineBuf=responseBufStream.readLine();
            QTextStream lineBufStream(&lineBuf);

            lineBufStream>>permission>>tmp>>owner>>group>>size;

            while(!lineBufStream.atEnd()){
                QString tmpBuf;
                lineBufStream>>tmpBuf;
                if(lineBufStream.atEnd()){
                    name=tmpBuf;
                }
                else{
                    time+=" ";
                    time+=tmpBuf;
                }
            }

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
    qInfo().noquote() << "Available dimensions " << width << "x" << height;
    width *= 0.8; // 60% of the screen size
    height *= 0.8; // 60% of the screen size
    qInfo().noquote() << "Computed dimensions " << width << "x" << height;
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

    if(status==ServerStatus::PWD_START){
        return "PWD_START";
    }
    if(status==ServerStatus::PWD_END){
        return "PWD_END";
    }
    if(status==ServerStatus::CWD_START){
        return "CWD_START";
    }
    if(status==ServerStatus::CWD_END){
        return "CWD_END";
    }
    if(status==ServerStatus::STOR_START){
        return "STOR_START";
    }
    if(status==ServerStatus::STOR_END){
        return "STOR_END";
    }
    if(status==ServerStatus::PASV_START){
        return "PASV_START";
    }
    if(status==ServerStatus::PASV_END){
        return "PASV_END";
    }
    if(status==ServerStatus::MKD_START){
        return "MKD_START";
    }
    if(status==ServerStatus::MKD_END){
        return "MKD_END";
    }
    if(status==ServerStatus::RMD_START){
        return "MKD_START";
    }
    if(status==ServerStatus::RMD_END){
        return "MKD_END";
    }
    if(status==ServerStatus::DELE_START){
        return "DELE_START";
    }
    if(status==ServerStatus::DELE_END){
        return "DELE_END";
    }
    if(status==ServerStatus::RNFR_START){
        return "RNFR_START";
    }
    if(status==ServerStatus::RNFR_END){
        return "RNTO_START";
    }
    if(status==ServerStatus::RNTO_START){
        return "RNTO_START";
    }
    if(status==ServerStatus::RNTO_END){
        return "RNTO_END";
    }
    if(status==ServerStatus::COMMAND_DISCONNECT){
        return "COMMAND_DISCONNECT";
    }
    if(status==ServerStatus::QUIT_START){
        return "QUIT_START";
    }
    if(status==ServerStatus::QUIT_END){
        return "QUIT_END";
    }
    if(status==ServerStatus::REST_START){
        return "REST_START";
    }
    if(status==ServerStatus::REST_END){
        return "REST_END";
    }
    if(status==ServerStatus::APPE_START){
        return "APPE_START";
    }
    if(status==ServerStatus::APPE_END){
        return "APPE_END";
    }
    return "UNKNOWN_STATUS";
}

void MainWindow::addLogToPanel(QString requestBuf){
    QString infoLog="<font color='orange'>%1</font>";
    QString logHtml=infoLog.arg(requestBuf);
    logPanel->appendHtml(logHtml);
    logPanel->verticalScrollBar()->setValue(logPanel->verticalScrollBar()->maximum());
    return;
}

void MainWindow::addLogToPanel(int responseCode,QString tmpBuf){
    QString successLog="<font color='green'>%1</font>";
    QString errorLog="<font color='red'>%1</font>";
    QString infoLog="<font color='blue'>%1</font>";

    QString logHtml;
    if(responseCode>=200 && responseCode<300){
        logHtml=successLog.arg(tmpBuf);
    }
    if(responseCode>=400){
        logHtml=errorLog.arg(tmpBuf);
    }
    if(responseCode>=100 && responseCode<200){
        logHtml=successLog.arg(tmpBuf);
    }
    if(responseCode>=300&& responseCode<400){
        logHtml=infoLog.arg(tmpBuf);
    }

    logPanel->appendHtml(logHtml);
    logPanel->verticalScrollBar()->setValue(logPanel->verticalScrollBar()->maximum());
}
