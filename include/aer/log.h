/**
 * @file
 */
#ifndef AER_LOG_H
#define AER_LOG_H



/**
 * @defgroup log Logging
 *
 * This module contains all logging related utilities.
 * 
 * @sa aer/log.h
 *
 * @{
 */

/**
 * Display an informational message to STDOUT.
 *
 * Use this function to output general information about mod state
 * that the user may find useful.
 * 
 * @param[in] moduleName display name of the mod
 * @param[in] fmt message string with standard C-style formatting
 * @param[in] ... optional format replacement arguments
 */
void AERLogInfo(const char * moduleName, const char * fmt, ...);

/**
 * Display a warning message to STDERR.
 *
 * It is the mod developer's decision in which circumstances to display
 * a warning vs. an error. The MRE displays warnings when it encounters
 * unexpected, (potentially) non-fatal state.
 * 
 * @param[in] moduleName display name of the mod
 * @param[in] fmt message string with standard C-style formatting
 * @param[in] ... optional format replacement arguments
 */
void AERLogWarn(const char * moduleName, const char * fmt, ...);

/**
 * Display an error message to STDERR.
 *
 * It is the mod developer's decision in which circumstances to display
 * an error vs. a warning. The MRE displays errors when it encounters
 * unexpected, fatal state before terminating the game process.
 * 
 * @param[in] moduleName display name of the mod
 * @param[in] fmt message string with standard C-style formatting
 * @param[in] ... optional format replacement arguments
 */
void AERLogErr(const char * moduleName, const char * fmt, ...);

/**
 * @}
 */



#endif /* AER_LOG_H */
