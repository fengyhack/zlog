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

#pragma once
#ifndef _ZLOG_H_
#define _ZLOG_H_

#include <string>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <cmath>
#include <stdlib.h>
#include <vector>
#include <map>
#include <list>
#include <queue>
#include <deque>

// !!!NOTE!!!
// We are not going to use 'Windows.h' here, see the remarks-3 on the above
//#if _MSC_VER >= 1400 // if using MSC and MSVC >= VS2005
//#	define WIN32_LEAN_AND_MEAN
//#	include <Windows.h>
//#undef min
//#undef max
//#endif

#if defined ZLOG_QT_VERSION // if using Qt
#   include <QtCore/qglobal.h>
#   if defined(ZLOG_LIBRARY)
#       define ZDLL Q_DECL_EXPORT
#   else
#       define ZDLL Q_DECL_IMPORT
#   endif
#elif defined WIN32
#   ifdef DLL_EXPORT
#       define ZDLL __declspec(dllexport)
#   else
#       define ZDLL __declspec(dllimport)
#   endif
#endif

// 'newline' using '\r\n' on Windows, or '\n' on Linux
#ifdef WIN32
#	define NEW_LINE_USING_CRLF
#endif

// !!!remarks!!!
// If using Visual Studio C++:
// format macro can not be used in VC6 or VC2003
// instead, you can use macros like: LOGI, LOGD, LOG_DEBUG, LOG_STREAM
#if _MSC_VER >= 1400 // if using MSC and MSVC >= VC2005
#	define ZLOG_FORMAT_INPUT_ENABLE
#endif

// !!!remarks!!!
// If not using Visual Studio C++:
// format macro can only be used in C99 and later version
#ifdef WIN32 // on Windows
#   if __STDC_VERSION__ >= 199901L && defined __GNUC__ // MinGW & C99+
#		define ZLOG_FORMAT_INPUT_ENABLE
#	endif
#else //  on *nix
#	if __STDC_VERSION__ >= 199901L //  C99+
#		define ZLOG_FORMAT_INPUT_ENABLE
#	endif
#endif

// supports ANSI->OEM console conversion on Windows
#undef ZLOG_OEM_CONSOLE

namespace zlog
{
    enum ENUM_LOG_LEVEL
    {
        LOG_LEVEL_TRACE = 0,
        LOG_LEVEL_DEBUG = 1,
        LOG_LEVEL_INFO = 2,
        LOG_LEVEL_WARN = 3,
        LOG_LEVEL_ERROR = 4,
        LOG_LEVEL_ALARM = 5,
        LOG_LEVEL_FATAL = 6,
        LOG_LEVEL_MIN__ = 0,
        LOG_LEVEL_MAX__ = 6
    };

    typedef int LoggerId;

    ////////////////////////////////////////////////////////////
    // below are constants, do NOT touch
    ////////////////////////////////////////////////////////////
    const LoggerId ZLOG_INVALID_LOGGER_ID = -1;
    const LoggerId ZLOG_MAIN_LOGGER_ID = 0;
    const char* const ZLOG_MAIN_LOGGER_KEY = "Main";
    // milisec to delay, suggested value is 10~100
    const unsigned int ZLOG_DELAY_MS = 10;
	// log data to be free later
	const int ZLOG_FREE_LOG_DATA_QUEUE_SIZE = 10;
	// max count of logger
	const int ZLOG_LOGGER_MAX_COUNT = 20;
	// max content length of the log
	const int ZLOG_LOG_BUFFER_SIZE = 8 * 1024;
	// max stl max container depth of the STL
	const int ZLOG_STL_CONTAINER_DEPTH = 5;
	// limit size of the log queue
	const int ZLOG_LOG_QUEUE_SIZE_LIMIT = 500;
	// write to file sync, may be slow
	const bool ZLOG_SYNC_WRITE_TO_FILE = false;
	// display to the console window sync
	const bool ZLOG_SYNC_DISPLAY_ON_CONSOLE = false;
	// reserved log file count
	const size_t ZLOG_LOG_FILE_RESERVE_COUNT = 7;

#ifdef NEW_LINE_USING_CRLF
    const char* const STR_NEW_LINE = "\r\n";
    const size_t LEN_NEW_LINE = 2;
#else
    const char* const STR_NEW_LINE = "\n";
    const size_t LEN_NEW_LINE = 1;
#endif

