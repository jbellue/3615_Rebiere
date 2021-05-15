#ifndef _UTILS_H
#define _UTILS_H

// counts the number of characters in a null-terminated string,
// skipping over the diacritics and multibytes characters
unsigned int characterCount(const char* str);

#endif