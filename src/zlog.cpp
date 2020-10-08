/*
**********************************************************************************
*
* zlog is licensed under the terms of the MIT license reproduced below.
*
* ===============================================================================
*
* Copyright (C) 2017-2020 (#vmx https://amote.xxx/, last update: 2017/12/11)
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*
* ===============================================================================
*
* CONTRIBUTOR(S)
*   #vmx https://amote.xxx/
*   and thanks to the original author 'zsummer@github'
*
* CHANGELOG
*
* VERSION 0.1.0 <DATE: 2017.11.30>
*   migrate the project (with lots of modification) from [log4z]
*
* VERSION 0.2.0 <DATE: 2017.12.02>
*   fixed some miscellaneous problems
*
* VERSION 0.3.0 <DATE: 2017.12.04>
*   modified the examples: test-simple, test-advanced, test-stress
*
* VERSION 0.4.0 <DATE: 2017.12.05>
*   add support for WIN32/WIN64 DLL export
*
* VERSION 0.5.0 <DATE: 2017.12.06>
*   add support for Ubuntu SO (shared-library) export
*
* VERSION 0.6.0 <DATE: 2017.12.07>
*   add multi-thread example code for this project
*
* VERSION 0.7.0 <DATE: 2017.12.10>
*   passed tests for Qt5.8 with MSVS2015 and Qt5.9.3 on Ubuntu1710
*
* VERSION 0.8.0 <DATE: 2017.12.11>
*   add a zlog-lib project for Qt5.8 with MSVS2015
*   fixed a problem of DLL_EXPORT definition macro in Qt5.8 with MSVS2015
*
* VERSION 0.9.0 <DATE: 2017.12.12>
*   misc fixture of 'newline' (CRLF on Windows, LF on *nix)
*
* VERSION 0.10.0 <DATE: 2017.12.13>
*   add support for Qt-MinGW (Windows, 32bit)
*   fixed some misc problems like: '>' brackets, GNU_CXX's isnan/isinf
*
* VERSION 0.11.0 <DATE: 2018.01.05>
*   add support for LOGFMT
*   please see the remarks below
*
* VERSION 0.12.0 <DATE: 2018.01.09>
*   add a lookup table (ASCII 0~127) for ZLogBinary
*
* VERSION 0.13.0 <DATE: 2018.01.19>
*   change some default settings, especially, 'is_write_to_console'
*
* VERSION 0.14.0 <DATE: 2018.01.20>
*   add a 'global-switch' for zlog
*
* VERSION 0.15.0 <DATE: 2018.01.24>
*   add implementation for 'sleepMicrosecond' on Windows
*
* REMARKS
*   1. The macro '__cplusplus' was defined as 199711L in VC2013, VC2015.
*      However, VC2013+ supports C++11 (201103L)
*   2. Macro '__VAR_ARGS__' (variable arguments) was added since C99
*      This if used as 'func(fmt,...)', like 'printf("%d,%s",a,b)'
*   3. In Qt on Windows, we are not going to use 'Windows.h' , because
*      the macros 'min' and 'max' were defined in the header file 'minwindef.h'
*      however these macros were also defined in 'qtrandom.h'
*      these re-definition will cause a build-time error
*      And the header files include chain on Windows is shown below:
*      'minwindef.h' -INCUDED-BY-> 'windef.h' -INCUDED-BY-> 'Windows.h'
*   4. If not necessary, turn off 'is_write_to_console'
*
**********************************************************************************
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <algorithm>
#include <iostream>

#ifdef ZLOG_QT_VERSION
#   include <QFileInfo>
#endif

// !!! Remarks about 'shlwapi.lib' and 'user32.lib' !!!
// When using MSVS:
// If this library can not be found, please check to make sure it is installed correctly
// Or you may try this below if installed:
// "C:/Program Files (x86)/Windows Kits/8.1/Lib/winv6.3/um/x86/shlwapi.lib"

#ifdef WIN32
#	include <io.h>
#	include <shlwapi.h>
#	include <process.h>
#   ifdef _MSC_VER
#       pragma comment(lib, "shlwapi.lib")
#       pragma comment(lib, "user32.lib")
#       pragma warning(disable:4996)
#   endif
#else
#	include <unistd.h>
#	include <netinet/in.h>
#	include <arpa/inet.h>
#	include <sys/types.h>
#	include <sys/socket.h>
#	include<pthread.h>
#	include <sys/time.h>
#	include <sys/stat.h>
#	include <dirent.h>
#	include <fcntl.h>
#	include <semaphore.h>
#	include <sys/syscall.h>
#endif

#ifdef __APPLE__
#	include "TargetConditionals.h"
#	include <dispatch/dispatch.h>
#	if !TARGET_OS_IPHONE
#		define ZLOG_HAVE_LIBPROC
#		include <libproc.h>
#	endif
#endif

#include "zlog.h"

namespace zlog
{
enum ENUM_CONFIG_TYPE
{
    SET_GENERAL,
    SET_IS_LOGGER_ENABLED,
    SET_LOGGER_NAME,
    SET_LOG_FILE_PATH,
    SET_FILTER_LOG_LEVEL,
    SET_IS_DISPLAY_ON_CONSOLE,
    SET_IS_WRITE_TO_FILE,
    SET_LOG_FILE_SIZE_LIMIT,
    SET_IS_USING_MONTH_DIR,
    SET_IS_USING_FILE_LINE,
    SET_LOG_FILE_RESERVE_TIME
};

const int IVAL_DONT_CARE = 0;
const bool BVAL_DONT_CRAE = false;
const std::string SVAL_DONT_CARE = "";

static const char* const LOG_STRING[LOG_LEVEL_MAX__ + 1] =
{
    "TRACE",
    "DEBUG",
    "INFO ",
    "WARN ",
    "ERROR",
    "ALARM",
    "FATAL",
};

static const size_t LOG_STRING_LEN[LOG_LEVEL_MAX__ + 1] =
{
    sizeof("TRACE") - 1,
    sizeof("DEBUG") - 1,
    sizeof("INFO ") - 1,
    sizeof("WARN ") - 1,
    sizeof("ERROR") - 1,
    sizeof("ALARM") - 1,
    sizeof("FATAL") - 1,
};

static const char* const LOG_LEVEL_STRING[LOG_LEVEL_MAX__ + 1] =
{
    "LOG_LEVEL_TRACE",
    "LOG_LEVEL_DEBUG",
    "LOG_LEVEL_INFO ",
    "LOG_LEVEL_WARN ",
    "LOG_LEVEL_ERROR",
    "LOG_LEVEL_ALARM",
    "LOG_LEVEL_FATAL"
};

#ifdef WIN32
const static WORD LOG_COLOR[LOG_LEVEL_MAX__ + 1] = {
    0,                                  // TRACE
    0,                                  // DEBUG
    FOREGROUND_GREEN,                   // INFO
    FOREGROUND_GREEN | FOREGROUND_RED,  // WARN
    FOREGROUND_RED,                     // ERROR
    FOREGROUND_BLUE | FOREGROUND_GREEN, // ALARM
    FOREGROUND_RED | FOREGROUND_BLUE    // FATAL
};
#else
const static char LOG_COLOR[LOG_LEVEL_MAX__ + 1][32] = {
    "\e[0m",       // TRACE
    "\e[0m",       // DEBUG
    "\e[32m",      // INFO green
    "\e[33m",      // WARN yellow
    "\e[31m",      // ERROR red
    "\e[34m\e[1m", // ALARM hight blue
    "\e[35m"       // FATAL
};
#endif

// will be used later
static inline void sleepMicrosecond(unsigned int us);
static inline void sleepMillisecond(unsigned int ms);

class ZLogFileHandler
{
public:
    ZLogFileHandler()
    {
        _file = NULL;
    }

    ~ZLogFileHandler()
    {
        close();
    }

    inline bool isOpen()
    {
        return _file != NULL;
    }

    inline long open(const char* path, const char* mod)
    {
        if (_file != NULL)
        {
            fclose(_file);
            _file = NULL;
        }

        _file = fopen(path, mod);
        if (_file)
        {
            long tel = 0;
            long cur = ftell(_file);
            fseek(_file, 0L, SEEK_END);
            tel = ftell(_file);
            fseek(_file, cur, SEEK_SET);
            return tel;
        }

        return -1;
    }

    inline void clean(int index, int len)
    {
#if !defined(__APPLE__) && !defined(WIN32)
        if (_file != NULL)
        {
            int fd = fileno(_file);
            fsync(fd);
            posix_fadvise(fd, index, len, POSIX_FADV_DONTNEED);
            fsync(fd);
        }
#else
		(void)index;
		(void)len;
#endif
    }

    inline void close()
    {
        if (_file != NULL)
        {
            clean(0, 0);
            fclose(_file);
            _file = NULL;
        }
    }

    inline void write(const char* data, size_t len)
    {
        if (_file && len > 0)
        {
            if (fwrite(data, 1, len, _file) != len)
            {
                close();
            }
        }
    }

    inline void flush()
    {
        if (_file)
        {
            fflush(_file);
        }
    }

    inline std::string readLine()
    {
        char buf[500] = { 0 };
        if (_file && fgets(buf, 500, _file) != NULL)
        {
            return std::string(buf);
        }

        return std::string();
    }

    inline const std::string readContent()
    {
        std::string content;

        if (!_file)
        {
            return content;
        }

        // in 'stdio.h', BUFSIZ = 512
        char buf[BUFSIZ];
        size_t ret = 0;
        do
        {
            ret = fread(buf, sizeof(char), BUFSIZ, _file);
            content.append(buf, ret);
        } while (ret == BUFSIZ);

        return content;
    }

    inline bool removeFile(const std::string& path)
    {
        return ::remove(path.c_str()) == 0;
    }

public:
    FILE* _file;
};

class LockHelper
{
public:
    LockHelper()
    {
#ifdef WIN32
        InitializeCriticalSection(&_crit);
#else
        //_crit = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        //pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_TIMED_NP);
        //pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(&_crit, &attr);
        pthread_mutexattr_destroy(&attr);
#endif
    }

    virtual ~LockHelper()
    {
#ifdef WIN32
        DeleteCriticalSection(&_crit);
#else
        pthread_mutex_destroy(&_crit);
#endif
    }

public:
#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning(disable:26135)
#endif
    void lock()
    {
#ifdef WIN32
        EnterCriticalSection(&_crit);
#else
        pthread_mutex_lock(&_crit);
#endif
    }

    void unLock()
    {
#ifdef WIN32
        LeaveCriticalSection(&_crit);
#else
        pthread_mutex_unlock(&_crit);
#endif
    }
#ifdef _MSC_VER
#	pragma warning(pop)
#endif

private:
#ifdef WIN32
    CRITICAL_SECTION _crit;
#else
    pthread_mutex_t  _crit;
#endif
};

class AutoLock
{
public:
    explicit AutoLock(LockHelper& lk)
        :_lock(lk)
    {
        _lock.lock();
    }

    ~AutoLock()
    {
        _lock.unLock();
    }

private:
    LockHelper & _lock;
};

class SemaphoreHelper
{
public:
    SemaphoreHelper()
    {
#ifdef WIN32
        _hSem = NULL;
#elif defined(__APPLE__)
        _semId = NULL;
#else
        _isCreate = false;
#endif
    }

    virtual ~SemaphoreHelper()
    {
#ifdef WIN32
        if (_hSem != NULL)
        {
            CloseHandle(_hSem);
            _hSem = NULL;
        }
#elif defined(__APPLE__)
        if (_semId)
        {
            dispatch_release(_semId);
            _semId = NULL;
        }
#else
        if (_isCreate)
        {
            _isCreate = false;
            sem_destroy(&_semId);
        }
#endif
    }

public:
    bool create(int initCount)
    {
        if (initCount < 0)
        {
            initCount = 0;
        }
#ifdef WIN32
        if (initCount > 64)
        {
            return false;
        }
        _hSem = CreateSemaphore(NULL, initCount, 64, NULL);
        if (_hSem == NULL)
        {
            return false;
        }
#elif defined(__APPLE__)
        _semId = dispatch_semaphore_create(initCount);
        if (!_semId)
        {
            return false;
        }
#else
        if (sem_init(&_semId, 0, initCount) != 0)
        {
            return false;
        }
        _isCreate = true;
#endif
        return true;
    }

    bool wait(int timeout = 0)
    {
#ifdef WIN32
        if (timeout <= 0)
        {
            timeout = INFINITE;
        }
        if (WaitForSingleObject(_hSem, timeout) != WAIT_OBJECT_0)
        {
            return false;
        }
#elif defined(__APPLE__)
        if (dispatch_semaphore_wait(_semId, dispatch_creationTime(DISPATCH_TIME_NOW, timeout * 1000)) != 0)
        {
            return false;
        }
#else
        if (timeout <= 0)
        {
            return (sem_wait(&_semId) == 0);
        }
        else
        {
            struct timeval tm;
            gettimeofday(&tm, NULL);
            long long endtime = tm.tv_sec * 1000 + tm.tv_usec / 1000 + timeout;
            do
            {
                sleepMillisecond(50);
                int ret = sem_trywait(&_semId);
                if (ret == 0)
                {
                    return true;
                }
                struct timeval tv_cur;
                gettimeofday(&tv_cur, NULL);
                if (tv_cur.tv_sec * 1000 + tv_cur.tv_usec / 1000 > endtime)
                {
                    return false;
                }

                if (ret == -1 && errno == EAGAIN)
                {
                    continue;
                }
                else
                {
                    return false;
                }
            } while (true);
            return false;
        }
#endif
        return true;
    }

    bool post()
    {
#ifdef WIN32
        return ReleaseSemaphore(_hSem, 1, NULL) ? true : false;
#elif defined(__APPLE__)
        return dispatch_semaphore_signal(_semId) == 0;
#else
        return (sem_post(&_semId) == 0);
#endif
    }

private:
#ifdef WIN32
    HANDLE _hSem;
#elif defined(__APPLE__)
    dispatch_semaphore_t _semId;
#else
    sem_t _semId;
    bool  _isCreate;
#endif
};

// This 'threadProc' below will be used by 'ThreadHelper'
#ifdef WIN32
static unsigned int WINAPI threadProc(LPVOID lpParam);
#else
static void* threadProc(void* pParam);
#endif

class ThreadHelper
{
public:
    ThreadHelper()
    {
        _hThreadID = 0;
    }

    virtual ~ThreadHelper() {}

public:
    bool start()
    {
#ifdef WIN32
        unsigned long long ret = _beginthreadex(NULL, 0, threadProc, (void*)this, 0, NULL);

        if (ret == -1 || ret == 0)
        {
            printf("zlog: create zlog thread error! %s", STR_NEW_LINE);
            return false;
        }
        _hThreadID = ret;
#else
        int ret = pthread_create(&_phtreadID, NULL, threadProc, (void*)this);
        if (ret != 0)
        {
            printf("zlog: create zlog thread error! %s", STR_NEW_LINE);
            return false;
        }
#endif
        return true;
    }

    bool wait()
    {
#ifdef WIN32
        if (WaitForSingleObject((HANDLE)_hThreadID, INFINITE) != WAIT_OBJECT_0)
        {
            return false;
        }
#else
        if (pthread_join(_phtreadID, NULL) != 0)
        {
            return false;
        }
#endif
        return true;
    }

    virtual void run() = 0;
private:
    unsigned long long _hThreadID;
#ifndef WIN32
    pthread_t _phtreadID;
#endif
};

// Implementation of 'threadProc'
#ifdef WIN32
unsigned int WINAPI threadProc(LPVOID lpParam)
{
    ThreadHelper* p = (ThreadHelper*)lpParam;
    p->run();
    return 0;
}
#else
void* threadProc(void* pParam)
{
    ThreadHelper* p = (ThreadHelper*)pParam;
    p->run();
    return NULL;
}
#endif

struct LoggerInfo
{
public:
    LoggerInfo()
    {
        _isLoggerEnabled = false;
        _logFilePath = ZLOG_LOG_FILE_PATH_DEFAULT;
        _filterLogLevel = ZLOG_FILTER_LOG_LEVEL_DEFAULT;
        _isDisplayOnConsole = ZLOG_IS_DISPLAY_ON_CONSOLE_DEFAULT;
        _isWriteToFile = ZLOG_IS_WRITE_TO_FILE_DEFAULT;

        _isUsingMonthDir = ZLOG_IS_USING_MONTH_DIR_DEFAULT;
        _logFileSizeLimitMB = ZLOG_LOG_FILE_SIZE_LIMIT_DEFAULT;
        _isUsingFileLine = ZLOG_IS_USING_FILE_LINE_DEFAULT;

		_logFileReserveTimeSec = ZLOG_LOG_FILE_RESERVE_SEC_DEFAULT;

        _curFileCreationTime = 0;
        _curFileCreationDay = 0;
        _curFileIndex = 0;
        _curWriteLen = 0;		
    }

    std::string _loggerKey;
    std::string _loggerName;
    std::string _logFilePath;
    int  _filterLogLevel;
    bool _isDisplayOnConsole;
    bool _isWriteToFile;
    bool _isUsingMonthDir;
    unsigned int _logFileSizeLimitMB;
    bool _isLoggerEnabled;
    bool _isUsingFileLine;
    time_t _logFileReserveTimeSec;
    time_t _curFileCreationTime;
    time_t _curFileCreationDay;
    unsigned int _curFileIndex;
    unsigned int _curWriteLen;
    ZLogFileHandler  _loggerFileHandler;
    std::list< std::pair<time_t, std::string> > _historyLogs;
};

// These 'util-functions' below will be used by 'ZLogManager'
// See their implementation on the bottom
static void trimLogConfig(std::string& str, std::string suffixToIgnore = std::string());
static std::pair<std::string, std::string> splitPairString(const std::string& str, const std::string& delimiter);
static bool parseConfigLine(const std::string& line, int curLineNumber, std::string& key, std::map<std::string, LoggerInfo>& outInfo);
static bool parseConfigFromString(std::string content, std::map<std::string, LoggerInfo>& outInfo);
static void fixPath(std::string& path);
static bool isDirectory(std::string path);
static bool createRecursionDir(std::string path);
static inline struct tm timeToTm(time_t t);
static std::string getProcessID();
static std::string getProcessName();

class ZLogManager : public ThreadHelper, public IZLogManager
{
public:
    ZLogManager()
    {
		_isGlobalSwitchOn = false;
        _isRunning = false;
        _lastLoggerId = ZLOG_MAIN_LOGGER_ID;
        _hotUpdateInterval = 0;

        _totalpushLogDataCount = 0;
        _totalPopLogCount = 0;
        _totalWriteFileCount = 0;
        _totalWriteFileBytes = 0;

        _processId = getProcessID();
        _processName = getProcessName();
        _loggerInfoArr[ZLOG_MAIN_LOGGER_ID]._isLoggerEnabled = true;
        _idMap[ZLOG_MAIN_LOGGER_KEY] = ZLOG_MAIN_LOGGER_ID;
        _loggerInfoArr[ZLOG_MAIN_LOGGER_ID]._loggerKey = ZLOG_MAIN_LOGGER_KEY;
        _loggerInfoArr[ZLOG_MAIN_LOGGER_ID]._loggerName = ZLOG_MAIN_LOGGER_KEY;

        _chunk1[0] = '\0';
        _chunk2[1] = '\0';
        _chunk3[2] = '\0';
        _chunk4[3] = '\0';
    }

    virtual ~ZLogManager()
    {
        stop();
    }

    bool configFromStdString(std::string content, bool canIgnore)
    {
        unsigned int sum = 0;
        for (std::string::iterator iter = content.begin(); iter != content.end(); ++iter)
        {
            sum += (unsigned char)*iter;
        }
        if (sum == _checksum)
        {
            return true;
        }
        _checksum = sum;


        std::map<std::string, LoggerInfo> loggerMap;
        if (!parseConfigFromString(content, loggerMap))
        {
            printf(" !!! !!! !!! !!!%s", STR_NEW_LINE);
            printf(" !!! !!! zlog load config file error %s", STR_NEW_LINE);
            printf(" !!! !!! !!! !!!%s", STR_NEW_LINE);
            return false;
        }

        for (std::map<std::string, LoggerInfo>::iterator iter = loggerMap.begin(); iter != loggerMap.end(); ++iter)
        {
            LoggerId loggerId = ZLOG_INVALID_LOGGER_ID;
            loggerId = findLoggerByKey(iter->second._loggerKey.c_str());
            if (loggerId == ZLOG_INVALID_LOGGER_ID)
            {
                if (canIgnore)
                {
                    continue;
                }
                else
                {
                    loggerId = createLogger(iter->second._loggerKey.c_str());
                    if (loggerId == ZLOG_INVALID_LOGGER_ID)
                    {
                        continue;
                    }
                }
            }

            enableLogger(loggerId, iter->second._isLoggerEnabled);
            setLoggerName(loggerId, iter->second._loggerName.c_str());
            setLogFilePath(loggerId, iter->second._logFilePath.c_str());
            setFilterLogLevel(loggerId, iter->second._filterLogLevel);
            setIsUsingFileLine(loggerId, iter->second._isUsingFileLine);
            setIsDisplayOnConsole(loggerId, iter->second._isDisplayOnConsole);
            setIsWriteToFile(loggerId, iter->second._isWriteToFile);
            setLogFileSizeLimit(loggerId, iter->second._logFileSizeLimitMB);
            setIsUsingMonthDir(loggerId, iter->second._isUsingMonthDir);
        }

        return true;
    }

    virtual bool configFromFile(const char* configFilePath)
    {
        if (!_configFile.empty())
        {
            printf(" !!! !!! !!! !!!%s", STR_NEW_LINE);
            printf(" !!! !!! zlog configure error: too many calls to Config. the old config file=%s,  the new config file=%s !!! !!! %s",
                   _configFile.c_str(), configFilePath, STR_NEW_LINE);
            printf(" !!! !!! !!! !!!%s", STR_NEW_LINE);
            return false;
        }
        _configFile = configFilePath;

        ZLogFileHandler f;
        f.open(_configFile.c_str(), "rb");
        if (!f.isOpen())
        {
            printf(" !!! !!! !!! !!!%s", STR_NEW_LINE);
            printf(" !!! !!! zlog load config file error. filename=%s  !!! !!! %s", configFilePath, STR_NEW_LINE);
            printf(" !!! !!! !!! !!!%s", STR_NEW_LINE);
            return false;
        }

        return configFromStdString(f.readContent(), false);
    }

    virtual bool configFromString(const char* configContent)
    {
        return configFromStdString(configContent, false);
    }

    virtual LoggerId createLogger(const char* key)
    {
        if (key == NULL)
        {
            return ZLOG_INVALID_LOGGER_ID;
        }

        std::string copyKey = key;
        trimLogConfig(copyKey);

        LoggerId newID = ZLOG_INVALID_LOGGER_ID;
        std::map<std::string, LoggerId>::iterator iter = _idMap.find(copyKey);
        if (iter != _idMap.end())
        {
            newID = iter->second;
        }

        if (newID == ZLOG_INVALID_LOGGER_ID)
        {
            if (_lastLoggerId + 1 >= ZLOG_LOGGER_MAX_COUNT)
            {
                showColorText("zlog: CreateLogger can not create|writeover, because loggerid need < LOGGER_MAX!", LOG_LEVEL_FATAL);
                return ZLOG_INVALID_LOGGER_ID;
            }

            newID = ++_lastLoggerId;
            _idMap[copyKey] = newID;
            _loggerInfoArr[newID]._isLoggerEnabled = true;
            _loggerInfoArr[newID]._loggerKey = copyKey;
            _loggerInfoArr[newID]._loggerName = copyKey;
        }

        return newID;
    }

    virtual bool start(bool isGlobalSwitchOn = true)
    {
        if (_isRunning)
        {
            showColorText("zlog: already started", LOG_LEVEL_FATAL);
            return false;
        }

		_isGlobalSwitchOn = isGlobalSwitchOn;
        _semaphore.create(0);
        bool ret = ThreadHelper::start();
        return ret && _semaphore.wait(3000);
    }

    virtual bool stop()
    {
        if (_isRunning)
        {
            showColorText("zlog: stopping", LOG_LEVEL_FATAL);
            _isRunning = false;
            wait();
            while (!_freeLogDataVec.empty())
            {
                delete _freeLogDataVec.back();
                _freeLogDataVec.pop_back();
            }
            return true;
        }

        return false;
    }

    virtual bool prePushLogDataData(LoggerId loggerId, int level)
    {
		if (!_isGlobalSwitchOn)
		{
			return false;
		}

        if (loggerId < ZLOG_MAIN_LOGGER_ID || loggerId > _lastLoggerId ||
                !_isRunning || !_loggerInfoArr[loggerId]._isLoggerEnabled)
        {
            return false;
        }
        if (level < _loggerInfoArr[loggerId]._filterLogLevel)
        {
            return false;
        }
        size_t count = _logDataQueue.size();

        if (count > ZLOG_LOG_QUEUE_SIZE_LIMIT)
        {
            size_t rate = (count - ZLOG_LOG_QUEUE_SIZE_LIMIT) * 100 / ZLOG_LOG_QUEUE_SIZE_LIMIT;
            if (rate > 100)
            {
                rate = 100;
            }
            if ((size_t)rand() % 100 < rate)
            {
                if (rate > 50)
                {
                    AutoLock l(_logDataQueueLock);
                    count = _logDataQueue.size();
                }
                if (count > ZLOG_LOG_QUEUE_SIZE_LIMIT)
                {
                    sleepMillisecond((unsigned int)(rate));
                }
            }
        }
        return true;
    }

    virtual bool pushLogData(LogData* pLogData, const char* file, int line)
    {
		if (!_isGlobalSwitchOn)
		{
			sleepMicrosecond(1);
			return false;
		}

        // discard log
        if (pLogData->_loggerId < ZLOG_MAIN_LOGGER_ID || pLogData->_loggerId > _lastLoggerId || !_isRunning || !_loggerInfoArr[pLogData->_loggerId]._isLoggerEnabled)
        {
            freeLogData(pLogData);
            return false;
        }

        //filter log
        if (pLogData->_filterLogLevel < _loggerInfoArr[pLogData->_loggerId]._filterLogLevel)
        {
            freeLogData(pLogData);
            return false;
        }

        if (_loggerInfoArr[pLogData->_loggerId]._isUsingFileLine && file)
        {
            const char* pNameEnd = file + strlen(file);
            const char* pNameBegin = pNameEnd;

            do
            {
                if (*pNameBegin == '\\' || *pNameBegin == '/')
                {
                    ++pNameBegin;
                    break;
                }
                if (pNameBegin == file)
                {
                    break;
                }

                --pNameBegin;
            } while (true);

            ZLogStream ss(pLogData->_content + pLogData->_contentLength, ZLOG_LOG_BUFFER_SIZE - pLogData->_contentLength);
            ss.writeChar(' ');
            ss.writeString(pNameBegin, pNameEnd - pNameBegin);
            ss.writeChar(':');
            ss.writeULongLong((unsigned long long)line);
            pLogData->_contentLength += ss.getCurrentLen();
        }

        if (pLogData->_contentLength + 3 > ZLOG_LOG_BUFFER_SIZE)
        {
            pLogData->_contentLength = ZLOG_LOG_BUFFER_SIZE - 3;
        }

        // append 'newline'
        for (size_t i = 0; i < LEN_NEW_LINE; ++i)
        {
            pLogData->_content[pLogData->_contentLength + i] = STR_NEW_LINE[i];
        }
        pLogData->_content[pLogData->_contentLength + LEN_NEW_LINE] = '\0';
        pLogData->_contentLength += LEN_NEW_LINE;

        if (_loggerInfoArr[pLogData->_loggerId]._isDisplayOnConsole && ZLOG_SYNC_WRITE_TO_FILE)
        {
            showColorText(pLogData->_content, pLogData->_filterLogLevel);
        }

        if (ZLOG_SYNC_DISPLAY_ON_CONSOLE && ZLOG_SYNC_WRITE_TO_FILE)
        {
#ifdef WIN32
            OutputDebugStringA(pLogData->_content);
#endif
        }

        if (_loggerInfoArr[pLogData->_loggerId]._isWriteToFile && ZLOG_SYNC_WRITE_TO_FILE)
        {
            AutoLock l(_logDataQueueLock);
            if (openLogger(pLogData))
            {
                _loggerInfoArr[pLogData->_loggerId]._loggerFileHandler.write(pLogData->_content, pLogData->_contentLength);
                _loggerInfoArr[pLogData->_loggerId]._curWriteLen += (unsigned int)pLogData->_contentLength;
                closeLogger(pLogData->_loggerId);
                ++_totalWriteFileCount;
                _totalWriteFileBytes += pLogData->_contentLength;
            }
        }

        if (ZLOG_SYNC_WRITE_TO_FILE)
        {
            freeLogData(pLogData);
            return true;
        }

        AutoLock l(_logDataQueueLock);
        _logDataQueue.push_back(pLogData);
        ++_totalpushLogDataCount;

        return true;
    }

    virtual LoggerId findLoggerByKey(const char* key)
    {
        std::map<std::string, LoggerId>::iterator iter;
        iter = _idMap.find(key);
        if (iter != _idMap.end())
        {
            return iter->second;
        }

        return ZLOG_INVALID_LOGGER_ID;
    }

    bool hotChange(LoggerId loggerId, ENUM_CONFIG_TYPE configType, int num, bool flag, const std::string& text)
    {
        if (loggerId < ZLOG_MAIN_LOGGER_ID || loggerId > _lastLoggerId)
        {
            return false;
        }

        if (text.length() >= ZLOG_LOG_BUFFER_SIZE)
        {
            return false;
        }

        if (!_isRunning || ZLOG_SYNC_WRITE_TO_FILE)
        {
            return onHotChange(loggerId, configType, num, flag, text);
        }

        LogData* pLogData = makeLogData(loggerId, ZLOG_FILTER_LOG_LEVEL_DEFAULT);
        pLogData->_loggerId = loggerId;
        pLogData->_configType = configType;
        pLogData->_configValue = num;
        memcpy(pLogData->_content, text.c_str(), text.length());
        pLogData->_contentLength = (int)text.length();
        AutoLock l(_logDataQueueLock);
        _logDataQueue.push_back(pLogData);

        return true;
    }

    virtual bool enableLogger(LoggerId loggerId, bool flag)
    {
        if (loggerId < ZLOG_MAIN_LOGGER_ID || loggerId > _lastLoggerId)
        {
            return false;
        }

        if (flag)
        {
            _loggerInfoArr[loggerId]._isLoggerEnabled = true;
            return true;
        }

        return hotChange(loggerId, SET_IS_LOGGER_ENABLED, IVAL_DONT_CARE, false, SVAL_DONT_CARE);
    }

    virtual bool setLoggerName(LoggerId loggerId, const char* name)
    {
        if (loggerId < ZLOG_MAIN_LOGGER_ID || loggerId > _lastLoggerId)
        {
            return false;
        }
        //the name by main logger is the process name and it's can't change.
        //if (loggerId == ZLOG_MAIN_LOGGER_ID) return false;

        if (name == NULL || strlen(name) == 0)
        {
            return false;
        }

        return hotChange(loggerId, SET_LOGGER_NAME, IVAL_DONT_CARE, BVAL_DONT_CRAE, name);
    }

    virtual bool setLogFilePath(LoggerId loggerId, const char* path)
    {
        if (loggerId < ZLOG_MAIN_LOGGER_ID || loggerId > _lastLoggerId)
        {
            return false;
        }

        if (path == NULL || strlen(path) == 0)
        {
            return false;
        }
 
        std::string copyPath = path;
        char ch = copyPath.at(copyPath.length() - 1);
        if (ch != '\\' && ch != '/')
        {
            copyPath.append("/");
        }

        return hotChange(loggerId, SET_LOG_FILE_PATH, IVAL_DONT_CARE, BVAL_DONT_CRAE, copyPath);
    }

    virtual bool setFilterLogLevel(LoggerId loggerId, int level)
    {
        if (loggerId < ZLOG_MAIN_LOGGER_ID || loggerId > _lastLoggerId)
        {
            return false;
        }

        if (level < LOG_LEVEL_MIN__ || level > LOG_LEVEL_MAX__)
        {
            return false;
        }

        if (level <= _loggerInfoArr[loggerId]._filterLogLevel)
        {
            _loggerInfoArr[loggerId]._filterLogLevel = level;
            return true;
        }

        return hotChange(loggerId, SET_FILTER_LOG_LEVEL, level, BVAL_DONT_CRAE, SVAL_DONT_CARE);
    }

    virtual bool setIsUsingFileLine(LoggerId loggerId, bool flag)
    {
        return hotChange(loggerId, SET_IS_USING_FILE_LINE, IVAL_DONT_CARE, flag, SVAL_DONT_CARE);
    }

    virtual bool setIsDisplayOnConsole(LoggerId loggerId, bool flag)
    {
        return hotChange(loggerId, SET_IS_DISPLAY_ON_CONSOLE, IVAL_DONT_CARE, flag, SVAL_DONT_CARE);
    }

    virtual bool setIsWriteToFile(LoggerId loggerId, bool flag)
    {
        return hotChange(loggerId, SET_IS_WRITE_TO_FILE, IVAL_DONT_CARE,flag, SVAL_DONT_CARE);
    }

    virtual bool setLogFileSizeLimit(LoggerId loggerId, unsigned int size)
    {
        if (size == 0)
        {
            size = (unsigned int)-1;
        }

        return hotChange(loggerId, SET_LOG_FILE_SIZE_LIMIT, size, BVAL_DONT_CRAE, SVAL_DONT_CARE);
    }

    virtual bool setIsUsingMonthDir(LoggerId loggerId, bool flag)
    {
        return hotChange(loggerId, SET_IS_USING_MONTH_DIR, IVAL_DONT_CARE, flag, SVAL_DONT_CARE);
    }

    virtual bool setLogFileReserveTimeSec(LoggerId loggerId, time_t sec)
    {
        return hotChange(loggerId, SET_LOG_FILE_RESERVE_TIME, (int)sec, BVAL_DONT_CRAE, SVAL_DONT_CARE);
    }

    virtual bool setAutoUpdateInterval(int interval)
    {
        _hotUpdateInterval = interval;
        return true;
    }

    virtual bool updateConfig()
    {
        if (_configFile.empty())
        {
            printf("zlog: update config file error. filename is empty.");
            return false;
        }

        ZLogFileHandler f;
        f.open(_configFile.c_str(), "rb");
        if (!f.isOpen())
        {
            printf(" !!! !!! !!! !!!%s", STR_NEW_LINE);
            printf(" !!! !!! zlog load config file error. filename=%s  !!! !!! %s", _configFile.c_str(), STR_NEW_LINE);
            printf(" !!! !!! !!! !!!%s", STR_NEW_LINE);
            return false;
        }

        return configFromStdString(f.readContent(), true);
    }

    virtual bool isLoggerEnabled(LoggerId loggerId)
    {
        if (loggerId < ZLOG_MAIN_LOGGER_ID || loggerId > _lastLoggerId)
        {
            return false;
        }

        return _loggerInfoArr[loggerId]._isLoggerEnabled;
    }

    virtual unsigned long long getTotalWriteFileCount()
    {
        return _totalWriteFileCount;
    }

    virtual unsigned long long getTotalWriteFileBytes()
    {
        return _totalWriteFileBytes;
    }

    virtual unsigned long long getTotalPushLogDataCount()
    {
        return _totalpushLogDataCount;
    }

    virtual unsigned long long getTotalPopLogDataCount()
    {
        return _totalPopLogCount;
    }

    virtual unsigned int getTotalActiveLoggerCount()
    {
        unsigned int activeCount = 0;
        for (int i = 0; i <= _lastLoggerId; ++i)
        {
            if (_loggerInfoArr[i]._isLoggerEnabled)
            {
                ++activeCount;
            }
        }
        return activeCount;
    }

protected:

    virtual LogData* makeLogData(LoggerId loggerId, int level)
    {
        LogData* pLogData = NULL;

        if (!_freeLogDataVec.empty())
        {
            AutoLock l(_freeDataLock);
            if (!_freeLogDataVec.empty())
            {
                pLogData = _freeLogDataVec.back();
                _freeLogDataVec.pop_back();
            }
        }
        if (pLogData == NULL)
        {
            pLogData = new(malloc(sizeof(LogData) + ZLOG_LOG_BUFFER_SIZE - 1))LogData();
        }

        //append precision time to logData
        pLogData->_loggerId = loggerId;
        pLogData->_filterLogLevel = level;
        pLogData->_configType = SET_GENERAL;
        pLogData->_configValue = 0;
        pLogData->_threadId = 0;
        pLogData->_contentLength = 0;
#ifdef WIN32
        FILETIME ft;
        GetSystemTimeAsFileTime(&ft);
        unsigned long long now = ft.dwHighDateTime;
        now <<= 32;
        now |= ft.dwLowDateTime;
        now /= 10;
        now -= 11644473600000000ULL;
        now /= 1000;
        pLogData->_creationTime = now / 1000;
        pLogData->_precision = (unsigned int)(now % 1000);
#else
        struct timeval tm;
        gettimeofday(&tm, NULL);
        pLogData->_creationTime = tm.tv_sec;
        pLogData->_precision = tm.tv_usec / 1000;
#endif
#ifdef WIN32
        pLogData->_threadId = GetCurrentThreadId();
#elif defined(__APPLE__) // ifdef WIN32
        unsigned long long tid = 0;
        pthread_threadid_np(NULL, &tid);
        pLogData->_threadId = (unsigned int)tid;
#else // ifdef WIN32, elif defined(__APPLE__)
        pLogData->_threadId = (unsigned int)syscall(SYS_gettid);
#endif // ifdef WIN32, elif defined(__APPLE__), else

        //format log
#ifdef WIN32
        static __declspec(thread) struct tm g_tt = { 0 };
        static __declspec(thread) time_t g_curDayTime = 0;
#else
        static __thread struct tm g_tt = { 0 };
        static __thread time_t g_curDayTime = 0;
#endif // WIN32
        if (pLogData->_creationTime < g_curDayTime || pLogData->_creationTime >= g_curDayTime + 24 * 3600)
        {
            g_tt = timeToTm(pLogData->_creationTime);
            g_tt.tm_hour = 0;
            g_tt.tm_min = 0;
            g_tt.tm_sec = 0;
            g_curDayTime = mktime(&g_tt);
        }

        time_t sec = pLogData->_creationTime - g_curDayTime;
        ZLogStream ls(pLogData->_content, ZLOG_LOG_BUFFER_SIZE);
        ls.writeULongLong(g_tt.tm_year + 1900, 4);
        ls.writeChar('-');
        ls.writeULongLong(g_tt.tm_mon + 1, 2);
        ls.writeChar('-');
        ls.writeULongLong(g_tt.tm_mday, 2);
        ls.writeChar(' ');
        ls.writeULongLong(sec / 3600, 2);
        ls.writeChar(':');
        ls.writeULongLong((sec % 3600) / 60, 2);
        ls.writeChar(':');
        ls.writeULongLong(sec % 60, 2);
        ls.writeChar('.');
        ls.writeULongLong(pLogData->_precision, 3);
        ls.writeChar(' ');
        ls.writeChar('[');
        ls.writeULongLong(pLogData->_threadId, 6);
        ls.writeChar(']');

        ls.writeChar(' ');
        ls.writeString(LOG_STRING[pLogData->_filterLogLevel], LOG_STRING_LEN[pLogData->_filterLogLevel]);
        ls.writeChar(' ');
        pLogData->_contentLength = ls.getCurrentLen();

        return pLogData;
    }

    virtual void freeLogData(LogData* pLogData)
    {
        if (_freeLogDataVec.size() < ZLOG_FREE_LOG_DATA_QUEUE_SIZE)
        {
            AutoLock l(_freeDataLock);
            _freeLogDataVec.push_back(pLogData);
        }
        else
        {
            pLogData->~LogData();
            free(pLogData);
        }
    }

    void showColorText(const char* text, int level = LOG_LEVEL_DEBUG, bool toggleNewLine = true)
    {
#if defined(WIN32) && defined(ZLOG_OEM_CONSOLE)
        char oem[ZLOG_LOG_BUFFER_SIZE] = { 0 };
        CharToOemBuffA(text, oem, ZLOG_LOG_BUFFER_SIZE);
#endif

        if (level <= LOG_LEVEL_DEBUG || level > LOG_LEVEL_FATAL)
        {
#if defined(WIN32) && defined(ZLOG_OEM_CONSOLE)
            printf("%s", oem);
#else
            printf("%s", text);
#endif
            return;
        }
#ifndef WIN32
        printf("%s%s\e[0m", LOG_COLOR[level], text);
#else
        AutoLock l(_consoleDisplayLock);
        HANDLE hStd = ::GetStdHandle(STD_OUTPUT_HANDLE);
        if (hStd == INVALID_HANDLE_VALUE) return;
        CONSOLE_SCREEN_BUFFER_INFO oldInfo;
        if (!GetConsoleScreenBufferInfo(hStd, &oldInfo))
        {
            return;
        }
        else
        {
            SetConsoleTextAttribute(hStd, LOG_COLOR[level]);
#ifdef ZLOG_OEM_CONSOLE
            printf("%s", oem);
#else
            printf("%s", text);
#endif
            SetConsoleTextAttribute(hStd, oldInfo.wAttributes);
        }
#endif

        if (toggleNewLine)
        {
            printf("%s", STR_NEW_LINE);
        }

        return;
    }

    bool onHotChange(LoggerId loggerId, ENUM_CONFIG_TYPE configType, int num, bool flag, const std::string& text)
    {
        if (loggerId < ZLOG_MAIN_LOGGER_ID || loggerId > _lastLoggerId)
        {
            return false;
        }

        LoggerInfo& loggerInfo = _loggerInfoArr[loggerId];

        switch(configType)
        {
        case SET_IS_LOGGER_ENABLED:
            loggerInfo._isLoggerEnabled = flag;
            break;
        case SET_LOGGER_NAME:
            loggerInfo._loggerName = text;
            break;
        case SET_LOG_FILE_PATH:
            loggerInfo._logFilePath = text;
            break;
        case SET_FILTER_LOG_LEVEL:
            loggerInfo._filterLogLevel = num;
            break;
        case SET_IS_USING_FILE_LINE:
            loggerInfo._isUsingFileLine = flag;
            break;
        case SET_IS_DISPLAY_ON_CONSOLE:
            loggerInfo._isDisplayOnConsole = flag;
            break;
        case SET_IS_WRITE_TO_FILE:
            loggerInfo._isWriteToFile = flag;
            break;
        case SET_LOG_FILE_SIZE_LIMIT:
            loggerInfo._logFileSizeLimitMB = num;
            break;
        case SET_IS_USING_MONTH_DIR:
            loggerInfo._isUsingMonthDir = flag;
            break;
        case SET_LOG_FILE_RESERVE_TIME:
            loggerInfo._logFileReserveTimeSec = num >= 0 ? num : 0;
            break;
        default:
            break;
        }

        return true;
    }

    bool openLogger(LogData* pLogData)
    {
        int loggerId = pLogData->_loggerId;

        if (loggerId < ZLOG_MAIN_LOGGER_ID || loggerId >_lastLoggerId)
        {
            showColorText("zlog: openLogger can not open, invalide logger id!", LOG_LEVEL_FATAL);
            printf("%s", STR_NEW_LINE);
            return false;
        }

        LoggerInfo* pLoggerInfo = &_loggerInfoArr[loggerId];

        if (!pLoggerInfo->_isLoggerEnabled || !pLoggerInfo->_isWriteToFile || pLogData->_filterLogLevel < pLoggerInfo->_filterLogLevel)
        {
            return false;
        }

        bool sameDay = pLogData->_creationTime >= pLoggerInfo->_curFileCreationDay && pLogData->_creationTime - pLoggerInfo->_curFileCreationDay < 24 * 3600;
        bool needChageFile = pLoggerInfo->_curWriteLen > pLoggerInfo->_logFileSizeLimitMB * 1024 * 1024;

        if (!sameDay || needChageFile)
        {
            if (!sameDay)
            {
                pLoggerInfo->_curFileIndex = 0;
            }
            else
            {
                ++pLoggerInfo->_curFileIndex;
            }
            if (pLoggerInfo->_loggerFileHandler.isOpen())
            {
                pLoggerInfo->_loggerFileHandler.close();
            }
        }

        if (!pLoggerInfo->_loggerFileHandler.isOpen())
        {
            pLoggerInfo->_curFileCreationTime = pLogData->_creationTime;
            pLoggerInfo->_curWriteLen = 0;

            tm t = timeToTm(pLoggerInfo->_curFileCreationTime);
            if (true) //process day time
            {
                tm day = t;
                day.tm_hour = 0;
                day.tm_min = 0;
                day.tm_sec = 0;
                pLoggerInfo->_curFileCreationDay = mktime(&day);
            }

            std::string name;
            std::string path;

            name = pLoggerInfo->_loggerName;
            path = pLoggerInfo->_logFilePath;

            char buf[500] = { 0 };
            if (pLoggerInfo->_isUsingMonthDir)
            {
                sprintf(buf, "%04d_%02d/", t.tm_year + 1900, t.tm_mon + 1);
                path += buf;
            }

            if (!isDirectory(path))
            {
                createRecursionDir(path);
            }
            if (ZLOG_SYNC_WRITE_TO_FILE)
            {
                sprintf(buf, "%s_%s_%04d%02d%02d%02d_%s_%03u.log",
                        _processName.c_str(), name.c_str(), t.tm_year + 1900, t.tm_mon + 1, t.tm_mday,
                        t.tm_hour, _processId.c_str(), pLoggerInfo->_curFileIndex);
            }
            else
            {

                sprintf(buf, "%s_%s_%04d%02d%02d%02d%02d_%s_%03u.log",
                        _processName.c_str(), name.c_str(), t.tm_year + 1900, t.tm_mon + 1, t.tm_mday,
                        t.tm_hour, t.tm_min, _processId.c_str(), pLoggerInfo->_curFileIndex);
            }
            path += buf;
            long curLen = pLoggerInfo->_loggerFileHandler.open(path.c_str(), "ab");
            if (!pLoggerInfo->_loggerFileHandler.isOpen() || curLen < 0)
            {
                sprintf(buf, "zlog: can not open log file %s.", path.c_str());
                showColorText("!!!!!!!!!!!!!!!!!!!!!!!!!!", LOG_LEVEL_FATAL);
                showColorText(buf, LOG_LEVEL_FATAL);
                showColorText("!!!!!!!!!!!!!!!!!!!!!!!!!!", LOG_LEVEL_FATAL);
                pLoggerInfo->_isWriteToFile = false;
                return false;
            }
            pLoggerInfo->_curWriteLen = (unsigned int)curLen;

            if (pLoggerInfo->_logFileReserveTimeSec > 0)
            {
                if (pLoggerInfo->_historyLogs.size() > ZLOG_LOG_FILE_RESERVE_COUNT)
                {
                    while (!pLoggerInfo->_historyLogs.empty() && pLoggerInfo->_historyLogs.front().first < time(NULL) - pLoggerInfo->_logFileReserveTimeSec)
                    {
                        pLoggerInfo->_loggerFileHandler.removeFile(pLoggerInfo->_historyLogs.front().second.c_str());
                        pLoggerInfo->_historyLogs.pop_front();
                    }
                }
                if (pLoggerInfo->_historyLogs.empty() || pLoggerInfo->_historyLogs.back().second != path)
                {
                    pLoggerInfo->_historyLogs.push_back(std::make_pair(time(NULL), path));
                }
            }
            return true;
        }

        return true;
    }

    bool closeLogger(LoggerId loggerId)
    {
        if (loggerId < ZLOG_MAIN_LOGGER_ID || loggerId >_lastLoggerId)
        {
            showColorText("zlog: closeLogger can not close, invalide logger id!", LOG_LEVEL_FATAL);
            return false;
        }
        LoggerInfo* pLoggerInfo = &_loggerInfoArr[loggerId];
        if (pLoggerInfo->_loggerFileHandler.isOpen())
        {
            pLoggerInfo->_loggerFileHandler.close();
            return true;
        }
        return false;
    }

    bool popLog(LogData*& rpLogData)
    {
        if (_logDataQueueCache.empty())
        {
            if (!_logDataQueue.empty())
            {
                AutoLock l(_logDataQueueLock);
                if (_logDataQueue.empty())
                {
                    return false;
                }
                _logDataQueueCache.swap(_logDataQueue);
            }
        }
        if (!_logDataQueueCache.empty())
        {
            rpLogData = _logDataQueueCache.front();
            _logDataQueueCache.pop_front();
            return true;
        }
        return false;
    }

    virtual void run()
    {
        _isRunning = true;
        LOGI("zlog: \'ZLogManager\' thread started");
        for (int i = 0; i <= _lastLoggerId; ++i)
        {
            if (_loggerInfoArr[i]._isLoggerEnabled)
            {
                LOGI(STR_NEW_LINE << "\t-----------------------------------------------------------------"
                  << STR_NEW_LINE << "\t\t           logger_id = " << i
                  << STR_NEW_LINE << "\t\t          logger_key = " << _loggerInfoArr[i]._loggerKey
                  << STR_NEW_LINE << "\t\t         logger_name = " << _loggerInfoArr[i]._loggerName
                  << STR_NEW_LINE << "\t\t       log_file_path = " << _loggerInfoArr[i]._logFilePath
                  << STR_NEW_LINE << "\t\t    filter_log_level = " << LOG_LEVEL_STRING[_loggerInfoArr[i]._filterLogLevel]
                  << STR_NEW_LINE << "\t\t  display_on_console = " << (_loggerInfoArr[i]._isDisplayOnConsole ? "true" : "false")
                  << STR_NEW_LINE << "\t----------------------------------------------------------------- ");
            }
        }

        _semaphore.post();

        LogData* pLogData = NULL;
        int needFlush[ZLOG_LOGGER_MAX_COUNT] = { 0 };
        time_t lastUpdateTime = time(NULL);

        while (true)
        {
            while (popLog(pLogData))
            {
                if (pLogData->_loggerId < ZLOG_MAIN_LOGGER_ID || pLogData->_loggerId > _lastLoggerId)
                {
                    freeLogData(pLogData);
                    continue;
                }
                LoggerInfo& curLoggerInfo = _loggerInfoArr[pLogData->_loggerId];

                if (pLogData->_configType != SET_GENERAL)
                {
                    bool bValue = (pLogData->_configValue != 0);
                    onHotChange(pLogData->_loggerId, (ENUM_CONFIG_TYPE)pLogData->_configType, pLogData->_configValue, bValue, std::string(pLogData->_content, pLogData->_contentLength));
                    curLoggerInfo._loggerFileHandler.close();
                    freeLogData(pLogData);
                    continue;
                }

                ++_totalPopLogCount;

                if (!curLoggerInfo._isLoggerEnabled || pLogData->_filterLogLevel < curLoggerInfo._filterLogLevel)
                {
                    freeLogData(pLogData);
                    continue;
                }

                if (curLoggerInfo._isDisplayOnConsole)
                {
                    showColorText(pLogData->_content, pLogData->_filterLogLevel);
                }
                if (ZLOG_SYNC_DISPLAY_ON_CONSOLE)
                {
#ifdef WIN32
                    OutputDebugStringA(pLogData->_content);
#endif
                }

                if (curLoggerInfo._isWriteToFile)
                {
                    if (!openLogger(pLogData))
                    {
                        freeLogData(pLogData);
                        continue;
                    }

                    curLoggerInfo._loggerFileHandler.write(pLogData->_content, pLogData->_contentLength);
                    curLoggerInfo._curWriteLen += (unsigned int)pLogData->_contentLength;
                    ++needFlush[pLogData->_loggerId];
                    ++_totalWriteFileCount;
                    _totalWriteFileBytes += pLogData->_contentLength;
                }
                else
                {
                    ++_totalWriteFileCount;
                    _totalWriteFileBytes += pLogData->_contentLength;
                }

                freeLogData(pLogData);
            }

            for (int i = 0; i <= _lastLoggerId; ++i)
            {
                if (_loggerInfoArr[i]._isLoggerEnabled && needFlush[i] > 0)
                {
                    _loggerInfoArr[i]._loggerFileHandler.flush();
                    needFlush[i] = 0;
                }
                if (!_loggerInfoArr[i]._isLoggerEnabled && _loggerInfoArr[i]._loggerFileHandler.isOpen())
                {
                    _loggerInfoArr[i]._loggerFileHandler.close();
                }
            }

            // delay
            sleepMillisecond(ZLOG_DELAY_MS);

            // quit
            if (!_isRunning && _logDataQueue.empty())
            {
                break;
            }

            if (_hotUpdateInterval != 0 && time(NULL) - lastUpdateTime > _hotUpdateInterval)
            {
                updateConfig();
                lastUpdateTime = time(NULL);
            }
        }

        for (int i = 0; i <= _lastLoggerId; ++i)
        {
            if (_loggerInfoArr[i]._isLoggerEnabled)
            {
                _loggerInfoArr[i]._isLoggerEnabled = false;
                closeLogger(i);
            }
        }

    }

private:
	// this is the global switch of zlog
	bool _isGlobalSwitchOn;
    bool _isRunning;
    SemaphoreHelper _semaphore;
    int _hotUpdateInterval;
    unsigned int _checksum;
    std::string _processId;
    std::string _processName;
    std::string _configFile;
    std::map<std::string, LoggerId> _idMap;
    LoggerId _lastLoggerId;
    LoggerInfo _loggerInfoArr[ZLOG_LOGGER_MAX_COUNT];

    char _chunk1[256];
    LockHelper _logDataQueueLock;
    std::deque<LogData*> _logDataQueue;
    unsigned long long _totalpushLogDataCount;

    char _chunk2[256];
    LockHelper _freeDataLock;
    std::vector<LogData*> _freeLogDataVec;

    char _chunk3[256];
    LockHelper _consoleDisplayLock;

    char _chunk4[256];
    std::deque<LogData*> _logDataQueueCache;
    unsigned long long _totalPopLogCount;
    unsigned long long _totalWriteFileCount;
    unsigned long long _totalWriteFileBytes;
};

// This is the real implementation for IZLogManager-Singleton
IZLogManager* IZLogManager::getInstance()
{
    static ZLogManager m;
    return &m;
}

/////////////////////////////////////////////////////////////////
// Implementation of 'util-functions' on the above
/////////////////////////////////////////////////////////////////

static inline void sleepMicrosecond(unsigned int us)
{
#ifdef WIN32
	LARGE_INTEGER freq;
	LARGE_INTEGER start;
	LARGE_INTEGER current;
	LONGLONG elapsed;
	LONGLONG total;
	QueryPerformanceFrequency(&freq);
	total = (LONGLONG)(us * freq.QuadPart / 100000);	
	QueryPerformanceCounter(&start);
	do
	{
		QueryPerformanceCounter(&current);
		elapsed = current.QuadPart - start.QuadPart;
	} while (elapsed < total);
#else
	usleep(us);
#endif
}

static inline void sleepMillisecond(unsigned int ms)
{
#ifdef WIN32
    ::Sleep(ms);
#else
    usleep(1000 * ms);
#endif
}

static inline struct tm timeToTm(time_t t)
{
#ifdef WIN32
#	if _MSC_VER < 1400 //VS2003
    return *localtime(&t);
#	else //vs2005->vs2013->
    struct tm tt = { 0 };
    localtime_s(&tt, &t);
    return tt;
#	endif
#else //linux
    struct tm tt = { 0 };
    localtime_r(&t, &tt);
    return tt;
#endif
}

static void fixPath(std::string& path)
{
    if (path.empty()) { return; }
    for (std::string::iterator iter = path.begin(); iter != path.end(); ++iter)
    {
        if (*iter == '\\') { *iter = '/'; }
    }
    if (path.at(path.length() - 1) != '/') { path.append("/"); }
}

static void trimLogConfig(std::string& str, std::string suffixToIgnore)
{
    if (str.empty()) { return; }
    suffixToIgnore += STR_NEW_LINE;
    suffixToIgnore += "\t ";
    int length = (int)str.length();
    int posBegin = 0;
    int posEnd = 0;

    //trim utf8 file bom
    if (str.length() >= 3
            && (unsigned char)str[0] == 0xef
            && (unsigned char)str[1] == 0xbb
            && (unsigned char)str[2] == 0xbf)
    {
        posBegin = 3;
    }

    //trim character
    for (int i = posBegin; i < length; ++i)
    {
        bool bCheck = false;
        for (int j = 0; j < (int)suffixToIgnore.length(); ++j)
        {
            if (str[i] == suffixToIgnore[j])
            {
                bCheck = true;
            }
        }
        if (bCheck)
        {
            if (i == posBegin)
            {
                ++posBegin;
            }
        }
        else
        {
            posEnd = i + 1;
        }
    }
    if (posBegin < posEnd)
    {
        str = str.substr(posBegin, posEnd - posBegin);
    }
    else
    {
        str.clear();
    }
}

static std::pair<std::string, std::string> splitPairString(const std::string& str, const std::string& delimiter)
{
    std::string::size_type pos = str.find(delimiter.c_str());
    if (pos == std::string::npos)
    {
        return std::make_pair(str, "");
    }
    return std::make_pair(str.substr(0, pos), str.substr(pos + delimiter.length()));
}

static bool parseConfigLine(const std::string& line, int curLineNum, std::string& key, std::map<std::string, LoggerInfo>& outInfo)
{
    std::pair<std::string, std::string> kv = splitPairString(line, "=");
    if (kv.first.empty())
    {
        return false;
    }

    trimLogConfig(kv.first);
    trimLogConfig(kv.second);

    if (kv.first.empty() || kv.first.at(0) == '#')
    {
        return true;
    }

    if (kv.first.at(0) == '[')
    {
        trimLogConfig(kv.first, "[]");
        key = kv.first;
        std::string tmpstr = kv.first;
        std::transform(tmpstr.begin(), tmpstr.end(), tmpstr.begin(), ::tolower);
        if (tmpstr == "main")
        {
            key = "Main";
        }

        std::map<std::string, LoggerInfo>::iterator iter = outInfo.find(key);

        if (iter == outInfo.end())
        {
            LoggerInfo li;
            li._isLoggerEnabled = true;
            li._loggerKey = key;
            li._loggerName = key;
            outInfo.insert(std::make_pair(key, li));
        }
        else
        {
            printf("zlog: configure warning: duplicate logger key:[%s] at line: %d %s", key.c_str(), curLineNum, STR_NEW_LINE);
        }

        return true;
    }

    trimLogConfig(kv.first);
    trimLogConfig(kv.second);
    std::map<std::string, LoggerInfo>::iterator iter = outInfo.find(key);

    if (iter == outInfo.end())
    {
        printf("zlog: configure warning: current logger name not found :[%s] at line:%d, key=%s, value=%s %s",
               key.c_str(), curLineNum, kv.first.c_str(), kv.second.c_str(), STR_NEW_LINE);

        return true;
    }

    std::transform(kv.first.begin(), kv.first.end(), kv.first.begin(), ::tolower);

    if (kv.first == "log_file_path")
    {
        iter->second._logFilePath = kv.second;
        return true;
    }
    else if (kv.first == "logger_name")
    {
        iter->second._loggerName = kv.second;
        return true;
    }
    std::transform(kv.second.begin(), kv.second.end(), kv.second.begin(), ::tolower);

    if (kv.first == "filter_log_level")
    {
        if (kv.second == "trace" || kv.second == "all")
        {
            iter->second._filterLogLevel = LOG_LEVEL_TRACE;
        }
        else if (kv.second == "debug")
        {
            iter->second._filterLogLevel = LOG_LEVEL_DEBUG;
        }
        else if (kv.second == "info")
        {
            iter->second._filterLogLevel = LOG_LEVEL_INFO;
        }
        else if (kv.second == "warn" || kv.second == "warning")
        {
            iter->second._filterLogLevel = LOG_LEVEL_WARN;
        }
        else if (kv.second == "error")
        {
            iter->second._filterLogLevel = LOG_LEVEL_ERROR;
        }
        else if (kv.second == "alarm")
        {
            iter->second._filterLogLevel = LOG_LEVEL_ALARM;
        }
        else if (kv.second == "fatal")
        {
            iter->second._filterLogLevel = LOG_LEVEL_FATAL;
        }
    }
    else if (kv.first == "display_on_console")
    {
        if (kv.second == "false" || kv.second == "0")
        {
            iter->second._isDisplayOnConsole = false;
        }
        else
        {
            iter->second._isDisplayOnConsole = true;
        }
    }
    else if (kv.first == "write_to_file")
    {
        if (kv.second == "false" || kv.second == "0")
        {
            iter->second._isWriteToFile = false;
        }
        else
        {
            iter->second._isWriteToFile = true;
        }
    }
    else if (kv.first == "use_month_dir")
    {
        if (kv.second == "false" || kv.second == "0")
        {
            iter->second._isUsingMonthDir = false;
        }
        else
        {
            iter->second._isUsingMonthDir = true;
        }
    }
    else if (kv.first == "log_file_size_limit")
    {
        iter->second._logFileSizeLimitMB = atoi(kv.second.c_str());
    }
    else if (kv.first == "use_file_line")
    {
        if (kv.second == "false" || kv.second == "0")
        {
            iter->second._isUsingFileLine = false;
        }
        else
        {
            iter->second._isUsingFileLine = true;
        }
    }
    else if (kv.first == "logger_enabled")
    {
        if (kv.second == "false" || kv.second == "0")
        {
            iter->second._isLoggerEnabled = false;
        }
        else
        {
            iter->second._isLoggerEnabled = true;
        }
    }
    else if (kv.first == "log_file_reserve_time")
    {
        iter->second._logFileReserveTimeSec = atoi(kv.second.c_str());
    }

    return true;
}

static bool parseConfigFromString(std::string content, std::map<std::string, LoggerInfo>& outInfo)
{
    std::string key;
    int curLine = 1;
    std::string line;
    std::string::size_type curPos = 0;
    if (content.empty())
    {
        return true;
    }
    do
    {
        std::string::size_type pos = std::string::npos;
        for (std::string::size_type i = curPos; i < content.length(); ++i)
        {
            //support linux/unix/windows LRCF
            if (content[i] == '\r' || content[i] == '\n')
            {
                pos = i;
                break;
            }
        }
        line = content.substr(curPos, pos - curPos);
        parseConfigLine(line, curLine, key, outInfo);
        ++curLine;

        if (pos == std::string::npos)
        {
            break;
        }
        else
        {
            curPos = pos + 1;
        }
    } while (1);
    return true;
}

bool isDirectory(std::string path)
{
#ifdef ZLOG_QT_VERSION
    QFileInfo fi(QString::fromStdString(path));
    return fi.isDir();
#elif defined WIN32
    return PathIsDirectoryA(path.c_str()) ? true : false;
#else
    DIR* pdir = opendir(path.c_str());
    if (pdir == NULL)
    {
        return false;
    }
    else
    {
        closedir(pdir);
        pdir = NULL;
        return true;
    }
#endif
}

bool createRecursionDir(std::string path)
{
    if (path.length() == 0) return true;
    std::string sub;
    fixPath(path);

    std::string::size_type pos = path.find('/');
    while (pos != std::string::npos)
    {
        std::string cur = path.substr(0, pos - 0);
        if (cur.length() > 0 && !isDirectory(cur))
        {
            bool ret = false;
#ifdef WIN32
            ret = CreateDirectoryA(cur.c_str(), NULL) ? true : false;
#else
            ret = (mkdir(cur.c_str(), S_IRWXU | S_IRWXG | S_IRWXO) == 0);
#endif
            if (!ret)
            {
                return false;
            }
        }
        pos = path.find('/', pos + 1);
    }

    return true;
}

std::string getProcessID()
{
    std::string pid = "0";
    char buf[260] = { 0 };
#ifdef WIN32
    DWORD winPID = GetCurrentProcessId();
    sprintf(buf, "%06u", winPID);
    pid = buf;
#else
    sprintf(buf, "%06d", getpid());
    pid = buf;
#endif
    return pid;
}

std::string getProcessName()
{
    std::string name = "process";
    char buf[260] = { 0 };
#ifdef WIN32
    if (GetModuleFileNameA(NULL, buf, 259) > 0)
    {
        name = buf;
    }
    std::string::size_type pos = name.rfind("\\");
    if (pos != std::string::npos)
    {
        name = name.substr(pos + 1, std::string::npos);
    }
    pos = name.rfind(".");
    if (pos != std::string::npos)
    {
        name = name.substr(0, pos - 0);
    }

#elif defined(ZLOG_HAVE_LIBPROC) // ifdef WIN32
    proc_loggerName(getpid(), buf, 260);
    name = buf;
    return name;;
#else // ifdef WIN32, elif defined(ZLOG_HAVE_LIBPROC)
    sprintf(buf, "/proc/%d/cmdline", (int)getpid());
    ZLogFileHandler i;
    i.open(buf, "rb");
    if (!i.isOpen())
    {
        return name;
    }
    name = i.readLine();
    i.close();

    std::string::size_type pos = name.rfind("/");
    if (pos != std::string::npos)
    {
        name = name.substr(pos + 1, std::string::npos);
    }
#endif // ifdef WIN32, elif defined(ZLOG_HAVE_LIBPROC), else
    return name;
  }
}


// EOF
