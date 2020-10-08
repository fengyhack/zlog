#ifdef ZLOG_QT_VERSION
#	include <QCoreApplication>
#endif
#include <stdio.h>
#include <signal.h>
#include <math.h>
#include <time.h>
#include <stdint.h>
#if WIN32
#include <Windows.h>
#include <process.h>
#else
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#endif

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

LoggerId logid_foo;
LoggerId logid_bar;

bool g_quit;
void signalFunc(int id)
{
    g_quit = false;
#ifdef WIN32
    signal(id, &signalFunc);
#endif
}
int main(int argc, char *argv[])
{
    g_quit = true;
    signal(SIGINT, &signalFunc);

    IZLogManager::getRef().configFromFile("config.cfg");
    logid_foo = IZLogManager::getRef().findLoggerByKey("foo");
    logid_bar = IZLogManager::getRef().createLogger("bar");
    //start zlog
    IZLogManager::getRef().start();

    //configure the output behaviour
    IZLogManager::getRef().setIsDisplayOnConsole(ZLOG_MAIN_LOGGER_ID, true);
    IZLogManager::getRef().setIsDisplayOnConsole(logid_bar, true);
    IZLogManager::getRef().setFilterLogLevel(ZLOG_MAIN_LOGGER_ID, LOG_LEVEL_DEBUG);
    IZLogManager::getRef().setFilterLogLevel(logid_bar, LOG_LEVEL_DEBUG);
    IZLogManager::getRef().setIsUsingMonthDir(logid_bar, true);
    IZLogManager::getRef().setIsUsingFileLine(ZLOG_MAIN_LOGGER_ID, false);
    IZLogManager::getRef().setLoggerName(ZLOG_MAIN_LOGGER_ID, "main");
    IZLogManager::getRef().setLogFilePath(ZLOG_MAIN_LOGGER_ID, "./zlog");
    IZLogManager::getRef().setLogFileSizeLimit(logid_bar, 1);
    IZLogManager::getRef().setIsWriteToFile(ZLOG_MAIN_LOGGER_ID, true);


    IZLogManager::getRef().setFilterLogLevel(ZLOG_MAIN_LOGGER_ID, LOG_LEVEL_TRACE);
    LOGT("set LOG_LEVEL_TRACE ----------------------------");
    IZLogManager::getRef().setFilterLogLevel(ZLOG_MAIN_LOGGER_ID, LOG_LEVEL_DEBUG);
    LOGT("set LOG_LEVEL_TRACE ============================");

    char buf[100];
    for (size_t j = 0; j < 100000; ++j)
    {
        ZLogStream ls(buf, 100);

        for (size_t i = 0; i < 100; ++i)
        {
            int r = rand() % 14;
            switch (r)
            {
            case 0:
                ls << 'c';
                break;
            case 1:
                ls << UINT8_MAX;
                break;
            case 2:
                ls << INT16_MIN;
                break;
            case 3:
                ls << INT16_MAX;
                break;
            case 4:
                ls << UINT16_MAX;
                break;
            case 5:
                ls << INT32_MIN;
                break;
            case 6:
                ls << INT32_MAX;
                break;
            case 7:
                ls << UINT32_MAX;
                break;
            case 8:
                ls << INT64_MIN;
                break;
            case 9:
                ls << INT64_MAX;
                break;
            case 10:
                ls << UINT64_MAX;
                break;
            case 11:
                ls << (float)pow(rand()%100, rand()%20) *((rand()%2 == 0? -1.0 : 1.0));
                break;
            case 12:
                ls << (double)pow(rand() % 100, rand() % 200)*((rand() % 2 == 0 ? -1.0 : 1.0));
                break;
            default:
                ls << "8";
                break;
            }
            if (ls.getCurrentLen() > 100)
            {
                LOGE("overwrite ls.getCurrentLen()=" << ls.getCurrentLen());
                return 1;
            }
        }

    }

    //hot update configure
    IZLogManager::getRef().setAutoUpdateInterval(10);
    time_t now = time(NULL);
    while(g_quit &&  time(NULL) - now < 10)
    {
        LOG_TRACE(logid_foo, "foo LOG_TRACE");
        LOG_DEBUG(logid_foo, "foo LOG_DEBUG");
        LOG_INFO(logid_foo, "foo LOG_INFO");
        LOG_WARN(logid_foo, "foo LOG_WARN");
        LOG_ERROR(logid_foo, "foo LOG_ERROR");
        LOG_ALARM(logid_foo, "foo LOG_ALARM");
        LOG_FATAL(logid_foo, "foo LOG_FATAL");

        LOG_TRACE(logid_bar, "bar LOG_TRACE");
        LOG_DEBUG(logid_bar, "bar LOG_DEBUG");
        LOG_INFO(logid_bar, "bar LOG_INFO");
        LOG_WARN(logid_bar, "bar LOG_WARN");
        LOG_ERROR(logid_bar, "bar LOG_ERROR");
        LOG_ALARM(logid_bar, "bar LOG_ALARM");
        LOG_FATAL(logid_bar, "bar LOG_FATAL");

        LOGT("main LOGT");
        LOGD("main LOGD");
        LOGI("main LOGI");
        LOGW("main LOGW");
        LOGE("main LOGE");
        LOGA("main LOGA");
        LOGF("main LOGF");
        LOGF(" = = = = = = = = = = = = = = =  = = = = = ");
#ifdef WIN32
        ::Sleep(rand()%3000);
#else
        usleep((rand()%3000)*1000);
#endif
    }


    LOGI("main quit .. hit 'enter' to exit.");
    getchar();
    return 0;
}
