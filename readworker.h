#ifndef READWORKER_H
#define READWORKER_H

#include <QThread>
#include <QFile>
#include <QDebug>

class ReadWorker:public QThread
{
    Q_OBJECT
public: 
    QFile *currentFile;

    ReadWorker();
    void run(){
        qDebug()<<"Current Thread: "<<currentThread()<<endl;
        if(!currentFile->isReadable()){
            return;
        }
        else{
            while(!currentFile->atEnd()){
                QByteArray resultBuf=currentFile->read(BUFSIZ*20);
                emit resultReady(resultBuf);
            }
            emit finished();
        }
    }
signals:
    void resultReady(QByteArray resultBuf);
    void finished();

};

#endif // READWORKER_H
