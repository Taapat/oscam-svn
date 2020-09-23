#include "globals.h"
#include "oscam-string.h"

/* This function encapsulates malloc. It automatically adds an error message
   to the log if it failed and calls cs_exit(quiterror) if quiterror > -1.
   result will be automatically filled with the new memory position or NULL
   on failure. */
bool cs_malloc(void *result, size_t size)
{
	void **tmp = result;
	*tmp = malloc(size);

	if(*tmp == NULL)
	{
		fprintf(stderr, "%s: ERROR: Can't allocate %zu bytes!", __func__, size);
	}
	else
	{
		memset(*tmp, 0, size);
	}
	return !!*tmp;
}

/* This function encapsulates realloc. It automatically adds an error message
   to the log if it failed and calls cs_exit(quiterror) if quiterror > -1.
   result will be automatically filled with the new memory position or NULL
   on failure. If a failure occured, the existing memory in result will
   be freed. */
bool cs_realloc(void *result, size_t size)
{
	void **tmp = result, **tmp2 = result;
	*tmp = realloc(*tmp, size);

	if(*tmp == NULL)
	{
		fprintf(stderr, "%s: ERROR: Can't allocate %zu bytes!", __func__, size);
		NULLFREE(*tmp2);
	}
	return !!*tmp;
}

/* Allocates a new empty string and copies str into it. You need to free() the result. */
char *cs_strdup(const char *str)
{
	char *newstr;
	if(!str)
		{ return NULL; }

	if(cs_malloc(&newstr, strlen(str) + 1))
	{
		cs_strncpy(newstr, str, strlen(str) + 1);
		return newstr;
	}
	return NULL;
}

/* Ordinary strncpy does not terminate the string if the source is exactly
   as long or longer as the specified size. This can raise security issues.
   This function is a replacement which makes sure that a \0 is always added.
   num should be the real size of char array (do not subtract -1). */
void cs_strncpy(char *destination, const char *source, size_t num)
{
	if(!source)
	{
		destination[0] = '\0';
		return;
	}

	uint32_t l, size = strlen(source);
	if(size > num - 1)
		{ l = num - 1; }
	else
		{ l = size; }

	memcpy(destination, source, l);
	destination[l] = '\0';
}

bool cs_strncat(char *destination, char *source, size_t destination_size)
{
	uint32_t dest_sz = 0;
	uint32_t source_sz = 0;

	if (!destination_size)
	{
		cs_log("ERROR, destination_size 0!");
		return false;
	}

	if (destination)
	{
		dest_sz += strlen(destination);
	}
	else
	{
		cs_log("ERROR, destination pointer NULL!");
		return false;
	}

	if (source)
	{
		source_sz += strlen(source);
	}
	else
	{
		cs_log("ERROR, source pointer NULL!");
		return false;
	}

	if ((dest_sz + source_sz) == 0)
	{
		cs_log("ERROR, booth destination and source with zero size!");
		return false;
	}

	if ((dest_sz + source_sz) < destination_size)
	{
		if (dest_sz)
		{
			void *dest = (void *)destination;
			memcpy(destination, dest, dest_sz);
		}

		if (source_sz)
			memcpy(destination + dest_sz, source, source_sz);

		destination[dest_sz + source_sz] = '\0';
	}
	else
	{
		cs_log("ERROR, buffer overflow!");
		return false;	
	}

	return true;
}

/* Converts the string txt to it's lower case representation. */
char *strtolower(char *txt)
{
	char *p;
	for(p = txt; *p; p++)
	{
		if(isupper((uint8_t)*p))
			{ *p = tolower((uint8_t) * p); }
	}
	return txt;
}

/* Converts the string txt to it's upper case representation. */
char *strtoupper(char *txt)
{
	char *p;
	for(p = txt; *p; p++)
	{
		if(islower((uint8_t)*p))
			{ *p = toupper((uint8_t)*p); }
	}
	return txt;
}

char *trim(char *txt)
{
	int32_t l;
	char *p1, *p2;

	if(*txt == ' ')
	{
		for(p1 = p2 = txt; (*p1 == ' ') || (*p1 == '\t') || (*p1 == '\n') || (*p1 == '\r'); p1++)
			{ ; }

		while(*p1)
			{ *p2++ = *p1++; }

		*p2 = '\0';
	}

	l = strlen(txt);
	if(l > 0)
	{
		for(p1 = txt + l - 1; l > 0 && ((*p1 == ' ') || (*p1 == '\t') || (*p1 == '\n') || (*p1 == '\r')); *p1-- = '\0', l--)
			{ ; }
	}
	return txt;
}

char *trim2(char *txt)
{
	int32_t i, n;

	for(i = n = 0; i < (int32_t)strlen(txt); i++)
	{
		if(txt[i] == ' ' || txt[i] == '\t') continue;
		if(txt[i] == '#') {break;}
		txt[n] = txt[i];
		n++;
	}
	txt[n] = '\0';

	return txt;
}

char *remove_white_chars(char *txt)
{
	char *p1 = txt, *p2 = txt;

	if(NULL != p1)
	{
		while('\0' != *p1)
		{
			if((' ' != *p1) && ('\t' != *p1) && ('\n' != *p1) && ('\r' != *p1))
			{
				*p2++ = *p1;
			}
			p1++;
		}
		*p2 = '\0';
	}
	return txt;
}

bool streq(const char *s1, const char *s2)
{
	if(!s1 && s2) { return 0; }
	if(s1 && !s2) { return 0; }
	if(!s1 && !s2) { return 1; }
	return strcmp(s1, s2) == 0;
}

