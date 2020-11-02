/**
 * @file
 *
 * @brief Logging utilities.
 *
 * @since 1.0.0
 */
#ifndef AER_LOG_H
#define AER_LOG_H



/* ----- PUBLIC FUNCTIONS ----- */

/**
 * @brief Log an informational message to the console.
 *
 * @param[in] fmt `printf` style format string.
 * @param[in] ... Variadic arguments to substitute into format string.
 *
 * @since 1.0.0
 */
void AERLogInfo(const char * fmt, ...);

/**
 * @brief Log a warning message to the console.
 *
 * @param[in] fmt `printf` style format string.
 * @param[in] ... Variadic arguments to substitute into format string.
 *
 * @since 1.0.0
 */
void AERLogWarn(const char * fmt, ...);

/**
 * @brief Log an error message to the console.
 *
 * @note A call to this function should be proceeded by a call to `abort`.
 *
 * @param[in] fmt `printf` style format string.
 * @param[in] ... Variadic arguments to substitute into format string.
 *
 * @since 1.0.0
 */
void AERLogErr(const char * fmt, ...);



#endif /* AER_LOG_H */
