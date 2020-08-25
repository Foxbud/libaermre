/**
 * @file
 */
#ifndef AER_LOG_H
#define AER_LOG_H



/* ----- PUBLIC FUNCTIONS ----- */

void AERLogInfo(const char * moduleName, const char * fmt, ...);

void AERLogWarn(const char * moduleName, const char * fmt, ...);

void AERLogErr(const char * moduleName, const char * fmt, ...);



#endif /* AER_LOG_H */
