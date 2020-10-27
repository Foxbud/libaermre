#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <time.h>

#include "foxutils/arraymacs.h"

#include "aer/log.h"
#include "internal/log.h"
#include "internal/modman.h"



/* ----- PRIVATE MACROS ----- */

#define FillMsgBufFromVA(fmt, lastArg) \
	do { \
		va_list FillMsgBufFromVA_va; \
		va_start(FillMsgBufFromVA_va, lastArg); \
		vsnprintf(msgBuf, MSG_BUF_SIZE, (fmt), FillMsgBufFromVA_va); \
		va_end(FillMsgBufFromVA_va); \
	} while (0)

#define GetCurrentModName() \
	( \
		(FoxArrayMEmpty(Mod *, &modman.context)) ? \
		"?" \
		: (*FoxArrayMPeek(Mod *, &modman.context))->name \
	)



/* ----- PRIVATE TYPES ----- */

typedef enum LogLevel {
	LOG_INFO,
	LOG_WARN,
	LOG_ERR
} LogLevel;



/* ----- PRIVATE CONSTANTS ----- */

static const char * MSG_FMT = "[%s][AER][%s] (%s) %s\n";

static const char * LVL_STRS[3] = {
	"INFO",
	"WARNING",
	"ERROR"
};

static const size_t MSG_BUF_SIZE = 1024;

static const char * INTERNAL_MOD_NAME = "MRE";



/* ----- PRIVATE GLOBALS ----- */

static char msgBuf[1024];



/* ----- PRIVATE FUNCTIONS ----- */

static void FmtCurTime(char buf[9]) {
	assert(buf != NULL);

	time_t rawtime;
	time(&rawtime);
	struct tm * timeinfo = localtime(&rawtime);
	strftime(buf, 9, "%H:%M:%S", timeinfo);

	return;
}

static void Log(
		FILE * fp,
		LogLevel logLvl,
		const char * moduleName,
		const char * msg
) {
	assert(fp != NULL);
	assert(logLvl <= LOG_ERR);
	assert(moduleName != NULL);
	assert(msg != NULL);

	/* Get current time. */
	char timeBuf[9];
	FmtCurTime(timeBuf);

	/* Print formatted message. */
	fprintf(
			fp,
			MSG_FMT,
			timeBuf,
			moduleName,
			LVL_STRS[logLvl],
			msg
	);

	return;
}



/* ----- INTERNAL FUNCTIONS ----- */

void LogInfo(const char * fmt, ...) {
	assert(fmt);

	/* Construct message string. */
	FillMsgBufFromVA(fmt, fmt);

	/* Call common log function. */
	Log(stdout, LOG_INFO, INTERNAL_MOD_NAME, msgBuf);

	return;
}

void LogWarn(const char * fmt, ...) {
	assert(fmt);

	/* Construct message string. */
	FillMsgBufFromVA(fmt, fmt);

	/* Call common log function. */
	Log(stderr, LOG_WARN, INTERNAL_MOD_NAME, msgBuf);

	return;
}

void LogErr(const char * fmt, ...) {
	assert(fmt);

	/* Construct message string. */
	FillMsgBufFromVA(fmt, fmt);

	/* Call common log function. */
	Log(stderr, LOG_ERR, INTERNAL_MOD_NAME, msgBuf);

	return;
}



/* ----- PUBLIC FUNCTIONS ----- */

void AERLogInfo(const char * fmt, ...) {
	assert(fmt);

	/* Construct message string. */
	FillMsgBufFromVA(fmt, fmt);

	/* Call common log function. */
	Log(
			stdout,
			LOG_INFO,
			GetCurrentModName(),
			msgBuf
	);

	return;
}

void AERLogWarn(const char * fmt, ...) {
	assert(fmt);

	/* Construct message string. */
	FillMsgBufFromVA(fmt, fmt);

	/* Call common log function. */
	Log(
			stdout,
			LOG_WARN,
			GetCurrentModName(),
			msgBuf
	);

	return;
}

void AERLogErr(const char * fmt, ...) {
	assert(fmt);

	/* Construct message string. */
	FillMsgBufFromVA(fmt, fmt);

	/* Call common log function. */
	Log(
			stdout,
			LOG_ERR,
			GetCurrentModName(),
			msgBuf
	);

	return;
}
