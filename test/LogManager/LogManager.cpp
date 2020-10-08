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

#include "LogManager.h"

namespace zlog
{
    // shared variables of this class
    bool LogManager::configIsDone = false;
    bool LogManager::loggerStarted = false;

    LogManager::LogManager(const char* name, bool globalSwitchOn)
    {
		// Preparing: logger configurations
		if (!LogManager::configIsDone)
		{
			zlog::IZLogManager::getRef().configFromFile("./LoggerConfig.ini");
			zlog::IZLogManager::getRef().updateConfig();
			LogManager::configIsDone = true;
		}

		// start the logger
		if (!LogManager::loggerStarted)
		{
			// 'Disable' the 'main logger'
			zlog::IZLogManager::getRef().setIsWriteToFile(zlog::ZLOG_MAIN_LOGGER_ID, false);
			zlog::IZLogManager::getRef().setIsDisplayOnConsole(zlog::ZLOG_MAIN_LOGGER_ID, false);

			zlog::IZLogManager::getRef().start(globalSwitchOn);
			LogManager::loggerStarted = true;
		}

        // create the logger
        zlog::LoggerId id = zlog::IZLogManager::getRef().createLogger(name);

        if(id == zlog::ZLOG_INVALID_LOGGER_ID)
        {
            m_loggerId = -1;
        }
        else
        {
            m_loggerId = (int)id;            
        }
    }
}

// EOF
