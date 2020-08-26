
void main(void)
{
    char *vidptr = (char*)0xb8000;
	vidptr[0] = 'X';
	vidptr[1] = 0x02;
	
	
	while(1);
}