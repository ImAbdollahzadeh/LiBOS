#ifndef _VIDEO_PLAYER__H__
#define _VIDEO_PLAYER__H__

#include "LiBOS_CORE.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#define BIT(X) (1<<(X))

static UINT_32 IMAGE_WIDTH  = 800;
static UINT_32 IMAGE_HEIGHT = 600;

extern void _uv_transform(float* result, float* Fuv, float back_dc);
extern void _ycc_to_rgb(float* y, float* cb, float* cr, float* r, float* g, float* b);
extern void _final_rgb_sorting(float* packed);
extern void _short_to_float(INT_16* s, float* f);
extern void _float_to_UINT_8(UINT_8* rgb_array, float* r, float* g, float* b);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#define ReadOneWord(SHORT_POINTER, FILE_BUFFER)    do { *SHORT_POINTER  = *(UINT_16*)(FILE_BUFFER); } while(0)
#define ReadOneByte(BYTE_POINTER, FILE_BUFFER)     do { *BYTE_POINTER   = *(UINT_8*)(FILE_BUFFER);  } while(0)
#define ReadOneDirectByte(_BYTE, FILE_BUFFER)      do { _BYTE           = *(UINT_8*)(FILE_BUFFER);  } while(0)
#define ReadOneDword(DWORD_POINTER, FILE_BUFFER)   do { *DWORD_POINTER  = *(UINT_32*)(FILE_BUFFER); } while(0)
#define ReadOneDqword(DQWORD_POINTER, FILE_BUFFER) do { *DQWORD_POINTER = *(UINT_64*)(FILE_BUFFER); } while(0)
#define Read_N_Byte(N_BYTE_POINTER, BYTES, FILE_BUFFER) do {       \
	UINT_32 ii;                                                    \
	UINT_8* __tmp = (UINT_8*)(N_BYTE_POINTER);                     \
	for (ii = 0; ii < BYTES; ii++)                                 \
	{                                                              \
		ReadOneByte(__tmp, (FILE_BUFFER));                         \
		__tmp++;                                                   \
		FILE_BUFFER = (void*)(PHYSICAL_ADDRESS(FILE_BUFFER) + 1);  \
	}                                                              \
} while(0)

#define SetBitInWord(_WORD, POS, VALUE) do {  \
	if (!VALUE) (_WORD) &= ~BIT(POS);         \
	else        (_WORD) |=  BIT(POS);         \
} while(0)

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

enum {
	JPG_SOI     = 0xD8FF,
	JPG_JFIF    = 0xE0FF,
	JPG_DQT     = 0xDBFF,
	JPG_DHT     = 0xC4FF,
	JPG_SOF     = 0xC0FF,
	JPG_SOS     = 0xDAFF,
	JPG_COMMENT = 0XFEFF,
	JPG_EOI     = 0xD9FF,
	JPG_EOB     = 0x00
};

typedef struct _JFIF {
	UINT_16 length;
	INT_8   identifier[5];
	UINT_16 version;
	UINT_8  units;
	UINT_16 Xdensity;
	UINT_16 Ydensity;
	UINT_8  Xthumbnail;
	UINT_8  Ythumbnail;
} __attribute__ ((packed)) JFIF;

typedef struct _SOF {
	UINT_16 length;
	UINT_8  P;
	UINT_16 height;
	UINT_16 width;
	UINT_8  components;
	UINT_8  c1ID;
	UINT_8  c1HVSample;
	UINT_8  c1QTable;
	UINT_8  c2ID;
	UINT_8  c2HVSample;
	UINT_8  c2QTable;
	UINT_8  c3ID;
	UINT_8  c3HVSample;
	UINT_8  c3QTable;
} __attribute__ ((packed)) SOF;

typedef struct _SOS {
	UINT_16 length;
	UINT_8  components;
	UINT_8  c1ID;
	UINT_8  c1HTable; /* DC (high nibble) and AC (low nibble) table numbers */
	UINT_8  c2ID;
	UINT_8  c2HTable;
	UINT_8  c3ID;
	UINT_8  c3HTable;
	UINT_8  Ss;
	UINT_8  Se;
	UINT_8  AhAl;
} __attribute__ ((packed)) SOS;

typedef struct _COMMENT {
	UINT_16  length;
	char*    string;
} __attribute__ ((packed)) COMMENT;

typedef struct _JPG {
	INT_8   y_ac_table[100];
	UINT_8  y_dc_table[100];
	UINT_8  Cbr_ac_table[100];
	UINT_8  Cbr_dc_table[100];
	UINT_8  y_q_table[64];
	UINT_8  C_q_table[64];
	UINT_32 width;
	UINT_32 height;
	void*   y_ac_htree;
	void*   y_dc_htree;
	void*   Cbr_ac_htree;
	void*   Cbr_dc_htree;
	void*   actual_data;
	UINT_32 actual_data_byte_count;
	UINT_32 number_of_mcu;
	INT_16** ptr_to_all_mcus_coefficients;
	float**  ptr_to_all_mcus_coefficients_float;
	float**  ptr_to_unordered_YCbCr_mcus;
	float**  ptr_to_r;
	float**  ptr_to_g;
	float**  ptr_to_b;
	UINT_8* rgb;
	UINT_8* compressed_data;
} JPG;

typedef struct _VIDEO_PLAYER {
	INT_8*  video_player_name;
	UINT_32 video_player_id;
	JPG*    jpg_object;
	UINT_8  reserved[4];
} __attribute__ ((packed)) VIDEO_PLAYER;

typedef struct _Tree {
	struct _Tree* left;
	struct _Tree* right;
	UINT_8        value;
	BOOL          leaf;
} __attribute__ ((packed)) Tree;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

UINT_32 regiser_video_player(VIDEO_PLAYER* video_player, UINT_8* framebuffer_pointer);
void    play                (VIDEO_PLAYER* video_player, INT_8* file_address);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#endif
