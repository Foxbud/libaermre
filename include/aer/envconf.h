/**
 * @file
 */
#ifndef AER_ENVCONF_H
#define AER_ENVCONF_H

#include <stddef.h>



/* ----- PUBLIC TYPES ----- */

typedef enum AEREnvConfErrCode {
	AER_ENVCONF_OK,
	AER_ENVCONF_NULL_ARG,
	AER_ENVCONF_NO_SUCH_VAR,
	AER_ENVCONF_COULD_NOT_PARSE
} AEREnvConfErrCode;



/* ----- PUBLIC FUNCTIONS ----- */

AEREnvConfErrCode AEREnvConfString(
		const char * name,
		const char ** str
);

AEREnvConfErrCode AEREnvConfStringList(
		const char * name,
		size_t bufSize,
		const char ** strBuf,
		size_t * numStrs
);



#endif /* AER_ENVCONF_H */
