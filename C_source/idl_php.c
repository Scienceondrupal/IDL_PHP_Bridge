/*
 * IDL_PHP_Bridge Copyright (C) 2014  New Media Studio, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY * without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "include/idl_rpc.h"
#include "include/idl_export.h"

/*
 * Static variable used to signal that the memory callback is called
 */
static int iCBFlag=0;

/*
 * Prototypes
 */
void RPCArrCBTest(UCHAR *pData);
int RPCVarTest(void);
int RPCStringTest(void);
int RPCScalarTest(CLIENT *idl_client);
int RPCGetArrayTest(CLIENT *idl_client);
int RPCSetArrayTest(CLIENT *idl_client);
int RPCMemoryTest(CLIENT *idl_client);
int RPCExeTest(CLIENT *idl_client);
void RPCFlushOutput(CLIENT *idl_client);


#include "php.h"
#include "php_idlphp.h"

static function_entry idlphp_functions[] = {
    PHP_FE(idlphp, NULL)
    PHP_FE(idlphp_executestr, NULL)
    PHP_FE(idlphp_getmainvariable, NULL)
    PHP_FE(idlphp_setmainstringvar, NULL)
    PHP_FE(idlphp_setmainlongvar, NULL)
    PHP_FE(idlphp_setmaindoublevar, NULL)
    PHP_FE(idlphp_setmainlongarray, NULL)
    PHP_FE(idlphp_setmaindoublearray, NULL)
    PHP_FE(idlphp_setmainstringarray, NULL)
    {NULL, NULL, NULL}
};

zend_module_entry idlphp_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    PHP_IDLPHP_EXTNAME,
    idlphp_functions,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
#if ZEND_MODULE_API_NO >= 20010901
    PHP_IDLPHP_VERSION,
#endif
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_IDLPHP
ZEND_GET_MODULE(idlphp)
#endif

// ==================================================================

