#include "../include/DOSSPEC.h"
#include "../include/MEMORY.h"
#include "../include/PRINT.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ GLOBAL AND STATIC VARIABLES

char ForbiddenAsciiCharactersForAddressing[] =
{
	' ',';',',',':','|','\\',
	'"','%','&','(',')','=',
	'+', '-', '*','^','!',
	'<','>','$','#'
};

char return_string_to_kernel[192]; //maximum 12 bytes

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

unsigned int DosStringToDosNamePackage(char* DosPath, PDOSNAME dosNamePackage)
{
	unsigned int i = 0;
	PDOSNAME     tmp = dosNamePackage;
	tmp->next = 0;
	
	if(!DosPath)
	{
		panic("Given path is invalid\n");
		return 0;
	}
	
	if(!dosNamePackage)
	{
		panic("Given package pointer is invalid\n");
		return 0;
	}
	
	while(*DosPath)
	{
		if(*DosPath != '/')
		{
			tmp->name[i] = *DosPath;
			i++;
		}
		else
		{
			tmp->name[i] = '\0';
			tmp->next = (PDOSNAME)Alloc(sizeof(DOSNAME), 1, 1);
			tmp->next->next = 0;
			tmp = tmp->next;
			i = 0;
		}
		DosPath++;
	}
	tmp->name[i]= '\0';
	return 1;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

unsigned int IsInForbiddenList(char* CStringAddress)
{
	int i;
	while (*CStringAddress)
	{
		for (i = 0; i<sizeof(ForbiddenAsciiCharactersForAddressing); i++)
		{
			if (*CStringAddress == ForbiddenAsciiCharactersForAddressing[i])
				return 1;
		}
		CStringAddress++;
	}
	return 0;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

unsigned int HowManyPackes(char* CStringAddress)
{
	unsigned int i = 0;
	while (*CStringAddress)
	{
		if (*CStringAddress == '/')
			i++;
		CStringAddress++;
	}
	return i;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

unsigned int ExtractCStringPathToPacks(char* CStringAddress, char* trg, unsigned int which_pack)
{
	unsigned int j = 0, i;

	while (1)
	{
		if (j == which_pack)
		{
			i = 0;
			while (CStringAddress[i] != '/')
			{
				if (CStringAddress[i] == '\0')
				{
					trg[i] = '\0';
					break;
				}
				trg[i] = CStringAddress[i];
				i++;
				if (i == 32)
					return 0;
			}
			trg[i] = '\0';
			break;
		}
		else
		{
			while (*CStringAddress != '/')
				CStringAddress++;
			CStringAddress++; // to over pass the '/' character
			j++;
		}
	}

	return 1;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void GetDosName(char* cstr, char* dostr)
{
	char*        tmp = cstr;
	unsigned int before_dot = 0;
	unsigned int dot = 0, i;

	while (*tmp)
	{
		if (*tmp == '.')
		{
			dot = 1;
			break;
		}
		if (before_dot == 8)
		{
			tmp = cstr;
			while (*tmp)
			{
				if (*tmp == '.')
					dot = 1;
				tmp++;
			}
			break;
		}
		before_dot++;
		tmp++;
	}

	for (i = 0; i < 11; i++)
		dostr[i] = ' ';

	tmp = cstr;
	for (i = 0; i < before_dot; i++)
		dostr[i] = tmp[i];

	if (before_dot == 8)
	{
		dostr[6] = '~';
		dostr[7] = '1';
	}

	if (dot)
	{
		tmp = cstr;
		while (*tmp)
		{
			if (*tmp == '.')
			{
				tmp++;
				dostr[8] = *tmp++;
				dostr[9] = *tmp++;
				dostr[10] = *tmp;
				dostr[11] = '\0';
				return;
			}
			tmp++;
		}
	}

	dostr[11] = '\0';
	return;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

char* CStringAddressToDosString(char* CStringAddress)
{
	CSTRING_32_LIMIT   cstrs[16];
	DOSSTRING_11_LIMIT dosstrs[16];
	unsigned int status;
	unsigned int packs;
	unsigned int i;
	unsigned int j;
	char* tmp  = 0;
	char* dos  = 0;
	char* cstr = 0;

	status = IsInForbiddenList(CStringAddress);
	if (status)
	{
		printk("given address contains forbiden characters\n");
		return (char*)0;
	}

	packs = HowManyPackes(CStringAddress);
	if (packs > 16)
	{
		printk("given address is directories exceeded 16 limitation\n");
		return (char*)0;
	}

	for (i = 0; i<packs + 1; i++)
	{
		tmp = cstrs[i].tmp;
		status = ExtractCStringPathToPacks(CStringAddress, tmp, i);
		if (!status)
		{
			printk("given address contains longer than 32 bytes characters in\n");
			return (char*)0;
		}
	}

	for (i = 0; i<packs + 1; i++)
	{
		dos = dosstrs[i].tmp;
		cstr = cstrs[i].tmp;
		GetDosName(cstr, dos);
	}

	for (i = 0; i<192; i++)
		return_string_to_kernel[i] = '$'; //fill up with a forbidden character for null termination later

	j = 0;
	for (i = 0; i<packs + 1; i++)
	{
		tmp = dosstrs[i].tmp;
		while (*tmp)
		{
			return_string_to_kernel[j] = (*tmp>=97 && *tmp<=122) ? (*tmp-32) : *tmp;
			tmp++;
			j++;
		}
		return_string_to_kernel[j] = '/';
		j++;
	}

	if (return_string_to_kernel[j] = '$')
		return_string_to_kernel[j-1] = '\0';

	return (char*)return_string_to_kernel;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void DosPrint(PDOSNAME dos)
{
	PDOSNAME tmp = dos;
	while (tmp->next != 0)
	{
		printk((char*)(tmp->name));
		printk("\n");
		tmp = tmp->next;
	}
	
	printk((char*)(tmp->name));
	printk("\n");
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

unsigned int DosStrcmp(char* src, char* trg, unsigned int length)
{
	unsigned int status = 1, i;

	for(i=0;i<length;i++)
		if(src[i] != trg[i])
			status = 0;
	
	return status;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

unsigned int ReleaseDosPackage(PDOSNAME dosNamePackage)
{
	PDOSNAME     tmp    = dosNamePackage;
	unsigned int status = 0;
	
	if(!dosNamePackage)
	{
		panic("Given DosNamePackage pointer is invalid\n");
		return 0;
	}
	
	if(tmp->next)
	{
		Free(tmp->next);
	}
	
	tmp = tmp->next;
	if(tmp)
		ReleaseDosPackage(tmp);
	
	return 1;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
