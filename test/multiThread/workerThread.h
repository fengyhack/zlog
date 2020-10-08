#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

#ifdef ZLOG_QT_VERSION

#include <QThread>

#define MAX_NAME_LEN 256

class WorkerThread : public QThread
{
public:
    WorkerThread();

    void setName(const char* name);
    void setCount(int count);

#ifdef ZLOG_QT_VERSION
    void run() override;
#endif

    void exit(int retcode = 0);

private:
    char m_Name[MAX_NAME_LEN];
    int m_Count;
	int _loggerId;
};

#endif

#endif // WORKERTHREAD_H
