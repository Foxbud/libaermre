/**
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
#include <assert.h>
#include <dlfcn.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "foxutils/arraymacs.h"

#include "aer/envconf.h"
#include "aer/err.h"
#include "aer/modman.h"
#include "internal/confvars.h"
#include "internal/log.h"
#include "internal/modman.h"



/* ----- PRIVATE MACROS ----- */

#define FormatLibname(name, bufSize, buf) \
	snprintf((buf), (bufSize), MOD_LIBNAME_FMT, (name))



/* ----- PRIVATE CONSTANTS ----- */

static const char * MOD_LIBNAME_FMT = "lib%s.so";

static const char * DEF_MOD_NAMES[] = {
	"define_mod",
	"definemod",
	"defineMod",
	"DefineMod"
};



/* ----- INTERNAL GLOBALS ----- */

ModMan modman = {0};



/* ----- PRIVATE FUNCTIONS ----- */

static void ModInit(Mod * mod, const char * name) {
	LogInfo("Loading mod \"%s\"...", name);

	/* Set name. */
	mod->name = name;

	/* Load library. */
	char libname[128];
	FormatLibname(name, 128, libname);
	void * libHandle = dlopen(libname, RTLD_NOW);
	if (!(mod->libHandle = libHandle)) {
		LogErr(
				"While loading mod \"%s,\" could not load corresponding library "
				"\"%s.\" Make sure its directory is in the \"LD_LIBRARY_PATH\" "
				"environment variable.",
				name,
				libname
		);
		abort();
	}

	/* Load mod definition function. */
	AERModDef (* defMod)(void);
	size_t numDefModNames = sizeof(DEF_MOD_NAMES) / sizeof(const char *);
	for (uint32_t idx = 0; idx < numDefModNames; idx++) {
		defMod = dlsym(libHandle, DEF_MOD_NAMES[idx]);
		if (defMod) break;
	}
	if (!defMod) {
		/* TODO Create dedicated string concatenation function. */
		/* Efficiently concatenate all valid def mod function names. */
		char defModNamesBuf[256];
		uint32_t charIdxOut = 0;
		for (uint32_t nameIdx = 0; nameIdx < numDefModNames; nameIdx++) {
			/* Prevent buffer overflow. */
			if (charIdxOut == sizeof(defModNamesBuf) - 1) break;
			defModNamesBuf[charIdxOut++] = ' ';
			const char * name = DEF_MOD_NAMES[nameIdx];
			uint32_t charIdxIn = 0;
			char charCur = name[charIdxIn];
			while (charCur != '\0') {
				/* Prevent buffer overflow. */
				if (charIdxOut == sizeof(defModNamesBuf) - 1) break;
				defModNamesBuf[charIdxOut++] = charCur;
				charCur = name[++charIdxIn];
			}
		}
		defModNamesBuf[charIdxOut] = '\0';
		/* Display error. */
		LogErr(
				"While loading mod \"%s,\" could not find mod definition function "
				"with one of the following names:%s.",
				name,
				defModNamesBuf
		);
		abort();
	}
	AERModDef def = defMod();

	/* Record registration callbacks. */
	mod->regSprites = def.regSprites;
	mod->regObjects = def.regObjects;
	mod->regObjListeners = def.regObjListeners;

	/* Record pseudoevent listeners. */
	if (def.roomStepListener) {
		ModListener * listener = FoxArrayMPush(
				ModListener,
				&modman.roomStepListeners
		);
		listener->mod = mod;
		listener->func = def.roomStepListener;
	}
	if (def.roomChangeListener) {
		ModListener * listener = FoxArrayMPush(
				ModListener,
				&modman.roomChangeListeners
		);
		listener->mod = mod;
		listener->func = (void (*)(void))def.roomChangeListener;
	}

	/* Record mod library management callbacks. */
	mod->constructor = def.constructor;
	mod->destructor = def.destructor;

	LogInfo("Successfully loaded mod \"%s.\"", name);
	return;
}

static void ModDeinit(Mod * mod) {
	LogInfo("Unloading mod \"%s\"...", mod->name);

	dlclose(mod->libHandle);

	const char * name = mod->name;

	mod->libHandle = NULL;
	mod->name = NULL;
	mod->constructor = NULL;
	mod->destructor = NULL;
	mod->regSprites = NULL;
	mod->regObjects = NULL;
	mod->regObjListeners = NULL;

	LogInfo("Successfully unloaded mod \"%s.\"", name);
	return;
}



/* ----- INTERNAL FUNCTIONS ----- */

void ModManConstructor(void) {
	LogInfo("Loading mods...");

	/* Initialize mod manager. */
	/* Prevent arrays from having initial capacity of zero. */
	size_t initArrCap = confNumModNames + (confNumModNames == 0);
	FoxArrayMInitExt(Mod, &modman.mods, initArrCap);
	FoxArrayMInit(Mod *, &modman.context);
	FoxArrayMInitExt(ModListener, &modman.roomStepListeners, initArrCap);
	FoxArrayMInitExt(ModListener, &modman.roomChangeListeners, initArrCap);

	/* Load mod libraries. */
	for (uint32_t idx = 0; idx < confNumModNames; idx++) {
		Mod * mod = FoxArrayMPush(Mod, &modman.mods);
		ModInit(mod, confModNames[idx]);
		if (mod->constructor) {
			*FoxArrayMPush(Mod *, &modman.context) = mod;
			mod->constructor();
			FoxArrayMPop(Mod *, &modman.context);
		}
	}

	LogInfo("Done. Loaded %zu mod(s).", confNumModNames);
	return;
}

void ModManDestructor(void) {
	/* Unload mods. */
	LogInfo("Unloading mods...");
	size_t numMods = FoxArrayMSize(Mod, &modman.mods);
	for (uint32_t idx = 0; idx < numMods; idx++) {
		Mod * mod = FoxArrayMPeek(Mod, &modman.mods);
		if (mod->destructor) {
			*FoxArrayMPush(Mod *, &modman.context) = mod;
			mod->destructor();
			FoxArrayMPop(Mod *, &modman.context);
		}
		ModDeinit(mod);
		FoxArrayMPop(Mod, &modman.mods);
	}
	LogInfo("Done. Unloaded %zu mod(s).", numMods);

	/* Deinit modman. */
	FoxArrayMDeinit(Mod, &modman.mods);
	FoxArrayMDeinit(Mod *, &modman.context);
	FoxArrayMDeinit(ModListener, &modman.roomStepListeners);
	FoxArrayMDeinit(ModListener, &modman.roomChangeListeners);

	return;
}
