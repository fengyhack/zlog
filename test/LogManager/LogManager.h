/************************************************************
 *
 * New feature: config from file
 * Last update by fengyhack 2017-12-14
 *
 * New feature: dumpbin
 * Last update by fengyhack 2017-12-18
 *
 * New feature: logfmt
 * Last update by fengyhack 2018-01-12
 * 
 ************************************************************/

#ifndef ZZ_LOG_MANAGER_H
#define ZZ_LOG_MANAGER_H

#include "zlog.h"

#define LOG_MODEL_LIBRARY

#if defined(LOG_MODEL_LIBRARY)
#  define LOGMODEL_SHARED __declspec(dllexport)
#else
#  define LOGMODEL_SHARED __declspec(dllimport)
#endif

#ifndef ZZ_LOG_MACROS
#define ZZ_LOG_MACROS

#define ZZ_LOG_DEBUG(logger,x) LOG_STREAM(logger.GetLoggerId(),zlog::LOG_LEVEL_DEBUG,__FILE__,__LINE__,x)
#define ZZ_LOG_INFO(logger,x) LOG_STREAM(logger.GetLoggerId(),zlog::LOG_LEVEL_INFO,__FILE__, __LINE__,x)
#define ZZ_LOG_WARN(logger,x) LOG_STREAM(logger.GetLoggerId(),zlog::LOG_LEVEL_WARN,__FILE__, __LINE__,x)
#define ZZ_LOG_ERROR(logger,x) LOG_STREAM(logger.GetLoggerId(),zlog::LOG_LEVEL_ERROR,__FILE__, __LINE__,x)

#define ZZ_LOGFMT_DEBUG(logger,fmt,...) LOG_FORMAT(logger.GetLoggerId(), zlog::LOG_LEVEL_DEBUG, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define ZZ_LOGFMT_INFO(logger,fmt,...) LOG_FORMAT(logger.GetLoggerId(), zlog::LOG_LEVEL_INFO, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define ZZ_LOGFMT_WARN(logger,fmt,...) LOG_FORMAT(logger.GetLoggerId(), zlog::LOG_LEVEL_WARN, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define ZZ_LOGFMT_ERROR(logger,fmt,...) LOG_FORMAT(logger.GetLoggerId(), zlog::LOG_LEVEL_ERROR, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#define ZZ_DUMP_INFO(logger,opaque,len) \
    LOG_STREAM(logger.GetLoggerId(),zlog::LOG_LEVEL_INFO,__FILE__, __LINE__,zlog::ZLogBinary(opaque,len))

#define ZZ_DUMP_DEBUG(logger,opaque,len) \
    LOG_STREAM(logger.GetLoggerId(),zlog::LOG_LEVEL_DEBUG,__FILE__, __LINE__,zlog::ZLogBinary(opaque,len))

#endif

namespace zlog
{
    class LOGMODEL_SHARED LogManager
    {
    public:
        LogManager(const char* name, bool globalSwitchOn = true);

        int GetLoggerId() const
        {
            return m_loggerId;
        }

    private:
        // This is the ID for the logger itself
        int m_loggerId;

        // Config from file, only once before start
        static bool configIsDone;
        static bool loggerStarted;
    };
}

#endif // ZZ_LOG_MANAGER_H
// EOF
