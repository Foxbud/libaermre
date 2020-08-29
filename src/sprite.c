#include <stdio.h>

#include "foxutils/arraymacs.h"

#include "aer/sprite.h"
#include "internal/err.h"
#include "internal/hld.h"
#include "internal/log.h"
#include "internal/modman.h"
#include "internal/mre.h"



/* ----- PUBLIC FUNCTIONS ----- */

int32_t AERSpriteRegister(
		const char * filename,
		size_t numFrames,
		uint32_t origX,
		uint32_t origY
) {
	LogInfo(
			"Registering sprite \"%s\" for mod \"%s\"...",
			filename,
			(*FoxArrayMPeek(Mod *, &modman.context))->name
	);

	ErrIf(mre.stage != STAGE_SPRITE_REG, AER_SEQ_BREAK, -1);
	ErrIf(!filename, AER_NULL_ARG, -1);

	/* Construct asset path. */
	char pathBuf[256];
	snprintf(
			pathBuf,
			256,
			MRE_ASSET_PATH_FMT,
			(*FoxArrayMPeek(Mod *, &modman.context))->slug,
			filename
	);

	int32_t spriteIdx = hldfuncs.actionSpriteAdd(
			pathBuf,
			numFrames,
			0,
			0,
			0,
			0,
			origX,
			origY
	);
	ErrIf(spriteIdx < 0, AER_BAD_FILE, -1);

	LogInfo("Successfully registered sprite to index %i.", spriteIdx);

	return spriteIdx;
}
