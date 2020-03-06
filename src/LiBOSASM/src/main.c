#include "../include/LiBOSASM_CORE.h"
#include "../include/LiBOSASM_STRING.h"

int main(void)
{
	unsigned int lines = 0;
	unsigned int j;
	TRIPLE_PACKET* tp = 0;
	char p[128];
	
	const char* file = 
	//"[LiBOSASM 32-bit]\n"
	//"[ORIGIN 0x38EF501A]\n"
	//"[SECTION .CODE]\n"
	//"global _start\n"
	//"global vid_pl\n"
	//"_start:\n"
	//"\tpush 0xAA001258\n"
	//"\tpush ebp\n"
	//"\t$1\n"
	//"\tmov ebp, esp\n"
	//"\tmov  WORD[memory_libos], cx\n"
	//"\tmov dh,  BYTE[_THIRD_AND_LAST@@_LABEL:]\n"
	//"\tmov ebp, DWORD[0xA0AB9856]\n"
	//"\tmov dh,  BYTE[ebp]\n"
	//"\tmov dx,  WORD[__SECOND_LABEL:]\n"
	//"\tmov edi, 0xAABBCCDD\n"
	//"\tmov bx, 0x45FF\n"
	//"\tmov ch, 0x7E\n"
	//"\tnop\n"
	//"\tadd edx, 0xDDCCBB56\n"
	//"\tadd BYTE[edi+0xAABBCCDD], al\n"
	//"\tadd DWORD[eax+0xFD996614], 0xAABBCCDD\n"
	//"\tsub eax, 0x43546576\n"
	//"\tsub bp,  0xAABB\n"
	//"\tsub bl,  0xA0\n"
	//"\tsub DWORD[ecx], 0x43546576\n"
	//"\tsub WORD[ebp+0x88202772], 0x4354\n"
	//"\tsub BYTE[ebp], 0x44\n"
	//"\tsub DWORD[esp], 0x43546576\n"
	//"\tsub WORD[esp+0x88202772], 0x4354\n"
	//"\tsub BYTE[esp], 0x44\n"
	//"\tsub DWORD[_THIRD_AND_LAST@@_LABEL:], 0x11002399\n"
	//"\tsub WORD[__SECOND_LABEL:], 0x1199\n"
	//"\tnop\n"
	//"\tsub ebp, DWORD[esp+0x12131415]\n"
	//"\tsub bp, WORD[__SECOND_LABEL:]\n"
	//"\tsub ah, BYTE[0x000000DF]\n"
	//"\tsub DWORD[esp+0x12131415], esi\n"
	//"\tsub WORD[__SECOND_LABEL:], sp\n"
	//"\tsub BYTE[0x000055DF], al\n"
	//"\tsub BYTE[esp+0x690055DF], dh\n"
	//"\tnop\n"
	//"\tsub cl, dh\n"
	//"\tsub eax, edi\n"
	//"\tsub bx, cx\n"
	//"\txor di, 0x41FF\n"
	//"\txor DWORD[esp+0x11223344], 0xfedcab12\n"
	//"\txor WORD[ebp+0xFD551122], 0xFED2\n"
	//"\txor BYTE[eax], 0x49\n"
	//"\txor BYTE[__SECOND_LABEL:], 0xE5\n"
	//"\tnop\n"
	//"\tnop\n"
	//"\tnop\n"
	//"\txor eax, DWORD[__SECOND_LABEL:]\n"
	//"\txor esp, DWORD[ebp+0x45879563]\n"
	//"\txor si,  WORD[__SECOND_LABEL:]\n"
	//"\txor cx,  WORD[ebx+0x66885511]\n"
	//"\txor cx,  WORD[esp+0x000023F1]\n"
	//"\txor bl,  BYTE[esp+0x998823F1]\n"
	//"\txor ah,  BYTE[edx]\n"
	//"\tnop\n"
	//"\tnop\n"
	//"\tnop\n"
	//"\txor DWORD[__SECOND_LABEL:], esp\n"
	//"\txor DWORD[ebp+0x45879563], ebx\n"
	//"\txor WORD[__SECOND_LABEL:], cx\n"
	//"\txor WORD[ebx+0x66885511], ax\n"
	//"\txor WORD[esp+0x000023F1], bp\n"
	//"\txor BYTE[esp+0x998823F1], bl\n"
	//"\txor BYTE[edx], al\n"
	//"\tmov BYTE[esp+0x28], 0x16\n"
	//"\tnop\n"
	//"\tnop\n"
	//"\txor ebx, esp\n"
	//"\txor eax, eax\n"
	//"\txor esp, esp\n"
	//"\txor ax, bx\n"
	//"\txor dh, cl\n"
	//"\txor bp, bp\n"
	//"\txor bl, bl\n"
	//"\tnop\n"
	//"\tcmp eax, 0x45789641\n"
	//"\tcmp ax, 0xCF12\n"
	//"\tcmp bh, 0xCD\n"
	//"\tnop\n"
	//"\tcmp DWORD[esp+0x99977853], 0x45789641\n"
	//"\tcmp WORD[ebp], 0xCF12\n"
	//"\tcmp BYTE[_THIRD_AND_LAST@@_LABEL:], 0xCD\n"
	//"\tnop\n"
	//"\t$--numeric_token--\n"
	//"\tcmp edi, DWORD[esp+0x99977853]\n"
	//"\tcmp dx, WORD[ebp+0x00000021]\n"
	//"\tcmp bh, BYTE[_THIRD_AND_LAST@@_LABEL:]\n"
	//"\tnop\n"
	//"\tcmp DWORD[ecx+0x99977853], esp\n"
	//"\tcmp WORD[ebp+0x00000021], cx\n"
	//"\tcmp BYTE[_THIRD_AND_LAST@@_LABEL:], ah\n"
	//"\tnop\n"
	//"\tcmp eax, esp\n"
	//"\tcmp bp, cx\n"
	//"\tcmp dl, ah\n"
	//"\tnop\n"
	//"\tjmp eax\n"
	//"\tjmp esp\n"
	//"\tjmp DWORD[eax]\n"
	//"\tjmp DWORD[esp]\n"
	//"\tjmp DWORD[ebp]\n"
	//"\tjmp DWORD[edi+0xFFDDCCAA]\n"
	//"\tjmp DWORD[esp+0xFFDDCCAA]\n"
	//"\tjmp DWORD[ebp+0xFFDDCCAA]\n"
	//"\tjmp DWORD[_THIRD_AND_LAST@@_LABEL:]\n"
	//"\tjmp 0xFF0000A0\n"
	//"\tjmp 0xC42C0048\n"
	//"\tnop\n"
	//"\tjne 0xC42C0048\n"
	//"\tjne 0xC42C0048\n"
	//"\tjne __SECOND_LABEL:\n"
	//"\tjne _start:\n"
	//"\tjne _THIRD_AND_LAST@@_LABEL:\n"
	//"\tnop\n"
	//"\tjnz __SECOND_LABEL:\n"
	//"\tjz _start:\n"
	//"\tje _start:\n"
	//"\tmov esp, ebp\n"
	//"\t$_SECOND_LABEL_start\n"
	//"__SECOND_LABEL:\n"
	//"\tjmp DWORD[__SECOND_LABEL:]\n"
	//"\tadd esp, ebp\n"
	//"\tadd dx, di\n"
	//"\tadd dh, cl\n"
	//"\tpop DWORD[esp+0xFFDDAA56]\n"
	//"\tint 0x03\n"
	//"\tint 0x16\n"
	//"\t$_SECOND_LABEL_end\n"
	//"_THIRD_AND_LAST@@_LABEL:\n"
	//"\tcall 0xFFCCBBAA\n"
	//"\tcall DWORD[_THIRD_AND_LAST@@_LABEL:]\n"
	//"\tcall DWORD[eax]\n"
	//"\tcall DWORD[edi+0xFF130827]\n"
	//"\tcall DWORD[esp]\n"
	//"\tcall DWORD[esp+0xFF130827]\n"
	//"\tcall DWORD[ebp]\n"
	//"\tcall DWORD[ebp+0xFF130827]\n"
	//"\tcall ecx\n"
	//"\tret\n"
	//"[SECTION .DATA]\n"
	//"string_to_be_printed: db 'ImanAbdollahzadehLiBOS'\n"
	//"_rgb@@mask_black:     dd  0x23AB8FC6\n"
	//"libos_word_data:      dw  0x8F7A\n"
	//"simd_operation_rgba:  dX  0xFF000000FF111111FF222222FF33333333\n"
	//"_@@an_example_qword:  dq  0x112233445678AEF0\n";
	
	//"[LiBOSASM 32-bit]\n"
	//"[ORIGIN 0x00000000]\n"
	//"[SECTION .CODE]\n"
	//"_LiBOS_V0_printk:\n"
	//"\tpush epb\n"
	//"\tmov  ebp, esp\n"
	//"\tmov  eax, DWORD[ebp+0x00000008]\n"
	//"\tmov  ecx, DWORD[ebp+0x0000000C]\n"
	//"\txor  edx, edx\n"
	//"\tmov  edx, 0x0008B000\n"
	//"__@@next:\n"
	//"\tmov  bl, BYTE[eax]\n"
	//"\tmov  BYTE[edx], bl\n"
	//"\tmov  BYTE[edx+0x00000001], 0x02\n"
	//"\tadd  edx, 0x00000002\n"
	//"\tadd  eax, 0x00000001\n"
	//"\tsub  ecx, 0x00000001\n"
	//"\tcmp  ecx, 0x00000000\n"
	//"\tjne  __@@next:\n"
	//"\tmov  esp, ebp\n"
	//"\tpop  ebp\n"
	//"\tret\n"
	//"[SECTION .DATA]\n"
	//"name_of_module: db 'LiBOS_V0_printk'\n";
	
	//"[LiBOSASM 32-bit]\n"
	//"[ORIGIN 0x00000000]\n"
	//"[SECTION .CODE]\n"
	//"\tmov  eax, DWORD[global_module_table_address:]\n"
	//"\tmov  ebx, DWORD[global_module_table_count_address:]\n"
	//"\tmov  edi, DWORD[root_module_0:]\n"
	//"\tmov  edx, DWORD[root_module_0_address:]\n"
	//"\txor  esi, esi\n"
	//"\tmov  DWORD[ebx], 0x00000000\n"
	//"\tpush ebp\n"
	//"\txor  ebp, ebp\n"
	//"__@next:\n"
	//"\txor  ecx, ecx\n"
	//"\tmov  cl, BYTE[edi]\n"
	//"\tmov  BYTE[eax], cl\n"
	//"\tadd  edi, 0x00000001\n"
	//"\tadd  esi, 0x00000001\n"
	//"\tadd  eax, 0x00000001\n"
	//"\tcmp  ecx, 0x00000000\n"
	//"\tjne  __@next:\n"
	//"\tadd  DWORD[ebx], 0x00000001\n"
	//"__@@next:\n"
	//"\tmov  BYTE[eax], 0x00\n"
	//"\tadd  eax, 0x00000001\n"
	//"\tadd  esi, 0x00000001\n"
	//"\tcmp  esi, 0x00000080\n"
	//"\tjne  __@@next:\n"
	//"\tmov  DWORD[eax], edx\n"
	//"\tadd  eax, 0x00000004\n"
	//"\tmov  edi, DWORD[root_module_1:]\n"
	//"\tmov  edx, DWORD[root_module_1_address:]\n"
	//"\txor  esi, esi\n"
	//"\tadd  ebp, 0x00000001\n"
	//"\tcmp  ebp, 0x00000002\n"
	//"\tjne  __@next:\n"
	//"\tpop  ebp\n"
	//"\tret\n"
	//"[SECTION .DATA]\n"
	//"root_module_0:                     db 'LiBOS_V0_printk'\n"
	//"root_module_0_address:             dd 0x20004004\n"
	//"root_module_1:                     db 'LiBOS_V1_printk'\n"
	//"root_module_1_address:             dd 0x20008004\n"
	//"global_module_table_address:       dd 0x20000000\n"
	//"global_module_table_count_address: dd 0x20004000\n";
	
	//"[LiBOSASM 32-bit]\n"
	//"[ORIGIN 0xFF05E000]\n"
	//"[SECTON .CODE]\n"
	//"LiBOS_str_length:\n"
	//"\tpush ebp\n"
	//"\tmov  ebp, esp\n"
	//"\tmov  eax, DWORD[ebp+0x00000008]\n"
	//"\txor  ecx, ecx\n"
	//"_repeat_1:\n"
	//"\txor  edx, edx\n"
	//"\tmov  dl, BYTE[eax]\n"
	//"\tadd  ecx, 0x00000001\n"
	//"\tadd  eax, 0x00000001\n"
	//"\tcmp  dl, 0x00\n"
	//"\tjne  _repeat_1:\n"
	//"\tsub  ecx, 0x00000001\n"
	//"\tmov  DWORD[ebp+0x0000000C], ecx\n"
	//"\tmov  esp, ebp\n"
	//"\tpop  ebp\n"
	//"\tret\n"
	//"\t$_numeric_1\n"
	//"_start:\n"  
	//"\tmov  eax, DWORD[str1:]\n"
	//"\tmov  ebx, DWORD[str2:]\n"
	//"\tmov  ecx, DWORD[status_val:]\n"
	//"\tpush ecx\n"
	//"\tpush ebx\n"
	//"\tpush eax\n"
	//"\tcall DWORD[LiBOS_strcmp:]\n"
	//"\tpop  eax\n"
	//"\tpop  ebx\n"
	//"\tpop  ecx\n"
	//"\tcmp  ecx, 0x00000001\n"
	//"\tjne  not_equal_jump:\n"
	//"\tret\n"
	//"LiBOS_strcmp:\n"
	//"\tpush ebp\n"
	//"\tmov  ebp, esp\n"
	//"\tmov  eax, DWORD[ebp+0x00000008]\n"
	//"\tmov  ebx, DWORD[ebp+0x0000000C]\n"
	//"\txor  edx, edx\n"
	//"\tpush edx\n"
	//"\tpush eax\n"
	//"\tcall DWORD[LiBOS_str_length:]\n"
	//"\tpop  eax\n"
	//"\tpop  edx\n"
	//"\tmov  DWORD[string1_length:], edx\n"
	//"\txor  edx, edx\n"
	//"\tpush edx\n"
	//"\tpush ebx\n"
	//"\tcall DWORD[LiBOS_str_length:]\n"
	//"\tpop  ebx\n"
	//"\tpop  edx\n"
	//"\tmov  DWORD[string2_length:], edx\n"
	//"\tmov  ebx, DWORD[string1_length:]\n"
	//"\tmov  eax, DWORD[ebx]\n"
	//"\tmov  edx, DWORD[string2_length:]\n"
	//"\tmov  ebx, DWORD[edx]\n"
	//"\tcmp  eax, ebx\n"
	//"\tjne  not_equal_return\n"
	//"\tmov  eax, DWORD[ebp+0x00000008]\n"
	//"\tmov  ebx, DWORD[ebp+0x0000000C]\n"
	//"_Y:\n" 
	//"\txor  edx, edx\n"
	//"\tmov  dl, BYTE[eax]\n"
	//"\tmov  dh, BYTE[ebx]\n"
	//"\tcmp  dl, dh\n"
	//"\tjne  not_equal_return:\n"
	//"\tcmp  dl, 0x00\n"
	//"\tje   success_compare:\n"
	//"\tadd  eax, 0x00000001\n"
	//"\tadd  ebx, 0x00000001\n"
	//"\tjmp  DWORD[_Y:]\n"
	//"success_compare:\n"
	//"\tmov  DWORD[ebp+0x00000010], 0x00000001\n"
	//"\tmov  esp, ebp\n"
	//"\tpop  ebp\n"
	//"\tret\n"
	//"not_equal_return:\n"
	//"\tmov  DWORD[ebp+0x00000010], 0x00000000\n"
	//"\tmov  esp, ebp\n"
	//"\tpop  ebp\n"
	//"\tret\n"
	//"[SECTION .DATA]\n"
	//"\tstr1:              db 'NLAJM'\n"
	//"\tstr2:              db 'NLAJM'\n"
	//"\tstring1_length:    dd 0x00000000\n"
	//"\tstring2_length:    dd 0x00000000\n"
	//"\tstatus_val:        dd 0xFFFFFFFF\n";
	
	//"[LiBOSASM 32-bit]\n"
	//"[ORIGIN 0x01000000]\n"
	//"[SECTION .CODE]\n"
	//"_@@__ext_loader__@@_:\n"
	//"\tpush   ebp\n"
	//"\tmov    ebp,         esp\n"
	//"\tmov    eax,         DWORD[ebp+0x00000008]\n"
	//"\tmov    ebx,         DWORD[ebp+0x0000000C]\n"
	//"\t;----------------------------------------------------------------------now eax is pointer to the name string\n"
	//"\t;----------------------------------------------------------------------now ebx is pointer to 1KB binary data\n"
	//"\tmov    ecx,         DWORD[0x0104837C]\n"
	//"\tmov    esi,         0x01000000\n"
	//"_find_next_free_ext_chunk:\n"
	//"\tadd    esi,         0x00000400\n"
	//"\tsub    ecx,         0x00000001\n"
	//"\tcmp    ecx,         0x00000000\n"
	//"\tjne    _find_next_free_ext_chunk:\n"
	//"\tpush   esi\n"
	//"\t;----------------------------------------------------------------------keep esi's value for later uses\n"
	//"\tmov    ecx,         0x00000400\n"
	//"_copy_bytes:\n"
	//"\txor    edx,         edx\n"
	//"\tmov    dl,          BYTE[ebx]\n"
	//"\tmov    BYTE[esi],   dl\n"
	//"\tadd    ebx,         0x00000001\n"
	//"\tadd    esi,         0x00000001\n"
	//"\tcmp    ecx,         0x00000000\n"
	//"\tjne    _copy_bytes:\n"
	//"\tmov    ecx,         DWORD[0x01048380]\n"
	//"\t;----------------------------------------------------------------------now ecx contains the address of start_of_global_ext_table\n"
	//"_create_new_global_ext_table_entry:\n"
	//"\txor    edx,         edx\n"
	//"\tmov    dl,          BYTE[ecx]\n"
	//"\tcmp    dl,          0x00\n"
	//"\tje     _found_entry:\n"
	//"\tadd    ecx,         0x00000084\n"
	//"\t;----------------------------------------------------------------------one step = 132 bytes\n"
	//"\tjmp    _create_new_global_ext_table_entry:\n"
	//"\txor    esi,         esi\n"
	//"_found_entry:\n"
	//"\txor    edx,         edx\n"
	//"\tmov    dl,          BYTE[eax]\n"
	//"\tcmp    dl,          0x00\n"
	//"\tje     _set_entry_address:\n"
	//"\tmov    BYTE[ecx],   dl\n"
	//"\tadd    eax,         0x00000001\n"
	//"\tadd    ecx,         0x00000001\n"
	//"\tadd    esi,         0x00000001\n"
	//"\tjmp    _found_entry:\n"
	//"_set_entry_address:\n"
	//"\tmov    BYTE[ecx],   0x00\n"
	//"\tadd    esi,         0x00000001\n"
	//"\tadd    ecx,         0x00000001\n"
	//"\tcmp    esi,         0x00000080\n"
	//"\tjne    _set_entry_address:\n"
	//"\tpop    esi\n"
	//"\tmov    DWORD[ecx],  esi\n"
	//"_update_global_ext_table_count:\n"
	//"\tmov    ecx,         DWORD[0x0104837C]\n"
	//"\tadd    DWORD[ecx],  0x00000001\n"
	//"\tmov    esp,         ebp\n"
	//"\tpop    ebp\n"
	//"\tret\n"
	//"[SECTION .DATA]\n"
	//"\t_@@__ext_loader__@@_LiBOSASM_internal_data_buffer: dd 0xFFFFFFFF\n";
	
	
//"\t[LiBOSASM 32-bit]\n"
//"\t[ORIGIN 0x00000000]\n"
//"\t[SECTION .CODE]\n"
//"__@@_get_global_ext_table_attributes_@@__:\n"
//"\tpush ebp\n"
//"\tmov  ebp, esp\n"
//"\tmov  ecx, DWORD[ebp+0x00000008]\n"
//"\tcmp  ecx, 0x00000000\n"
//"\tje   __@@_get_global_ext_table_address_@@__:\n"
//"\tcmp  ecx, 0x00000001\n"
//"\tje   __@@_get_global_ext_table_count_@@__:\n"
//"\tmov  eax, DWORD[LiBOS_internal_@@_get_global_ext_table_attributes_@@_:]\n"
//"\tmov  DWORD[ebp+0x0000000C], eax\n"
//"\tjmp  DWORD[__@@_get_global_ext_table_attributes_end_@@__:]\n"
//"__@@_get_global_ext_table_address_@@__:\n"
//"\tmov  eax, DWORD[0x01048380]\n"
//"\tmov  DWORD[ebp+0x0000000C], eax\n"
//"\tjmp  DWORD[__@@_get_global_ext_table_attributes_end_@@__:]\n"
//"__@@_get_global_ext_table_count_@@__:\n"
//"\tmov  eax, DWORD[0x0104837C]\n"
//"\tmov  DWORD[ebp+0x0000000C], eax\n"
//"\tjmp  DWORD[__@@_get_global_ext_table_attributes_end_@@__:]\n"
//"__@@_get_global_ext_table_attributes_end_@@__:\n"
//"\tmov  esp, ebp\n"
//"\tpop  ebp\n"
//"\tret\n"
//"[SECTION .DATA]\n"
//"\tLiBOS_internal_@@_get_global_ext_table_attributes_@@_: dd 0xFFFFFFFF\n";
		
"[LiBOSASM 32-bit]\n"
"[ORIGIN 0xFED20000]\n"
"[SECTION .CODE]\n"
"_@@__find_ext_address__@@_:\n"
";---------_@@__find_ext_address__@@_(char* name, DWORD* ret_address)\n"
"\tpush   ebp\n"
"\tmov    ebp,                   esp\n"
"\tmov    eax,                   0x00000000\n"
";--------eax_is_the_id=table_count\n"
"\tpush   eax\n"
"\tmov    eax,                   DWORD[_tab_count:]\n"
"\tpush   eax\n"
"\tmov    ecx,                   0x01000400\n"
"\tadd    ecx,                   0x00000040\n"
"\tcall   DWORD[ecx]\n"
";--------call_LiBOS_GET_GLOBAL_EXT_TABLE_ATTRIBUTES.EXT_entry_point\n"
"\tpop    eax\n"
"\tpop    eax\n"
"\tmov    eax,                   0x00000001\n"
";--------eax_is_the_id=table_address\n"
"\tpush   eax\n"
"\tmov    eax,                   DWORD[_tab_address:]\n"
"\tpush   eax\n"
"\tmov    ecx,                   0x01000400\n"
"\tadd    ecx,                   0x00000040\n"
"\tcall   DWORD[ecx]\n"
";--------call_LiBOS_GET_GLOBAL_EXT_TABLE_ATTRIBUTES.EXT_entry_point\n"
"\tpop    eax\n"
"\tpop    eax\n"
"\t;--------\n"
"\tmov    eax,                   DWORD[_tab_count:]\n"
"\tmov    ecx,                   DWORD[eax]\n"
"\tmov    edx,                   0x01000800\n"
"\tadd    edx,                   0x00000040\n"
";--------now_edx_has_the_address_of_LiBOS_STRCMP.EXT_entry_point\n"
"_look_for_entry_name_match:\n"
"\tcmp    ecx,                   0x00000000\n"
"\tje     _look_finished_without_match:\n"
"\tmov    eax,                   DWORD[_find_ext_address_int:]\n"
"\tpush   eax\n"
"\tmov    eax,                   DWORD[ebp+0x00000008]\n"
"\tpush   eax\n"
"\tmov    eax,                   DWORD[_tab_address:]\n"
"\tpush   eax\n"
"\tcall   DWORD[edx]\n"
"\tpop    eax\n"
"\tpop    eax\n"
"\tpop    eax\n"
"\tcmp    eax,                   0x00000001\n"
"\tje     _found_entry_name_match:\n"
";---------now_eax_has_the_result_of_strcmp\n"
"\tmov    ebx,                   DWORD[_tab_address:]\n"
"\tmov    esi,                   DWORD[ebx]\n"
"\tadd    esi,                   0x00000084\n"
"\tmov    DWORD[ebx],            esi\n"
"\tsub    ecx,                   0x00000000\n"
"\tjmp    DWORD[_look_for_entry_name_match:]\n"
"_found_entry_name_match:\n"
"\tmov    ebx,                   DWORD[_tab_address:]\n"
"\tmov    eax,                   DWORD[ebx]\n"
"\tadd    eax,                   0x00000080\n"
"\tmov    ebx,                   DWORD[eax]\n"
";---------now_ebx=the_physical_address_of_the_expected_EXT_file\n"
"\tmov    DWORD[ebp+0x0000000C], ebx\n"
"\tjmp    DWORD[__@@libos_find_ext_address_end@@__:]\n"
"_look_finished_without_match:\n"
"\tmov    DWORD[ebp+0x0000000C], 0x00000000\n"
"__@@libos_find_ext_address_end@@__:\n"
"\tmov    esp,                   ebp\n"
"\tpop    ebp\n"
"\tret\n"
"[SECTION .DATA]\n"
"\t_find_ext_address_int: dd 0xFFFFFFFF\n"
"\t_tab_count:            dd 0xFFFFFFFF\n"
"\t_tab_address:          dd 0xFFFFFFFF\n";
	
	parse_0(file, &tp, &lines, p);
	print_file(file);
	extract_origin(tp, lines);
	
	parse_1_or__convert_instructions_line_by_line(tp, lines);
	dump_table_of_labels();

	append_data_section_after_code_section();
	
	zero_programCounter();
	zero_data_section_identifier();
	
	parse_2(tp, lines);
	dump_table_of_labels();
	
	//IMAGE_FILE_MEMORY ifm;
	//ifm.total_sizeof_image = get_programCounter();
	//image_file_make(tp, lines, &ifm);
	//dump_image_file_memory(&ifm);
	
	dump_output_beffer();
	dump_data_section_table_entries();
	dump_numeric_table();
}