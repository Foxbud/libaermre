/**
 * @copyright 2021 the libaermre authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <time.h>

#include "aer/log.h"
#include "internal/err.h"
#include "internal/export.h"
#include "internal/log.h"
#include "internal/mod.h"

/* ----- PRIVATE MACROS ----- */

#define BuildMsgFromVA(fmt, lastArg)                                 \
    ({                                                               \
        va_list BuildMsgFromVA_va;                                   \
        va_start(BuildMsgFromVA_va, (lastArg));                      \
        vsnprintf(msgBuf, sizeof(msgBuf), (fmt), BuildMsgFromVA_va); \
        va_end(BuildMsgFromVA_va);                                   \
        msgBuf;                                                      \
    })

#define GetCurrentModName() \
    ((ModManHasContext()) ? ModManGetMod(ModManPeekContext())->name : "?")

/* ----- PRIVATE TYPES ----- */

typedef enum LogLevel { LOG_INFO, LOG_WARN, LOG_ERR } LogLevel;

/* ----- PRIVATE CONSTANTS ----- */

static const char* MSG_FMT = "[%s][aer][%s] (%s) %s\n";

static const char* LVL_STRS[3] = {"INFO", "WARNING", "ERROR"};

static const char* INTERNAL_MOD_NAME = "mre";

/* ----- PRIVATE GLOBALS ----- */

static char msgBuf[8 * 1024];

/* ----- PRIVATE FUNCTIONS ----- */

static void FmtCurTime(char buf[9]) {
    assert(buf != NULL);

    time_t rawtime;
    time(&rawtime);
    struct tm* timeinfo = localtime(&rawtime);
    strftime(buf, 9, "%H:%M:%S", timeinfo);

    return;
}

static void Log(FILE* fp,
                LogLevel logLvl,
                const char* moduleName,
                const char* msg) {
    assert(fp != NULL);
    assert(logLvl <= LOG_ERR);
    assert(moduleName != NULL);
    assert(msg != NULL);

    /* Get current time. */
    char timeBuf[9];
    FmtCurTime(timeBuf);

    /* Print formatted message. */
    fprintf(fp, MSG_FMT, timeBuf, moduleName, LVL_STRS[logLvl], msg);

    return;
}

/* ----- INTERNAL FUNCTIONS ----- */

void LogInfo(const char* fmt, ...) {
    assert(fmt);

    /* Call common log function. */
    Log(stdout, LOG_INFO, INTERNAL_MOD_NAME, BuildMsgFromVA(fmt, fmt));

    return;
}

void LogWarn(const char* fmt, ...) {
    assert(fmt);

    /* Call common log function. */
    Log(stderr, LOG_WARN, INTERNAL_MOD_NAME, BuildMsgFromVA(fmt, fmt));

    return;
}

void LogErr(const char* fmt, ...) {
    assert(fmt);

    /* Call common log function. */
    Log(stderr, LOG_ERR, INTERNAL_MOD_NAME, BuildMsgFromVA(fmt, fmt));

    return;
}

/* ----- PUBLIC FUNCTIONS ----- */

AER_EXPORT void AERLogInfo(const char* fmt, ...) {
#define errRet
    EnsureArg(fmt);

    /* Call common log function. */
    Log(stdout, LOG_INFO, GetCurrentModName(), BuildMsgFromVA(fmt, fmt));

    Ok();
#undef errRet
}

AER_EXPORT void AERLogWarn(const char* fmt, ...) {
#define errRet
    EnsureArg(fmt);

    /* Call common log function. */
    Log(stdout, LOG_WARN, GetCurrentModName(), BuildMsgFromVA(fmt, fmt));

    Ok();
#undef errRet
}

AER_EXPORT void AERLogErr(const char* fmt, ...) {
#define errRet
    EnsureArg(fmt);

    /* Call common log function. */
    Log(stdout, LOG_ERR, GetCurrentModName(), BuildMsgFromVA(fmt, fmt));

    Ok();
#undef errRet
}
