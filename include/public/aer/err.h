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
   * @brief Function did not report an error.
   */
  AER_OK,
  /**
   * @brief Function received `NULL` pointer argument.
   */
  AER_NULL_ARG,
  /**
   * @brief Function called at incorrect stage of runtime execution.
   */
  AER_SEQ_BREAK,
  /**
   * @brief Function unable to allocate necessary memory required for proper
   * execution.
   */
  AER_OUT_OF_MEM,
  /**
   * @brief Function called with an invalid index, ID or key.
   */
  AER_FAILED_LOOKUP,
  /**
   * @brief Function unable to parse resource.
   */
  AER_FAILED_PARSE,
  /**
   * @brief Function unable to read file.
   */
  AER_BAD_FILE,
  /**
   * @brief Function encountered an invalid value or combination of values.
   */
  AER_BAD_VAL
} AERErrCode;

/* ----- PUBLIC GLOBALS ----- */

/**
 * @brief Error state of most recently called MRE function.
 *
 * @note Always reset this global to ::AER_OK before calling the
 * function to be error-checked.
 *
 * @since 1.0.0
 */
extern AERErrCode aererr;

#endif /* AER_ERR_H */
