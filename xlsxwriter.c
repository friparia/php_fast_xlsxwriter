/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2015 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_xlsxwriter.h"
#include "xlsxwriter.h"

/* If you declare any globals in php_xlsxwriter.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(xlsxwriter)
*/

/* True global resources - no need for thread safety here */

/* {{{ xlsxwriter_functions[]
 *
 * Every user visible function must have an entry in xlsxwriter_functions[].
 */
ZEND_BEGIN_ARG_INFO(arginfo_xlsx_write, 0)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO(arginfo_xlsx_write_by_sheet, 0)
ZEND_END_ARG_INFO()
const zend_function_entry xlsxwriter_functions[] = {
	PHP_FE(xlsx_write, arginfo_xlsx_write)
	PHP_FE(xlsx_write_by_sheet, arginfo_xlsx_write_by_sheet)
	PHP_FE_END	/* Must be the last line in xlsxwriter_functions[] */
};
/* }}} */

/* {{{ xlsxwriter_module_entry
 */
zend_module_entry xlsxwriter_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"xlsxwriter",
	xlsxwriter_functions,
	PHP_MINIT(xlsxwriter),
	PHP_MSHUTDOWN(xlsxwriter),
	NULL,		/* Replace with NULL if there's nothing to do at request start */
	NULL,	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(xlsxwriter),