PHP_FUNCTION(idlphp_setmainstringarray)
{
	char   *var_name;
	int      var_name_len;
	
  CLIENT            * idl_client;
  IDL_LONG       server_id = 0;
  char                 * host_name      = NULL;
  IDL_VPTR       idl_var, idl_test_var;
  IDL_MEMINT   array_length;
  IDL_STRING   * idl_var_data;
  
  zval                  * zend_array;
  zval                  ** zend_arr_elmt;
  HashTable       * zend_array_hash;
  HashPosition   pointer;
  char                 * string;
  int                    i;
  
 	    
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sa", 
  	 &var_name, &var_name_len,&zend_array) == FAILURE) { 
	   fprintf(stderr,"error parsing arguments\n");
     RETURN_LONG(2);
	 }
	 //printf("zend_hash_get_current_key_type(ht) = %d\n", zend_hash_get_current_key_type(Z_ARRVAL_P(zend_array)));
	
  if( (idl_client = IDL_RPCInit( server_id, host_name)) == (CLIENT*)NULL) {
    printf("Can't register with server on \"%s\"\n", host_name ? host_name : "localhost");
    RETURN_LONG(1);
  }

  zend_array_hash = Z_ARRVAL_P(zend_array);
  array_length = zend_hash_num_elements(zend_array_hash);

  if( (idl_var_data = (IDL_STRING *) IDL_RPCMakeArray(IDL_TYP_STRING, 1, &array_length, 
    IDL_BARR_INI_ZERO, &idl_var)) == (IDL_STRING *) NULL) {
    printf("Can't create IDL array\n");
  }
  //  php_printf("The array contains %lld elements\n", array_length);
  	
	i = 0;
  for(zend_hash_internal_pointer_reset_ex(zend_array_hash, &pointer); 
    zend_hash_get_current_data_ex(zend_array_hash, (void**) &zend_arr_elmt, &pointer) == SUCCESS; 
    zend_hash_move_forward_ex(zend_array_hash, &pointer)) {

    separate_zval(zend_arr_elmt);
    convert_to_string_ex(zend_arr_elmt);
    string = (*zend_arr_elmt)->value.str.val;
    php_printf("Got zend string %s\n", string);
    
    IDL_RPCStrStore(&idl_var_data[i++], string);

  }

  php_printf("setting...\n");
  if(!IDL_RPCSetMainVariable(idl_client, var_name, idl_var)){
     fprintf(stderr,"error setting output variable\n");
     RETURN_LONG(1);
   }
    php_printf("Set!\n");

  // test the new variable
  //IDL_RPCExecuteStr(idl_client, "help, my_string_array");
  //IDL_RPCExecuteStr(idl_client, "print, my_string_array");

  idl_test_var = IDL_RPCGetMainVariable(idl_client, var_name);
  
  if(!idl_test_var) {
      fprintf(stderr, "IDL_RPCSetMainLongArray: Error retrieving variable %s.\n", var_name);
      RETURN_LONG(1);
    }
    
	 if (idl_test_var->type == IDL_TYP_UNDEF) { 
    php_printf("The variable is UNDEFINED\n"); 
    fprintf(stderr, "IDL_RPCSetMainLongArray: Error variable type undefined\n");
    RETURN_LONG(5);
  }	

  if(IDL_RPCGetVarType(idl_test_var) != IDL_TYP_STRING){
      fprintf(stderr, "Return array variable type wrong.\n");
      RETURN_LONG(1);
   }
  if(!IDL_RPCVarIsArray(idl_test_var)){
      fprintf(stderr, "Return array variable is not an array.\n");
      RETURN_LONG(1);
   }
  if(IDL_RPCGetArrayNumElts(idl_test_var) != array_length){
      fprintf(stderr, "Return array variable has wrong legth.\n");
      RETURN_LONG(1);
   }
  IDL_RPCDeltmp(idl_var);
  IDL_RPCDeltmp(idl_test_var);
   
  php_printf("Fini!\n");

  RETURN_LONG(0);

}

// ==================================================================

PHP_FUNCTION(idlphp_setmaindoublearray)
{
	char   *var_name;
	int      var_name_len;
	
  CLIENT       * idl_client;
  IDL_LONG  server_id = 0;
  char            * host_name      = NULL;
  IDL_VPTR           idl_var, idl_test_var;
  IDL_MEMINT      array_length;
  
  zval                  * zend_array;
  zval                  ** zend_arr_elmt;
  HashTable       * zend_array_hash;
  HashPosition   pointer;
  
  double               * pDouble;
  double               num;
  int                       i;
    
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sa", 
  	 &var_name, &var_name_len, &zend_array) == FAILURE) { 
	   fprintf(stderr,"error parsing arguments\n");
     RETURN_LONG(2);
	 }
	 printf("zend_hash_get_current_key_type(ht) = %d\n", zend_hash_get_current_key_type(Z_ARRVAL_P(zend_array)));
	
   zend_array_hash = Z_ARRVAL_P(zend_array);
   array_length = zend_hash_num_elements(zend_array_hash);

   pDouble = (double *)IDL_RPCMakeArray(IDL_TYP_DOUBLE, 1, &array_length, 
				    IDL_BARR_INI_ZERO, &idl_var);
	 
	 php_printf("The array contains %lld elements\n", array_length);

	i = 0;
  for(zend_hash_internal_pointer_reset_ex(zend_array_hash, &pointer); 
    zend_hash_get_current_data_ex(zend_array_hash, (void**) &zend_arr_elmt, &pointer) == SUCCESS; 
    zend_hash_move_forward_ex(zend_array_hash, &pointer)) {

    SEPARATE_ZVAL(zend_arr_elmt);
    convert_to_double_ex(zend_arr_elmt);
    num = (*zend_arr_elmt)->value.dval;
    php_printf("Converted to double %f\n", num);
    
    pDouble[i++] = num;
    
  }

   if( (idl_client = IDL_RPCInit( server_id, host_name)) == (CLIENT*)NULL) {
     printf("Can't register with server on \"%s\"\n",
        host_name ? host_name : "localhost");
     RETURN_LONG(1);
   }

  if(!IDL_RPCSetMainVariable(idl_client, var_name, idl_var)){
     fprintf(stderr,"error setting output variable\n");
     RETURN_LONG(1);
   }
  
  // test the new variable
	 IDL_RPCExecuteStr(idl_client, "print, my_double_array");

  idl_test_var = IDL_RPCGetMainVariable(idl_client, var_name);
  
   if(!idl_test_var){
      fprintf(stderr, "IDL_RPCSetMainLongArray: Error retrieving variable %s.\n", var_name);
      RETURN_LONG(1);
    }
    
	 if (idl_test_var->type == IDL_TYP_UNDEF) { 
    php_printf("The variable is UNDEFINED\n"); 
    fprintf(stderr, "IDL_RPCSetMainLongArray: Error variable type undefined\n");
    RETURN_LONG(5);
  }	

  if(IDL_RPCGetVarType(idl_test_var) != IDL_TYP_DOUBLE){
      fprintf(stderr, "Return array variable type wrong.\n");
      RETURN_LONG(1);
   }
  if(!IDL_RPCVarIsArray(idl_test_var)){
      fprintf(stderr, "Return array variable is not an array.\n");
      RETURN_LONG(1);
   }
  if(IDL_RPCGetArrayNumElts(idl_test_var) != array_length){
      fprintf(stderr, "Return array variable has wrong legth.\n");
      RETURN_LONG(1);
   }
  IDL_RPCDeltmp(idl_var);
  IDL_RPCDeltmp(idl_test_var);
   
  RETURN_LONG(0);
   
}

