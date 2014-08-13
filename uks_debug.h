/*
 * uks_debug.h
 *
 *  Created on: 2011-7-21
 *      Author: wei.liu
 */

#ifndef UKS_DEBUG_H_
#define UKS_DEBUG_H_


#define  LPRINT(fmt, args ...) printf("");//do { if (DEBUG_PRINT) printf("%s:%s(%d) " fmt, __FILE__, __func__, __LINE__, ##args); } while(0)
//#define  LPRINT(a...)  do { if(1) { printf("%s:%d - ", __FILE__, __LINE__); printf(a); } } while(0)


//#define  LPRINT(a...)  do { if(1) { printf("%s:%d - ", __FILE__, __LINE__); printf(a); } } while(0)
#define  PRINT(args ...)  printf("");//do { if(1) { printf(##args); } } while(0)



#endif /* UKS_DEBUG_H_ */