    ////////////////////////////////////////////////////////////
    // below are default logger configurations
    // can be changed in client code or in configuration file
    ////////////////////////////////////////////////////////////
    // default path of output logger file	
    const char* const ZLOG_LOG_FILE_PATH_DEFAULT = "./log/";
    // default log filter level
    const int ZLOG_FILTER_LOG_LEVEL_DEFAULT = LOG_LEVEL_DEBUG;
    // display on the console
	const bool ZLOG_IS_DISPLAY_ON_CONSOLE_DEFAULT = false;
    // write to file
    const bool ZLOG_IS_WRITE_TO_FILE_DEFAULT = true;
    // use month dir
    const bool ZLOG_IS_USING_MONTH_DIR_DEFAULT = false;
    // logger file size limit, unit:MB
    const int ZLOG_LOG_FILE_SIZE_LIMIT_DEFAULT = 100;
    // use fileline suffix
    const bool ZLOG_IS_USING_FILE_LINE_DEFAULT = true;
	// history logs
	const time_t ZLOG_LOG_FILE_RESERVE_SEC_DEFAULT = 0;

    struct LogData
    {
        LoggerId     _loggerId;
        int          _configType;
        int          _configValue;
        int          _filterLogLevel;
        time_t       _creationTime;
        unsigned int _precision;
        unsigned int _threadId;
        int          _contentLength;
        char         _content[1];
    };

    // We will use this "IZLogManager' in real programs
    class ZDLL IZLogManager
    {
    public:
        IZLogManager() {}
        virtual ~IZLogManager() {}

        // see the implementation in *.cpp file
        static IZLogManager* getInstance();

        inline static IZLogManager& getRef()
        {
            return *getInstance();
        }

        inline static IZLogManager* getPtr()
        {
            return getInstance();
        }

        // This 'configFromFile' can ONLY be called before IZLogManager::Start, or else do NOT call it.
        virtual bool configFromFile(const char* configFilePath) = 0;
        // similar to the above
        virtual bool configFromString(const char* configContent) = 0;
        // This 'createLogger' can ONLY be called before IZLogManager::Start, or else do NOT call it.
        virtual LoggerId createLogger(const char* key) = 0;
        // Start the logger thread, can ONLY be called ONCE by one process.
        virtual bool start(bool isGlobalSwitchOn = true) = 0;
        // Will be called automatically at exit
        virtual bool stop() = 0;
        // fing logger, thread-safe
        virtual LoggerId findLoggerByKey(const char* key) = 0;
        // pre-check the log filter. return false if filterd out.
        virtual bool prePushLogDataData(LoggerId loggerId, int level) = 0;
        // push log, thread-safe
        virtual bool pushLogData(LogData* pLogData, const char* file = NULL, int line = 0) = 0;
        // immediately if enabled, or queue up if not.
        virtual bool enableLogger(LoggerId loggerId, bool flag) = 0;
        virtual bool isLoggerEnabled(LoggerId loggerId) = 0;
        virtual bool setLoggerName(LoggerId loggerId, const char* name) = 0;
        virtual bool setLogFilePath(LoggerId loggerId, const char* path) = 0;
        virtual bool setFilterLogLevel(LoggerId loggerId, int level) = 0;
        virtual bool setIsUsingFileLine(LoggerId loggerId, bool flag) = 0;
        virtual bool setIsDisplayOnConsole(LoggerId loggerId, bool flag) = 0;
        virtual bool setIsWriteToFile(LoggerId loggerId, bool flag) = 0;
        virtual bool setLogFileSizeLimit(LoggerId loggerId, unsigned int size) = 0;
        virtual bool setIsUsingMonthDir(LoggerId loggerId, bool flag) = 0;
        virtual bool setLogFileReserveTimeSec(LoggerId loggerId, time_t sec) = 0;

        // Update logger's attribute from config file, thread safe.
        // unit:second, if set to 0, it means disable auto update
        virtual bool setAutoUpdateInterval(int interval) = 0;
        virtual bool updateConfig() = 0;

        // statistic
        virtual unsigned long long getTotalWriteFileCount() = 0;
        virtual unsigned long long getTotalWriteFileBytes() = 0;
        virtual unsigned long long getTotalPushLogDataCount() = 0;
        virtual unsigned long long getTotalPopLogDataCount() = 0;
        virtual unsigned int getTotalActiveLoggerCount() = 0;

