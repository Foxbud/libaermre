/**
 * @file
 *
 * @brief TODO.
 *
 * @since 1.0.0
 *
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
#ifndef AER_EVENTTRAP_H
#define AER_EVENTTRAP_H

#include <stdbool.h>

#include "aer/instance.h"



/* ----- PUBLIC TYPES ----- */

/**
 * @brief TODO.
 *
 * @since 1.0.0
 */
typedef struct AEREventTrapIter {
	bool (* next)(
			struct AEREventTrapIter * event,
			AERInstance * target,
			AERInstance * other
	);
} AEREventTrapIter;

/**
 * @var next
 *
 * @brief TODO.
 *
 * @param[in] event TODO.
 * @param[in] target TODO.
 * @param[in] other TODO.
 *
 * @return TODO.
 *
 * @since 1.0.0
 *
 * @memberof AEREventTrapIter
 */



#endif /* AER_EVENTTRAP_H */
