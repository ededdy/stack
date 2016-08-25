/**
 *
 * jump.c:	Modify the value of the return address from the called
 * 		function. So that when the function call returns the
 * 		IP points to a code which is different to the one saved
 * 		in the stack before the function was called.
 *
 * Is a 2016 (gcc 5.4.0) version of the famous example
 * 		code from "Smashing the stack for fun and profit."
 *
 * Author: Sougata Santra (sougata.santra@gmail.com)
 */

#include <stdio.h>
#include <stdlib.h>
 
void anamolous_frame(int num)
{
	char buffer1[5], buffer2[10];
	int *ret;
	long rbp;

	/* Read the value of base pointer. */
	asm("mov %%rbp, %0\n" :"=r"(rbp));
	/*
	 * Add sizeof(long) to get the address of the saved return address
	 * saved in the stack.
	 */
	ret = (void *)(rbp + sizeof(long));
	(*ret) += 0xa;
}

/*
 *  $objdump -S -M intel jump | grep .main.: -A40
 *  00000000004005e9 <main>:
 *  int main(int argc __attribute__((__unused__)),
 *  		char **argv __attribute__((__unused__)))
 *  {
 *    4005e9:	55                   	push   rbp
 *    4005ea:	48 89 e5             	mov    rbp,rsp
 *    4005ed:	48 83 ec 10          	sub    rsp,0x10
 *    4005f1:	89 7d fc             	mov    DWORD PTR [rbp-0x4],edi
 *    4005f4:	48 89 75 f0          	mov    QWORD PTR [rbp-0x10],rsi
 *          printf(" Modify return aggress.\n");
 *    4005f8:	bf b8 06 40 00       	mov    edi,0x4006b8
 *    4005fd:	e8 5e fe ff ff       	call   400460 <puts@plt>
 *          anamolous_frame(1);
 *    400602:	bf 01 00 00 00       	mov    edi,0x1
 *    400607:	e8 8a ff ff ff       	call   400596 <anamolous_frame>
 *          printf(" 2nd Print: attempt to skip this.\n");
 *    40060c:	bf d0 06 40 00       	mov    edi,0x4006d0	<== (orig ret)
 *    400611:	e8 4a fe ff ff       	call   400460 <puts@plt>
 *          printf(" 3rd Print \n");
 *    400616:	bf f2 06 40 00       	mov    edi,0x4006f2	<== (intend ret)
 *    40061b:	e8 40 fe ff ff       	call   400460 <puts@plt>
 *          return EXIT_SUCCESS;
 *    400620:	b8 00 00 00 00       	mov    eax,0x0
 *  }
 *    400625:	c9                   	leave  
 *    400626:	c3                   	ret    
 *    400627:	66 0f 1f 84 00 00 00 	nop    WORD PTR [rax+rax*1+0x0]
 *    40062e:	00 00 
 *
 * From the disassm it is clear that we have to increment the return address by
 * intended return addrsss - original return address = 0x400616 - 0x40060c = 0xa
 * bytes.
 */
int main(int argc __attribute__((__unused__)),
		char **argv __attribute__((__unused__)))
{
	long rip;
	printf("==> Modify return address <==\n");
	anamolous_frame(1);
	printf("\t Failed.\n");
	printf("\t Succeeded. \n");
	return EXIT_SUCCESS;
}
