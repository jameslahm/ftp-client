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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    enum class ServerStatus {
        // command socket start connect
        COMMAND_CONNECT_START,
        // command socket end
        COMMAND_CONNECT_END,
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
        PWD_START,
        PWD_END,
        LIST_START,
        LIST_END,
        IDLE
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
        LIST
    };

    QMap<RequestType,QString> RequestTypeMap{{RequestType::USER,"USER"},{RequestType::PASS,"PASS"},{RequestType::TYPE,"TYPE"},{RequestType::SYST,"SYST"}
                                             ,{RequestType::PORT,"PORT"},{RequestType::PASV,"PASV"},{RequestType::LIST,"LIST"}};

    QLineEdit *serverAdressInput;
    QLineEdit *serverPortInput;
    QLineEdit *userNameInput;
    QLineEdit *passwordInput;

    QPushButton *connectButton;
    QPushButton *quitButton;

    QTreeWidget *localFileList;
    QTreeWidget *remoteFileList;

    QTreeWidget *taskList;
    QLabel *statusLabel;

    QPlainTextEdit *logPanel;

    QTcpSocket* commandSocket=nullptr;

    QTcpSocket* dataSocket=nullptr;
    QTcpServer* dataServer=nullptr;

    QString localDirPath=nullptr;
    QString remoteDirPath=nullptr;

    QHash<QString, bool> isLocalDirectory;
    QHash<QString,bool> isRemoteDirectory;

    DataConnMode dataConnMode=DataConnMode::PORT;

    DataConnStatus dataConnStatus=DataConnStatus::DISCONNECT;

    ServerStatus serverStatus=ServerStatus::COMMAND_CONNECT_START;

    void centerAndResize();
    void connectToServer();
    void handleCommandResponse();
    void handleDataConnResponse();
    void sendCommandRequest(RequestType requestType,QString args);
    void handleCommandError(QAbstractSocket::SocketError socketError);
    void changeDataConnMode(DataConnMode mode);
    void changeServerStatus(ServerStatus status);
    void initDataConn();
    QPair<int,QString> parseCommandResponse(QString commandBuf);
    void handlePortDataConn();

};

#endif // MAINWINDOW_H
