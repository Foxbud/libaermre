/**
 * @file
 *
 * @brief Utilities for querying and manipulating object events.
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
#ifndef AER_EVENT_H
#define AER_EVENT_H

#include <stdbool.h>

#include "aer/instance.h"

/* ----- PUBLIC TYPES ----- */

/**
 * @brief Semi-opaque type for an object event.
 *
 * For more information about how to use this object see @ref ObjListeners.
 *
 * @since 1.0.0
 */
typedef struct AEREvent {
  /**
   * @var handle
   *
   * @brief Handle the current event.
   *
   * This function has the effect of calling the next event listener attached to
   * this object event.
   *
   * @param[in] event Event object passed to the currently executing listener.
   * @param[in] target Target instance passed to the currently executing
   * listener.
   * @param[in] other Other instance passed to the currently executing listener.
   *
   * @return Whether or not the event was handled.
   *
   * @since 1.0.0
   *
   * @memberof AEREvent
   */
  bool (*handle)(struct AEREvent *event, AERInstance *target,
                 AERInstance *other);
} AEREvent;

#endif /* AER_EVENT_H */
