/** \file
 * \brief String Utilities
 *
 * See Copyright Notice in "iup.h"
 */

 
#ifndef __IUP_STR_H 
#define __IUP_STR_H

#ifdef __cplusplus
extern "C" {
#endif


/** \defgroup str String Utilities
 * \par
 * See \ref iup_str.h
 * \ingroup util */

/** Returns a non zero value if the two strings are equal.
 * str1 or str2 can be NULL.
 * \ingroup str */
int iupStrEqual(const char* str1, const char* str2);

/** Returns a non zero value if the two strings are equal but ignores case.
 * str1 or str2 can be NULL.
 * It will work only for character codes <128.
 * \ingroup str */
int iupStrEqualNoCase(const char* str1, const char* str2);

/** Returns a non zero value if the two strings are equal but ignores case and spaces. \n
 * str1 or str2 can be NULL. \n
 * It will work only for character codes <128.
 * \ingroup str */
int iupStrEqualNoCaseNoSpace(const char* str1, const char* str2);

/** Returns a non zero value if the two strings are equal 
 * up to a number of characters defined by the strlen of the second string. \n
 * str1 or str2 can be NULL.
 * \ingroup str */
int iupStrEqualPartial(const char* str1, const char* str2);

/** Returns a non zero value if the two strings are equal but ignores case 
 * up to a number of characters defined by the strlen of the second string. \n
 * str1 or str2 can be NULL. \n
 * It will work only for character codes <128.
 * \ingroup str */
int iupStrEqualNoCasePartial(const char* str1, const char* str2);



/** Returns 1 if the string is "YES" or "ON". \n
 * Returns 0 otherwise (including NULL or empty).
 * \ingroup str */
int iupStrBoolean(const char* str);

/** Returns 1 if the string is "NO" or "OFF". \n
 * Returns 0 otherwise (including NULL or empty). \n
 * To be used when value can be "False" or others different than "True".
 * \ingroup str */
int iupStrFalse(const char* str);



/** Returns the number of lines in a string.
 * It works for UNIX, DOS and MAC line ends.
 * \ingroup str */
int iupStrLineCount(const char* str);

/** Returns a pointer to the next line and the size of the current line.
 * It works for UNIX, DOS and MAC line ends. The size does not includes the line end.
 * If str is NULL it will return NULL.
 * \ingroup str */
const char* iupStrNextLine(const char* str, int *len);

/** Returns a pointer to the next value and the size of the current value.
 * The size does not includes the separator.
 * If str is NULL it will return NULL.
 * \ingroup str */
const char* iupStrNextValue(const char* str, int str_len, int *len, char sep);

/** Returns the number of repetitions of the character occours in the string.
 * \ingroup str */
int iupStrCountChar(const char *str, char c);



/** Returns a copy of the given string.
 * If str is NULL it will return NULL.
 * \ingroup str */
char* iupStrDup(const char* str); 

/** Returns a new string containing a copy of the string up to the character.
 * The string is then incremented to after the position of the character.
 * \ingroup str */
char *iupStrDupUntil(const char **str, char c);

/** Copy the string to the buffer, but limited to the max_size of the buffer.
 * buffer is always properly ended.
 * \ingroup str */
void iupStrCopyN(char* dst_str, int dst_max_size, const char* src_str);



/** Returns a buffer with the specified size+1. \n
 * The buffer is resused after 50 calls. It must NOT be freed.
 * Use size=-1 to free all the internal buffers.
 * \ingroup str */
char *iupStrGetMemory(int size);

/** Returns a very large buffer to be used in unknown size string construction.
 * Use snprintf or vsnprintf with the given size.
 * \ingroup str */
char *iupStrGetLargeMem(int *size);



/** Converts a string into lower case. Can be used in-place. \n
 * It will work only for character codes <128.
 * \ingroup str */
void iupStrLower(char* dstr, const char* sstr);

/** Converts a string into upper case. Can be used in-place. \n
 * It will work only for character codes <128.
 * \ingroup str */
void iupStrUpper(char* dstr, const char* sstr);

/** Checks if the string has at least 1 space character.
 * \ingroup str */
int iupStrHasSpace(const char* str);

/** Checks if the character is a digit. 
 * \ingroup str */
#define iup_isdigit(_c) (_c>='0' && _c<='9')

#define iup_isnumber(_c) ((_c>='0' && _c<='9') || \
                          _c == '.'  || _c == ',' || \
                          _c == '-' || _c == '+' || \
                          _c == 'e' || _c == 'E')

