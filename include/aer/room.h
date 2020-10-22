/**
 * @file
 */
#ifndef AER_ROOM_H
#define AER_ROOM_H

#include <stddef.h>
#include <stdint.h>

#include "aer/instance.h"



/* ----- PUBLIC FUNCTIONS ----- */

int32_t AERRoomGetCurrent(void);

void AERRoomGoto(int32_t roomIdx);



#endif /* AER_ROOM_H */
