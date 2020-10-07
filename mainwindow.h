#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QApplication>
#include <QScreen>
#include <QStyle>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPlainTextEdit>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QHeaderView>
#include <QPushButton>
#include <QTcpSocket>
#include <QTcpServer>
#include <QPair>
#include <QAbstractSocket>
#include <QGroupBox>
#include <QRadioButton>
#include <QNetworkInterface>
#include <QMessageBox>
#include <QAction>
#include <QMenu>
#include <QIcon>
#include <QProgressBar>
#include <QDir>
#include <QFileInfo>
#include <QDateTime>
#include <QFileDialog>
#include <QInputDialog>
#include <QScrollBar>
#include <QThread>
#include <QVector>

#include "readworker.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    enum class ServerStatus {
        COMMAND_DISCONNECT,
        // command socket start connect
        COMMAND_CONNECT_START,
        // command socket end
        COMMAND_CONNECT_END,
        IDLE,
        USER_START,
        USER_END,
        PASS_START,
        PASS_END,
        SYST_START,
        SYST_END,
        TYPE_START,
        TYPE_END,
        PORT_START,
        PORT_END,
        LIST_START,
        LIST_END,
        RETR_START,
        RETR_END,
        PWD_START,
        PWD_END,
        CWD_START,
        CWD_END,
        STOR_START,
        STOR_END,
        PASV_START,
        PASV_END,
        MKD_START,
        MKD_END,
        RMD_START,
        RMD_END,
        DELE_START,
        DELE_END,
        RNFR_START,
        RNFR_END,
        RNTO_START,
        RNTO_END,
        QUIT_START,
        QUIT_END
    };


    enum class DataConnMode {
        PASV,
        PORT
    };

    enum class DataConnStatus {
        DISCONNECT,
        CONNECT
    };

    enum class RequestType {
        USER,
        PASS,
        SYST,
        TYPE,
        PORT,
        PASV,
        LIST,
        RETR,
        PWD,
        CWD,
        STOR,
        MKD,
        RMD,
        DELE,
        RNFR,
        RNTO,
        QUIT
    };

    QMap<RequestType,QString> RequestTypeMap{{RequestType::USER,"USER"},{RequestType::PASS,"PASS"},{RequestType::TYPE,"TYPE"},{RequestType::SYST,"SYST"}
                                             ,{RequestType::PORT,"PORT"},{RequestType::PASV,"PASV"},{RequestType::LIST,"LIST"},{RequestType::RETR,"RETR"},
                                             {RequestType::PWD,"PWD"},{RequestType::CWD,"CWD"},{RequestType::STOR,"STOR"},
                                             {RequestType::MKD,"MKD"},{RequestType::RMD,"RMD"},{RequestType::DELE,"DELE"},
                                             {RequestType::RNFR,"RNFR"},{RequestType::RNTO,"RNTO"},{RequestType::QUIT,"QUIT"}
                                             };

    QLineEdit *serverAdressInput;
    QLineEdit *serverPortInput;
    QLineEdit *userNameInput;
    QLineEdit *passwordInput;

    QPushButton *connectButton;
    QPushButton *quitButton;

    QPushButton *changeLocalDirButton;
    QPushButton *returnToParentButton;

    QTreeWidget *localFileList;
    QTreeWidget *remoteFileList;

    QTreeWidget *taskList;
    QLabel *statusLabel;

    QPlainTextEdit *logPanel;

    QTcpSocket* commandSocket=nullptr;

    QTcpSocket* dataSocket=nullptr;
    QTcpServer* dataServer=nullptr;

    QDir localDir;
    QLabel* localDirPathLabel;
    QLabel* remoteDirPathLabel;
    QString remoteDirPath=nullptr;
    QString remoteRootDirPath=nullptr;

    QFile currentFile;
    QString currentFileName=nullptr;

    QHash<QString, bool> isLocalDirectory;
    QHash<QString,bool> isRemoteDirectory;

    DataConnMode dataConnMode=DataConnMode::PORT;

    DataConnStatus dataConnStatus=DataConnStatus::DISCONNECT;

    ServerStatus serverStatus=ServerStatus::COMMAND_DISCONNECT;
    ServerStatus resumeServerStatus = ServerStatus::COMMAND_DISCONNECT;

    bool isEmptyDataConnResponse=true;

    int cmdResponseStatus=-1;

    void centerAndResize();
    void connectToServer();
    void handleCommandResponse();
    void handleDataConnResponse();
    void handleDataConnRequest();
    void sendCommandRequest(RequestType requestType,QString args);
    void handleCommandError(QAbstractSocket::SocketError socketError);
    void changeDataConnMode(DataConnMode mode);
    void changeServerStatus(ServerStatus status);
    void initDataConn();
    void handleContextMenu(QPoint pos,QString fileListType);
    QPair<int,QString> parseCommandResponse(QString commandBuf);
    void handlePortDataConn();
    void handleDataConnDisconnect();
    QString stringifyServerStatus(ServerStatus status);
    void changeLocalDir(QString path);
    void changeRemoteDir(QString path);
    void addTask(QString taskName,int maximum);
    void addLogToPanel(QString requestBuf);
    void addLogToPanel(int responseCode,QString tmpBuf);
    void handleDataConnEmptyResponse();
};

#endif // MAINWINDOW_H