// ==================================================================

PHP_FUNCTION(idlphp_setmainlongarray)
{
	char   *var_name;
	int      var_name_len;
	
  CLIENT       *idl_client;
  IDL_LONG  server_id = 0;
  char            * host_name      = NULL;
    
  IDL_VPTR           idl_var, idl_test_var;
  IDL_MEMINT      array_length;
  IDL_LONG         *pLong;
  
  zval *zend_array, **zend_arr_elmt;
  HashTable *zend_array_hash;
  HashPosition pointer;
  long num;
  int    i;
    
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sa", 
  	 &var_name, &var_name_len,&zend_array) == FAILURE) { 
	   fprintf(stderr,"error parsing arguments\n");
     RETURN_LONG(2);
	 }
	 printf("zend_hash_get_current_key_type(ht) = %d\n", zend_hash_get_current_key_type(Z_ARRVAL_P(zend_array)));
	
   zend_array_hash = Z_ARRVAL_P(zend_array);
   array_length = zend_hash_num_elements(zend_array_hash);

   pLong = (IDL_LONG*)IDL_RPCMakeArray(IDL_TYP_LONG, 1, &array_length, 
				    IDL_BARR_INI_ZERO, &idl_var);
	 
	 php_printf("The array passed contains %lld elements\n", array_length);

	i = 0;
  for(zend_hash_internal_pointer_reset_ex(zend_array_hash, &pointer); 
    zend_hash_get_current_data_ex(zend_array_hash, (void**) &zend_arr_elmt, &pointer) == SUCCESS; 
    zend_hash_move_forward_ex(zend_array_hash, &pointer)) {

    SEPARATE_ZVAL(zend_arr_elmt);
    convert_to_long_ex(zend_arr_elmt);
    num = (*zend_arr_elmt)->value.lval;
    php_printf("Converted to long %ld\n", num);
    
    pLong[i++] = num;
    
  }

   if( (idl_client = IDL_RPCInit( server_id, host_name)) == (CLIENT*)NULL) {
     printf("Can't register with server on \"%s\"\n",
        host_name ? host_name : "localhost");
     RETURN_LONG(1);
   }

  if(!IDL_RPCSetMainVariable(idl_client, var_name, idl_var)){
     fprintf(stderr,"error setting output variable\n");
     RETURN_LONG(1);
   }
  
  // test the new variable
	 IDL_RPCExecuteStr(idl_client, "help");

  idl_test_var = IDL_RPCGetMainVariable(idl_client, var_name);
  
   if(!idl_test_var){
      fprintf(stderr, "IDL_RPCSetMainLongArray: Error retrieving variable %s.\n", var_name);
      RETURN_LONG(1);
    }
    
	 if (idl_test_var->type == IDL_TYP_UNDEF) { 
    php_printf("The variable is UNDEFINED\n"); 
    fprintf(stderr, "IDL_RPCSetMainLongArray: Error variable type undefined\n");
    RETURN_LONG(5);
  }	

  if(IDL_RPCGetVarType(idl_test_var) != IDL_TYP_LONG){
      fprintf(stderr, "Return array variable type wrong.\n");
      RETURN_LONG(1);
   }
  if(!IDL_RPCVarIsArray(idl_test_var)){
      fprintf(stderr, "Return array variable is not an array.\n");
      RETURN_LONG(1);
   }
  if(IDL_RPCGetArrayNumElts(idl_test_var) != array_length){
      fprintf(stderr, "Return array variable has wrong legth.\n");
      RETURN_LONG(1);
   }
  IDL_RPCDeltmp(idl_var);
  IDL_RPCDeltmp(idl_test_var);
   
  RETURN_LONG(0);

}

