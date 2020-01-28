#ifndef MAKEWORD
#define MAKEWORD(b1, b2) ((WORD)(((BYTE)(b1)) | ((WORD)((BYTE)(b2))) << 8))
#endif

#ifndef LOBYTE
#define LOBYTE(w) ((BYTE)(w))
#endif

#ifndef HIBYTE
#define HIBYTE(w) ((BYTE)(((WORD)(w) >> 8) & 0xFF))
#endif

