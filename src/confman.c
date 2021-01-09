/**
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
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "foxutils/arraymacs.h"
#include "foxutils/math.h"
#include "foxutils/stringmapmacs.h"

#include "toml.h"

#include "aer/confman.h"
#include "internal/confman.h"
#include "internal/err.h"
#include "internal/export.h"
#include "internal/log.h"
#include "internal/modman.h"

/* ----- PRIVATE MACROS ----- */

#define GetAbsKey(key)                                                         \
  ({                                                                           \
    char GetAbsKey_buf[192];                                                   \
    if ((size_t)snprintf(GetAbsKey_buf, sizeof(GetAbsKey_buf), "%s.%s",        \
                         ((ModManHasContext())                                 \
                              ? ModManGetMod(ModManPeekContext())->name        \
                              : INTERNAL_CONF_NAME),                           \
                         (key)) >= sizeof(GetAbsKey_buf)) {                    \
      if (ModManHasContext()) {                                                \
        LogErr("Key overflow while constructing configuration key for mod "    \
               "\"%s\". Key must be less than %zu characters in length, "      \
               "but key was \"%s\".",                                          \
               ModManGetMod(ModManPeekContext())->name, sizeof(GetAbsKey_buf), \
               GetAbsKey_buf);                                                 \
      } else {                                                                 \
        LogErr("Key overflow while constructing internal configuration key. "  \
               "Key must be less than %zu characters in length, "              \
               "but key was \"%s\".",                                          \
               sizeof(GetAbsKey_buf), GetAbsKey_buf);                          \
      }                                                                        \
      abort();                                                                 \
    }                                                                          \
    GetAbsKey_buf;                                                             \
  })

/* ----- PRIVATE TYPES ----- */

typedef enum ConfType {
  CONF_NULL,
  CONF_BOOL,
  CONF_INT,
  CONF_DOUBLE,
  CONF_STRING
} ConfType;

typedef struct ConfEntry {
  bool isArray;
  ConfType type;
  union {
    bool b;
    int64_t i;
    double d;
    char *s;
    FoxArray *a;
  } value;
} ConfEntry;

/* ----- PRIVATE CONSTANTS ----- */

static const char *CONF_FILE = "aer/conf.toml";

static const char *INTERNAL_CONF_NAME = "mre";

/* ----- PRIVATE GLOBALS ----- */

static FoxMap conf = {0};

static char workingKey[128];

static FoxArray workingBreaks = {0};

/* ----- PRIVATE FUNCTIONS ----- */

static void PushKey(const char *key) {
  uint32_t keyIdx = 0;
  uint32_t workingIdx = 0;
  if (!FoxArrayMEmpty(uint32_t, &workingBreaks)) {
    workingIdx = *FoxArrayMPeek(uint32_t, &workingBreaks);
    workingKey[workingIdx++] = '.';
  }
  do {
    if (workingIdx >= sizeof(workingKey)) {
      workingKey[sizeof(workingKey) - 1] = '\0';
      LogErr("Key overflow while parsing configuration data. "
             "Key must be less than %zu characters in length, "
             "but key was \"%s\".",
             sizeof(workingKey), workingKey);
      abort();
    }
  } while ((workingKey[workingIdx++] = key[keyIdx++]) != '\0');
  *FoxArrayMPush(uint32_t, &workingBreaks) = workingIdx - 1;

  return;
}

static void PopKey(void) {
  FoxArrayMPop(uint32_t, &workingBreaks);
  if (!FoxArrayMEmpty(uint32_t, &workingBreaks))
    workingKey[*FoxArrayMPeek(uint32_t, &workingBreaks)] = '\0';

  return;
}