// ==================================================================

PHP_FUNCTION(idlphp_setmaindoublevar)
{
	char   *var_name;
	int      var_name_len;
	double   double_var;
	
  CLIENT       *idl_client;
  IDL_LONG  server_id = 0;
  char            * host_name      = NULL;
    
   IDL_VPTR           idl_var, idl_test_var;
   IDL_ALLTYPES   IDLAllType;
         
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sd", 
  	&var_name, &var_name_len,&double_var) == FAILURE) { 
	  fprintf(stderr,"error parsing arguments\n");
    RETURN_LONG(2);
	}

	/*
 * Register the client, check for errors
 */  
   //printf("server_id %d , host_name %s\n", server_id, host_name);
   if( (idl_client = IDL_RPCInit( server_id, host_name)) == (CLIENT*)NULL) {
     printf("Can't register with server on \"%s\"\n",
        host_name ? host_name : "localhost");
     RETURN_LONG(3);
   }

  IDL_RPCExecuteStr(idl_client, "message, /reset");
  
  printf("Double value = %f\n", double_var);

  // create a temporary IDL_VARIABLE
   idl_var = IDL_RPCGettmp();  
   IDLAllType.d = double_var;
   IDL_RPCStoreScalar(idl_var, IDL_TYP_DOUBLE, &IDLAllType);
   
  if(!IDL_RPCSetMainVariable(idl_client, var_name, idl_var)){
    fprintf(stderr,"idlphp_setmaindoublevar error setting variable in server: string ");
    IDL_RPCDeltmp(idl_var);
    RETURN_LONG(4);
  }

	 // now test the variable
	 IDL_RPCExecuteStr(idl_client, "help");
	 
	 idl_test_var = IDL_RPCGetMainVariable(idl_client, var_name);
	 
   if(idl_test_var->type != IDL_TYP_DOUBLE) {
       fprintf(stderr, "idlphp_setmaindoublevar: Error getting variable type\n");
       RETURN_LONG(6);
   }

   if(idl_var->value.l != idl_test_var->value.l) {
       fprintf(stderr, "idlphp_setmaindoublevar: Error getting variable value\n");
       RETURN_LONG(7);
   }

	 if (idl_test_var->type == IDL_TYP_UNDEF) { 
    php_printf("The variable is UNDEFINED\n"); 
    fprintf(stderr, "idlphp_setmaindoublevar: Error variable type undefined\n");
    RETURN_LONG(5);
  }	

   IDL_RPCDeltmp(idl_var);
   IDL_RPCDeltmp(idl_test_var);
   
   RETURN_LONG(1);
}

