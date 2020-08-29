/**
 * @file
 */
#ifndef AER_SPRITE_H
#define AER_SPRITE_H

#include <stddef.h>
#include <stdint.h>



/* ----- PUBLIC FUNCTIONS ----- */

int32_t AERSpriteRegister(
		const char * filename,
		size_t numFrames,
		uint32_t origX,
		uint32_t origY
);



#endif /* AER_SPRITE_H */