char *cs_hexdump(int32_t m, const uint8_t *buf, int32_t n, char *target, int32_t len)
{
	int32_t i = 0;
	if(target == NULL || buf == NULL)
	{
		return NULL;
	}
	target[0] = '\0';
	m = m ? 3 : 2;

	if(m * n >= len)
	{
		n = (len / m) - 1;
	}

	while(i < n)
	{
		snprintf(target + (m * i), len - (m * i), "%02X%s", *buf++, m > 2 ? " " : "");
		i++;
	}
	return target;
}

/*
 * For using gethexval we must check if char c is within range othervise gethexval return
 * negative value so we must ensure we not shift left those negative value! So before
 * using gethexval function you need to check char c with function gethexval_within_range to
 * ensure char c is within accepted range!
 */
bool gethexval_within_range(char c)
{
	if((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
	{
		return true;
	}
	return false;
}

int32_t gethexval(char c)
{
	if(c >= '0' && c <= '9') { return c - '0'; }
	if(c >= 'A' && c <= 'F') { return c - 'A' + 10; }
	if(c >= 'a' && c <= 'f') { return c - 'a' + 10; }
	return -1;
}

int32_t cs_atob(uint8_t *buf, char *asc, int32_t n)
{
	int32_t i, rc;
	if(buf == NULL || asc == NULL)
	{
		return -1;
	}
	for(i = 0; i < n; i++)
	{
		if(!gethexval_within_range(asc[i << 1]) || !gethexval_within_range(asc[(i << 1) + 1]))
		{
			return -1;
		}
		rc = (gethexval(asc[i << 1]) << 4) | gethexval(asc[(i << 1) + 1]);
		if(rc & 0x100)
		{
			return -1;
		}
		buf[i] = rc;
	}
	return n;
}

uint32_t cs_atoi(char *asc, int32_t l, int32_t val_on_err)
{
	int32_t i, n = 0;
	uint32_t rc = 0;
	if(asc == NULL)
	{
			errno = EINVAL;
			rc = val_on_err ? 0xFFFFFFFF : 0;
			return rc;
	}
	for(i = ((l - 1) << 1), errno = 0; i >= 0 && n < 4; i -= 2)
	{
		if(!gethexval_within_range(asc[i]) || !gethexval_within_range(asc[i + 1]))
		{
			errno = EINVAL;
			rc = val_on_err ? 0xFFFFFFFF : 0;
			break;
		}
		int32_t b = (gethexval(asc[i]) << 4) | gethexval(asc[i + 1]);
		if(b < 0)
		{
			errno = EINVAL;
			rc = val_on_err ? 0xFFFFFFFF : 0;
			break;
		}
		rc |= b << (n << 3);
		n++;
	}
	return rc;
}

int32_t byte_atob(char *asc)
{
	int32_t rc = (-1);
	if(asc == NULL)
	{
		return rc;
	}
	if(strlen(trim(asc)) != 2)
	{
		rc = -1;
	}
	else
	{
		if(!gethexval_within_range(asc[0]) || !gethexval_within_range(asc[1]))
		{
			return rc;
		}
		else
		{
			rc = (gethexval(asc[0]) << 4) | gethexval(asc[1]);
			if(rc & 0x100)
			{
				rc = -1;
			}
		}
	}
	return rc;
}

int32_t word_atob(char *asc)
{
	int32_t rc = (-1);

	if(asc == NULL)
	{
		return rc;
	}

	if(strlen(trim(asc)) != 4)
	{
		rc = -1;
	}
	else
	{
		if(!gethexval_within_range(asc[0]) || !gethexval_within_range(asc[1]) || !gethexval_within_range(asc[2]) || !gethexval_within_range(asc[3]))
		{
			return rc;
		}
		else
		{
			rc = gethexval(asc[0]) << 12 | gethexval(asc[1]) << 8 | gethexval(asc[2]) << 4  | gethexval(asc[3]);
			if(rc & 0x10000)
			{
				rc = -1;
			}
		}
	}
	return rc;
}

/*
 * dynamic word_atob
 * converts an 1-6 digit asc hexstring
 */
int32_t dyn_word_atob(char *asc)
{
	int32_t rc = (-1);
	int32_t i, len;

	if(asc == NULL)
	{
		return rc;
	}
	len = strlen(trim(asc));
	if(len <= 6 && len > 0)
	{
		for(i = 0, rc = 0; i < len; i++)
		{
			if(!gethexval_within_range(asc[0]) || !gethexval_within_range(asc[1]) || !gethexval_within_range(asc[2]) || !gethexval_within_range(asc[3]))
			{
				return -1;
			}
			rc = rc << 4 | gethexval(asc[i]);
		}
		if(rc & 0x1000000)
		{
			rc = -1;
		}
	}
	return rc;
}

int32_t key_atob_l(char *asc, uint8_t *bin, int32_t l)
{
	int32_t i, n1, n2, rc;

	if(asc == NULL || bin == NULL)
	{
		return -1;
	}
	for(i = rc = 0; i < l; i += 2)
	{
		if(!gethexval_within_range(asc[i]) || !gethexval_within_range(asc[i + 1]))
		{
			rc = -1;
		}
		else
		{
			n1 = gethexval(asc[i]);
			n2 = gethexval(asc[i + 1]);
			bin[i >> 1] = (n1 << 4) + (n2 & 0xff);
		}
	}
	return rc;
}

uint32_t b2i(int32_t n, const uint8_t *b)
{
	if(b == NULL)
	{
		return 0;
	}
	switch(n)
	{
		case 1:
			return b[0];
		case 2:
			return (b[0] << 8) | b[1];

		case 3:
			return (b[0] << 16) | (b[1] << 8) | b[2];

		case 4:
			return ((b[0] << 24) | (b[1] << 16) | (b[2] << 8) | b[3]) & 0xffffffffL;

		default:
			cs_log("Error in b2i, n=%i", n);
	}
	return 0;
}

uint64_t b2ll(int32_t n, const uint8_t *b)
{
	int32_t i;
	uint64_t k = 0;
	if(b == NULL)
	{
		return k;
	}
	for(i = 0; i < n; k += b[i++])
	{
		k <<= 8;
	}
	return k;
}

uint8_t *i2b_buf(int32_t n, uint32_t i, uint8_t *b)
{
	switch(n)
	{
		case 2:
			b[0] = (i >> 8) & 0xff;
			b[1] = (i) & 0xff;
			break;

		case 3:
			b[0] = (i >> 16) & 0xff;
			b[1] = (i >> 8) & 0xff;
			b[2] = (i) & 0xff;
			break;

		case 4:
			b[0] = (i >> 24) & 0xff;
			b[1] = (i >> 16) & 0xff;
			b[2] = (i >> 8) & 0xff;
			b[3] = (i) & 0xff;
			break;
	}
	return b;
}

uint8_t *ull2b_buf(uint64_t i, uint8_t *b)
{
	b[0] = (i >> 56) & 0xff;
	b[1] = (i >> 48) & 0xff;
	b[2] = (i >> 40) & 0xff;
	b[3] = (i >> 32) & 0xff;
	b[4] = (i >> 24) & 0xff;
	b[5] = (i >> 16) & 0xff;
	b[6] = (i >> 8) & 0xff;
	b[7] = (i) & 0xff;
	return b;
}

uint32_t a2i(char *asc, int32_t bytes)
{
	int32_t i, n;
	uint32_t rc;
	if(asc == NULL)
	{
		errno = EINVAL;
		return 0x1f1f1f;
	}
	for(rc = i = 0, n = strlen(trim(asc)) - 1; i < abs(bytes) << 1; n--, i++)
	{
		if(n >= 0)
		{
			int32_t rcl;
			if(!gethexval_within_range(asc[n]))
			{
				errno = EINVAL;
				return 0x1f1f1f;
			}
			else
			{
				rcl = gethexval(asc[n]);
				rc |= rcl << (i << 2);
			}
		}
		else
		{
			if(bytes < 0)
			{
				rc |= 0xf << (i << 2);
			}
		}
	}
	errno = 0;
	return rc;
}

int32_t boundary(int32_t exp, int32_t n)
{
	return (((n - 1) >> exp) + 1) << exp;
}

/* Checks whether an array has at least one non-zero byte.
   length specifies the maximum length to check for. */
int32_t array_has_nonzero_byte(uint8_t *value, int32_t length)
{
	if(!value)
	{
		return 0;
	}

	int32_t i;
	for(i = 0; i < length; i++)
	{
		if(value[i] > 0)
		{
			return 1;
		}
	}
	return 0;
}

#define RAND_POOL_SIZE 64

// The last bytes are used to init random seed
static uint8_t rand_pool[RAND_POOL_SIZE + sizeof(uint32_t)];

void get_random_bytes_init(void)
{
	srand(time(NULL));
	int fd = open("/dev/urandom", O_RDONLY);

	if(fd < 0)
	{
		fd = open("/dev/random", O_RDONLY);
		if(fd < 0)
		{
			return;
		}
	}

	if(read(fd, rand_pool, RAND_POOL_SIZE + sizeof(uint32_t)) > -1)
	{
		uint32_t pool_seed = b2i(4, rand_pool + RAND_POOL_SIZE);
		srand(pool_seed);
	}
	close(fd);
}

void get_random_bytes(uint8_t *dst, uint32_t dst_len)
{
	static uint32_t rand_pool_pos; // *MUST* be static
	uint32_t i;

	for(i = 0; i < dst_len; i++)
	{
		rand_pool_pos++; // Races are welcome...
		dst[i] = rand() ^ rand_pool[rand_pool_pos % RAND_POOL_SIZE];
	}
}

static uint32_t crc_table[256] =
{
	0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
	0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
	0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
	0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
	0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
	0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
	0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
	0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
	0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
	0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
	0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
	0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
	0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
	0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
	0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
	0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
	0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
	0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
	0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
	0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
	0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
	0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
	0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
	0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
	0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
	0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
	0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
	0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
	0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
	0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
	0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
	0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
	0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
	0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
	0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
	0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
	0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
	0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
	0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
	0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
	0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
	0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
	0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
	0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
	0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
	0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
	0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
	0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
	0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
	0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
	0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
	0x2d02ef8dL
};

/*
 * crc32 -- compute the CRC-32 of a data stream
 * Copyright (C) 1995-1996 Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h
 */
#define DO1(buf) crc = crc_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
#define DO2(buf) DO1(buf); DO1(buf);
#define DO4(buf) DO2(buf); DO2(buf);
#define DO8(buf) DO4(buf); DO4(buf);

uint32_t crc32(uint32_t crc, const uint8_t *buf, uint32_t len)
{
	if(!buf)
	{
		return 0L;
	}
	crc = crc ^ 0xffffffffL;
	while(len >= 8)
	{
		DO8(buf);
		len -= 8;
	}
	if(len)
	{
		do
		{
			DO1(buf);
		}
		while(--len);
	}
	return crc ^ 0xffffffffL;
}

static uint16_t ccitt_crc_table [256] =
{
	0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5,
	0x60c6, 0x70e7, 0x8108, 0x9129, 0xa14a, 0xb16b,
	0xc18c, 0xd1ad, 0xe1ce, 0xf1ef, 0x1231, 0x0210,
	0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
	0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c,
	0xf3ff, 0xe3de, 0x2462, 0x3443, 0x0420, 0x1401,
	0x64e6, 0x74c7, 0x44a4, 0x5485, 0xa56a, 0xb54b,
	0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
	0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6,
	0x5695, 0x46b4, 0xb75b, 0xa77a, 0x9719, 0x8738,
	0xf7df, 0xe7fe, 0xd79d, 0xc7bc, 0x48c4, 0x58e5,
	0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
	0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969,
	0xa90a, 0xb92b, 0x5af5, 0x4ad4, 0x7ab7, 0x6a96,
	0x1a71, 0x0a50, 0x3a33, 0x2a12, 0xdbfd, 0xcbdc,
	0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
	0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03,
	0x0c60, 0x1c41, 0xedae, 0xfd8f, 0xcdec, 0xddcd,
	0xad2a, 0xbd0b, 0x8d68, 0x9d49, 0x7e97, 0x6eb6,
	0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
	0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a,
	0x9f59, 0x8f78, 0x9188, 0x81a9, 0xb1ca, 0xa1eb,
	0xd10c, 0xc12d, 0xf14e, 0xe16f, 0x1080, 0x00a1,
	0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
	0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c,
	0xe37f, 0xf35e, 0x02b1, 0x1290, 0x22f3, 0x32d2,
	0x4235, 0x5214, 0x6277, 0x7256, 0xb5ea, 0xa5cb,
	0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
	0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447,
	0x5424, 0x4405, 0xa7db, 0xb7fa, 0x8799, 0x97b8,
	0xe75f, 0xf77e, 0xc71d, 0xd73c, 0x26d3, 0x36f2,
	0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
	0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9,
	0xb98a, 0xa9ab, 0x5844, 0x4865, 0x7806, 0x6827,
	0x18c0, 0x08e1, 0x3882, 0x28a3, 0xcb7d, 0xdb5c,
	0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
	0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0,
	0x2ab3, 0x3a92, 0xfd2e, 0xed0f, 0xdd6c, 0xcd4d,
	0xbdaa, 0xad8b, 0x9de8, 0x8dc9, 0x7c26, 0x6c07,
	0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
	0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba,
	0x8fd9, 0x9ff8, 0x6e17, 0x7e36, 0x4e55, 0x5e74,
	0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};

uint16_t ccitt_crc(uint8_t *data, size_t length, uint16_t seed, uint16_t final)
{
	size_t count;
	uint32_t crc = seed;
	uint32_t temp;

	for (count = 0; count < length; ++count)
	{
		temp = (*data++ ^ (crc >> 8)) & 0xff;
		crc = ccitt_crc_table[temp] ^ (crc << 8);
	}
	return (uint16_t)(crc ^ final);
}

static const uint32_t ccitt32_crc_table[256] =
{
	0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9, 0x130476dc, 0x17c56b6b, 0x1a864db2, 0x1e475005,
	0x2608edb8, 0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61, 0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd,
	0x4c11db70, 0x48d0c6c7, 0x4593e01e, 0x4152fda9, 0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75,
	0x6a1936c8, 0x6ed82b7f, 0x639b0da6, 0x675a1011, 0x791d4014, 0x7ddc5da3, 0x709f7b7a, 0x745e66cd,
	0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039, 0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5,
	0xbe2b5b58, 0xbaea46ef, 0xb7a96036, 0xb3687d81, 0xad2f2d84, 0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d,
	0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49, 0xc7361b4c, 0xc3f706fb, 0xceb42022, 0xca753d95,
	0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1, 0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d,
	0x34867077, 0x30476dc0, 0x3d044b19, 0x39c556ae, 0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072,
	0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16, 0x018aeb13, 0x054bf6a4, 0x0808d07d, 0x0cc9cdca,
	0x7897ab07, 0x7c56b6b0, 0x71159069, 0x75d48dde, 0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02,
	0x5e9f46bf, 0x5a5e5b08, 0x571d7dd1, 0x53dc6066, 0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
	0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e, 0xbfa1b04b, 0xbb60adfc, 0xb6238b25, 0xb2e29692,
	0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6, 0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a,
	0xe0b41de7, 0xe4750050, 0xe9362689, 0xedf73b3e, 0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2,
	0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686, 0xd5b88683, 0xd1799b34, 0xdc3abded, 0xd8fba05a,
	0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637, 0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb,
	0x4f040d56, 0x4bc510e1, 0x46863638, 0x42472b8f, 0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53,
	0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47, 0x36194d42, 0x32d850f5, 0x3f9b762c, 0x3b5a6b9b,
	0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff, 0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623,
	0xf12f560e, 0xf5ee4bb9, 0xf8ad6d60, 0xfc6c70d7, 0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b,
	0xd727bbb6, 0xd3e6a601, 0xdea580d8, 0xda649d6f, 0xc423cd6a, 0xc0e2d0dd, 0xcda1f604, 0xc960ebb3,
	0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7, 0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b,
	0x9b3660c6, 0x9ff77d71, 0x92b45ba8, 0x9675461f, 0x8832161a, 0x8cf30bad, 0x81b02d74, 0x857130c3,
	0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640, 0x4e8ee645, 0x4a4ffbf2, 0x470cdd2b, 0x43cdc09c,
	0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8, 0x68860bfd, 0x6c47164a, 0x61043093, 0x65c52d24,
	0x119b4be9, 0x155a565e, 0x18197087, 0x1cd86d30, 0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
	0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088, 0x2497d08d, 0x2056cd3a, 0x2d15ebe3, 0x29d4f654,
	0xc5a92679, 0xc1683bce, 0xcc2b1d17, 0xc8ea00a0, 0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c,
	0xe3a1cbc1, 0xe760d676, 0xea23f0af, 0xeee2ed18, 0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4,
	0x89b8fd09, 0x8d79e0be, 0x803ac667, 0x84fbdbd0, 0x9abc8bd5, 0x9e7d9662, 0x933eb0bb, 0x97ffad0c,
	0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668, 0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4,
};

uint32_t ccitt32_crc(uint8_t *data, size_t len)
{
	uint32_t crc = 0xffffffff;
	while(len--)
	{
		crc = ((crc << 8) & 0xffffff00) ^ ccitt32_crc_table[((crc >> 24) & 0xff) ^ *data++];
	}
	return(crc);
}

// https://en.wikipedia.org/wiki/Jenkins_hash_function
uint32_t jhash(const char *key, size_t len)
{
	uint32_t hash, i;
	for(hash = i = 0; i < len; i++)
	{
		hash += key[i];
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}
	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);
	return hash;
}

/* Converts a char to it's hex representation. See char_to_hex on how to use it. */
char to_hex(char code)
{
	static const char hex[] = "0123456789abcdef";
	return hex[(int)code & 15];
}

/* Converts a char array to a char array with hex values (needed for example for md5).
   Note that result needs to be at least (p_array_len * 2) + 1 large. */
void char_to_hex(const uint8_t *p_array, uint32_t p_array_len, uint8_t *result)
{
	result[p_array_len * 2] = '\0';
	const uint8_t *p_end = p_array + p_array_len;
	uint32_t pos = 0;
	const uint8_t *p;

	for(p = p_array; p != p_end; p++, pos += 2)
	{
		result[pos] = to_hex(*p >> 4);
		result[pos + 1] = to_hex(*p & 15);
	}
}

static inline uint8_t to_uchar(char ch)
{
	return ch;
}

void base64_encode(const char *in, size_t inlen, char *out, size_t outlen)
{
	static const char b64str[64] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	while(inlen && outlen)
	{
		*out++ = b64str[(to_uchar(in[0]) >> 2) & 0x3f];
		if(!--outlen) { break; }
		*out++ = b64str[((to_uchar(in[0]) << 4) + (--inlen ? to_uchar(in[1]) >> 4 : 0)) & 0x3f];
		if(!--outlen) { break; }
		*out++ = (inlen ? b64str[((to_uchar(in[1]) << 2) + (--inlen ? to_uchar(in[2]) >> 6 : 0)) & 0x3f] : '=');
		if(!--outlen) { break; }
		*out++ = inlen ? b64str[to_uchar(in[2]) & 0x3f] : '=';
		if(!--outlen) { break; }
		if(inlen) { inlen--; }
		if(inlen) { in += 3; }
		if(outlen) { *out = '\0'; }
	}
}

size_t b64encode(const char *in, size_t inlen, char **out)
{
	size_t outlen = 1 + BASE64_LENGTH(inlen);
	if(inlen > outlen)
	{
		*out = NULL;
		return 0;
	}
	if(!cs_malloc(out, outlen))
	{
		return -1;
	}
	base64_encode(in, inlen, *out, outlen);
	return outlen - 1;
}

static int8_t b64decoder[256];

/* Prepares the base64 decoding array */
void b64prepare(void)
{
	const uint8_t alphabet[64] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	int32_t i;

	for(i = sizeof(b64decoder) - 1; i >= 0; --i)
	{
		b64decoder[i] = -1;
	}

	for(i = sizeof(alphabet) - 1; i >= 0; --i)
	{
		b64decoder[alphabet[i]] = i;
	}
}

/* Decodes a base64-encoded string. The given array will be used directly for output and is thus modified! */
int32_t b64decode(uint8_t *result)
{
	int32_t i, len = strlen((char *)result), j = 0, bits = 0, char_count = 0;

	if(!b64decoder[0])
	{
		b64prepare();
	}

	for(i = 0; i < len; ++i)
	{
		if(result[i] == '=') { break; }
		int8_t tmp = b64decoder[result[i]];
		if(tmp == -1) { continue; }
		bits += tmp;
		++char_count;
		if(char_count == 4)
		{
			result[j++] = bits >> 16;
			result[j++] = (bits >> 8) & 0xff;
			result[j++] = bits & 0xff;
			bits = 0;
			char_count = 0;
		}
		else
		{
			bits <<= 6;
		}
	}
	if(i == len)
	{
		if(char_count)
		{
			result[j] = '\0';
			return 0;
		}
	}
	else
	{
		switch(char_count)
		{
			case 1:
				result[j] = '\0';
				return 0;
			case 2:
				result[j++] = bits >> 10;
				result[j] = '\0';
				break;
			case 3:
				result[j++] = bits >> 16;
				result[j++] = (bits >> 8) & 0xff;
				result[j] = '\0';
				break;
		}
	}
	return j;
}


#ifdef READ_SDT_CHARSETS

// ISO_6937 function taken from VLC
/*****************************************************************************
 * Local conversion routine from ISO_6937 to UTF-8 charset. Support for this
 * is still missing in libiconv, hence multiple operating systems lack it.
 * The conversion table adds Euro sign (0xA4) as per ETSI EN 300 468 Annex A
 *****************************************************************************/

static const uint16_t iso_6937_to_ucs4[128] =
{
	/* 0x80 */ 0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087,
	/* 0x88 */ 0x0088, 0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f,
	/* 0x90 */ 0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097,
	/* 0x98 */ 0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e, 0x009f,
	/* 0xa0 */ 0x00a0, 0x00a1, 0x00a2, 0x00a3, 0x20ac, 0x00a5, 0x0000, 0x00a7,
	/* 0xa8 */ 0x00a4, 0x2018, 0x201c, 0x00ab, 0x2190, 0x2191, 0x2192, 0x2193,
	/* 0xb0 */ 0x00b0, 0x00b1, 0x00b2, 0x00b3, 0x00d7, 0x00b5, 0x00b6, 0x00b7,
	/* 0xb8 */ 0x00f7, 0x2019, 0x201d, 0x00bb, 0x00bc, 0x00bd, 0x00be, 0x00bf,
	/* 0xc0 */ 0x0000, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
	/* 0xc8 */ 0xffff, 0x0000, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff, 0xffff,
	/* 0xd0 */ 0x2014, 0x00b9, 0x00ae, 0x00a9, 0x2122, 0x266a, 0x00ac, 0x00a6,
	/* 0xd8 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x215b, 0x215c, 0x215d, 0x215e,
	/* 0xe0 */ 0x2126, 0x00c6, 0x00d0, 0x00aa, 0x0126, 0x0000, 0x0132, 0x013f,
	/* 0xe8 */ 0x0141, 0x00d8, 0x0152, 0x00ba, 0x00de, 0x0166, 0x014a, 0x0149,
	/* 0xf0 */ 0x0138, 0x00e6, 0x0111, 0x00f0, 0x0127, 0x0131, 0x0133, 0x0140,
	/* 0xf8 */ 0x0142, 0x00f8, 0x0153, 0x00df, 0x00fe, 0x0167, 0x014b, 0x00ad
};

/* The outer array range runs from 0xc1 to 0xcf, the inner range from 0x40
   to 0x7f.  */
static const uint16_t iso_6937_to_ucs4_comb[15][64] =
{
/* 0xc1 */
	{
		/* 0x40 */ 0x0000, 0x00c0, 0x0000, 0x0000, 0x0000, 0x00c8, 0x0000, 0x0000,
		/* 0x48 */ 0x0000, 0x00cc, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x00d2,
		/* 0x50 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x00d9, 0x0000, 0x0000,
		/* 0x58 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		/* 0x60 */ 0x0000, 0x00e0, 0x0000, 0x0000, 0x0000, 0x00e8, 0x0000, 0x0000,
		/* 0x68 */ 0x0000, 0x00ec, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x00f2,
		/* 0x70 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x00f9, 0x0000, 0x0000,
		/* 0x78 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
	},
/* 0xc2 */
	{
		/* 0x40 */ 0x0000, 0x00c1, 0x0000, 0x0106, 0x0000, 0x00c9, 0x0000, 0x0000,
		/* 0x48 */ 0x0000, 0x00cd, 0x0000, 0x0000, 0x0139, 0x0000, 0x0143, 0x00d3,
		/* 0x50 */ 0x0000, 0x0000, 0x0154, 0x015a, 0x0000, 0x00da, 0x0000, 0x0000,
		/* 0x58 */ 0x0000, 0x00dd, 0x0179, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		/* 0x60 */ 0x0000, 0x00e1, 0x0000, 0x0107, 0x0000, 0x00e9, 0x0000, 0x0000,
		/* 0x68 */ 0x0000, 0x00ed, 0x0000, 0x0000, 0x013a, 0x0000, 0x0144, 0x00f3,
		/* 0x70 */ 0x0000, 0x0000, 0x0155, 0x015b, 0x0000, 0x00fa, 0x0000, 0x0000,
		/* 0x78 */ 0x0000, 0x00fd, 0x017a, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
	},
/* 0xc3 */
	{
		/* 0x40 */ 0x0000, 0x00c2, 0x0000, 0x0108, 0x0000, 0x00ca, 0x0000, 0x011c,
		/* 0x48 */ 0x0124, 0x00ce, 0x0134, 0x0000, 0x0000, 0x0000, 0x0000, 0x00d4,
		/* 0x50 */ 0x0000, 0x0000, 0x0000, 0x015c, 0x0000, 0x00db, 0x0000, 0x0174,
		/* 0x58 */ 0x0000, 0x0176, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		/* 0x60 */ 0x0000, 0x00e2, 0x0000, 0x0109, 0x0000, 0x00ea, 0x0000, 0x011d,
		/* 0x68 */ 0x0125, 0x00ee, 0x0135, 0x0000, 0x0000, 0x0000, 0x0000, 0x00f4,
		/* 0x70 */ 0x0000, 0x0000, 0x0000, 0x015d, 0x0000, 0x00fb, 0x0000, 0x0175,
		/* 0x78 */ 0x0000, 0x0177, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
	},
/* 0xc4 */
	{
		/* 0x40 */ 0x0000, 0x00c3, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		/* 0x48 */ 0x0000, 0x0128, 0x0000, 0x0000, 0x0000, 0x0000, 0x00d1, 0x00d5,
		/* 0x50 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0168, 0x0000, 0x0000,
		/* 0x58 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		/* 0x60 */ 0x0000, 0x00e3, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		/* 0x68 */ 0x0000, 0x0129, 0x0000, 0x0000, 0x0000, 0x0000, 0x00f1, 0x00f5,
		/* 0x70 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0169, 0x0000, 0x0000,
		/* 0x78 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
	},
/* 0xc5 */
	{
		/* 0x40 */ 0x0000, 0x0100, 0x0000, 0x0000, 0x0000, 0x0112, 0x0000, 0x0000,
		/* 0x48 */ 0x0000, 0x012a, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x014c,
		/* 0x50 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x016a, 0x0000, 0x0000,
		/* 0x58 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		/* 0x60 */ 0x0000, 0x0101, 0x0000, 0x0000, 0x0000, 0x0113, 0x0000, 0x0000,
		/* 0x68 */ 0x0000, 0x012b, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x014d,
		/* 0x70 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x016b, 0x0000, 0x0000,
		/* 0x78 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
	},
/* 0xc6 */
	{
		/* 0x40 */ 0x0000, 0x0102, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x011e,
		/* 0x48 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		/* 0x50 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x016c, 0x0000, 0x0000,
		/* 0x58 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		/* 0x60 */ 0x0000, 0x0103, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x011f,
		/* 0x68 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		/* 0x70 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x016d, 0x0000, 0x0000,
		/* 0x78 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
	},
/* 0xc7 */
	{
		/* 0x40 */ 0x0000, 0x0000, 0x0000, 0x010a, 0x0000, 0x0116, 0x0000, 0x0120,
		/* 0x48 */ 0x0000, 0x0130, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		/* 0x50 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		/* 0x58 */ 0x0000, 0x0000, 0x017b, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		/* 0x60 */ 0x0000, 0x0000, 0x0000, 0x010b, 0x0000, 0x0117, 0x0000, 0x0121,
		/* 0x68 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		/* 0x70 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		/* 0x78 */ 0x0000, 0x0000, 0x017c, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
	},
/* 0xc8 */
	{
		/* 0x40 */ 0x0000, 0x00c4, 0x0000, 0x0000, 0x0000, 0x00cb, 0x0000, 0x0000,
		/* 0x48 */ 0x0000, 0x00cf, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x00d6,
		/* 0x50 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x00dc, 0x0000, 0x0000,
		/* 0x58 */ 0x0000, 0x0178, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		/* 0x60 */ 0x0000, 0x00e4, 0x0000, 0x0000, 0x0000, 0x00eb, 0x0000, 0x0000,
		/* 0x68 */ 0x0000, 0x00ef, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x00f6,
		/* 0x70 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x00fc, 0x0000, 0x0000,
		/* 0x78 */ 0x0000, 0x00ff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
	},
/* 0xc9 */
	{
		0x0000,
	},
/* 0xca */
	{
		/* 0x40 */ 0x0000, 0x00c5, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		/* 0x48 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		/* 0x50 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x016e, 0x0000, 0x0000,
		/* 0x58 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		/* 0x60 */ 0x0000, 0x00e5, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		/* 0x68 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		/* 0x70 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x016f, 0x0000, 0x0000,
		/* 0x78 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
	},
/* 0xcb */
	{
		/* 0x40 */ 0x0000, 0x0000, 0x0000, 0x00c7, 0x0000, 0x0000, 0x0000, 0x0122,
		/* 0x48 */ 0x0000, 0x0000, 0x0000, 0x0136, 0x013b, 0x0000, 0x0145, 0x0000,
		/* 0x50 */ 0x0000, 0x0000, 0x0156, 0x015e, 0x0162, 0x0000, 0x0000, 0x0000,
		/* 0x58 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		/* 0x60 */ 0x0000, 0x0000, 0x0000, 0x00e7, 0x0000, 0x0000, 0x0000, 0x0123,
		/* 0x68 */ 0x0000, 0x0000, 0x0000, 0x0137, 0x013c, 0x0000, 0x0146, 0x0000,
		/* 0x70 */ 0x0000, 0x0000, 0x0157, 0x015f, 0x0163, 0x0000, 0x0000, 0x0000,
		/* 0x78 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
	},
/* 0xcc */
	{
		0x0000,
	},
/* 0xcd */
	{
		/* 0x40 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		/* 0x48 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0150,
		/* 0x50 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0170, 0x0000, 0x0000,
		/* 0x58 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		/* 0x60 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		/* 0x68 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0151,
		/* 0x70 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0171, 0x0000, 0x0000,
		/* 0x78 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
	},
/* 0xce */
	{
		/* 0x40 */ 0x0000, 0x0104, 0x0000, 0x0000, 0x0000, 0x0118, 0x0000, 0x0000,
		/* 0x48 */ 0x0000, 0x012e, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		/* 0x50 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0172, 0x0000, 0x0000,
		/* 0x58 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		/* 0x60 */ 0x0000, 0x0105, 0x0000, 0x0000, 0x0000, 0x0119, 0x0000, 0x0000,
		/* 0x68 */ 0x0000, 0x012f, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		/* 0x70 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0173, 0x0000, 0x0000,
		/* 0x78 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
	},
/* 0xcf */
	{
		/* 0x40 */ 0x0000, 0x0000, 0x0000, 0x010c, 0x010e, 0x011a, 0x0000, 0x0000,
		/* 0x48 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x013d, 0x0000, 0x0147, 0x0000,
		/* 0x50 */ 0x0000, 0x0000, 0x0158, 0x0160, 0x0164, 0x0000, 0x0000, 0x0000,
		/* 0x58 */ 0x0000, 0x0000, 0x017d, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		/* 0x60 */ 0x0000, 0x0000, 0x0000, 0x010d, 0x010f, 0x011b, 0x0000, 0x0000,
		/* 0x68 */ 0x0000, 0x0000, 0x0000, 0x0000, 0x013e, 0x0000, 0x0148, 0x0000,
		/* 0x70 */ 0x0000, 0x0000, 0x0159, 0x0161, 0x0165, 0x0000, 0x0000, 0x0000,
		/* 0x78 */ 0x0000, 0x0000, 0x017e, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
	}
};

size_t ISO6937toUTF8(const uint8_t **inbuf, size_t *inbytesleft, uint8_t **outbuf, size_t *outbytesleft)
{
	if(!inbuf || !(*inbuf))
	{
		return (size_t)(0); /* Reset state requested */
	}

	const uint8_t *iptr = *inbuf;
	const uint8_t *iend = iptr + *inbytesleft;
	uint8_t *optr = *outbuf;
	uint8_t *oend = optr + *outbytesleft;
	uint16_t ch;
	int err = 0;

	while (iptr < iend)
	{
		if(*iptr < 0x80)
		{
			if(optr >= oend)
			{
				err = E2BIG;
				break; /* No space in outbuf */
			}
			*optr++ = *iptr++;
			continue;
		}

		if (optr + 2 >= oend)
		{
			err = E2BIG;
			break; /* No space in outbuf for multibyte char */
		}
		ch = iso_6937_to_ucs4[*iptr - 0x80];

		if(ch == 0xffff)
		{
			/* Composed character */
			if (iptr + 1 >= iend)
			{
				err = EINVAL;
				break; /* No next character */
			}
			if (iptr[1] < 0x40 || iptr[1] >= 0x80 || !(ch = iso_6937_to_ucs4_comb[iptr[0] - 0xc1][iptr[1] - 0x40]))
			{
				err = EILSEQ;
				break; /* Illegal combination */
			}
			iptr += 2;
		}
		else
		{
			if (!ch)
			{
				err = EILSEQ;
				break;
			}
			iptr++;
		}

		if (ch < 0x800)
		{
			optr[1] = 0x80 | (ch & 0x3f);
			optr[0] = 0xc0 | (ch >> 6);
			optr +=2;
		}
		else
		{
			optr[2] = 0x80 | (ch & 0x3f);
			ch >>= 6;
			optr[1] = 0x80 | (ch & 0x3f);
			optr[0] = 0xe0 | (ch >> 6);
			optr += 3;
		}

	}
	*inbuf = iptr;
	*outbuf = optr;
	*inbytesleft = iend - iptr;
	*outbytesleft = oend - optr;
	if(err)
	{
		errno = err;
		return (size_t)(-1);
	}
	return (size_t)(0);
}

#include "oscam-string-isotables.h"

static const uint16_t *get_iso8859_table(int8_t iso_table_number)
{
	if(iso_table_number > 1 && iso_table_number < 17 && iso_table_number != 12)
	{
		if(iso_table_number < 12)
		{
			return iso_8859_to_unicode[iso_table_number - 2];
		}
		else
		{
			return iso_8859_to_unicode[iso_table_number - 3];
		}
	}
	else
	{
		return NULL;
	}
}

size_t ISO8859toUTF8(int8_t iso_table_number, const uint8_t **inbuf, size_t *inbytesleft, uint8_t **outbuf, size_t *outbytesleft)
{
	if(!inbuf || !(*inbuf))
	{
		return (size_t)(0); /* Reset state requested */
	}

	const uint8_t *iptr = *inbuf;
	const uint8_t *iend = iptr + *inbytesleft;
	uint8_t *optr = *outbuf;
	uint8_t *oend = optr + *outbytesleft;
	uint16_t ch;
	int err = 0;
	const uint16_t *iso_table = NULL;

	if( iso_table_number != 1 )
	{
		iso_table = get_iso8859_table(iso_table_number);
		if ( iso_table == NULL )
		{
			errno = EINVAL;
			return (size_t)(-1);
		}
	}

	while (iptr < iend)
	{
		if(*iptr < 0x80)
		{
			if(optr >= oend)
			{
				err = E2BIG;
				break; /* No space in outbuf */
			}
			*optr++ = *iptr++;
			continue;
		}

		if( iso_table_number == 1 || *iptr < 0xA1 )
		{
			ch = *iptr;
		}
		else
		{
			ch = iso_table[*iptr - 0xA1];
		}
		iptr++;

		if (ch < 0x80)
		{
			if (optr >= oend)
			{
				err = E2BIG;
				break; /* No space in outbuf for char */
			}
			optr[0] = ch & 0xff;
			optr += 1;
		}
		else if (ch < 0x800)
		{
			if (optr + 1 >= oend)
			{
				err = E2BIG;
				break; /* No space in outbuf for multibyte char */
			}
			optr[1] = 0x80 | (ch & 0x3f);
			optr[0] = 0xc0 | (ch >> 6);
			optr += 2;
		}
		else
		{
			if (optr + 2 >= oend)
			{
				err = E2BIG;
				break; /* No space in outbuf for multibyte char */
			}
			optr[2] = 0x80 | (ch & 0x3f);
			ch >>= 6;
			optr[1] = 0x80 | (ch & 0x3f);
			optr[0] = 0xe0 | (ch >> 6);
			optr += 3;
		}
	}
	*inbuf = iptr;
	*outbuf = optr;
	*inbytesleft = iend - iptr;
	*outbytesleft = oend - optr;

	if(err)
	{
		errno = err;
		return (size_t)(-1);
	}
	return (size_t)(0);
}

#endif

size_t UnicodetoUTF8(const uint8_t **inbuf, size_t *inbytesleft, uint8_t **outbuf, size_t *outbytesleft)
{
	if(!inbuf || !(*inbuf))
	{
		return (size_t)(0); /* Reset state requested */
	}

	const uint8_t *iptr = *inbuf;
	const uint8_t *iend = iptr + *inbytesleft;
	uint8_t*optr = *outbuf;
	uint8_t *oend = optr + *outbytesleft;
	uint16_t ch;
	int err = 0;

	while (iptr + 1 < iend)
	{
		ch = (iptr[0] << 8) | iptr[1];
		iptr += 2;

		if (ch < 0x80)
		{
			if (optr >= oend)
			{
				err = E2BIG;
				break; /* No space in outbuf for char */
			}
			optr[0] = ch & 0xff;
			optr += 1;
		}
		else if (ch < 0x800)
		{
			if (optr + 1 >= oend)
			{
				err = E2BIG;
				break; /* No space in outbuf for multibyte char */
			}
			optr[1] = 0x80 | (ch & 0x3f);
			optr[0] = 0xc0 | (ch >> 6);
			optr += 2;
		}
		else
		{
			if (optr + 2 >= oend)
			{
				err = E2BIG;
				break; /* No space in outbuf for multibyte char */
			}
			optr[2] = 0x80 | (ch & 0x3f);
			ch >>= 6;
			optr[1] = 0x80 | (ch & 0x3f);
			optr[0] = 0xe0 | (ch >> 6);
			optr += 3;
		}
	}

	*inbuf = iptr;
	*outbuf = optr;
	*inbytesleft = iend - iptr;
	*outbytesleft = oend - optr;

	if(err)
	{
		errno = err;
		return (size_t)(-1);
	}
	return (size_t)(0);
}
