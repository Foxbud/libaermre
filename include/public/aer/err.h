/**
 * @file
 *
 * @brief Global error state of the MRE.
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
#ifndef AER_ERR_H
#define AER_ERR_H

/* ----- PUBLIC TYPES ----- */

/**
 * @brief Possible values of ::aererr.
 *
 * @since 1.0.0
 */
typedef enum AERErrCode {
    /**
     * @brief Flag that informs the MRE that the next public function call is
     * being error-checked. Functions will never report this error.
     *
     * @since 1.3.0
     */
    AER_TRY = -1,
    /**
     * @brief Function did not report an error.
     *
     * @deprecated Partially since 1.3.0. This symbol, itself, is not
     * deprecated, but setting ::aererr to it when error-checking a function
     * *is* deprecated. Instead, use ::AER_TRY. Functions will still report this
     * if they did not encounter any errors; this functionality is *not*
     * deprecated.
     *
     * @since 1.0.0
     */
    AER_OK = 0,
    /**
     * @brief Function received `NULL` pointer argument.
     *
     * @since 1.0.0
     */
    AER_NULL_ARG = 1,
    /**
     * @brief Function called at incorrect stage of runtime execution.
     *
     * @since 1.0.0
     */
    AER_SEQ_BREAK = 2,
    /**
     * @brief Function unable to allocate necessary memory required for proper
     * execution.
     *
     * @deprecated Since 1.1.0.
     *
     * @since 1.0.0
     */
    AER_OUT_OF_MEM = 3,
    /**
     * @brief Function called with an invalid index, ID or key.
     *
     * @since 1.0.0
     */
    AER_FAILED_LOOKUP = 4,
    /**
     * @brief Function unable to parse resource.
     *
     * @since 1.0.0
     */
    AER_FAILED_PARSE = 5,
    /**
     * @brief Function unable to read file.
     *
     * @since 1.0.0
     */
    AER_BAD_FILE = 6,
    /**
     * @brief Function encountered an invalid value or combination of values.
     *
     * @since 1.0.0
     */
    AER_BAD_VAL = 7
} AERErrCode;

/* ----- PUBLIC GLOBALS ----- */

/**
 * @brief Error state of most recently called MRE function.
 *
 * @note Always reset this global to ::AER_TRY before calling the
 * function to be error-checked.
 *
 * @since 1.0.0
 */
extern AERErrCode aererr;

#endif /* AER_ERR_H */