static void ParseValueFromTable(toml_table_t *table, const char *key) {
  /* Create configuration entry. */
  ConfEntry *entry = FoxMapMInsert(const char *, ConfEntry, &conf, workingKey);
  entry->isArray = false;
  toml_datum_t datum;

  /* Boolean. */
  if ((datum = toml_bool_in(table, key)).ok) {
    entry->type = CONF_BOOL;
    entry->value.b = datum.u.b;
  }
  /* Integer. */
  else if ((datum = toml_int_in(table, key)).ok) {
    entry->type = CONF_INT;
    entry->value.i = datum.u.i;
  }
  /* Double. */
  else if ((datum = toml_double_in(table, key)).ok) {
    entry->type = CONF_DOUBLE;
    entry->value.d = datum.u.d;
  }
  /* String. */
  else if ((datum = toml_string_in(table, key)).ok) {
    entry->type = CONF_STRING;
    entry->value.s = datum.u.s;
  }
  /* Invalid. */
  else {
    LogErr("Configuration key \"%s\" was not a supported type. "
           "Supported types are boolean, integer, double, string and array.",
           workingKey);
    abort();
  }

  return;
}

static toml_datum_t ParseValueFromArray(toml_array_t *array, uint32_t idx,
                                        ConfType *type) {
  toml_datum_t result;

  /* Boolean. */
  if ((result = toml_bool_at(array, idx)).ok)
    *type = CONF_BOOL;
  /* Integer. */
  else if ((result = toml_int_at(array, idx)).ok)
    *type = CONF_INT;
  /* Double. */
  else if ((result = toml_double_at(array, idx)).ok)
    *type = CONF_DOUBLE;
  /* String. */
  else if ((result = toml_string_at(array, idx)).ok)
    *type = CONF_STRING;
  /* Invalid. */
  else {
    LogErr("Array at configuration key \"%s\" "
           "had unsupported type at index %zu. "
           "Supported array types are boolean, integer, double and string.",
           workingKey, idx);
    abort();
  }

  return result;
}

static void ParseArray(toml_array_t *array) {
  /* Create configuration entry. */
  ConfEntry *entry = FoxMapMInsert(const char *, ConfEntry, &conf, workingKey);
  entry->isArray = true;
  ConfType entType = CONF_NULL;
  FoxArray *entArr = NULL;

  /* Parse each array index. */
  size_t numElems = toml_array_nelem(array);
  for (uint32_t idx = 0; idx < numElems; idx++) {
    toml_datum_t datum = ParseValueFromArray(array, idx, &entType);

    /* Initialize entry array on first iteration. */
    if (idx == 0) {
      switch (entType) {
      case CONF_BOOL:
        entArr = FoxArrayMNewExt(bool, numElems);
        break;
      case CONF_INT:
        entArr = FoxArrayMNewExt(int64_t, numElems);
        break;
      case CONF_DOUBLE:
        entArr = FoxArrayMNewExt(double, numElems);
        break;
      case CONF_STRING:
        entArr = FoxArrayMNewExt(char *, numElems);
        break;
      default:
        break;
      }
    }

    /* Add value to entry array. */
    switch (entType) {
    case CONF_BOOL:
      *FoxArrayMPush(bool, entArr) = datum.u.b;
      break;
    case CONF_INT:
      *FoxArrayMPush(int64_t, entArr) = datum.u.i;
      break;
    case CONF_DOUBLE:
      *FoxArrayMPush(double, entArr) = datum.u.d;
      break;
    case CONF_STRING:
      *FoxArrayMPush(char *, entArr) = datum.u.s;
      break;
    default:
      break;
    }
  }

  /* Copy data to entry. */
  entry->type = entType;
  entry->value.a = entArr;

  return;
}

static void ParseTable(toml_table_t *table) {
  /* Parse each table key. */
  const char *key;
  toml_table_t *nextTable;
  toml_array_t *nextArray;
  for (uint32_t idx = 0;; idx++) {
    if (!(key = toml_key_in(table, idx)))
      break;
    PushKey(key);

    /* Table. */
    if ((nextTable = toml_table_in(table, key)))
      ParseTable(nextTable);
    /* Array. */
    else if ((nextArray = toml_array_in(table, key)))
      ParseArray(nextArray);
    /* Value. */
    else
      ParseValueFromTable(table, key);

    PopKey();
  }

  return;
}

