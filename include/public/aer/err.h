/**
 * @file
 *
 * @brief Global error state of the MRE.
 *
 * @since 1.0.0
 *
 * @copyright 2020 the libaermre authors
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
	AER_OK, /**< Function did not report an error. */
	AER_NULL_ARG, /**< Function received `NULL` pointer argument. */
	AER_SEQ_BREAK, /**< Function called at incorrect stage of runtime
									 execution. */
	AER_OUT_OF_MEM, /**< Function unable to allocate necessary memory required
										for proper execution. */
	AER_FAILED_LOOKUP, /**< Function called with an invalid index, ID or key. */
	AER_FAILED_PARSE, /**< Function unable to parse resource. */
	AER_BAD_FILE, /**< Function unable to read file. */
	AER_BAD_VAL /**< Function encountered an invalid value or combination
								of values. */
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
