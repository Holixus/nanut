
#include "nano/ut_utf8.h"

/* -------------------------------------------------------------------------- */
int ut_utf8_to_code(char const **utf8)
{
	int code = 0;
	char const *raw = *utf8;
	if (!(*raw & 0x80))
		code = *raw++;
	else
		if ((*raw & 0xE0) == 0xC0) {
			if ((raw[1] & 0300) == 0200) {
				code = ((raw[0] & 0x1F) << 6) + (raw[1] & 0x3F);
				raw += 2;
			}
		} else
			if ((*raw & 0xF0) == 0xE0) {
				if ((raw[1] & 0300) == 0200 && (raw[2] & 0300) == 0200) {
					code = ((raw[0] & 0xF) << 12) + ((raw[1] & 0x3F) << 6) + (raw[2] & 0x3F);
					raw += 3;
				}
			} else
				if ((*raw & 0xF8) == 0xF0) {
					if ((raw[1] & 0300) == 0200 && (raw[2] & 0300) == 0200 && (raw[3] & 0300) == 0200) {
						code = ((raw[1] & 0x3F) << 12) + ((raw[2] & 0x3F) << 6) + (raw[3] & 0x3F);
						raw += 4;
					}
				}
	*utf8 = raw;
	return code;
}

/* ----------------------------<--------------------------------------------- */
char *ut_code_to_utf8(char *raw, int uc)
{
	if (!(uc & 0x1FFF80))
		*raw++ = uc;
	else
		if (!(uc & 0x1FF800)) {
			*raw++ = 0xC0 | (uc >> 6);
			*raw++ = 0x80 | (uc & 0x3F);
		} else
			if (!(uc & 0x1F0000)) {
				*raw++ = 0xE0 | (uc >> 12);
				*raw++ = 0x80 | (0x3F & uc >> 6);
				*raw++ = 0x80 | (0x3F & uc);
			} else {
				*raw++ = 0xF0;
				*raw++ = 0x80 | (0x3F & uc >> 12);
				*raw++ = 0x80 | (0x3F & uc >> 6);
				*raw++ = 0x80 | (0x3F & uc);
			}
	return raw;
}

