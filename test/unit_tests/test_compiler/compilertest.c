#include "opcode.h"
#include "yats.h"
#include "compilertest.h"
#include "binoptest.h"
#include "closuretest.h"
#include "unoptest.h"
#include "literaltest.h"
#include "iftest.h"
#include "whiletest.h"
#include "fortest.h"
#include "foreachtest.h"
#include "comprehensiontest.h"
#include "foldingtest.h"
#include "syntaxerrortest.h"

#define RUN(test) __YASL_TESTS_FAILED__ |= test()

SETUP_YATS();

// NOTE: these tests depend on the endianess of the system, so they may fail on big endian systems.

////////////////////////////////////////////////////////////////////////////////

int compilertest() {
	RUN(literaltest);
	RUN(unoptest);
	RUN(binoptest);
	RUN(closuretest);
	RUN(iftest);
	RUN(whiletest);
	RUN(fortest);
	RUN(foreachtest);
	RUN(comprehensiontest);
	RUN(foldingtest);
	RUN(syntaxerrortest);

	return __YASL_TESTS_FAILED__;
}
