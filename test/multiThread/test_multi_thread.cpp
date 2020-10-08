#ifdef ZLOG_QT_VERSION
#	include <QCoreApplication>
#   include <QThread>
#endif
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <iostream>

#ifdef ZLOG_QT_VERSION
#	include "zlog.h"
#else
#	include "../../src/zlog.h"
#endif

#ifdef WIN32
#	ifdef WIN64
#		ifdef _DEBUG
#			pragma comment(lib,"../lib/zlog_win64d.lib")
#		else
#			pragma comment(lib,"../lib/zlog_win64.lib")
#		endif
#	else
#		ifdef _DEBUG
#			pragma comment(lib,"../lib/zlog_win32d.lib")
#		else
#			pragma comment(lib,"../lib/zlog_win32.lib")
#		endif
#	endif
#endif

using namespace zlog;

#ifdef ZLOG_QT_VERSION
#	include "workerThread.h"
#elif defined WIN32
#	include <windows.h>
#endif

#ifndef ZLOG_QT_VERSION
#	ifdef WIN32

typedef struct _THREAD_PARAM
{
	char name[256];
	int count;
}THREAD_PARAM;

DWORD WINAPI subThreadFunc(LPVOID pParam);

#	endif
#endif

int main(int argc, char *argv[])
{
#ifdef ZLOG_QT_VERSION
    WorkerThread t;
    t.setName("SubThread");
    t.setCount(200);
    t.start();
#else
	THREAD_PARAM p = { "SubThread",200 };
	HANDLE h = CreateThread(NULL, 0, subThreadFunc, &p, 0, NULL);
	WaitForSingleObject(h, 3000);
#endif

    IZLogManager::getRef().setLogFilePath(ZLOG_MAIN_LOGGER_ID,"./zlog/");
    IZLogManager::getRef().setIsDisplayOnConsole(ZLOG_MAIN_LOGGER_ID,false);
    //IZLogManager::getRef().start();

    LOGI("main " << "LOGI " << "111");
    LOGI("main " << "LOGI " << "222");

#ifdef WIN32
    printf("[PRINTF] Press any key to continue...\r\n");
    getchar();
#endif // WIN32

#ifdef ZLOG_QT_VERSION
    t.exit();
#endif

    LOGI("main " << "LOGI " << "333");
    IZLogManager::getRef().stop();

    return 0;
}

#ifdef WIN32

DWORD WINAPI subThreadFunc(LPVOID pParam)
{
	THREAD_PARAM* p = (THREAD_PARAM*)pParam;
	int _loggerId = IZLogManager::getRef().createLogger(p->name);
	IZLogManager::getRef().setLogFilePath(_loggerId, "./mt_log/");
	IZLogManager::getRef().start();
	LOGFMT_ALARM(_loggerId, "NAME = %s", p->name);
	LOG_WARN(_loggerId, "workerThread is running");
	LOGFMT_ALARM(_loggerId, "COUNT = %d", p->count);
	printf("[PRINTF] workerThread is running\n");

	return 0;
}

#endif
