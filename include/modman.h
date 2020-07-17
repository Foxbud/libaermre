#ifndef MODMAN_H
#define MODMAN_H



/* ----- PUBLIC TYPES ----- */

typedef enum ModManErrCode {
	MOD_MAN_OK,
	MOD_MAN_NO_SUCH_MOD,
	MOD_MAN_NAME_NOT_FOUND,
	MOD_MAN_VERSION_NOT_FOUND,
	MOD_MAN_OUT_OF_MEM,
} ModManErrCode;

typedef struct AERMod {
	/* Technical. */
	void * libHandle;
	/* General. */
	const char * name;
	const char * version;
	/* Registration. */
	void (* registerSpritesCallback)(void);
	void (* registerObjectsCallback)(void);
	/* Event. */
	void (* roomStepCallback)(void);
	void (* roomChangeCallback)(int32_t, int32_t);
} AERMod;



/* ----- PUBLIC FUNCTIONS ----- */

ModManErrCode ModManLoad(const char * modLib, AERMod ** mod);

ModManErrCode ModManUnload(AERMod * mod);



#endif /* MODMAN_H */
