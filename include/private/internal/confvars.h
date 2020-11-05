#ifndef INTERNAL_CONFVARS_H
#define INTERNAL_CONFVARS_H

#include <stddef.h>



/* ----- INTERNAL GLOBALS ----- */

extern size_t confNumModNames;

extern const char ** confModNames;



/* ----- INTERNAL FUNCTIONS ----- */

void ConfVarsConstructor(void);

void ConfVarsDestructor(void);



#endif /* INTERNAL_CONFVARS_H */