        virtual LogData* makeLogData(LoggerId loggerId, int level) = 0;
        virtual void freeLogData(LogData* pLogData) = 0;
    };

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning(disable:4996)
#endif

    class ZDLL ZLogBinary
    {
    public:
        ZLogBinary(const void* buf, size_t len)
        {
            this->buf = (const char*)buf;
            this->len = len;
        }

        const char* buf;
        size_t  len;
    };

    class ZDLL ZLogString
    {
    public:
        ZLogString(const char* buf, size_t len)
        {
            this->buf = (const char*)buf;
            this->len = len;
        }

        const char* buf;
        size_t  len;
    };

    class ZDLL ZLogStream
    {
    public:
        inline ZLogStream(char* buf, int len)
        {
            _begin = buf;
            _end = buf + len;
            _cur = _begin;
        }

        inline int getCurrentLen()
        {
            return (int)(_cur - _begin);
        }

    public:
        inline ZLogStream& writeLongLong(long long t, int width = 0, int dec = 10)
        {
            if (t < 0)
            {
                t = -t;
                writeChar('-');
            }
            writeULongLong((unsigned long long)t, width, dec);
            return *this;
        }

        inline ZLogStream& writeULongLong(unsigned long long t, int width = 0, int dec = 10)
        {
            static const char* lut =
                "0123456789abcdef";

            static const char* lutDec =
                "00010203040506070809"
                "10111213141516171819"
                "20212223242526272829"
                "30313233343536373839"
                "40414243444546474849"
                "50515253545556575859"
                "60616263646566676869"
                "70717273747576777879"
                "80818283848586878889"
                "90919293949596979899";

            static const char* lutHex =
                "000102030405060708090A0B0C0D0E0F"
                "101112131415161718191A1B1C1D1E1F"
                "202122232425262728292A2B2C2D2E2F"
                "303132333435363738393A3B3C3D3E3F"
                "404142434445464748494A4B4C4D4E4F"
                "505152535455565758595A5B5C5D5E5F"
                "606162636465666768696A6B6C6D6E6F"
                "707172737475767778797A7B7C7D7E7F"
                "808182838485868788898A8B8C8D8E8F"
                "909192939495969798999A9B9C9D9E9F"
                "A0A1A2A3A4A5A6A7A8A9AAABACADAEAF"
                "B0B1B2B3B4B5B6B7B8B9BABBBCBDBEBF"
                "C0C1C2C3C4C5C6C7C8C9CACBCCCDCECF"
                "D0D1D2D3D4D5D6D7D8D9DADBDCDDDEDF"
                "E0E1E2E3E4E5E6E7E8E9EAEBECEDEEEF"
                "F0F1F2F3F4F5F6F7F8F9FAFBFCFDFEFF";

            const unsigned long long cacheSize = 64;

            if ((unsigned long long)(_end - _cur) > cacheSize)
            {
                char buf[cacheSize];
                unsigned long long val = t;
                unsigned long long i = cacheSize;
                unsigned long long digit = 0;

                if (dec == 10)
                {
                    do
                    {
                        const unsigned long long m2 = (unsigned long long)((val % 100) * 2);
                        *(buf + i - 1) = lutDec[m2 + 1];
                        *(buf + i - 2) = lutDec[m2];
                        i -= 2;
                        val /= 100;
                        digit += 2;
                    } while (val && i >= 2);

                    if (digit >= 2 && buf[cacheSize - digit] == '0')
                    {
                        --digit;
                    }
                }
                else if (dec == 16)
                {
                    do
                    {
                        const unsigned long long m2 = (unsigned long long)((val % 256) * 2);
                        *(buf + i - 1) = lutHex[m2 + 1];
                        *(buf + i - 2) = lutHex[m2];
                        i -= 2;
                        val /= 256;
                        digit += 2;
                    } while (val && i >= 2);

                    if (digit >= 2 && buf[cacheSize - digit] == '0')
                    {
                        --digit;
                    }
                }
                else
                {
                    do
                    {
                        buf[--i] = lut[val % dec];
                        val /= dec;
                        ++digit;
                    } while (val && i > 0);
                }

                while (digit < (unsigned long long)width)
                {
                    ++digit;
                    buf[cacheSize - digit] = '0';
                }

                writeString(buf + (cacheSize - digit), (size_t)digit);
            }
            return *this;
        }

