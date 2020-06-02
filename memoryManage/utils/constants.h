#ifndef MEMORYMANAGE_CONSTANTS_H
#define MEMORYMANAGE_CONSTANTS_H

#include "memoryManage_declare.h"

MM_NAMESPACE_START

#define JOURNAL_SHORT_NAME_MAX_LENGTH   30
#define JOURNAL_FOLDER_MAX_LENGTH       100

const long TIME_FROM_FIRST = 0;
const long TIME_TO_LAST = -1;

const int KB = 1024;
const int MB = KB * KB;
const int JOURNAL_PAGE_SIZE = 128 * MB;
const int PAGE_MIN_HEADROOM = 2 * MB;

MM_NAMESPACE_END
#endif
