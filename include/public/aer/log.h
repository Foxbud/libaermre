/**
 * @file
 *
 * @brief Logging utilities.
 *
 * @since 1.0.0
 *
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
#ifndef AER_LOG_H
#define AER_LOG_H

/* ----- PUBLIC FUNCTIONS ----- */

/**
 * @brief Log an informational message to the console.
 *
 * @param[in] fmt `printf` style format string.
 * @param[in] ... Variadic arguments to substitute into format string.
 *
 * @throw ::AER_NULL_ARG if argument `fmt` is `NULL`.
 *
 * @since 1.0.0
 */
void AERLogInfo(const char* fmt, ...);

/**
 * @brief Log a warning message to the console.
 *
 * @param[in] fmt `printf` style format string.
 * @param[in] ... Variadic arguments to substitute into format string.
 *
 * @throw ::AER_NULL_ARG if argument `fmt` is `NULL`.
 *
 * @since 1.0.0
 */
void AERLogWarn(const char* fmt, ...);

/**
 * @brief Log an error message to the console.
 *
 * @note A call to this function should be proceeded by a call to `abort`.
 *
 * @param[in] fmt `printf` style format string.
 * @param[in] ... Variadic arguments to substitute into format string.
 *
 * @throw ::AER_NULL_ARG if argument `fmt` is `NULL`.
 *
 * @since 1.0.0
 */
void AERLogErr(const char* fmt, ...);

#endif /* AER_LOG_H */
