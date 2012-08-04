/* Author:  Corentin Debains
 * Email:   cdebains@iit.edu
 *
 *
 */
#include "ffsnet.h"
#include "ffsnetCPP.h"

int * Transfer_init_c(UDTArray_c Ssocks, struct metadata * meta, int operation){
	UDTArray SsocksCPP = (UDTArray) Ssocks;
	
	return Transfer_init(UDTArray Ssocks, struct metadata * meta, int operation);
}

int bufferSend_c(UDTArray_c Ssocks, int index, char * buffer){
	UDTArray SsocksCPP = (UDTArray) Ssocks;
	
	return bufferSend(UDTArray Ssocks, int index, char * buffer);
}

int bufferRecv_c(UDTArray_c Ssocks, int index, char * buffer){
	UDTArray SsocksCPP = (UDTArray) Ssocks;
	
	return bufferRecv(UDTArray Ssocks, int index, char * buffer);
}

int Transfer_destroy_c(UDTArray_c Ssocks){
	UDTArray SsocksCPP = (UDTArray) Ssocks;
	
	return Transfer_destroy(UDTArray Ssocks);
}


