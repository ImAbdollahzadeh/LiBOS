#include "../include/VIDEO_PLAYER.h"
#include "../include/MEMORY.h"
#include "../include/PRINT.h"
#include "../include/FILE.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ GLOBAL AND STATIC VARIABLES

#define vpDebug FALSE

static UINT_8* tmp_video_player_jpg_object_compressed_data             = 0;
static INT_16* tmp_video_player_jpg_object_all_mcus_coefficients       = 0;
static float*  tmp_video_player_jpg_object_all_mcus_coefficients_float = 0;
static float*  tmp_video_player_jpg_object_unordered_YCbCr_mcus        = 0;
static float*  tmp_video_player_jpg_object_r                           = 0;
static float*  tmp_video_player_jpg_object_g                           = 0;
static float*  tmp_video_player_jpg_object_b                           = 0;
static UINT_8* rgb                                                     = 0;
static JPG*    jjpg                                                    = 0;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static Tree* new_node()
{
	Tree* tree = (Tree*)(Alloc(sizeof(Tree), 1, 1));
	tree->leaf = FALSE;
	return tree;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void assign_htree_values(Tree* node, UINT_8 current_level, UINT_8 target_level, UINT_8* loop_through_numbers, UINT_8 num_leafs, UINT_8* hValues, INT_32 offset)
{
	UINT_8 looper = *loop_through_numbers;

	if (current_level != target_level)
	{
		if (!node->left->leaf)
			assign_htree_values(node->left, current_level + 1, target_level, loop_through_numbers, num_leafs, hValues, offset);

		if (!node->right->leaf)
			assign_htree_values(node->right, current_level + 1, target_level, loop_through_numbers, num_leafs, hValues, offset);
	}

	else
	{
		node->left = new_node();
		if (looper < num_leafs)
		{
			node->left->leaf = TRUE;
			node->left->value = *(hValues + offset + looper);
			looper++;
		}

		node->right = new_node();
		if (looper < num_leafs)
		{
			node->right->leaf = TRUE;
			node->right->value = *(hValues + offset + looper);
			looper++;
		}

		*loop_through_numbers = looper;
	}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static Tree* construct_htree(void* table)
{
	Tree* root = new_node();

	UINT_8* number_of_code_of_length_1_to_16 = (UINT_8*)table;
	UINT_8* huff_values                      = (UINT_8*)((PHYSICAL_ADDRESS(table) + 16));
	UINT_8 loop_through_numbers_0            = 0;
	UINT_8 loop_through_numbers_1            = 0;
	UINT_8 loop_through_numbers_2            = 0;
	UINT_8 loop_through_numbers_3            = 0;
	UINT_8 loop_through_numbers_4            = 0;
	UINT_8 loop_through_numbers_5            = 0;
	UINT_8 loop_through_numbers_6            = 0;
	UINT_8 loop_through_numbers_7            = 0;
	UINT_8 loop_through_numbers_8            = 0;
	UINT_8 loop_through_numbers_9            = 0;
	UINT_8 loop_through_numbers_10           = 0;
	UINT_8 loop_through_numbers_11           = 0;
	UINT_8 loop_through_numbers_12           = 0;
	UINT_8 loop_through_numbers_13           = 0;
	UINT_8 loop_through_numbers_14           = 0;
	UINT_8 loop_through_numbers_15           = 0;
	INT_32 offset                            = 0;
	UINT_8 num_leaf                          = *number_of_code_of_length_1_to_16;

	assign_htree_values(root, 0, 0, &loop_through_numbers_0, num_leaf, huff_values, 0);
	num_leaf = *(number_of_code_of_length_1_to_16 + 1);
	assign_htree_values(root, 0, 1, &loop_through_numbers_1, num_leaf, huff_values, *number_of_code_of_length_1_to_16);
	offset += num_leaf;
	num_leaf = *(number_of_code_of_length_1_to_16 + 2);
	assign_htree_values(root, 0, 2, &loop_through_numbers_2, num_leaf, huff_values, offset);
	offset += num_leaf;
	num_leaf = *(number_of_code_of_length_1_to_16 + 3);
	assign_htree_values(root, 0, 3, &loop_through_numbers_3, num_leaf, huff_values, offset);
	offset += num_leaf;
	num_leaf = *(number_of_code_of_length_1_to_16 + 4);
	assign_htree_values(root, 0, 4, &loop_through_numbers_4, num_leaf, huff_values, offset);
	offset += num_leaf;
	num_leaf = *(number_of_code_of_length_1_to_16 + 5);
	assign_htree_values(root, 0, 5, &loop_through_numbers_5, num_leaf, huff_values, offset);
	offset += num_leaf;
	num_leaf = *(number_of_code_of_length_1_to_16 + 6);
	assign_htree_values(root, 0, 6, &loop_through_numbers_6, num_leaf, huff_values, offset);
	offset += num_leaf;
	num_leaf = *(number_of_code_of_length_1_to_16 + 7);
	assign_htree_values(root, 0, 7, &loop_through_numbers_7, num_leaf, huff_values, offset);
	offset += num_leaf;
	num_leaf = *(number_of_code_of_length_1_to_16 + 8);
	assign_htree_values(root, 0, 8, &loop_through_numbers_8, num_leaf, huff_values, offset);
	offset += num_leaf;
	num_leaf = *(number_of_code_of_length_1_to_16 + 9);
	assign_htree_values(root, 0, 9, &loop_through_numbers_9, num_leaf, huff_values, offset);
	offset += num_leaf;
	num_leaf = *(number_of_code_of_length_1_to_16 + 10);
	assign_htree_values(root, 0, 10, &loop_through_numbers_10, num_leaf, huff_values, offset);
	offset += num_leaf;
	num_leaf = *(number_of_code_of_length_1_to_16 + 11);
	assign_htree_values(root, 0, 11, &loop_through_numbers_11, num_leaf, huff_values, offset);
	offset += num_leaf;
	num_leaf = *(number_of_code_of_length_1_to_16 + 12);
	assign_htree_values(root, 0, 12, &loop_through_numbers_12, num_leaf, huff_values, offset);
	offset += num_leaf;
	num_leaf = *(number_of_code_of_length_1_to_16 + 13);
	assign_htree_values(root, 0, 13, &loop_through_numbers_13, num_leaf, huff_values, offset);
	offset += num_leaf;
	num_leaf = *(number_of_code_of_length_1_to_16 + 14);
	assign_htree_values(root, 0, 14, &loop_through_numbers_14, num_leaf, huff_values, offset);
	offset += num_leaf;
	num_leaf = *(number_of_code_of_length_1_to_16 + 15);
	assign_htree_values(root, 0, 15, &loop_through_numbers_15, num_leaf, huff_values, offset);

	return root;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static INT_16 decode_node_value_bits(UINT_8 cat, UINT_8* byte, INT_8* bit, INT_8** seek_pointer)
{
	INT_16 decoded = 0x0000;
	UINT_8  bbyte = *byte;
	INT_8   bbit = *bit;
	INT_8* sseek_pointer = *seek_pointer;
	UINT_8 category = cat;
	UINT_8  global_bit =  0;
	
	if (bbit & 0x80)
	{
		bbit = 7;
		ReadOneDirectByte(bbyte, sseek_pointer++);
	}

	global_bit = bbyte & BIT(bbit);

	while (category--)
	{
		SetBitInWord(decoded, category, (((global_bit) ? (bbyte) : (~bbyte)) & BIT(bbit)));

		bbit--;

		if (bbit & 0x80)
		{
			bbit = 7;
			ReadOneDirectByte(bbyte, sseek_pointer++);
		}
	}

	*byte = bbyte;
	*bit = bbit;
	*seek_pointer = sseek_pointer;

	return (global_bit) ? decoded : -decoded;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void IDCT(JPG* jpg)
{
	UINT_32 i, counter = 0;
	float* all_mcus_coefficients_float = *(jpg->ptr_to_all_mcus_coefficients_float);
	float* unordered_YCbCr_mcus        = *(jpg->ptr_to_unordered_YCbCr_mcus);

	// Y component
	_uv_transform(&(unordered_YCbCr_mcus[0]),   &(all_mcus_coefficients_float[0]),   0.0f);
	_uv_transform(&(unordered_YCbCr_mcus[64]),  &(all_mcus_coefficients_float[64]),  all_mcus_coefficients_float[0]);
	_uv_transform(&(unordered_YCbCr_mcus[128]), &(all_mcus_coefficients_float[128]), all_mcus_coefficients_float[64]);
	_uv_transform(&(unordered_YCbCr_mcus[192]), &(all_mcus_coefficients_float[192]), all_mcus_coefficients_float[128]);
	counter = 384;

	for (i = 0; i < jpg->number_of_mcu; i++)
	{
		_uv_transform(&(unordered_YCbCr_mcus[counter]), &(all_mcus_coefficients_float[counter]), all_mcus_coefficients_float[counter - 192]);
		counter += 64;
		_uv_transform(&(unordered_YCbCr_mcus[counter]), &(all_mcus_coefficients_float[counter]), all_mcus_coefficients_float[counter - 64]);
		counter += 64;
		_uv_transform(&(unordered_YCbCr_mcus[counter]), &(all_mcus_coefficients_float[counter]), all_mcus_coefficients_float[counter - 64]);
		counter += 64;
		_uv_transform(&(unordered_YCbCr_mcus[counter]), &(all_mcus_coefficients_float[counter]), all_mcus_coefficients_float[counter - 64]);
		counter += 192;
	}

	// Cb component
	_uv_transform(&(unordered_YCbCr_mcus[256]), &(all_mcus_coefficients_float[256]), 0.0f);
	counter = 640;

	for (i = 0; i < jpg->number_of_mcu; i++)
	{
		_uv_transform(&(unordered_YCbCr_mcus[counter]), &(all_mcus_coefficients_float[counter]), all_mcus_coefficients_float[counter - 384]);
		counter += 384;
	}

	// Cr component
	_uv_transform(&(unordered_YCbCr_mcus[320]), &(all_mcus_coefficients_float[320]), 0.0f);
	counter = 704;

	for (i = 0; i < jpg->number_of_mcu; i++)
	{
		_uv_transform(&(unordered_YCbCr_mcus[counter]), &(all_mcus_coefficients_float[counter]), all_mcus_coefficients_float[counter - 384]);
		counter += 384;
	}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void ycc_to_rgb(JPG* jpg)
{
	UINT_32 macroblock_counter = 0;
	UINT_32 counter            = 0;
	UINT_32 mcus               = jpg->number_of_mcu;
	UINT_32 mcux               = IMAGE_WIDTH >> 4;
	UINT_32 ccounter           = 0;
	UINT_32 w                  = 0;
	UINT_32 h                  = 0;

	float* r = *(jpg->ptr_to_r);
	float* g = *(jpg->ptr_to_g);
	float* b = *(jpg->ptr_to_b);
	float* unordered_YCbCr_mcus = *(jpg->ptr_to_unordered_YCbCr_mcus);
	float* r_origin = &(r[(h*mcux * 256) + (w * 16)]);
	float* g_origin = &(g[(h*mcux * 256) + (w * 16)]);
	float* b_origin = &(b[(h*mcux * 256) + (w * 16)]);

	while (macroblock_counter < mcus)
	{
		do {
			_ycc_to_rgb(&(unordered_YCbCr_mcus[ccounter + 64]),
				&(unordered_YCbCr_mcus[ccounter + 256]),
				&(unordered_YCbCr_mcus[ccounter + 320]),
				&(r_origin[counter * (mcux * 16) + 8]),
				&(g_origin[counter * (mcux * 16) + 8]),
				&(b_origin[counter * (mcux * 16) + 8]));

			_ycc_to_rgb(&(unordered_YCbCr_mcus[ccounter]),
				&(unordered_YCbCr_mcus[ccounter + 256]),
				&(unordered_YCbCr_mcus[ccounter + 320]),
				&(r_origin[counter * (mcux * 16)]),
				&(g_origin[counter * (mcux * 16)]),
				&(b_origin[counter * (mcux * 16)]));

			_ycc_to_rgb(&(unordered_YCbCr_mcus[ccounter + 192]),
				&(unordered_YCbCr_mcus[ccounter + 256]),
				&(unordered_YCbCr_mcus[ccounter + 320]),
				&(r_origin[counter * (mcux * 16) + 8 + (mcux * 128)]),
				&(g_origin[counter * (mcux * 16) + 8 + (mcux * 128)]),
				&(b_origin[counter * (mcux * 16) + 8 + (mcux * 128)]));

			_ycc_to_rgb(&(unordered_YCbCr_mcus[ccounter + 128]),
				&(unordered_YCbCr_mcus[ccounter + 256]),
				&(unordered_YCbCr_mcus[ccounter + 320]),
				&(r_origin[counter * (mcux * 16) + (mcux * 128)]),
				&(g_origin[counter * (mcux * 16) + (mcux * 128)]),
				&(b_origin[counter * (mcux * 16) + (mcux * 128)]));

			counter++;
			ccounter += 8;
		} while (counter < 8);

		ccounter += (64 * 5);
		counter = 0;
		macroblock_counter++;

		w++;
		if (w >= mcux)
		{
			w = 0;
			h++;
		}

		r_origin = &(r[(h*mcux * 256) + (w * 16)]);
		g_origin = &(g[(h*mcux * 256) + (w * 16)]);
		b_origin = &(b[(h*mcux * 256) + (w * 16)]);
	}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void construct_final_rgb_values(JPG* jpg)
{
	UINT_32 t  = (IMAGE_WIDTH * IMAGE_HEIGHT * 3);
	UINT_32 final_counter = 0;
	
	float* r = *(jpg->ptr_to_r);
	float* g = *(jpg->ptr_to_g);
	float* b = *(jpg->ptr_to_b);

	do {
		_final_rgb_sorting(&(r[final_counter]));
		_final_rgb_sorting(&(g[final_counter]));
		_final_rgb_sorting(&(b[final_counter]));

		final_counter += 12;
	} while (final_counter < t);
	
	UINT_32 i = 4;
	UINT_32 screen_pixels = IMAGE_HEIGHT * IMAGE_WIDTH;
	UINT_32 tmp = 16;
	UINT_8* jpg_rgb = jpg->rgb;

	UINT_32 j = 0;

	_float_to_UINT_8(&(jpg_rgb[0]), &(r[0]), &(g[0]), &(b[0]));
	while(i < screen_pixels)
	{
		_float_to_UINT_8(&(jpg_rgb[tmp]), &(r[i]), &(g[i]), &(b[i]));
		if((i % IMAGE_WIDTH) == 0)
		{
			j++;
			tmp = (j*4096);
		}
		else
			tmp += 16;
		i+=4;
	}
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void decode_component_trees(void* dc, void* ac, void* q, INT_16* uncompressed, INT_8** seek_pointer, UINT_8* byte, INT_8* bit_position)
{
	UINT_8* qTable = (UINT_8*)q;
	UINT_8  value;
	INT_32  i = 1;
	INT_8* sseek_pointer = *seek_pointer;
	INT_8  bbit_position = 0;
	UINT_8 bbyte = 0;
	
	Tree* node = (Tree*)dc;
	Tree* ac_  = (Tree*)ac;

	while (!(node->leaf))
	{
		if (*bit_position & 0x80)
		{
			*bit_position = 7;
			ReadOneByte(byte, sseek_pointer++);
		}

		node = ((*byte) & BIT(*bit_position)) ? node->right : node->left;
		(*bit_position)--;
	}

	uncompressed[0] = decode_node_value_bits(node->value, byte, bit_position, &sseek_pointer) * (*qTable);

	bbit_position = *bit_position;
	bbyte = *byte;

	while (i < 0x40)
	{
		node = ac_;

		while (!(node->leaf))
		{
			if (bbit_position & 0x80)
			{
				bbit_position = 7;
				ReadOneDirectByte(bbyte, sseek_pointer++);
			}

			node = (bbyte & BIT(bbit_position)) ? node->right : node->left;
			bbit_position--;
		}

		value = node->value;

		if (!value)
			break;

		else
		{
			i += (value >> 4);
			uncompressed[i] = decode_node_value_bits((value & 0x0F), &bbyte, &bbit_position, &sseek_pointer) * *(qTable + i);
		}

		i++;
	}

	*seek_pointer = sseek_pointer;
	*byte = bbyte;
	*bit_position = bbit_position;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void decode_yyyy_cc(JPG* jpg)
{
	UINT_32 i                           = 0;
	UINT_8  byte                        = 0;
	INT_8   bit_position                = -1;
	INT_8** actual_data                 = (INT_8**)(&jpg->actual_data);
	UINT_32 global_mcu_counter          = 0;
	void* y_dc_htree                    = jpg->y_dc_htree;
	void* y_ac_htree                    = jpg->y_ac_htree;
	void* y_q_table                     = jpg->y_q_table;
	void* Cbr_dc_htree                  = jpg->Cbr_dc_htree;
	void* Cbr_ac_htree                  = jpg->Cbr_ac_htree;
	void* C_q_table                     = jpg->C_q_table;
	INT_16* all_mcus_coefficients       = *(jpg->ptr_to_all_mcus_coefficients);
	float*  all_mcus_coefficients_float = *(jpg->ptr_to_all_mcus_coefficients_float);
	UINT_32 number_of_mcu               = jpg->number_of_mcu;

	while (i < number_of_mcu)
	{
		decode_component_trees(y_dc_htree, y_ac_htree, y_q_table, &all_mcus_coefficients[global_mcu_counter], actual_data, &byte, &bit_position);
		decode_component_trees(y_dc_htree, y_ac_htree, y_q_table, &all_mcus_coefficients[global_mcu_counter + 64], actual_data, &byte, &bit_position);
		decode_component_trees(y_dc_htree, y_ac_htree, y_q_table, &all_mcus_coefficients[global_mcu_counter + 128], actual_data, &byte, &bit_position);
		decode_component_trees(y_dc_htree, y_ac_htree, y_q_table, &all_mcus_coefficients[global_mcu_counter + 192], actual_data, &byte, &bit_position);
		decode_component_trees(Cbr_dc_htree, Cbr_ac_htree, C_q_table, &all_mcus_coefficients[global_mcu_counter + 256], actual_data, &byte, &bit_position);
		decode_component_trees(Cbr_dc_htree, Cbr_ac_htree, C_q_table, &all_mcus_coefficients[global_mcu_counter + 320], actual_data, &byte, &bit_position);

		global_mcu_counter += 384;
		i++;
	}

	i = 0;
	do {
		_short_to_float(&(all_mcus_coefficients[i]), &(all_mcus_coefficients_float[i]));
		i += 8;
	} while (i < global_mcu_counter);

	IDCT                      (jpg);
	ycc_to_rgb                (jpg);
	construct_final_rgb_values(jpg);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void populate_jpg_info_from_file(JPG* jpg, UINT_8* buff)
{
	void*   seek_pointer = (void*)buff;
	UINT_16 next_word = 0x0000;

	ReadOneWord((&next_word), (seek_pointer));

	if (next_word != JPG_SOI)
	{
		printk("Error: Not a jpeg file\n");
		return;
	}

	seek_pointer = (void*)(PHYSICAL_ADDRESS(seek_pointer) + 2);

	while (TRUE)
	{
		ReadOneWord((&next_word), (seek_pointer));
		if (next_word == JPG_JFIF)
		{
			seek_pointer = (void*)(PHYSICAL_ADDRESS(seek_pointer) + 2);
			UINT_16 length = 0x0000;

			ReadOneWord((&length), (seek_pointer));
			length = (((length & 0xFF00) >> 8) | ((length & 0x00FF) << 8));

			JFIF jfif;

			Read_N_Byte((&jfif), length, seek_pointer);
		}

		else if (next_word == JPG_DQT)
		{
			seek_pointer = (void*)(PHYSICAL_ADDRESS(seek_pointer) + 2);

			UINT_16 length = 0x0000;

			ReadOneWord((&length), (seek_pointer));
			length = (((length & 0xFF00) >> 8) | ((length & 0x00FF) << 8));

			seek_pointer = (void*)(PHYSICAL_ADDRESS(seek_pointer) + 2);

			/*	destination identifier of the table(0 - 3),
				for instance 0 for the Y component and 1 for the colour components.
				Next follow the 64 bytes in zigzag format. */

			UINT_8 q_table_type = 0x00;

			ReadOneByte((&q_table_type), (seek_pointer));

			seek_pointer = (void*)(PHYSICAL_ADDRESS(seek_pointer) + 1);

			if (!q_table_type)
				Read_N_Byte((jpg->y_q_table), (length - 3), seek_pointer);

			else if (q_table_type == 1)
				Read_N_Byte((jpg->C_q_table), (length - 3), seek_pointer);
		}

		else if (next_word == JPG_DHT)
		{
			seek_pointer = (void*)(PHYSICAL_ADDRESS(seek_pointer) + 2);

			UINT_16 length = 0x0000;

			ReadOneWord((&length), (seek_pointer));
			length = (((length & 0xFF00) >> 8) | ((length & 0x00FF) << 8));

			seek_pointer = (void*)(PHYSICAL_ADDRESS(seek_pointer) + 2);

			/*	The first half BYTE is 0 if the Huffman tables are for DC numbers
				and 1 if they are for the AC numbers.The next half BYTE is the Huffman
				table destination identifier(0 or 1), for instance 0 for the Y component
				and 1 for the colour components. */

			UINT_8 h_table_type = 0x00;

			ReadOneByte((&h_table_type), (seek_pointer));

			seek_pointer = (void*)(PHYSICAL_ADDRESS(seek_pointer) + 1);

			if ((h_table_type >> 4) == 0) // aka. DC
			{
				if ((h_table_type & 0x0F) == 0) // aka. Y-component
				{
					Read_N_Byte((jpg->y_dc_table), (length - 3), seek_pointer);
					jpg->y_dc_htree = construct_htree((jpg->y_dc_table));
				}
				else // aka. C-component
				{
					Read_N_Byte((jpg->Cbr_dc_table), (length - 3), seek_pointer);
					jpg->Cbr_dc_htree = construct_htree((jpg->Cbr_dc_table));
				}

			}

			else if ((h_table_type >> 4) == 1) // aka. AC
			{
				if ((h_table_type & 0x0F) == 0) // aka. Y-component
				{
					Read_N_Byte((jpg->y_ac_table), (length - 3), seek_pointer);
					jpg->y_ac_htree = construct_htree((jpg->y_ac_table));
				}
				else // aka. C-component
				{
					Read_N_Byte((jpg->Cbr_ac_table), (length - 3), seek_pointer);
					jpg->Cbr_ac_htree = construct_htree((jpg->Cbr_ac_table));
				}
			}
		}

		else if (next_word == JPG_SOF)
		{
			seek_pointer = (void*)(PHYSICAL_ADDRESS(seek_pointer) + 2);

			UINT_16 length = 0x0000;

			ReadOneWord((&length), (seek_pointer));
			length = (((length & 0xFF00) >> 8) | ((length & 0x00FF) << 8));

			if (length > sizeof(SOF))
				length = sizeof(SOF);

			SOF sof;
			UINT_8* sof_tmp = (UINT_8*)(&sof);

			Read_N_Byte(sof_tmp, length, seek_pointer);

			sof.width = (((sof.width & 0xFF00) >> 8) | ((sof.width & 0x00FF) << 8));
			sof.height = (((sof.height & 0xFF00) >> 8) | ((sof.height & 0x00FF) << 8));

			/*	usually we have 4:2:0 subsampling. Taking 4 Y, and 2 C components blocks: 1 macroblock.
				Then we have macroblocks of 16x16 as the mcu unit which is entropy-coded
				in the compressed data as Y-Y-Y-Y-C-C-Y-Y-Y-Y-C-C-...
				Therefore, we go mcu by mcu and get Y-Y-Y-Y-C-C from the data. We then need, DC and AC trees for Y and C components as
				well as q-table for Y and C components.
			*/

			UINT_32 mcuX = (sof.width  >> 4) + ((sof.width  % 16) ? 1 : 0);
			UINT_32 mcuY = (sof.height >> 4) + ((sof.height % 16) ? 1 : 0);

			jpg->number_of_mcu = mcuX * mcuY;
		}

		else if (next_word == JPG_SOS) // compressed data are here
		{
			jpg->actual_data_byte_count = 0;
			jpg->actual_data = (UINT_8*)( Alloc(512000, 1, 1) );

			UINT_8* compressed_actual_data = (UINT_8*)jpg->actual_data;

			seek_pointer = (void*)(PHYSICAL_ADDRESS(seek_pointer) + 2);

			UINT_16 length = 0x0000;

			ReadOneWord((&length), (seek_pointer));
			length = (((length & 0xFF00) >> 8) | ((length & 0x00FF) << 8));

			if (length > sizeof(SOS))
				length = sizeof(SOS);

			SOS sos;
			UINT_8* sos_tmp = (UINT_8*)(&sos);

			Read_N_Byte(sos_tmp, length, seek_pointer);

			UINT_8 scan_code = 0x00;
			while (TRUE)
			{
				ReadOneByte((&scan_code), (seek_pointer));
				seek_pointer = (void*)(PHYSICAL_ADDRESS(seek_pointer) + 1);

				if (scan_code == 0xFF)
				{
					ReadOneByte((&scan_code), (seek_pointer));
					seek_pointer = (void*)(PHYSICAL_ADDRESS(seek_pointer) + 1);

					if (scan_code == 0x00)
					{
						scan_code = 0xFF;
						goto SKIP;
					}

					else
					{
						if (scan_code == 0xD9)
							goto KILLER_TERMINATING;

						ReadOneByte((&scan_code), (seek_pointer));
						seek_pointer = (void*)(PHYSICAL_ADDRESS(seek_pointer) + 1);
					}
				}

			SKIP:

				compressed_actual_data[jpg->actual_data_byte_count] = scan_code;
				jpg->actual_data_byte_count++;
			}
		}

		else if (next_word == JPG_COMMENT)
		{
			seek_pointer = (void*)(PHYSICAL_ADDRESS(seek_pointer) + 2);
			UINT_16 length = 0x0000;
			ReadOneWord((&length), (seek_pointer));
			length = (((length & 0xFF00) >> 8) | ((length & 0x00FF) << 8));

			UINT_32 i;
			UINT_8 cmnt = 0;
			UINT_8* cmnt_tmp = (UINT_8*)&cmnt;
			for (i = 0; i < length; i++)
			{
				ReadOneByte(cmnt_tmp, (seek_pointer));
				seek_pointer = (void*)(PHYSICAL_ADDRESS(seek_pointer) + 1);
			}
		}

		else if (next_word == JPG_EOI)
		{
			seek_pointer = (void*)(PHYSICAL_ADDRESS(seek_pointer) + 2);

		KILLER_TERMINATING:

			//.printk("total actual data count byte: %\n", jpg->actual_data_byte_count);
			break;
		}

		else
			seek_pointer = (void*)(PHYSICAL_ADDRESS(seek_pointer) + 1);
	}

	decode_yyyy_cc(jpg);
	jjpg = jpg;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void play(VIDEO_PLAYER* video_player, INT_8* file_address)
{
	JPG*     jpg    = video_player->jpg_object;
	UINT_8*  buffer = jpg->compressed_data;
	BINPAPER bp     = PaperOpen(file_address);
	
	PaperRead(&bp, buffer, (512 * 1024), 0);
	
	populate_jpg_info_from_file(jpg, buffer);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

UINT_32 regiser_video_player(VIDEO_PLAYER* video_player, UINT_8* framebuffer_pointer)
{
	if(!video_player)
	{
		printk("Error: video_player object is not valid\n");
		return FALSE;
	}

	video_player->video_player_name = "LiBOS_VideoPlayer";
	video_player->video_player_id   = 0x1B010100;
	
	// allocate the object
	tmp_video_player_jpg_object_compressed_data             = (UINT_8*)(Alloc(512*1024, 1, 1));
	tmp_video_player_jpg_object_all_mcus_coefficients       = (INT_16*)(Alloc(6*64*5000*2, 16, 1));
	tmp_video_player_jpg_object_all_mcus_coefficients_float = (float*) (Alloc(6*64*5000*4, 16, 1));
	tmp_video_player_jpg_object_unordered_YCbCr_mcus        = (float*) (Alloc(6*64*5000*4, 16, 1));
	tmp_video_player_jpg_object_r                           = (float*) (Alloc(800*600*3*4, 16, 1));
	tmp_video_player_jpg_object_g                           = (float*) (Alloc(800*600*3*4, 16, 1));
	tmp_video_player_jpg_object_b                           = (float*) (Alloc(800*600*3*4, 16, 1));
	rgb                                                     = (UINT_8*)(Alloc((1024*768*4),1,1));
	
	__LiBOS_MemZero((void*)(tmp_video_player_jpg_object_compressed_data), (512*1024));
	__LiBOS_MemZero((void*)(tmp_video_player_jpg_object_all_mcus_coefficients), (6*64*5000*2));
	__LiBOS_MemZero((void*)(tmp_video_player_jpg_object_all_mcus_coefficients_float), (6*64*5000*4));
	__LiBOS_MemZero((void*)(tmp_video_player_jpg_object_unordered_YCbCr_mcus), (6*64*5000*4));
	__LiBOS_MemZero((void*)(tmp_video_player_jpg_object_r), (800*600*3*4));
	__LiBOS_MemZero((void*)(tmp_video_player_jpg_object_g), (800*600*3*4));
	__LiBOS_MemZero((void*)(tmp_video_player_jpg_object_b), (800*600*3*4));
	__LiBOS_MemZero((void*)(rgb), (1024*768*4));
	
	video_player->jpg_object->compressed_data                    =  tmp_video_player_jpg_object_compressed_data;
	video_player->jpg_object->ptr_to_all_mcus_coefficients       = &tmp_video_player_jpg_object_all_mcus_coefficients;
	video_player->jpg_object->ptr_to_all_mcus_coefficients_float = &tmp_video_player_jpg_object_all_mcus_coefficients_float;
	video_player->jpg_object->ptr_to_unordered_YCbCr_mcus        = &tmp_video_player_jpg_object_unordered_YCbCr_mcus;
	video_player->jpg_object->ptr_to_r                           = &tmp_video_player_jpg_object_r;
	video_player->jpg_object->ptr_to_g                           = &tmp_video_player_jpg_object_g;
	video_player->jpg_object->ptr_to_b                           = &tmp_video_player_jpg_object_b;
	video_player->jpg_object->rgb                                = rgb;//*/framebuffer_pointer;
	
	if(vpDebug)
	{
		printk("Address of all_mcus_coefficients:^\n",       PHYSICAL_ADDRESS(tmp_video_player_jpg_object_all_mcus_coefficients));
		printk("Address of all_mcus_coefficients_float:^\n", PHYSICAL_ADDRESS(tmp_video_player_jpg_object_all_mcus_coefficients_float));
		printk("Address of unordered_YCbCr_mcus:^\n",        PHYSICAL_ADDRESS(tmp_video_player_jpg_object_unordered_YCbCr_mcus));
		printk("Address of r:^\n",                           PHYSICAL_ADDRESS(tmp_video_player_jpg_object_r));
		printk("Address of g:^\n",                           PHYSICAL_ADDRESS(tmp_video_player_jpg_object_g));
		printk("Address of b:^\n",                           PHYSICAL_ADDRESS(tmp_video_player_jpg_object_b));
	}
	
	UINT_32 avx_buffer = 0;
	_check_avx(&avx_buffer);
	if(avx_buffer & BIT(28))
		printk("CPU SUPPORTS AVX\n");
	else
		printk("CPU DOES NOT SUPPORT AVX\n");

	return TRUE;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