static bool ConfEntryDeinitCallback(ConfEntry *entry, void *ctx) {
  (void)ctx;

  ConfType type = entry->type;
  if (entry->isArray && type != CONF_NULL) {
    FoxArray *array = entry->value.a;
    if (type == CONF_STRING) {
      size_t numStrings = FoxArrayMSize(char *, array);
      for (uint32_t idx = 0; idx < numStrings; idx++) {
        free(FoxArrayMPop(char *, array));
      }
    }
    FoxArrayFree(array);
  } else if (type == CONF_STRING) {
    free(entry->value.s);
  }

  *entry = (ConfEntry){0};

  return true;
}

/* ----- INTERNAL FUNCTIONS ----- */

void ConfManConstructor(void) {
  /* Initialize globals. */
  FoxStringMapMInit(ConfEntry, &conf);
  FoxArrayMInit(uint32_t, &workingBreaks);

  /* Open configuration file. */
  FILE *fp;
  if (!(fp = fopen(CONF_FILE, "r"))) {
    LogErr("Could not open configuration file \"%s\".", CONF_FILE);
    abort();
  }

  /* Parse file. */
  char errBuf[256];
  toml_table_t *data = toml_parse_file(fp, errBuf, sizeof(errBuf));
  fclose(fp);
  if (!data) {
    LogErr("Could not parse configuration file. "
           "Reported error: \"%s\".",
           errBuf);
    abort();
  }

  /* Parse data. */
  ParseTable(data);

  /* Cleanup. */
  toml_free(data);

  return;
}

void ConfManDestructor(void) {
  /* Deinitialize globals. */
  FoxMapMForEachElement(const char *, ConfEntry, &conf, ConfEntryDeinitCallback,
                        NULL);
  FoxMapMDeinit(const char *, ConfEntry, &conf);
  FoxArrayMDeinit(uint32_t, &workingBreaks);

  return;
}

/* ----- PUBLIC FUNCTIONS ----- */

AER_EXPORT bool AERConfManGetBool(const char *key) {
  ErrIf(!key, AER_NULL_ARG, false);

  char *absKey = GetAbsKey(key);
  ConfEntry *entry = FoxMapMIndex(const char *, ConfEntry, &conf, absKey);
  ErrIf(!entry, AER_FAILED_LOOKUP, false);
  ErrIf(entry->type != CONF_BOOL || entry->isArray, AER_FAILED_PARSE, false);

  return entry->value.b;
}

AER_EXPORT size_t AERConfManGetBools(const char *key, size_t bufSize,
                                     bool *boolBuf) {
  ErrIf(!key, AER_NULL_ARG, 0);
  ErrIf(!boolBuf && bufSize > 0, AER_NULL_ARG, 0);

  char *absKey = GetAbsKey(key);
  ConfEntry *entry = FoxMapMIndex(const char *, ConfEntry, &conf, absKey);
  ErrIf(!entry, AER_FAILED_LOOKUP, 0);
  ConfType type = entry->type;
  ErrIf(!(type == CONF_BOOL || type == CONF_NULL) || !entry->isArray,
        AER_FAILED_PARSE, 0);
  if (type == CONF_NULL)
    return 0;

  FoxArray *array = entry->value.a;
  size_t numElems = FoxArrayMSize(bool, array);
  size_t numToWrite = FoxMin(bufSize, numElems);
  for (unsigned int idx = 0; idx < numToWrite; idx++)
    boolBuf[idx] = *FoxArrayMIndex(bool, array, idx);

  return numElems;
}

AER_EXPORT int64_t AERConfManGetInt(const char *key) {
  ErrIf(!key, AER_NULL_ARG, 0);

  char *absKey = GetAbsKey(key);
  ConfEntry *entry = FoxMapMIndex(const char *, ConfEntry, &conf, absKey);
  ErrIf(!entry, AER_FAILED_LOOKUP, 0);
  ErrIf(entry->type != CONF_INT || entry->isArray, AER_FAILED_PARSE, 0);

  return entry->value.i;
}