// ==================================================================

PHP_FUNCTION(idlphp_setmainlongvar)
{
	char   *var_name;
	int      var_name_len;
	long   long_var;
	
  CLIENT       *idl_client;
  IDL_LONG  server_id = 0;
  char            * host_name      = NULL;
    
   IDL_VPTR           idl_var, idl_test_var;
   IDL_ALLTYPES   IDLAllType;
         
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl", 
  	&var_name, &var_name_len,&long_var) == FAILURE) { 
	  fprintf(stderr,"error parsing arguments\n");
    RETURN_LONG(2);
	}

	/*
 * Register the client, check for errors
 */  
   //printf("server_id %d , host_name %s\n", server_id, host_name);
   if( (idl_client = IDL_RPCInit( server_id, host_name)) == (CLIENT*)NULL) {
     printf("Can't register with server on \"%s\"\n",
        host_name ? host_name : "localhost");
     RETURN_LONG(3);
   }

  IDL_RPCExecuteStr(idl_client, "message, /reset");
  
  printf("Long value = %ld\n", long_var);

  // create a temporary IDL_VARIABLE
   idl_var = IDL_RPCGettmp();  
   IDLAllType.l = long_var;
   IDL_RPCStoreScalar(idl_var, IDL_TYP_LONG, &IDLAllType);
   
  if(!IDL_RPCSetMainVariable(idl_client, var_name, idl_var)){
    fprintf(stderr,"idlphp_setmainlongvar error setting variable in server: string ");
    IDL_RPCDeltmp(idl_var);
    RETURN_LONG(4);
  }

	 // now test the variable
	 IDL_RPCExecuteStr(idl_client, "help");
	 
	 idl_test_var = IDL_RPCGetMainVariable(idl_client, var_name);
	 
   if(idl_test_var->type != IDL_TYP_LONG) {
       fprintf(stderr, "idlphp_setmainlongvar: Error getting variable type\n");
       RETURN_LONG(6);
   }

   if(idl_var->value.l != idl_test_var->value.l) {
       fprintf(stderr, "idlphp_setmainlongvar: Error getting variable value\n");
       RETURN_LONG(7);
   }

	 if (idl_test_var->type == IDL_TYP_UNDEF) { 
    php_printf("The variable is UNDEFINED\n"); 
    fprintf(stderr, "idlphp_setmainlongvar: Error variable type undefined\n");
    RETURN_LONG(5);
  }	

   IDL_RPCDeltmp(idl_var);
   IDL_RPCDeltmp(idl_test_var);
   
   RETURN_LONG(1);
}

// ==================================================================