        inline ZLogStream& writeDouble(double t, bool isSimple)
        {
            // if using c++11 (and later version) or using MinGW
			// !!!NOTE!!! see the remarks on the above
#if __cplusplus >= 201103L || defined __GNUC__
            using std::isnan;
            using std::isinf;
#endif
            if (isnan(t))
            {
                writeString("nan", 3);
                return *this;
            }
            else if (isinf(t))
            {
                writeString("inf", 3);
                return *this;
            }

            size_t count = _end - _cur;
            double T = fabs(t);
            if (count > 30)
            {
                if (T < 0.0001 || (!isSimple && T > 4503599627370495ULL) || (isSimple && T > 8388607))
                {
                    gcvt(t, isSimple ? 7 : 16, _cur);
                    size_t len = strlen(_cur);
                    if (len > count) len = count;
                    _cur += len;
                    return *this;
                }
                else
                {
                    if (t < 0.0)
                    {
                        writeChar('-');
                    }

                    double intPart = 0;
                    unsigned long long fractPart = (unsigned long long)(modf(T, &intPart) * 10000);
                    writeULongLong((unsigned long long)intPart);

                    if (fractPart > 0)
                    {
                        writeChar('.');
                        writeULongLong(fractPart, 4);
                    }
                }
            }

            return *this;
        }

        inline ZLogStream& writePointer(const void* t)
        {
            sizeof(t) == 8 ? writeULongLong((unsigned long long)t, 16, 16) : writeULongLong((unsigned long long)t, 8, 16);
            return *this;
        }

        inline ZLogStream& writeNewLine()
        {
            return writeString(STR_NEW_LINE);
        }

        inline ZLogStream& writeString(const char* t)
        {
            return writeString(t, strlen(t));
        }

        inline ZLogStream& writeString(const char* t, size_t len)
        {
            size_t count = _end - _cur;
            if (len > count)
            {
                len = count;
            }
            if (len > 0)
            {
                memcpy(_cur, t, len);
                _cur += len;
            }

            return *this;
        }

        inline ZLogStream& writeChar(char ch)
        {
            if (_end - _cur > 1)
            {
                _cur[0] = ch;
                ++_cur;
            }

            return *this;
        }

        inline ZLogStream& writeBinary(const ZLogBinary& t)
        {
			// 0~127
			static const int ASCII_TBL_LEN = 128;
			static const char* asciiTable =
				"                                "
				" !\"#$%&'()*+,-./0123456789:;<=>?"
				"@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
				"`abcdefghijklmnopqrstuvwxyz{|}~ ";

            writeNewLine();
            writeString("\t[");
            writeNewLine();
			size_t lines = (t.len + 31) / 32;
            for (size_t i = 1, k = 0; i <= lines; ++i, k += 32)
            {
                writeString("\t  ");
                *this << (void*)(t.buf + k);
                writeString(": ");
                for (size_t j = k; j < k + 32 && j < t.len; ++j)
                {
                    writeULongLong((unsigned long long)(unsigned char)t.buf[j], 2, 16);
                    writeChar(' ');
                }
                writeNewLine();

				if (sizeof((void*)0) == 8)
				{
					writeString("\t            (Text): ");
				}
				else
				{
					writeString("\t    (Text): ");
				}

				char sz[4] = { ' ',' ',' ','\0' };
                for (size_t j = k; j < k + 32 && j < t.len; ++j)
                {
					int uc = (unsigned char)(t.buf[j]);
					int r = (uc >= ASCII_TBL_LEN ? 0 : uc);
					sz[1] = asciiTable[r];
					writeString(sz, 3);
                    /*if (isprint((unsigned char)t.buf[j]))
                    {
                        writeChar(' ');
                        writeChar(t.buf[j]);
                        writeChar(' ');
                    }
                    else
                    {
                        *this << " . ";
                    }*/
                }
                writeNewLine();
                if (i < lines )
                {
                    writeNewLine();
                }
            }

            writeString("\t]");
            writeNewLine();
            writeString("\t");
            return *this;
        }

    public:
        inline ZLogStream& operator <<(const void* t)
        {
            return  writePointer(t);
        }

        inline ZLogStream& operator <<(const char* t)
        {
            return writeString(t);
        }

        inline ZLogStream& operator <<(bool t)
        {
            return (t ? writeString("true", 4) : writeString("false", 5));
        }

        inline ZLogStream& operator <<(char t)
        {
            return writeChar(t);
        }

        inline ZLogStream& operator <<(unsigned char t)
        {
            return writeULongLong(t);
        }

