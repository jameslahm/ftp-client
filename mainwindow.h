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

    enum class DataConnMode {
        PASV,
        PORT
    };

    enum class DataConnStatus {
        IDLE,
        PREPARE,
        CONNECT
    };

    enum class RequestType {
        USER,
        PASS,
        SYST,
        TYPE,
        PORT,
        PASV
    };

    QMap<RequestType,QString> RequestTypeMap{{RequestType::USER,"USER"},{RequestType::PASS,"PASS"},{RequestType::TYPE,"TYPE"},{RequestType::SYST,"SYST"}
                                             ,{RequestType::PORT,"PORT"},{RequestType::PASV,"PASV"}};

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

    DataConnMode dataConnMode=DataConnMode::PORT;

    DataConnStatus dataConnStatus=DataConnStatus::PREPARE;
    void centerAndResize();
    void connectToServer();
    void handleCommandResponse();
    void sendCommandRequest(RequestType requestType,QString args);
    void handleCommandError(QAbstractSocket::SocketError socketError);
    void changeDataConnMode(DataConnMode mode);
    void initDataConn();
    QPair<int,QString> parseCommandResponse(QString commandBuf);
    void handlePortDataConn();

};

#endif // MAINWINDOW_H
