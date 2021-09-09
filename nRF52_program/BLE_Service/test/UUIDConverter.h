#ifndef UUIDConverter_H
#define UUIDConverter_H

char nibble2c(char c);
char hex2c(char c1, char c2);
void strUUID2Bytes(String strUUID, uint8_t binUUID[]);

#endif
