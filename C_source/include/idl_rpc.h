/*
 *   $Id: //depot/Release/ENVI50_IDL82/idl/idl_src/idl/idl_rpc.h#1 $
 */

/*
 *  Copyright (c) 1989-2012, Exelis Visual Information Solutions, Inc. All
 *        rights reserved. Unauthorized reproduction is prohibited.
*/

#ifndef _RPC_IDL_
#define _RPC_IDL_

#ifdef __cplusplus
    extern "C" {
#endif

#include <stdio.h>
#include <sys/types.h>
#include <ctype.h>
#include <rpc/rpc.h>
#include <stdlib.h>

#include "idl_export.h"

#define IDL_RPC_MAX_STRLEN	512		/* max string length */

/*
 * Define the default Service IDL and Version number for the
 * IDL rpc Server
 */
#define 	IDL_RPC_DEFAULT_ID		0x2010CAFE
#define     IDL_RPC_DEFAULT_PORT_ID 0xC350
#define  	IDL_RPC_DEFAULT_VERSION		1

/*
 * Define the Macros for the Server request messages
 */
#define 	IDL_RPC_SET_VAR		0x10
#define 	IDL_RPC_GET_VAR		0x20
#define 	IDL_RPC_SET_MAIN_VAR	0x30
#define		IDL_RPC_GET_MAIN_VAR	0x40
#define		IDL_RPC_EXE_STR		0x50
#define		IDL_RPC_OUT_CAPTURE	0x60
#define		IDL_RPC_OUT_STR	        0x70
#define 	IDL_RPC_CLEANUP		0x90

/*
 * Now define the helper routines used to access data in the vptr
 */

#define 	IDL_RPCGetVarType(v)		((v)->type)
#define		IDL_RPCGetVarByte(v)		((v)->value.c)
#define		IDL_RPCGetVarInt(v)		((v)->value.i)
#define		IDL_RPCGetVarUInt(v)		((v)->value.ui)
#define		IDL_RPCGetVarLong(v)		((v)->value.l)
#define		IDL_RPCGetVarULong(v)		((v)->value.ul)
#define		IDL_RPCGetVarLong64(v)		((v)->value.l64)
#define		IDL_RPCGetVarULong64(v)		((v)->value.ul64)
#define		IDL_RPCGetVarFloat(v)		((v)->value.f)
#define		IDL_RPCGetVarDouble(v)		((v)->value.d)
#define		IDL_RPCGetVarComplex(v)		((v)->value.cmp)
#define		IDL_RPCGetVarComplexR(v)	((v)->value.cmp.r)
#define		IDL_RPCGetVarComplexI(v)	((v)->value.cmp.i)
#define		IDL_RPCGetVarDComplex(v)	((v)->value.dcmp)
#define		IDL_RPCGetVarDComplexR(v)	((v)->value.dcmp.r)
#define		IDL_RPCGetVarDComplexI(v)	((v)->value.dcmp.i)
#define		IDL_RPCGetVarString(v)	      IDL_STRING_STR(&((v)->value.str))
#define		IDL_RPCVarIsArray(v)		((v)->flags & IDL_V_ARR)
#define		IDL_RPCGetArrayData(v)		( ((v)->flags & IDL_V_ARR)? \
				(v)->value.arr->data : (UCHAR*)NULL)

#define		IDL_RPCGetArrayDimensions(v)	( ((v)->flags & IDL_V_ARR)? \
				(v)->value.arr->dim : (IDL_MEMINT*)NULL)

#define 	IDL_RPCGetArrayNumDims(v)	( ((v)->flags * IDL_V_ARR)? \
				(v)->value.arr->n_dim : (IDL_MEMINT)NULL)

#define 	IDL_RPCGetArrayNumElts(v)	( ((v)->flags * IDL_V_ARR)? \
				(v)->value.arr->n_elts : 0)

/*
 * Define a structure used to pass variables between client and server.
 */
typedef struct idl_rpc_variable{
    char     *name;
    IDL_VPTR  pVariable;
}IDL_RPC_VARIABLE;

/*
 * Define a structure type that is used to pass output lines between
 * the server and the client
 */

typedef struct idl_rpc_line{
    int    flags;
    char  *buf;
}IDL_RPC_LINE_S;

/*
 * function prototypes for the client side
 */

void IDL_RPCStrEnsureLength(IDL_STRING *s, int n);
void IDL_RPCStrDup(IDL_STRING *str, IDL_MEMINT n);
void IDL_RPCStrDelete(IDL_STRING *str, IDL_MEMINT n);
void IDL_RPCStrStore( IDL_STRING *s, char *fs);

char *IDL_RPCMakeArray(int type, int n_dim, IDL_MEMINT dim[],
			    int init, IDL_VPTR *var);
IDL_VPTR IDL_RPCImportArray(int n_dim, IDL_MEMINT dim[], int type,
			    UCHAR *data, IDL_ARRAY_FREE_CB free_cb);
void IDL_RPCVarCopy(IDL_VPTR src, IDL_VPTR dst);
void IDL_RPCDeltmp(IDL_VPTR vTmp);
IDL_VPTR IDL_RPCGettmp(void);
int IDL_RPCSetVariable(CLIENT *pClient, char *Name, IDL_VPTR pVar);
IDL_VPTR IDL_RPCGetVariable(CLIENT *pClient, char *Name);
int IDL_RPCSetMainVariable(CLIENT *pClient, char *Name, IDL_VPTR pVar);
IDL_VPTR IDL_RPCGetMainVariable(CLIENT *pClient, char *Name);
CLIENT *IDL_RPCInit( long lServerId, char* pszHostname);
CLIENT *IDL_RPCInitWithLoopback( long lportID, char* pszHostname );
int IDL_RPCCleanup( CLIENT *pClient, int iKill);
int IDL_RPCTimeout(long lTimeOut);
int IDL_RPCExecuteStr( CLIENT *pClient, char * pszCommand);
int IDL_RPCOutputCapture( CLIENT *pClient, int n_lines);
int IDL_RPCOutputGetStr(CLIENT *pClient, IDL_RPC_LINE_S *pLine, int first);

void IDL_RPCOutListInit( int maxLines );
char *IDL_RPCOutListDequeue(void);
char *IDL_RPCOutListPop(void);
char * IDL_RPCOutListAdd( char *pData );
int   IDL_RPCOutListCnt(void);
void IDL_RPCToutFunc( int flags, char *buf, int n);
void IDL_RPCStoreScalar(IDL_VPTR dest, int type, IDL_ALLTYPES *value);


#ifdef __cplusplus
    }
#endif

#endif