        inline ZLogStream& operator <<(short t)
        {
            return writeLongLong(t);
        }

        inline ZLogStream& operator <<(unsigned short t)
        {
            return writeULongLong(t);
        }

        inline ZLogStream& operator <<(int t)
        {
            return writeLongLong(t);
        }

        inline ZLogStream& operator <<(unsigned int t)
        {
            return writeULongLong(t);
        }

        inline ZLogStream& operator <<(long t)
        {
            return writeLongLong(t);
        }

        inline ZLogStream& operator <<(unsigned long t)
        {
            return writeULongLong(t);
        }

        inline ZLogStream& operator <<(long long t)
        {
            return writeLongLong(t);
        }

        inline ZLogStream& operator <<(unsigned long long t)
        {
            return writeULongLong(t);
        }

        inline ZLogStream& operator <<(float t)
        {
            return writeDouble(t, true);
        }

        inline ZLogStream& operator <<(double t)
        {
            return writeDouble(t, false);
        }

        /////////////////////////////////////////////////////
        //templates CANNOT be export directly

        template<class _Elem, class _Traits, class _Alloc> //support std::string, std::wstring
        inline ZLogStream& operator <<(const std::basic_string<_Elem, _Traits, _Alloc>& t)
        {
            return writeString(t.c_str(), t.length());
        }

        inline ZLogStream& operator << (const ZLogBinary& binary)
        {
            return writeBinary(binary);
        }

        inline ZLogStream& operator << (const ZLogString& str)
        {
            return writeString(str.buf, str.len);
        }

        template<class _Ty1, class _Ty2>
        inline ZLogStream& operator <<(const std::pair<_Ty1, _Ty2>& t)
        {
            return *this << "pair(" << t.first << ":" << t.second << ")";
        }

        template<class _Elem, class _Alloc>
        inline ZLogStream& operator <<(const std::vector<_Elem, _Alloc>& t)
        {
            *this << "vector(" << t.size() << ")[";
            int inputCount = 0;
            for (typename std::vector<_Elem, _Alloc>::const_iterator iter = t.begin(); iter != t.end(); ++iter)
            {
                ++inputCount;
                if (inputCount > ZLOG_STL_CONTAINER_DEPTH)
                {
                    *this << "..., ";
                    break;
                }
                *this << *iter << ", ";
            }
            if (!t.empty())
            {
                _cur -= 2;
            }
            return *this << "]";
        }

        template<class _Elem, class _Alloc>
        inline ZLogStream& operator <<(const std::list<_Elem, _Alloc>& t)
        {
            *this << "list(" << t.size() << ")[";
            int inputCount = 0;
            for (typename std::list<_Elem, _Alloc>::const_iterator iter = t.begin(); iter != t.end(); ++iter)
            {
                ++inputCount;
                if (inputCount > ZLOG_STL_CONTAINER_DEPTH)
                {
                    *this << "..., ";
                    break;
                }
                *this << *iter << ", ";
            }
            if (!t.empty())
            {
                _cur -= 2;
            }
            return *this << "]";
        }

        template<class _Elem, class _Alloc>
        inline ZLogStream& operator <<(const std::deque<_Elem, _Alloc>& t)
        {
            *this << "deque(" << t.size() << ")[";
            int inputCount = 0;
            for (typename std::deque<_Elem, _Alloc>::const_iterator iter = t.begin(); iter != t.end(); ++iter)
            {
                ++inputCount;
                if (inputCount > ZLOG_STL_CONTAINER_DEPTH)
                {
                    *this << "..., ";
                    break;
                }
                *this << *iter << ", ";
            }
            if (!t.empty())
            {
                _cur -= 2;
            }
            return *this << "]";
        }

        template<class _Elem, class _Alloc>
        inline ZLogStream& operator <<(const std::queue<_Elem, _Alloc>& t)
        {
            *this << "queue(" << t.size() << ")[";
            int inputCount = 0;
            for (typename std::queue<_Elem, _Alloc>::const_iterator iter = t.begin(); iter != t.end(); ++iter)
            {
                ++inputCount;
                if (inputCount > ZLOG_STL_CONTAINER_DEPTH)
                {
                    *this << "..., ";
                    break;
                }
                *this << *iter << ", ";
            }
            if (!t.empty())
            {
                _cur -= 2;
            }
            return *this << "]";
        }

