#include <assert.h>
#include <dlfcn.h>
#include <stdlib.h>
#include "modman.h"



/* ----- PUBLIC FUNCTIONS ----- */

ModManErrCode ModManLoad(const char * modLib, AERMod ** mod) {
	assert(modLib != NULL);
	assert(mod != NULL);

	/* Open library. */
	void * tmpHandle = dlopen(modLib, RTLD_NOW);
	if (!tmpHandle) {
		return MOD_MAN_NO_SUCH_MOD;
	}

	/* Allocate memory for library. */
	AERMod * tmpMod = malloc(sizeof(AERMod));
	if (!tmpMod) {
		return MOD_MAN_OUT_OF_MEM;
	}
	tmpMod->libHandle = tmpHandle;

	/* Read MOD_NAME symbol. */
	const char ** tmpNameHandle = dlsym(tmpHandle, "MOD_NAME");
	const char * tmpName;
	if (!tmpNameHandle || !(tmpName = *tmpNameHandle)) {
		free(tmpMod);
		tmpMod = NULL;
		return MOD_MAN_NAME_NOT_FOUND;
	}
	tmpMod->name = tmpName;

	/* Read MOD_VERSION symbol. */
	const char ** tmpVersionHandle = dlsym(tmpHandle, "MOD_VERSION");
	const char * tmpVersion;
	if (!tmpVersionHandle || !(tmpVersion = *tmpVersionHandle)) {
		free(tmpMod);
		tmpMod = NULL;
		return MOD_MAN_VERSION_NOT_FOUND;
	}
	tmpMod->version = tmpVersion;

	/* Read ModRegisterSprites symbol. */
	void (* tmpRegSprites)(void) = dlsym(tmpHandle, "ModRegisterSprites");
	tmpMod->registerSprites = NULL;
	if (tmpRegSprites) {
		tmpMod->registerSprites = tmpRegSprites;
	}

	/* Read ModRegisterObjects symbol. */
	void (* tmpRegObjects)(void) = dlsym(tmpHandle, "ModRegisterObjects");
	tmpMod->registerObjects = NULL;
	if (tmpRegObjects) {
		tmpMod->registerObjects = tmpRegObjects;
	}

	/* Read ModRegisterListeners symbol. */
	void (* tmpRegListeners)(void) = dlsym(tmpHandle, "ModRegisterListeners");
	tmpMod->registerListeners = NULL;
	if (tmpRegListeners) {
		tmpMod->registerListeners = tmpRegListeners;
	}

	/* Read ModRoomStepListener symbol. */
	void (* tmpRoomStep)(void) = dlsym(tmpHandle, "ModRoomStepListener");
	tmpMod->roomStepListener = NULL;
	if (tmpRoomStep) {
		tmpMod->roomStepListener = tmpRoomStep;
	}

	/* Read ModRoomChangeListener symbol. */
	void (* tmpRoomChange)(int32_t, int32_t) = dlsym(
			tmpHandle,
			"ModRoomChangeListener"
	);
	tmpMod->roomChangeListener = NULL;
	if (tmpRoomChange) {
		tmpMod->roomChangeListener = tmpRoomChange;
	}

	/* Success. */
	*mod = tmpMod;
	return MOD_MAN_OK;
}

ModManErrCode ModManUnload(AERMod * mod) {
	assert(mod != NULL);

	/* Close library handle. */
	dlclose(mod->libHandle);
	/* Deallocate mod. */
	free(mod);

	return MOD_MAN_OK;
}
