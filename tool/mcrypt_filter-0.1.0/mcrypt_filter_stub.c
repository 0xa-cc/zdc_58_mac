/*
  +----------------------------------------------------------------------+
  | PHP Version 4                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2003 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 2.02 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available at through the world-wide-web at                           |
  | http://www.php.net/license/2_02.txt.                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+

  $Id: header,v 1.10.8.1 2003/07/14 15:59:18 sniper Exp $ 
*/

#include "php_mcrypt_filter.h"
#include "php_ini.h"
#include "ext/standard/info.h"

/* {{{ mcrypt_filter_module_entry
 */
zend_module_entry mcrypt_filter_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"mcrypt_filter",
	NULL, /* functions */
	PHP_MINIT(mcrypt_filter),
	PHP_MSHUTDOWN(mcrypt_filter),
	NULL, /* RINIT */
	NULL, /* RSHUTDOWN */
	PHP_MINFO(mcrypt_filter),
#if ZEND_MODULE_API_NO >= 20010901
	PHP_MCRYPT_FILTER_EXTNAME,
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_MCRYPT_FILTER
ZEND_GET_MODULE(mcrypt_filter)
#endif

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(mcrypt_filter)
{
	php_stream_filter_register_factory("mcrypt.*", &php_mcrypt_filter_factory TSRMLS_CC);
	php_stream_filter_register_factory("mdecrypt.*", &php_mcrypt_filter_factory TSRMLS_CC);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(mcrypt_filter)
{
	php_stream_filter_unregister_factory("mcrypt.*" TSRMLS_CC);
	php_stream_filter_unregister_factory("mdecrypt.*" TSRMLS_CC);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(mcrypt_filter)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "mcrypt_filter support", "enabled");
	php_info_print_table_end();
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
