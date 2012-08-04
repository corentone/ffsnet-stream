/* Author:  Corentin Debains
 * Email:   cdebains@iit.edu
 *
 *
 */
 
#include <udt.h>
#include <vector>
#include "ec.h" //for metadata struct
 
struct UDTArray_s{
	std::vector<UDTSOCKET> socks;
	struct metadata * meta;
	//int buffernumber;
	int * indexArray;
};

typedef struct UDTArray_s * UDTArray;

int * Transfer_init(UDTArray Ssocks, struct metadata * meta, int operation);
int bufferSend(UDTArray_c Ssocks, int index, char * buffer);
int bufferRecv(UDTArray_c Ssocks, int index, char * buffer);
int Transfer_destroy(UDTArray_c Ssocks); 
