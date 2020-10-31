/**
 * @file
 *
 * @brief Logging utilities.
 */
#ifndef AER_LOG_H
#define AER_LOG_H



/* ----- PUBLIC FUNCTIONS ----- */

/**
 * @brief Log an informational message to the console.
 *
 * @param[in] fmt `printf` style format string.
 * @param[in] ... Variadic arguments to substitute into format string.
 */
void AERLogInfo(const char * fmt, ...);

/**
 * @brief Log a warning message to the console.
 *
 * @param[in] fmt `printf` style format string.
 * @param[in] ... Variadic arguments to substitute into format string.
 */
void AERLogWarn(const char * fmt, ...);

/**
 * @brief Log an error message to the console.
 *
 * @note A call to this function should be proceeded by a call to `abort`.
 *
 * @param[in] fmt `printf` style format string.
 * @param[in] ... Variadic arguments to substitute into format string.
 */
void AERLogErr(const char * fmt, ...);



#endif /* AER_LOG_H */
