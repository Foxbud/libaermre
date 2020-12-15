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