AER_EXPORT size_t AERConfManGetInts(const char *key, size_t bufSize,
                                    int64_t *intBuf) {
  ErrIf(!key, AER_NULL_ARG, 0);
  ErrIf(!intBuf && bufSize > 0, AER_NULL_ARG, 0);

  char *absKey = GetAbsKey(key);
  ConfEntry *entry = FoxMapMIndex(const char *, ConfEntry, &conf, absKey);
  ErrIf(!entry, AER_FAILED_LOOKUP, 0);
  ConfType type = entry->type;
  ErrIf(!(type == CONF_INT || type == CONF_NULL) || !entry->isArray,
        AER_FAILED_PARSE, 0);
  if (type == CONF_NULL)
    return 0;

  FoxArray *array = entry->value.a;
  size_t numElems = FoxArrayMSize(int64_t, array);
  size_t numToWrite = FoxMin(bufSize, numElems);
  for (unsigned int idx = 0; idx < numToWrite; idx++)
    intBuf[idx] = *FoxArrayMIndex(int64_t, array, idx);

  return numElems;
}

AER_EXPORT double AERConfManGetDouble(const char *key) {
  ErrIf(!key, AER_NULL_ARG, 0);

  char *absKey = GetAbsKey(key);
  ConfEntry *entry = FoxMapMIndex(const char *, ConfEntry, &conf, absKey);
  ErrIf(!entry, AER_FAILED_LOOKUP, 0);
  ErrIf(entry->type != CONF_DOUBLE || entry->isArray, AER_FAILED_PARSE, 0);

  return entry->value.d;
}

AER_EXPORT size_t AERConfManGetDoubles(const char *key, size_t bufSize,
                                       double *doubleBuf) {
  ErrIf(!key, AER_NULL_ARG, 0);
  ErrIf(!doubleBuf && bufSize > 0, AER_NULL_ARG, 0);

  char *absKey = GetAbsKey(key);
  ConfEntry *entry = FoxMapMIndex(const char *, ConfEntry, &conf, absKey);
  ErrIf(!entry, AER_FAILED_LOOKUP, 0);
  ConfType type = entry->type;
  ErrIf(!(type == CONF_DOUBLE || type == CONF_NULL) || !entry->isArray,
        AER_FAILED_PARSE, 0);
  if (type == CONF_NULL)
    return 0;

  FoxArray *array = entry->value.a;
  size_t numElems = FoxArrayMSize(double, array);
  size_t numToWrite = FoxMin(bufSize, numElems);
  for (unsigned int idx = 0; idx < numToWrite; idx++)
    doubleBuf[idx] = *FoxArrayMIndex(double, array, idx);

  return numElems;
}

AER_EXPORT const char *AERConfManGetString(const char *key) {
  ErrIf(!key, AER_NULL_ARG, 0);

  char *absKey = GetAbsKey(key);
  ConfEntry *entry = FoxMapMIndex(const char *, ConfEntry, &conf, absKey);
  ErrIf(!entry, AER_FAILED_LOOKUP, 0);
  ErrIf(entry->type != CONF_STRING || entry->isArray, AER_FAILED_PARSE, 0);

  return entry->value.s;
}

AER_EXPORT size_t AERConfManGetStrings(const char *key, size_t bufSize,
                                       const char **strBuf) {
  ErrIf(!key, AER_NULL_ARG, 0);
  ErrIf(!strBuf && bufSize > 0, AER_NULL_ARG, 0);

  char *absKey = GetAbsKey(key);
  ConfEntry *entry = FoxMapMIndex(const char *, ConfEntry, &conf, absKey);
  ErrIf(!entry, AER_FAILED_LOOKUP, 0);
  ConfType type = entry->type;
  ErrIf(!(type == CONF_STRING || type == CONF_NULL) || !entry->isArray,
        AER_FAILED_PARSE, 0);
  if (type == CONF_NULL)
    return 0;

  FoxArray *array = entry->value.a;
  size_t numElems = FoxArrayMSize(char *, array);
  size_t numToWrite = FoxMin(bufSize, numElems);
  for (unsigned int idx = 0; idx < numToWrite; idx++)
    strBuf[idx] = *FoxArrayMIndex(char *, array, idx);

  return numElems;
}
