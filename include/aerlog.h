/**
 * @file
 *
 * @brief This module contains logging related functions.
 *
 * While not required, including this header file and using its functions
 * can make it easier for users to debug a mod.
 */
#ifndef AERLOG_H
#define AERLOG_H



/* ----- PUBLIC FUNCTIONS ----- */

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



#endif /* AERLOG_H */
