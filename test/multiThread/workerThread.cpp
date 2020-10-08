#ifdef ZLOG_QT_VERSION

#include "workerThread.h"
#include "string.h"
#include "stdio.h"

#include "zlog.h"

using namespace zlog;

WorkerThread::WorkerThread()
{
    memset(m_Name,0,MAX_NAME_LEN);
    m_Count = 0;
}

void WorkerThread::setName(const char *name)
{
    strcpy(m_Name,name);
}

void WorkerThread::setCount(int count)
{
    m_Count = count;
}

void WorkerThread::run()
{
    _loggerId = IZLogManager::getRef().createLogger(m_Name);
    IZLogManager::getRef().setLogFilePath(_loggerId,"./mt_log/");
    IZLogManager::getRef().start();
    LOGFMT_ALARM(_loggerId, "NAME = %s", m_Name);
    LOG_WARN(_loggerId,"workerThread is running");
    LOGFMT_ALARM(_loggerId, "COUNT = %d", m_Count);
    printf("[PRINTF] workerThread is running\n");
}

void WorkerThread::exit(int retcode)
{
    LOG_WARN(_loggerId,"workerThread exit");
    printf("[PRINTF] workerThread exit\n");

    QThread::exit(retcode);
}

#endif
