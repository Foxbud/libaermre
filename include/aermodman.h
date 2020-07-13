#ifndef AERMODMAN_H
#define AERMODMAN_H



/* ----- PUBLIC TYPES ----- */

typedef enum AERModManErrCode {
	AER_MOD_MAN_OK,
	AER_MOD_MAN_NO_SUCH_MOD,
	AER_MOD_MAN_NAME_NOT_FOUND,
	AER_MOD_MAN_VERSION_NOT_FOUND,
	AER_MOD_MAN_OUT_OF_MEM,
} AERModManErrCode;

typedef struct AERMod {
	/* Technical. */
	void * libHandle;
	/* General. */
	const char * name;
	const char * version;
	/* Registration. */
	void (* registerSpritesCallback)(void);
	/* Event. */
	void (* roomStepCallback)(void);
	void (* roomChangeCallback)(int32_t, int32_t);
} AERMod;



/* ----- PUBLIC FUNCTIONS ----- */

AERModManErrCode AERModManLoad(const char * modLib, AERMod ** mod);

AERModManErrCode AERModManUnload(AERMod * mod);



#endif /* AERMODMAN_H */