/** Converts a character into upper case. \n
 * It will work only for character codes <128.
 * \ingroup str */
#define iup_toupper(_c)  ((_c >= 'a' && _c <= 'z')? (_c - 'a') + 'A': _c)

/** Converts a character into lower case. \n
 * It will work only for character codes <128.
 * \ingroup str */
#define iup_tolower(_c)  ((_c >= 'A' && _c <= 'Z')? (_c - 'A') + 'a': _c)

/** Checks if the string has only ASCII codes.
 * \ingroup str */
int iupStrIsAscii(const char* str);



/** Returns combined values in a formatted string using \ref iupStrGetMemory.
 * This is not supposed to be used for very large strings,
 * just for combinations of numeric data or constant strings.
 * \ingroup str */
char* iupStrReturnStrf(const char* format, ...);

/** Returns a string value in a string using \ref iupStrGetMemory.
 * \ingroup str */
char* iupStrReturnStr(const char* str);

/** Returns a boolean value (as YES or NO) in a string.
 * \ingroup str */
char* iupStrReturnBoolean(int i);

/** Returns a checked value (as ON, OFF or NOTDEF (-1)) in a string.
 * \ingroup str */
char* iupStrReturnChecked(int i);

/** Returns an int value in a string using \ref iupStrGetMemory.
 * \ingroup str */
char* iupStrReturnInt(int i);

/** maximum float precision
* \ingroup str */
#define IUP_FLOAT2STR "%.9f"

/** Returns a float value in a string using \ref iupStrGetMemory.
 * \ingroup str */
char* iupStrReturnFloat(float f);

/** maximum double precision
 * \ingroup str */
#define IUP_DOUBLE2STR "%.18f"

/** Returns a double value in a string using \ref iupStrGetMemory.
 * \ingroup str */
char* iupStrReturnDouble(double d);

/** Returns a RGB value in a string using \ref iupStrGetMemory.
 * \ingroup str */
char* iupStrReturnRGB(unsigned char r, unsigned char g, unsigned char b);

/** Returns a RGBA value in a string using \ref iupStrGetMemory.
 * \ingroup str */
char* iupStrReturnRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

/** Returns two string values in a string using \ref iupStrGetMemory.
 * \ingroup str */
char* iupStrReturnStrStr(const char *str1, const char *str2, char sep);

/** Returns two int values in a string using \ref iupStrGetMemory.
 * \ingroup str */
char* iupStrReturnIntInt(int i1, int i2, char sep);


/** Returns the number of decimals in a format string for floating point output.
 * \ingroup str */
int iupStrGetFormatPrecision(const char* format);

/** Prints a double in a string using the given decimal symbol.
 * \ingroup str */
void iupStrPrintfDoubleLocale(char *str, const char *format, double d, const char* decimal_symbol);


/** Extract RGB components from the string. Returns 0 or 1.
 * \ingroup str */
int iupStrToRGB(const char *str, unsigned char *r, unsigned char *g, unsigned char *b);

/** Extract RGBA components from the string. Returns 0 or 1.
 * \ingroup str */
int iupStrToRGBA(const char *str, unsigned char *r, unsigned char *g, unsigned char *b, unsigned char *a);

/** Converts the string to an int. The string must contains only the integer value.
 * Returns a a non zero value if sucessfull.
 * \ingroup str */
int iupStrToInt(const char *str, int *i);

/** Converts the string to two int. The string must contains two integer values in sequence, 
 * separated by the given character (usually 'x' or ':').
 * Returns the number of converted values.
 * Values not extracted are not changed.
 * \ingroup str */
int iupStrToIntInt(const char *str, int *i1, int *i2, char sep);

/** Converts the string to a float. The string must contains only the real value.
 * Returns a a non zero value if sucessfull.
 * \ingroup str */
int iupStrToFloat(const char *str, float *f);
int iupStrToFloatDef(const char *str, float *f, float def);

/** Converts the string to a double. The string must contains only the real value.
 * Returns a a non zero value if sucessfull.
 * \ingroup str */
int iupStrToDouble(const char *str, double *d);
int iupStrToDoubleDef(const char *str, double *d, double def);

/** Converts the string to a double using the given decimal symbol. 
 * The string must contains only the real value.
 * Returns a a non zero value if sucessfull. Returns 2 if a locale was set.
 * \ingroup str */
int iupStrToDoubleLocale(const char *str, double *d, const char* decimal_symbol);

