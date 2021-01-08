/**
 * @file
 *
 * @brief Utilities for querying and manipulating entrapped object events.
 *
 * @since 1.0.0
 *
 * @copyright 2021 the libaermre authors
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
 * @brief Context object for object event listener iteration.
 *
 * For more information about how to use this object see @ref ObjListeners.
 *
 * @since 1.0.0
 */
typedef struct AEREventTrapIter {
  /**
   * @var next
   *
   * @brief Call the next event listener attached to the current event.
   *
   * @param[in] ctx Context object passed to the currently executing listener.
   * @param[in] target Target instance passed to the currently executing
   * listener.
   * @param[in] other Other instance passed to the currently executing listener.
   *
   * @return `true` if the original vanilla listener attached to this event was
   * called or `false` if any mod listener in this event trap "cancelled" the
   * event. The vanilla listener will always return `true`.
   *
   * @since 1.0.0
   *
   * @memberof AEREventTrapIter
   */
  bool (*next)(struct AEREventTrapIter *ctx, AERInstance *target,
               AERInstance *other);
} AEREventTrapIter;

#endif /* AER_EVENTTRAP_H */
