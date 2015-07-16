struct IFFHandle *StartIFF(char *filename,
                           long mode)
;
long PutChunk(struct IFFHandle   *iff,
              long               id,
              long               size,
              void               *data)
;
void EndIFF(struct IFFHandle *iff)
;
char *IffError(long error)
;