        template<class _K, class _V, class _Pr, class _Alloc>
        inline ZLogStream& operator <<(const std::map<_K, _V, _Pr, _Alloc>& t)
        {
            *this << "map(" << t.size() << ")[";
            int inputCount = 0;
            for (typename std::map < _K, _V, _Pr, _Alloc>::const_iterator iter = t.begin(); iter != t.end(); ++iter)
            {
                ++inputCount;
                if (inputCount > ZLOG_STL_CONTAINER_DEPTH)
                {
                    *this << "..., ";
                    break;
                }
                *this << *iter << ", ";
            }
            if (!t.empty())
            {
                _cur -= 2;
            }
            return *this << "]";
        }

    private:
        ZLogStream() {}
        ZLogStream(ZLogStream&) {}
        char*  _begin;
        char*  _end;
        char*  _cur;
    };

#ifdef _MSC_VER
#	pragma warning(pop)
#endif
}

// LOG_STREAM
#define LOG_STREAM(id, level, file, line, log)\
do{\
    if (zlog::IZLogManager::getPtr()->prePushLogDataData(id,level)) \
    {\
        zlog::LogData* __pLogData = zlog::IZLogManager::getPtr()->makeLogData(id, level); \
        zlog::ZLogStream __ss(__pLogData->_content + __pLogData->_contentLength, zlog::ZLOG_LOG_BUFFER_SIZE - __pLogData->_contentLength);\
        __ss << log;\
        __pLogData->_contentLength += __ss.getCurrentLen(); \
        zlog::IZLogManager::getPtr()->pushLogData(__pLogData, file, line);\
    }\
} while (0)

// LOG_XXX
#define LOG_TRACE(id, log) LOG_STREAM(id, zlog::LOG_LEVEL_TRACE, __FILE__, __LINE__, log)
#define LOG_DEBUG(id, log) LOG_STREAM(id, zlog::LOG_LEVEL_DEBUG, __FILE__, __LINE__, log)
#define LOG_INFO(id, log)  LOG_STREAM(id, zlog::LOG_LEVEL_INFO, __FILE__, __LINE__, log)
#define LOG_WARN(id, log)  LOG_STREAM(id, zlog::LOG_LEVEL_WARN, __FILE__, __LINE__, log)
#define LOG_ERROR(id, log) LOG_STREAM(id, zlog::LOG_LEVEL_ERROR, __FILE__, __LINE__, log)
#define LOG_ALARM(id, log) LOG_STREAM(id, zlog::LOG_LEVEL_ALARM, __FILE__, __LINE__, log)
#define LOG_FATAL(id, log) LOG_STREAM(id, zlog::LOG_LEVEL_FATAL, __FILE__, __LINE__, log)

// LOGX
#define LOGT(log) LOG_TRACE(zlog::ZLOG_MAIN_LOGGER_ID, log)
#define LOGD(log) LOG_DEBUG(zlog::ZLOG_MAIN_LOGGER_ID, log)
#define LOGI(log) LOG_INFO(zlog::ZLOG_MAIN_LOGGER_ID, log)
#define LOGW(log) LOG_WARN(zlog::ZLOG_MAIN_LOGGER_ID, log)
#define LOGE(log) LOG_ERROR(zlog::ZLOG_MAIN_LOGGER_ID, log)
#define LOGA(log) LOG_ALARM(zlog::ZLOG_MAIN_LOGGER_ID, log)
#define LOGF(log) LOG_FATAL(zlog::ZLOG_MAIN_LOGGER_ID, log)

