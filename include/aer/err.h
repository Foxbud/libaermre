/**
 * @file
 *
 * @brief Global error state of the MRE.
 */
#ifndef AER_ERR_H
#define AER_ERR_H



/* ----- PUBLIC TYPES ----- */

/**
 * @brief Possible values of ::aererr.
 */
typedef enum AERErrCode {
	AER_OK, /**< Function did not report an error. */
	AER_NULL_ARG, /**< Function received `NULL` pointer argument. */
	AER_SEQ_BREAK, /**< Function called at incorrect stage of runtime
									 execution. */
	AER_OUT_OF_MEM, /**< Function unable to allocate necessary memory required
										for proper execution. */
	AER_FAILED_LOOKUP, /**< Function called with invalid index, ID or key. */
	AER_FAILED_PARSE, /**< Function unable to parse resource. */
	AER_BAD_FILE /**< Function unable to read file. */
} AERErrCode;



/* ----- PUBLIC GLOBALS ----- */

/**
 * @brief Error state of most recently called MRE function.
 *
 * @note Always reset this global to ::AER_OK before calling the
 * function to be error-checked.
 */
extern AERErrCode aererr;



#endif /* AER_ERR_H */
