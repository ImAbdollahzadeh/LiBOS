#ifndef LiBOS_CORE__H__
#define LiBOS_CORE__H__

// core LiBOS types
typedef char               INT_8;
typedef short              INT_16;
typedef int                INT_32;
typedef long long          INT_64;
typedef unsigned char      UINT_8;
typedef unsigned short     UINT_16;
typedef unsigned int       UINT_32;
typedef unsigned long long UINT_64;
typedef unsigned int       BOOL;

// booleans
#	define TRUE  1
#	define FALSE 0

// platform target selection
#	define LiBOS_08_BIT FALSE
#	define LiBOS_16_BIT FALSE
#	define LiBOS_32_BIT TRUE
#	define LiBOS_64_BIT FALSE

// physical address definition
#	if LiBOS_08_BIT
#		define PHYSICAL_ADDRESS(ANY_POINTER) (UINT_8)((void*)(ANY_POINTER))
#	elif LiBOS_16_BIT
#		define PHYSICAL_ADDRESS(ANY_POINTER) (UINT_16)((void*)(ANY_POINTER))
#	elif LiBOS_32_BIT
#		define PHYSICAL_ADDRESS(ANY_POINTER) (UINT_32)((void*)(ANY_POINTER))
#	elif LiBOS_64_BIT
#		define PHYSICAL_ADDRESS(ANY_POINTER) (UINT_64)((void*)(ANY_POINTER))
#	else
#   		define PHYSICAL_ADDRESS(ANY_POINTER) (0x00)
#   	endif

// debuggers definitions
#	define LiBOS_SPECIAL_GDT_DEBUGGER        FALSE   /* special debugger for high priority GDT stuff        */
#	define LiBOS_SPECIAL_IDT_DEBUGGER        FALSE   /* special debugger for high priority IDT stuff        */
#	define LiBOS_SPECIAL_TIMER_DEBUGGER      FALSE   /* special debugger for high priority TIMER stuff      */
#	define LiBOS_SPECIAL_KEYBOARD_DEBUGGER   FALSE   /* special debugger for high priority KEYBOARD stuff   */
#	define LiBOS_SPECIAL_XHCI_DEBUGGER       FALSE   /* special debugger for high priority XHCI stuff       */
#	define LiBOS_SPECIAL_SVGA_DEBUGGER       FALSE   /* special debugger for high priority SVGA stuff       */
#	define LiBOS_SPECIAL_AHCI_DEBUGGER       FALSE   /* special debugger for high priority AHCI stuff       */
#	define LiBOS_SPECIAL_FILESYSTEM_DEBUGGER FALSE   /* special debugger for high priority FILESYSTEM stuff */
#	define LiBOS_SPECIAL_USB_MOUSE_DEBUGGER  FALSE   /* special debugger for high priority USB_MOUSE stuff  */

// memzero function implementation
static void __LiBOS_MemZero(void* address, UINT_32 bytes)
{
	UINT_32  blocks = bytes >> 3;
	UINT_8   remain = bytes & 7;
	UINT_64* dst    = (UINT_64*)address;
	UINT_8*  ddst   = 0;
	
	while (blocks--)
		*dst++ = 0ULL;
	
	ddst = (UINT_8*)dst;
	
	while (remain--) 
		*ddst++ = 0;
	
	return;
}

#endif // LiBOS_CORE__H__ !