// FORMATTING
#ifdef ZLOG_FORMAT_INPUT_ENABLE
#ifdef WIN32
#define LOG_FORMAT(id, level, file, line, logFormat, ...) \
do{ \
    if (zlog::IZLogManager::getPtr()->prePushLogDataData(id,level)) \
    {\
        zlog::LogData* __pLogData = zlog::IZLogManager::getPtr()->makeLogData(id, level); \
        int __logDataLen = _snprintf_s(__pLogData->_content + __pLogData->_contentLength, zlog::ZLOG_LOG_BUFFER_SIZE - __pLogData->_contentLength, _TRUNCATE, logFormat, ##__VA_ARGS__); \
        if (__logDataLen < 0) __logDataLen = zlog::ZLOG_LOG_BUFFER_SIZE - __pLogData->_contentLength; \
        __pLogData->_contentLength += __logDataLen; \
        zlog::IZLogManager::getPtr()->pushLogData(__pLogData, file, line); \
    }\
} while (0)
#else // ifdef WIN32
#define LOG_FORMAT(id, level, file, line, logFormat, ...) \
do{ \
    if(zlog::IZLogManager::getPtr()->prePushLogDataData(id,level)) \
    {\
        zlog::LogData* __pLogData = zlog::IZLogManager::getPtr()->makeLogData(id, level); \
        int __logDataLen = snprintf(__pLogData->_content + __pLogData->_contentLength, zlog::ZLOG_LOG_BUFFER_SIZE - __pLogData->_contentLength,logFormat, ##__VA_ARGS__); \
        if (__logDataLen < 0) __logDataLen = 0; \
        if (__logDataLen > zlog::ZLOG_LOG_BUFFER_SIZE - __pLogData->_contentLength) __logDataLen = zlog::ZLOG_LOG_BUFFER_SIZE - __pLogData->_contentLength; \
        __pLogData->_contentLength += __logDataLen; \
        zlog::IZLogManager::getPtr()->pushLogData(__pLogData, file, line); \
    } \
}while(0)
#endif // ifdef WIN32
#define LOGFMT_TRACE(id, fmt, ...)  LOG_FORMAT(id, zlog::LOG_LEVEL_TRACE, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOGFMT_DEBUG(id, fmt, ...)  LOG_FORMAT(id, zlog::LOG_LEVEL_DEBUG, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOGFMT_INFO(id, fmt, ...)  LOG_FORMAT(id, zlog::LOG_LEVEL_INFO, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOGFMT_WARN(id, fmt, ...)  LOG_FORMAT(id, zlog::LOG_LEVEL_WARN, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOGFMT_ERROR(id, fmt, ...)  LOG_FORMAT(id, zlog::LOG_LEVEL_ERROR, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOGFMT_ALARM(id, fmt, ...)  LOG_FORMAT(id, zlog::LOG_LEVEL_ALARM, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOGFMT_FATAL(id, fmt, ...)  LOG_FORMAT(id, zlog::LOG_LEVEL_FATAL, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOGFMTT(fmt, ...) LOGFMT_TRACE(zlog::ZLOG_MAIN_LOGGER_ID, fmt,  ##__VA_ARGS__)
#define LOGFMTD(fmt, ...) LOGFMT_DEBUG(zlog::ZLOG_MAIN_LOGGER_ID, fmt,  ##__VA_ARGS__)
#define LOGFMTI(fmt, ...) LOGFMT_INFO(zlog::ZLOG_MAIN_LOGGER_ID, fmt,  ##__VA_ARGS__)
#define LOGFMTW(fmt, ...) LOGFMT_WARN(zlog::ZLOG_MAIN_LOGGER_ID, fmt,  ##__VA_ARGS__)
#define LOGFMTE(fmt, ...) LOGFMT_ERROR(zlog::ZLOG_MAIN_LOGGER_ID, fmt,  ##__VA_ARGS__)
#define LOGFMTA(fmt, ...) LOGFMT_ALARM(zlog::ZLOG_MAIN_LOGGER_ID, fmt,  ##__VA_ARGS__)
#define LOGFMTF(fmt, ...) LOGFMT_FATAL(zlog::ZLOG_MAIN_LOGGER_ID, fmt,  ##__VA_ARGS__)
#else // ifdef ZLOG_FORMAT_INPUT_ENABLE
#   ifndef UNUSED
#       define UNUSED(x) (void)x
#   endif
inline void empty_log_format_function1(zlog::LoggerId loggerId, const char*, ...) { UNUSED(loggerId); }
inline void empty_log_format_function2(const char*, ...) {}
#define LOGFMT_TRACE empty_log_format_function1
#define LOGFMT_DEBUG LOGFMT_TRACE
#define LOGFMT_INFO LOGFMT_TRACE
#define LOGFMT_WARN LOGFMT_TRACE
#define LOGFMT_ERROR LOGFMT_TRACE
#define LOGFMT_ALARM LOGFMT_TRACE
#define LOGFMT_FATAL LOGFMT_TRACE
#define LOGFMTT empty_log_format_function2
#define LOGFMTD LOGFMTT
#define LOGFMTI LOGFMTT
#define LOGFMTW LOGFMTT
#define LOGFMTE LOGFMTT
#define LOGFMTA LOGFMTT
#define LOGFMTF LOGFMTT
#endif // ifdef ZLOG_FORMAT_INPUT_ENABLE

#endif // ifndef _ZLOG_H_
