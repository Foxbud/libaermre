/**
 * @file
 */
#ifndef AER_MRE_H
#define AER_MRE_H

#include <stdbool.h>
#include <stdint.h>



/* ----- PUBLIC FUNCTIONS ----- */

uint32_t AERGetNumSteps(void);

const bool * AERGetKeysPressed(void);

const bool * AERGetKeysHeld(void);

const bool * AERGetKeysReleased(void);



#endif /* AER_MRE_H */
