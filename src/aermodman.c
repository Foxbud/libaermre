#include <assert.h>
#include <dlfcn.h>
#include <stdlib.h>
#include "aermodman.h"



/* ----- PUBLIC FUNCTIONS ----- */

AERModManErrCode AERModManLoad(const char * modLib, AERMod ** mod) {
	assert(modLib != NULL);
	assert(mod != NULL);

	/* Open library. */
	void * tmpHandle = dlopen(modLib, RTLD_NOW);
	if (!tmpHandle) {
		return AER_MOD_MAN_NO_SUCH_MOD;
	}

	/* Allocate memory for library. */
	AERMod * tmpMod = malloc(sizeof(AERMod));
	if (!tmpMod) {
		return AER_MOD_MAN_OUT_OF_MEM;
	}
	tmpMod->libHandle = tmpHandle;

	/* Read MOD_NAME symbol. */
	const char ** tmpNameHandle = dlsym(tmpHandle, "MOD_NAME");
	const char * tmpName;
	if (!tmpNameHandle || !(tmpName = *tmpNameHandle)) {
		free(tmpMod);
		tmpMod = NULL;
		return AER_MOD_MAN_NAME_NOT_FOUND;
	}
	tmpMod->name = tmpName;

	/* Read MOD_VERSION symbol. */
	const char ** tmpVersionHandle = dlsym(tmpHandle, "MOD_VERSION");
	const char * tmpVersion;
	if (!tmpVersionHandle || !(tmpVersion = *tmpVersionHandle)) {
		free(tmpMod);
		tmpMod = NULL;
		return AER_MOD_MAN_VERSION_NOT_FOUND;
	}
	tmpMod->version = tmpVersion;

	/* Read REGISTER_SPRITES_CALLBACK symbol. */
	void (** tmpRegSpritesHandle)(void) = dlsym(
			tmpHandle,
			"REGISTER_SPRITES_CALLBACK"
	);
	void (* tmpRegSprites)(void);
	tmpMod->registerSpritesCallback = NULL;
	if (tmpRegSpritesHandle && (tmpRegSprites = *tmpRegSpritesHandle)) {
		tmpMod->registerSpritesCallback = tmpRegSprites;
	}

	/* Read REGISTER_OBJECTS_CALLBACK symbol. */
	void (** tmpRegObjectsHandle)(void) = dlsym(
			tmpHandle,
			"REGISTER_OBJECTS_CALLBACK"
	);
	void (* tmpRegObjects)(void);
	tmpMod->registerObjectsCallback = NULL;
	if (tmpRegObjectsHandle && (tmpRegObjects = *tmpRegObjectsHandle)) {
		tmpMod->registerObjectsCallback = tmpRegObjects;
	}

	/* Read ROOM_STEP_CALLBACK symbol. */
	void (** tmpRoomStepHandle)(void) = dlsym(
			tmpHandle,
			"ROOM_STEP_CALLBACK"
	);
	void (* tmpRoomStep)(void);
	tmpMod->roomStepCallback = NULL;
	if (tmpRoomStepHandle && (tmpRoomStep = *tmpRoomStepHandle)) {
		tmpMod->roomStepCallback = tmpRoomStep;
	}

	/* Read ROOM_CHANGE_CALLBACK symbol. */
	void (** tmpRoomChangeHandle)(int32_t, int32_t) = dlsym(
			tmpHandle,
			"ROOM_CHANGE_CALLBACK"
	);
	void (* tmpRoomChange)(int32_t, int32_t);
	tmpMod->roomChangeCallback = NULL;
	if (tmpRoomChangeHandle && (tmpRoomChange = *tmpRoomChangeHandle)) {
		tmpMod->roomChangeCallback = tmpRoomChange;
	}

	/* Success. */
	*mod = tmpMod;
	return AER_MOD_MAN_OK;
}

AERModManErrCode AERModManUnload(AERMod * mod) {
	assert(mod != NULL);

	/* Close library handle. */
	dlclose(mod->libHandle);
	/* Deallocate mod. */
	free(mod);

	return AER_MOD_MAN_OK;
}
