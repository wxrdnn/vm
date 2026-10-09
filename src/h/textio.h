#ifndef TEXTIO_H

#define TEXTIO_H

#include "errorHandle.h"
#include <sys/stat.h>

Error GetFileSize(const char *const path, size_t *size);

Error LoadText(const int fd, char *const textBuf, const size_t textBufSize);

Error CreateTextBuf(const size_t size, char **textBuf);

#endif
