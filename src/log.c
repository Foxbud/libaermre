#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <time.h>

#include "aer/log.h"



/* ----- PRIVATE TYPES ----- */

typedef enum AERLogLvl {
	AER_LOG_INFO,
	AER_LOG_WARN,
	AER_LOG_ERR
} AERLogLvl;



/* ----- PRIVATE CONSTANTS ----- */

static const char * MSG_FMT = "[%s][AER][%s] %s %s\n";

static const char * LVL_STRS[3] = {
	"INFO",
	"WARNING",
	"ERROR"
};

static const size_t MSG_BUF_SIZE = 1024;



/* ----- PRIVATE GLOBALS ----- */

static char msgBuf[1024];



/* ----- PRIVATE FUNCTIONS ----- */

static void LogFmtCurTime(char buf[9]) {
	assert(buf != NULL);

	time_t rawtime;
	time(&rawtime);
	struct tm * timeinfo = localtime(&rawtime);
	strftime(buf, 9, "%H:%M:%S", timeinfo);

	return;
}

static void Log(
		FILE * fp,
		AERLogLvl logLvl,
		const char * moduleName,
		const char * msg
) {
	assert(fp != NULL);
	assert(logLvl <= AER_LOG_ERR);
	assert(moduleName != NULL);
	assert(msg != NULL);

	/* Get current time. */
	char timeBuf[9];
	LogFmtCurTime(timeBuf);

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



/* ----- PUBLIC FUNCTIONS ----- */

void AERLogInfo(const char * moduleName, const char * fmt, ...) {
	assert(moduleName != NULL);
	assert(fmt != NULL);

	/* Construct message string. */
	va_list va;
	va_start(va, fmt);
	vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, va);
	va_end(va);

	/* Call common log function. */
	Log(stdout, AER_LOG_INFO, moduleName, msgBuf);

	return;
}

void AERLogWarn(const char * moduleName, const char * fmt, ...) {
	assert(moduleName != NULL);
	assert(fmt != NULL);

	/* Construct message string. */
	va_list va;
	va_start(va, fmt);
	vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, va);
	va_end(va);

	/* Call common log function. */
	Log(stderr, AER_LOG_WARN, moduleName, msgBuf);

	return;
}

void AERLogErr(const char * moduleName, const char * fmt, ...) {
	assert(moduleName != NULL);
	assert(fmt != NULL);

	/* Construct message string. */
	va_list va;
	va_start(va, fmt);
	vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, va);
	va_end(va);

	/* Call common log function. */
	Log(stderr, AER_LOG_ERR, moduleName, msgBuf);

	return;
}
