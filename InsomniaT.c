#include <sys/systm.h>
#include <mach/mach_types.h>

kern_return_t InsomniaT_start (kmod_info_t * ki, void * d) {
 printf("KEXT has loaded!\n");
    return KERN_SUCCESS;
}


kern_return_t InsomniaT_stop (kmod_info_t * ki, void * d) {
	
	printf("KEXT will be unloaded\n");
    return KERN_SUCCESS;
}