#if ZEND_MODULE_API_NO >= 20010901
	PHP_XLSXWRITER_VERSION,
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_XLSXWRITER
ZEND_GET_MODULE(xlsxwriter)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("xlsxwriter.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_xlsxwriter_globals, xlsxwriter_globals)
    STD_PHP_INI_ENTRY("xlsxwriter.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_xlsxwriter_globals, xlsxwriter_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_xlsxwriter_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_xlsxwriter_init_globals(zend_xlsxwriter_globals *xlsxwriter_globals)
{
	xlsxwriter_globals->global_value = 0;
	xlsxwriter_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(xlsxwriter)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(xlsxwriter)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(xlsxwriter)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "xlsxwriter support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */


/* Remove the following function when you have successfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

PHP_FUNCTION(xlsx_write)
{
#if PHP_MAJOR_VERSION >= 7
	zval *arr, *sub_arr, *data, *sub_data;
#else
	zval *arr, *sub_arr, **data, **sub_data;
	HashPosition pointer, sub_pointer;
#endif
	HashTable *arr_hash;
	HashTable *sub_arr_hash;
	char *filename;
	int filename_length;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "as", &arr, &filename, &filename_length) == FAILURE){
		RETURN_NULL();
	}

	arr_hash = Z_ARRVAL_P(arr);
	lxw_workbook *workbook = new_workbook(filename);
	lxw_worksheet *worksheet = workbook_add_worksheet(workbook, NULL);

	int row = 0;
	int col = 0;

#if PHP_MAJOR_VERSION >= 7
	ZEND_HASH_FOREACH_VAL(arr_hash, data){
		if(Z_TYPE_P(data) == IS_ARRAY){
			sub_arr = data;
			sub_arr_hash = Z_ARRVAL_P(sub_arr);
			col = 0;
			ZEND_HASH_FOREACH_VAL(sub_arr_hash, sub_data){
				if(Z_TYPE_P(sub_data) != IS_ARRAY){
					convert_to_string(sub_data);
					worksheet_write_string(worksheet, row, col, Z_STRVAL_P(sub_data), NULL);
					col++;
				}
			}ZEND_HASH_FOREACH_END();
			row++;
		}else{
			if(Z_TYPE_P(data) != IS_ARRAY){
				convert_to_string(data);
				worksheet_write_string(worksheet, row, col, Z_STRVAL_P(data), NULL);
				col++;
			}
		}
	}ZEND_HASH_FOREACH_END();
#else
	for(zend_hash_internal_pointer_reset_ex(arr_hash, &pointer); zend_hash_get_current_data_ex(arr_hash, (void**) &data, &pointer) == SUCCESS; zend_hash_move_forward_ex(arr_hash, &pointer)){
		if(Z_TYPE_PP(data) == IS_ARRAY){
			sub_arr = *data;
			sub_arr_hash = Z_ARRVAL_P(sub_arr);
			col = 0;
			for(zend_hash_internal_pointer_reset_ex(sub_arr_hash, &sub_pointer); zend_hash_get_current_data_ex(sub_arr_hash, (void**) &sub_data, &sub_pointer) == SUCCESS; zend_hash_move_forward_ex(sub_arr_hash, &sub_pointer)){
				if(Z_TYPE_PP(sub_data) != IS_ARRAY){
					convert_to_string(*sub_data);
					worksheet_write_string(worksheet, row, col, Z_STRVAL_PP(sub_data), NULL);
					col++;
				}
			}
			row++;
		}else{
			if(Z_TYPE_PP(data) != IS_ARRAY){
				convert_to_string(*data);
				worksheet_write_string(worksheet, row, col, Z_STRVAL_PP(data), NULL);
				col++;
			}
		}
	}
#endif
	workbook_close(workbook);
	RETURN_TRUE
}

PHP_FUNCTION(xlsx_write_by_sheet)
{
	HashTable *main_arr_hash;
	HashTable *arr_hash;
	HashTable *sub_arr_hash;
#if PHP_MAJOR_VERSION >= 7
	zval *main_arr, *arr, *sub_arr, *main_data, *data, *sub_data;
#else
	zval *main_arr, *arr, *sub_arr, **main_data, **data, **sub_data;
	HashPosition main_pointer, pointer, sub_pointer;
#endif
	char *filename;
	int filename_length;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "as", &main_arr, &filename, &filename_length) == FAILURE){
		RETURN_NULL();
	}

	lxw_workbook *workbook = new_workbook(filename);
	lxw_worksheet *worksheet;

	int row; 
	int col;
	main_arr_hash = Z_ARRVAL_P(main_arr);
#if PHP_MAJOR_VERSION >= 7
	ulong num_key;
	zend_string *key = NULL;
	ZEND_HASH_FOREACH_KEY_VAL(main_arr_hash, num_key, key, main_data){
		if (key) {
			worksheet = workbook_add_worksheet(workbook, key->val);
		}else{
			worksheet = workbook_add_worksheet(workbook, NULL);
		}
		arr = main_data;
		arr_hash = Z_ARRVAL_P(arr);
		row = 0;
		col = 0;
		ZEND_HASH_FOREACH_VAL(arr_hash, data){
			if(Z_TYPE_P(data) == IS_ARRAY){
				sub_arr = data;
				sub_arr_hash = Z_ARRVAL_P(sub_arr);
				col = 0;
				ZEND_HASH_FOREACH_VAL(sub_arr_hash, sub_data){
					if(Z_TYPE_P(sub_data) != IS_ARRAY){
						convert_to_string(sub_data);
						worksheet_write_string(worksheet, row, col, Z_STRVAL_P(sub_data), NULL);
						col++;
					}
				} ZEND_HASH_FOREACH_END();
				row++;
			}else{
				if(Z_TYPE_P(data) != IS_ARRAY){
					convert_to_string(data);
					worksheet_write_string(worksheet, row, col, Z_STRVAL_P(data), NULL);
					col++;
				}
			}
		} ZEND_HASH_FOREACH_END();
	} ZEND_HASH_FOREACH_END();
	zend_string_release(key);
#else
	long index;
	char *key;
	int key_len;
	for(zend_hash_internal_pointer_reset_ex(main_arr_hash, &main_pointer); zend_hash_get_current_data_ex(main_arr_hash, (void**) &main_data, &main_pointer) == SUCCESS; zend_hash_move_forward_ex(main_arr_hash, &main_pointer)){
		if(zend_hash_get_current_key_ex(main_arr_hash, &key, &key_len, &index, 0, &main_pointer) == HASH_KEY_IS_STRING){
			worksheet = workbook_add_worksheet(workbook, key);
		}else{
			php_printf("%ld", index);
			worksheet = workbook_add_worksheet(workbook, NULL);
		}
		arr = *main_data;
		arr_hash = Z_ARRVAL_P(arr);
		row = 0;
		col = 0;
		for(zend_hash_internal_pointer_reset_ex(arr_hash, &pointer); zend_hash_get_current_data_ex(arr_hash, (void**) &data, &pointer) == SUCCESS; zend_hash_move_forward_ex(arr_hash, &pointer)){
			if(Z_TYPE_PP(data) == IS_ARRAY){
				sub_arr = *data;
				sub_arr_hash = Z_ARRVAL_P(sub_arr);
				col = 0;
				for(zend_hash_internal_pointer_reset_ex(sub_arr_hash, &sub_pointer); zend_hash_get_current_data_ex(sub_arr_hash, (void**) &sub_data, &sub_pointer) == SUCCESS; zend_hash_move_forward_ex(sub_arr_hash, &sub_pointer)){
					if(Z_TYPE_PP(sub_data) != IS_ARRAY){
						convert_to_string(*sub_data);
						worksheet_write_string(worksheet, row, col, Z_STRVAL_PP(sub_data), NULL);
						col++;
					}
				}
				row++;
			}else{
				if(Z_TYPE_PP(data) != IS_ARRAY){
					convert_to_string(*data);
					worksheet_write_string(worksheet, row, col, Z_STRVAL_PP(data), NULL);
					col++;
				}
			}
		}
	}
#endif

	workbook_close(workbook);
	RETURN_TRUE
}

/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
