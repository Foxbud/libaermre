/**
 * @file
 */
#ifndef AER_ERR_H
#define AER_ERR_H



/* ----- PUBLIC TYPES ----- */

typedef enum AERErrCode {
	AER_OK,
	AER_NULL_ARG,
	AER_SEQ_BREAK,
	AER_OUT_OF_MEM,
	AER_FAILED_LOOKUP,
	AER_FAILED_PARSE,
	AER_BAD_FILE
} AERErrCode;



/* ----- PUBLIC GLOBALS ----- */

extern AERErrCode aererr;



#endif /* AER_ERR_H */
