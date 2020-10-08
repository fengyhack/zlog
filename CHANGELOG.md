**2018-01-24** `VERSION 0.15.0`

- add implementation for 'sleepMicrosecond' on Windows

**2018-01-20** `VERSION 0.14.0`

- add a `global-switch` for zlog, default value is *true*

**2018-01-019** `VERSION 0.13.0`

- change some default settings, especially, `is_write_to_console`

**2018.01.09** `VERSION 0.12.0`

- add a lookup table (ASCII 0~127) for `ZLogBinary`

- `threadId` is now print as a 6-digit-number

**2018.01.05** `VERSION 0.11.0`

- add support for LOGFMT

**2017.12.19** `VERSION 0.10.1`

- misc improvement: ZLogBinary hex format

- add a new feature to LogManager: `dumpbin`

**2017.12.13** `VERSION 0.10.0`

- add support for Qt-MinGW (win32)

- fixed some misc problems like: '>' brackets, GNU_CXX's isnan/isinf

- add an example of LogManager

**2017.12.12** `VERSION 0.9.0`

- misc fixture of 'newline' (CRLF on Windows, LF on *nix)

**2017.12.11** `VERSION 0.8.0`

- add a zlog-lib project for Qt5.8 with MSVS2015

- fixed a problem of DLL_EXPORT definition macro in Qt5.8 with MSVS2015


**2017.12.10** `VERSION 0.7.0`

- passed tests for Qt5.8 with MSVS2015 and Qt5.9.3 on Ubuntu1710

**2017.12.07** `VERSION 0.6.0`

- add multi-thread example code for this project

**2017.12.06** `VERSION 0.5.0`

- add support for Ubuntu SO (shared-library) export

**2017.12.05** `VERSION 0.4.0`

- add support for WIN32/WIN64 DLL export

**2017.12.04** `VERSION 0.3.0`

- modified the examples: test-simple, test-advanced, test-stress

**2017.12.02** `VERSION 0.2.0`

- fixed some miscellaneous problems

**2017.11.30** `VERSION 0.1.0`

- migrate the project (with lots of modification) from `log4z`
