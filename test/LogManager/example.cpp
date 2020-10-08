#include "LogManager.h"

// declaration
class Foo
{
public:
	Foo();
	void Bar();
private:
	zlog::LogManager m_fooLogger;
};

// implementation
Foo::Foo()
	: m_fooLogger("Foooo")
{
	//do some initialization here
}

void Foo::Bar()
{
	int   a = 100;
	char* s = "OK";
	ZZ_LOGFMT_DEBUG(m_fooLogger, "a = %d, s= %s", a, s);
}