/** Converts the string to two float. The string must contains two real values in sequence,
 * separated by the given character (usually 'x' or ':').
 * Returns the number of converted values.
 * Values not extracted are not changed.
 * ATENTION: AVOID DEFINING THIS TYPE OF ATTRIBUTE VALUE.
 * \ingroup str */
int iupStrToFloatFloat(const char *str, float *f1, float *f2, char sep);

/** Converts the string to two double. The string must contains two real values in sequence,
 * separated by the given character (usually 'x' or ':').
 * Returns the number of converted values.
 * Values not extracted are not changed.
 * ATENTION: AVOID DEFINING THIS TYPE OF ATTRIBUTE VALUE.
 * \ingroup str */
int iupStrToDoubleDouble(const char *str, double *f1, double *f2, char sep);

/** Extract two strings from the string.
 * separated by the given character (usually 'x' or ':').
 * Returns the number of converted values.
 * Values not extracted are set to empty strings.
 * \ingroup str */
int iupStrToStrStr(const char *str, char *str1, char *str2, char sep);



/** Returns the file extension of a file name.
 * Supports UNIX and Windows directory separators.
 * \ingroup str */
char* iupStrFileGetExt(const char *file_name);

/** Returns the file title of a file name.
 * Supports UNIX and Windows directory separators.
 * \ingroup str */
char* iupStrFileGetTitle(const char *file_name);

/** Returns the file path of a file name.
 * Supports UNIX and Windows directory separators.
 * The returned value includes the last separator.
 * \ingroup str */
char* iupStrFileGetPath(const char *file_name);

/** Concat path and title addind '/' between if path does not have it.
 * \ingroup str */
char* iupStrFileMakeFileName(const char* path, const char* title);

/** Split the filename in path and title using pre-allocated strings.
 * \ingroup str */
void iupStrFileNameSplit(const char* filename, char* path, char* title);



/** Replace a character in a string.
 * Returns the number of occurrences.
 * \ingroup str */
int iupStrReplace(char* str, char src, char dst);

/** Convert line ends to UNIX format in-place (one \n per line).
 * \ingroup str */
void iupStrToUnix(char* str);

/** Convert line ends to MAC format in-place (one \r per line).
 * \ingroup str */
void iupStrToMac(char* str);

/** Convert line ends to DOS/Windows format (the sequence \r\n per line).
 * If returned pointer different the input, it must be freed.
 * \ingroup str */
char* iupStrToDos(const char* str);

/** Convert string to C format. Process \n, \r and \t.
 * If returned pointer different the input, it must be freed.
 * \ingroup str */
char* iupStrConvertToC(const char* str);



/** Remove the interval from the string. Done in-place.
 * \ingroup str */
void iupStrRemove(char* value, int start, int end, int dir, int utf8);

/** Remove the interval from the string and insert the new string at the start.
 * \ingroup str */
char* iupStrInsert(const char* value, const char* insert_value, int start, int end, int utf8);



/** Process the mnemonic in the string. If not found returns str.
 * If found returns a new string. Action can be:
- 1: replace & by c
- -1: remove & and return in c
- 0: remove &
 * \ingroup str */
char* iupStrProcessMnemonic(const char* str, char *c, int action);

/** Returns the Mnemonic if found. Zero otherwise.
 * \ingroup str */
int iupStrFindMnemonic(const char* str);



/** Compare two strings using strcmp semantics, 
 *  but using the "Alphanum Algorithm" (A1 A2 A11 A30 ...). \n
 *  This means that numbers and text are sorted separately. \n
 *  Also natural alphabetic order is used: 123...aAáÁ...bBcC... \n
 *  Sorting and case insensitive will work only for Latin-1 characters, even when using utf8=1.
 * \ingroup str */
int iupStrCompare(const char* str1, const char* str2, int casesensitive, int utf8);

/** Returns a non zero value if the two strings are equal. \n
 *  If partial=1 the compare up to a number of characters defined by the strlen of the second string. \n
 *  Case insensitive will work only for Latin-1 characters, even when using utf8=1.
 * \ingroup str */
int iupStrCompareEqual(const char *str1, const char *str2, int casesensitive, int utf8, int partial);

/** Returns a non zero value if the second string is found inside the first string.  \n
 *  Uses \ref iupStrCompareEqual.
 * \ingroup str */
int iupStrCompareFind(const char *str1, const char *str2, int casesensitive, int utf8);



#ifdef __cplusplus
}
#endif

#endif
