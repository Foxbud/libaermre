#include <assert.h>
#include <dlfcn.h>
#include <stdlib.h>

#include "internal/modman.h"



/* ----- INTERNAL FUNCTIONS ----- */

ModManErrCode ModManLoad(const char * modLib, AERMod * mod) {
	assert(modLib != NULL);
	assert(mod != NULL);

	/* Open library. */
	void * tmpHandle = dlopen(modLib, RTLD_NOW);
	if (!tmpHandle) {
		return MOD_MAN_NO_SUCH_MOD;
	}

	/* Record handle to library. */
	mod->libHandle = tmpHandle;

	/* Read MOD_NAME symbol. */
	const char ** tmpNameHandle = dlsym(tmpHandle, "MOD_NAME");
	const char * tmpName;
	if (!tmpNameHandle || !(tmpName = *tmpNameHandle)) {
		return MOD_MAN_NAME_NOT_FOUND;
	}
	mod->name = tmpName;

	/* Read MOD_VERSION symbol. */
	const char ** tmpVersionHandle = dlsym(tmpHandle, "MOD_VERSION");
	const char * tmpVersion;
	if (!tmpVersionHandle || !(tmpVersion = *tmpVersionHandle)) {
		return MOD_MAN_VERSION_NOT_FOUND;
	}
	mod->version = tmpVersion;

	/* Read ModRegisterSprites symbol. */
	void (* tmpRegSprites)(void) = dlsym(tmpHandle, "ModRegisterSprites");
	mod->registerSprites = NULL;
	if (tmpRegSprites) {
		mod->registerSprites = tmpRegSprites;
	}

	/* Read ModRegisterObjects symbol. */
	void (* tmpRegObjects)(void) = dlsym(tmpHandle, "ModRegisterObjects");
	mod->registerObjects = NULL;
	if (tmpRegObjects) {
		mod->registerObjects = tmpRegObjects;
	}

	/* Read ModRegisterListeners symbol. */
	void (* tmpRegListeners)(void) = dlsym(tmpHandle, "ModRegisterListeners");
	mod->registerListeners = NULL;
	if (tmpRegListeners) {
		mod->registerListeners = tmpRegListeners;
	}

	/* Read ModRoomStepListener symbol. */
	RoomStepListener tmpRoomStep = dlsym(tmpHandle, "ModRoomStepListener");
	mod->roomStepListener = NULL;
	if (tmpRoomStep) {
		mod->roomStepListener = tmpRoomStep;
	}

	/* Read ModRoomChangeListener symbol. */
	RoomChangeListener tmpRoomChange = dlsym(
			tmpHandle,
			"ModRoomChangeListener"
	);
	mod->roomChangeListener = NULL;
	if (tmpRoomChange) {
		mod->roomChangeListener = tmpRoomChange;
	}

	/* Success. */
	return MOD_MAN_OK;
}

ModManErrCode ModManUnload(AERMod * mod) {
	assert(mod != NULL);

	/* Close library handle. */
	dlclose(mod->libHandle);

	return MOD_MAN_OK;
}
