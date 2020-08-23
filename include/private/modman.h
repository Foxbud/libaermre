#ifndef PRIVATE_MODMAN_H
#define PRIVATE_MODMAN_H



/* ----- PUBLIC TYPES ----- */

typedef enum ModManErrCode {
	MOD_MAN_OK,
	MOD_MAN_NO_SUCH_MOD,
	MOD_MAN_NAME_NOT_FOUND,
	MOD_MAN_VERSION_NOT_FOUND
} ModManErrCode;

typedef void (* RoomStepListener)(void);

typedef void (* RoomChangeListener)(int32_t, int32_t);

typedef struct AERMod {
	/* Technical. */
	void * libHandle;
	/* General. */
	const char * name;
	const char * version;
	/* Registration. */
	void (* registerSprites)(void);
	void (* registerObjects)(void);
	void (* registerListeners)(void);
	/* Event. */
	RoomStepListener roomStepListener;
	RoomChangeListener roomChangeListener;
} AERMod;



/* ----- PUBLIC FUNCTIONS ----- */

ModManErrCode ModManLoad(const char * modLib, AERMod * mod);

ModManErrCode ModManUnload(AERMod * mod);



#endif /* PRIVATE_MODMAN_H */
