/**
 * @file
 *
 * @brief Miscellaneous iterator utilities.
 *
 * @subsubsection IterUsage Iterator Usage
 *
 * @since 1.3.0
 *
 * @sa AERObjectIterChildren
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
#ifndef AER_ITER_H
#define AER_ITER_H

/* ----- PUBLIC FUNCTIONS ----- */

/**
 * @brief Destroy an iterator previously created using another MRE function.
 *
 * @param[in] iter Iterator of interest.
 *
 * @throw ::AER_BAD_VAL if argument `iter` is an invalid iterator.
 *
 * @since 1.3.0
 */
void AERIterDestroy(void *iter);

#endif /* AER_ITER_H */