PHP_FUNCTION(idlphp_setmainstringvar)
{
	char *var_name;
	int var_name_len;
	char *str_val;
	int str_val_len;
	
  CLIENT  *idl_client;
  IDL_LONG  server_id = 0;
  char   * host_name      = NULL;
    
   IDL_VPTR   idl_var, idl_test_var;
   IDL_ALLTYPES IDLAllType;
   long i;
         
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", 
  	&var_name, &var_name_len,&str_val,&str_val_len) == FAILURE) { 
	  fprintf(stderr,"error parsing arguments\n");
    RETURN_LONG(2);
	}

	/*
 * Register the client, check for errors
 */  
   //printf("server_id %d , host_name %s\n", server_id, host_name);
   if( (idl_client = IDL_RPCInit( server_id, host_name)) == (CLIENT*)NULL) {
     printf("Can't register with server on \"%s\"\n",
        host_name ? host_name : "localhost");
     RETURN_LONG(3);
   }

  IDL_RPCExecuteStr(idl_client, "message, /reset");
  
  printf("String value = %s\n", str_val);

  // create a temporary IDL_VARIABLE
   idl_var = IDL_RPCGettmp();  
   IDL_RPCStrStore(&IDLAllType.str, str_val);
   IDL_RPCStoreScalar(idl_var, IDL_TYP_STRING, &IDLAllType); 
   
  if(!IDL_RPCSetMainVariable(idl_client, var_name, idl_var)){
    fprintf(stderr,"idlphp_setmainstringvar error setting variable in server: string ");
    IDL_RPCDeltmp(idl_var);
    RETURN_LONG(4);
  }

	 // now test the variable
	 IDL_RPCExecuteStr(idl_client, "help");
	 
	 idl_test_var = IDL_RPCGetMainVariable(idl_client, var_name);
	 
   if(idl_test_var->type != IDL_TYP_STRING) {
       fprintf(stderr, "idlphp_setmainstringvar: Error getting variable type\n");
       RETURN_LONG(6);
   }

   if(strcmp(idl_var->value.str.s, idl_test_var->value.str.s)) {
       fprintf(stderr, "idlphp_setmainstringvar: Error getting variable value\n");
       RETURN_LONG(7);
   }

	 if (idl_test_var->type == IDL_TYP_UNDEF) { 
    php_printf("The variable is UNDEFINED\n"); 
    fprintf(stderr, "idlphp_setmainstringvar: Error variable type undefined\n");
    RETURN_LONG(5);
  }	

   IDL_RPCDeltmp(idl_var);
   IDL_RPCDeltmp(idl_test_var);
   
   RETURN_LONG(1);
}

