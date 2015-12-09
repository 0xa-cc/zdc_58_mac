/*
  +----------------------------------------------------------------------+
  | PHP Version 4                                                        |
  +----------------------------------------------------------------------+
  | This source file is subject to version 2.02 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available at through the world-wide-web at                           |
  | http://www.php.net/license/2_02.txt.                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Sara Golemon <pollita@php.net>                               |
  +----------------------------------------------------------------------+

  $Id: header,v 1.10.8.1 2003/07/14 15:59:18 sniper Exp $ 
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef PHP_MCRYPT_FILTER_H
#define PHP_MCRYPT_FILTER_H

#include "php.h"

#define PHP_MCRYPT_FILTER_EXTNAME		"mcrypt_filter"

extern php_stream_filter_factory php_mcrypt_filter_factory;
extern zend_module_entry mcrypt_filter_module_entry;
#define phpext_mcrypt_filter_ptr &mcrypt_filter_module_entry

PHP_MINIT_FUNCTION(mcrypt_filter);
PHP_MSHUTDOWN_FUNCTION(mcrypt_filter);
PHP_MINFO_FUNCTION(mcrypt_filter);

#endif	/* PHP_MCRYPT_FILTER_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
