/**
 * @file
 *
 * @brief Utilities for querying and manipulating savegame data.
 *
 * @since 1.2.0
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
#ifndef AER_SAVE_H
#define AER_SAVE_H

/* ----- PUBLIC FUNCTIONS ----- */

double AERSaveReadValue(const char *key);

void AERSaveWriteValue(const char *key, double value);

#endif /* AER_SAVE_H */