// ==================================================================
PHP_FUNCTION(idlphp_getmainvariable)
{
	char *var_name;
	int var_name_len;
  CLIENT  *idl_client;
  IDL_LONG  server_id = 0;
  char   * host_name      = NULL;
  long res;
 
   zval *pzv;  // ponter to zend variable
   
   IDL_VPTR   vPtr, vTmp;
   IDL_ALLTYPES type_s;
   IDL_RPC_LINE_S sLine;
   int status;
   long i;
   
   MAKE_STD_ZVAL(pzv);
   
   vPtr = IDL_RPCGettmp();  

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &var_name, &var_name_len) == FAILURE) { 
	  RETURN_NULL();
	}

	/*
 * Register the client, check for errors
 */  
   //printf("server_id %d , host_name %s\n", server_id, host_name);
   if( (idl_client = IDL_RPCInit( server_id, host_name)) == (CLIENT*)NULL){
     printf("Can't register with server on \"%s\"\n",
               host_name ? host_name : "localhost");
     RETURN_LONG(0);
   }

  IDL_RPCExecuteStr(idl_client, "message, /reset");
  
  vTmp = IDL_RPCGetMainVariable(idl_client, var_name); // rename vIdl
  
  if (vTmp->type == IDL_TYP_UNDEF) { 
    php_printf("The variable is UNDEFINED\n"); 
    fprintf(stderr, "IDLPHP_GetMainVariable: Error getting variable\n");
    RETURN_LONG(0);
  }
  //else { 
  //  php_printf("The variable is of type %d\n", vTmp->type); 
  // }
  
  if(vTmp->type == IDL_TYP_BYTE && !IDL_RPCVarIsArray(vTmp)){
  	php_printf("Returning byte as long, is array = %d\n", IDL_RPCVarIsArray(vTmp));
  	long v = vTmp->value.c;
  	//ZVAL_LONG(pzv, v);
  	RETURN_LONG(v);
  }

  if(vTmp->type == IDL_TYP_BYTE && IDL_RPCVarIsArray(vTmp)){
  	php_printf("Returning byte array as long array\n");
  	array_init(return_value);
  	IDL_MEMINT n_elts = vTmp->value.arr->n_elts;
  	
  	UCHAR *data;
  	data = (UCHAR *) vTmp->value.arr->data;
  	
  	//for (i = 0; i < n_elts; i++)  php_printf("data = %d\n", data[i]);
  	for (i = 0; i < n_elts; i++)  add_index_long(return_value, i, data[i]);
  	
  	return;
  }

  if(vTmp->type == IDL_TYP_INT && !IDL_RPCVarIsArray(vTmp)){
  	php_printf("Returning int as long\n");
  	long v = vTmp->value.i;
  	RETURN_LONG(v);
  }

  if(vTmp->type == IDL_TYP_INT && IDL_RPCVarIsArray(vTmp)){
  	php_printf("Returning int array as long array\n");
  	array_init(return_value);
  	IDL_MEMINT n_elts = vTmp->value.arr->n_elts;
  	
  	IDL_INT *data;
  	data = (IDL_INT *) vTmp->value.arr->data;
  	
  	//for (i = 0; i < n_elts; i++)  php_printf("data = %d\n", data[i]);
  	for (i = 0; i < n_elts; i++)  add_index_long(return_value, i, data[i]);
  	
  	return;
  }

  if(vTmp->type == IDL_TYP_UINT && !IDL_RPCVarIsArray(vTmp)){
  	php_printf("Returning uint as long\n");
  	long v = vTmp->value.ui;
  	RETURN_LONG(v);
  }

  if(vTmp->type == IDL_TYP_UINT && IDL_RPCVarIsArray(vTmp)){
  	php_printf("Returning uint array as long array\n");
  	array_init(return_value);
  	IDL_MEMINT n_elts = vTmp->value.arr->n_elts;
  	
  	IDL_UINT *data;
  	data = (IDL_UINT *) vTmp->value.arr->data;
  	
  	//for (i = 0; i < n_elts; i++)  php_printf("data = %d\n", data[i]);
  	for (i = 0; i < n_elts; i++)  add_index_long(return_value, i, data[i]);
  	
  	return;
  }

  if(vTmp->type == IDL_TYP_LONG && !IDL_RPCVarIsArray(vTmp)){
  	php_printf("Returning long\n");
  	RETURN_LONG(vTmp->value.l);
  }
  
  if(vTmp->type == IDL_TYP_LONG && IDL_RPCVarIsArray(vTmp)){
  	php_printf("Returning long array as long array\n");
  	array_init(return_value);
  	IDL_MEMINT n_elts = vTmp->value.arr->n_elts;
  	
  	IDL_LONG *data;
  	data = (IDL_LONG *) vTmp->value.arr->data;
  	
  	//for (i = 0; i < n_elts; i++)  php_printf("data = %d\n", data[i]);
  	for (i = 0; i < n_elts; i++)  add_index_long(return_value, i, data[i]);
  	
  	return;
  }

  if(vTmp->type == IDL_TYP_FLOAT && !IDL_RPCVarIsArray(vTmp)){
  	php_printf("Returning float as double\n");
  	long v = vTmp->value.f;
  	RETURN_LONG(v);
  }
  
  if(vTmp->type == IDL_TYP_FLOAT && IDL_RPCVarIsArray(vTmp)){
  	php_printf("Returning float array as double array\n");
  	array_init(return_value);
  	IDL_MEMINT n_elts = vTmp->value.arr->n_elts;
  	
  	float *data;
  	data = (float *) vTmp->value.arr->data;
  	
  	//for (i = 0; i < n_elts; i++)  php_printf("data = %d\n", data[i]);
  	for (i = 0; i < n_elts; i++)  add_index_double(return_value, i, data[i]);
  	
  	return;
  }
  
  if (Z_TYPE_P(vTmp) == IS_DOUBLE && !IDL_RPCVarIsArray(vTmp)) { 
  	php_printf("Returning double\n");
  	RETURN_DOUBLE(vTmp->value.d);
  }
 	 
  if(vTmp->type == IDL_TYP_DOUBLE && IDL_RPCVarIsArray(vTmp)){
  	php_printf("Returning double as double array\n");
  	array_init(return_value);
  	IDL_MEMINT n_elts = vTmp->value.arr->n_elts;
  	
  	double *data;
  	data = (double *) vTmp->value.arr->data;
  	
  	//for (i = 0; i < n_elts; i++)  php_printf("data = %d\n", data[i]);
  	for (i = 0; i < n_elts; i++)  add_index_double(return_value, i, data[i]);
  	
  	return;
  }
  
  if(vTmp->type == IDL_TYP_STRING && !IDL_RPCVarIsArray(vTmp)){
  	//php_printf("Returning string\n");
  	RETURN_STRING(vTmp->value.str.s, 1);
  }
  
  /*
   if(IDL_RPCExecuteStr(idl_client, "VA_STRING=sindgen(32)") != 1){
      fprintf(stderr,"Error executing command string\n");
      return 1;
   }
   pVtmp = IDL_RPCGetMainVariable(idl_client, "VA_STRING");
   if(IDL_RPCGetVarType(pVtmp) != IDL_TYP_STRING || !IDL_RPCVarIsArray(pVtmp) ||
      IDL_RPCGetArrayNumElts(pVtmp) != 32){
      fprintf(stderr, "IDL_RPCGetMainVariable: Error getting string array\n");
      return 1;
   }
   IDL_RPCDeltmp(pVtmp);
  */
  
  if(vTmp->type == IDL_TYP_STRING && IDL_RPCVarIsArray(vTmp)){
  	php_printf("Returning string array as string array\n");
  	array_init(return_value);
  
  	IDL_MEMINT elt_len = vTmp->value.arr->elt_len;
  	IDL_MEMINT n_elts = vTmp->value.arr->n_elts;
  	
  	IDL_STRING *data;
  	data = (IDL_STRING *) vTmp->value.arr->data;
  	
  	php_printf("element length = %lld\n", elt_len);
  	php_printf("number of elements = %lld\n", n_elts);
  	//for (i = 0; i < n_elts; i++)  php_printf("data = %s  %lld\n", IDL_STRING_STR(*data[i]), i);
  	//char * ptr = (char *) &data[i];
  	char * ptr; 
  	
  	//printf("idl string = %s\n", IDL_STRING_STR(&data[0]));
  	//printf("idl string = %s\n", IDL_STRING_STR(&data[1]));

  	for (i = 0; i < n_elts; i++)  {
  		ptr = IDL_STRING_STR(&data[i]);
  		add_index_string(return_value, i, ptr, 1);
  	}

  	return;
  }

  fprintf(stderr, "IDL_RPCGetMainVariable: Error getting variable\n");
  RETURN_LONG(0);

}
 
// ==================================================================
PHP_FUNCTION(idlphp_executestr)
{
	char *cmd;
	int cmd_len;
  CLIENT  *idl_client;
  IDL_LONG  server_id = 0;
  char   * host_name      = NULL;
  long res;
  
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &cmd, &cmd_len) == FAILURE) { 
	  RETURN_LONG(0);
	} 

	/*
 * Register the client, check for errors
 */  
  //printf("server_id %d , host_name %s\n", server_id, host_name);
   if( (idl_client = IDL_RPCInit( server_id, host_name)) == (CLIENT*)NULL){
     printf("Can't register with server on \"%s\"\n",
               host_name ? host_name : "localhost");
     RETURN_LONG(0);
   }

  IDL_RPCExecuteStr(idl_client, "message, /reset");
  
  res = IDL_RPCExecuteStr(idl_client, cmd);
	if(res != 1){
      fprintf(stderr, "IDL_RPCExecuteStr: Error executing string \n");
      RETURN_LONG(res);
   }
	RETURN_LONG(1);
}

