/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2008 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Andrey Hristov <andrey@php.net>                              |
  |         Ulf Wendel <uw@php.net>                                      |
  +----------------------------------------------------------------------+
*/

/* $Id: mysqlnd_ms.h 311091 2011-05-16 15:42:48Z andrey $ */
#ifndef MYSQLND_MS_ENUM_N_DEF_H
#define MYSQLND_MS_ENUM_N_DEF_H

#if MYSQLND_VERSION_ID < 50010 && !defined(MYSQLND_CONN_DATA_DEFINED)
typedef MYSQLND MYSQLND_CONN_DATA;
#endif

#if MYSQLND_VERSION_ID >= 50010
#define MS_DECLARE_AND_LOAD_CONN_DATA(conn_data, connection) \
	MYSQLND_MS_CONN_DATA ** conn_data = \
		(MYSQLND_MS_CONN_DATA **) mysqlnd_plugin_get_plugin_connection_data_data((connection), mysqlnd_ms_plugin_id)

#define MS_LOAD_CONN_DATA(conn_data, connection) \
	(conn_data) = (MYSQLND_MS_CONN_DATA **) mysqlnd_plugin_get_plugin_connection_data_data((connection), mysqlnd_ms_plugin_id)

#define MS_CALL_ORIGINAL_CONN_HANDLE_METHOD(method) ms_orig_mysqlnd_conn_handle_methods->method
#define MS_CALL_ORIGINAL_CONN_DATA_METHOD(method) ms_orig_mysqlnd_conn_methods->method
extern struct st_mysqlnd_conn_data_methods * ms_orig_mysqlnd_conn_methods;
extern struct st_mysqlnd_conn_methods * ms_orig_mysqlnd_conn_handle_methods;

#else

#define MS_DECLARE_AND_LOAD_CONN_DATA(conn_data, connection) \
	MYSQLND_MS_CONN_DATA ** conn_data = \
		(MYSQLND_MS_CONN_DATA **) mysqlnd_plugin_get_plugin_connection_data((connection), mysqlnd_ms_plugin_id)

#define MS_LOAD_CONN_DATA(conn_data, connection) \
	(conn_data) = (MYSQLND_MS_CONN_DATA **) mysqlnd_plugin_get_plugin_connection_data((connection), mysqlnd_ms_plugin_id)

#define MS_CALL_ORIGINAL_CONN_HANDLE_METHOD(method) ms_orig_mysqlnd_conn_methods->method
#define MS_CALL_ORIGINAL_CONN_DATA_METHOD(method) ms_orig_mysqlnd_conn_methods->method
extern struct st_mysqlnd_conn_methods * ms_orig_mysqlnd_conn_methods;
#endif

#ifndef MYSQLND_HAS_INJECTION_FEATURE
#define MS_LOAD_STMT_DATA(stmt_data, statement) \
	(stmt_data) = (MYSQLND_MS_STMT_DATA **)  mysqlnd_plugin_get_plugin_stmt_data((statement), mysqlnd_ms_plugin_id);
#endif


#if MYSQLND_VERSION_ID < 50010
#define MYSQLND_MS_ERROR_INFO(conn_object) ((conn_object)->error_info)
#else
#define MYSQLND_MS_ERROR_INFO(conn_object) (*((conn_object)->error_info))
#endif

#if MYSQLND_VERSION_ID < 50010
#define MYSQLND_MS_UPSERT_STATUS(conn_object) ((conn_object)->upsert_status)
#else
#define MYSQLND_MS_UPSERT_STATUS(conn_object) (*((conn_object)->upsert_status))
#endif



#define BEGIN_ITERATE_OVER_SERVER_LISTS(el, masters, slaves) \
{ \
	/* need to cast, as masters of slaves could be const. We use external llist_position, so this is safe */ \
	DBG_INF_FMT("master(%p) has %d, slave(%p) has %d", \
		(masters), zend_llist_count((zend_llist *) (masters)), (slaves), zend_llist_count((zend_llist *) (slaves))); \
	{ \
		MYSQLND_MS_LIST_DATA ** el_pp;\
		zend_llist * lists[] = {NULL, (masters), (zend_llist *) (slaves), NULL}; \
		zend_llist ** list = lists; \
		while (*++list) { \
			zend_llist_position	pos; \
			/* search the list of easy handles hanging off the multi-handle */ \
			for (el_pp = (MYSQLND_MS_LIST_DATA **) zend_llist_get_first_ex(*list, &pos); \
				 el_pp && ((el) = *el_pp) && (el)->conn; \
				 el_pp = (MYSQLND_MS_LIST_DATA **) zend_llist_get_next_ex(*list, &pos)) \
			{ \

#define END_ITERATE_OVER_SERVER_LISTS \
			} \
		} \
	} \
}


#define BEGIN_ITERATE_OVER_SERVER_LISTS_NEW(el, masters, slaves) \
{ \
	/* need to cast, as masters of slaves could be const. We use external llist_position, so this is safe */ \
	DBG_INF_FMT("master(%p) has %d, slave(%p) has %d", \
				(masters), zend_llist_count((zend_llist *) (masters)), (slaves), zend_llist_count((zend_llist *) (slaves))); \
	{ \
		MYSQLND_MS_LIST_DATA ** el_pp; \
		zend_llist * internal_master_list = (masters); \
		zend_llist * internal_slave_list = (slaves); \
		zend_llist * internal_list = internal_master_list; \
		zend_llist_position	pos; \
		/* search the list of easy handles hanging off the multi-handle */ \
		for ((el_pp = (MYSQLND_MS_LIST_DATA **) zend_llist_get_first_ex(internal_list, &pos)) \
			  || ((internal_list = internal_slave_list) \
			      && \
				  (el_pp = (MYSQLND_MS_LIST_DATA **) zend_llist_get_first_ex(internal_list, &pos))) ; \
			 el_pp && ((el) = *el_pp) && (el)->conn; \
		 	 (el_pp = (MYSQLND_MS_LIST_DATA **) zend_llist_get_next_ex(internal_list, &pos)) \
			 || \
			 ( \
				(internal_list == internal_master_list) \
				&& \
				/* yes, we need an assignment */ \
				(internal_list = internal_slave_list) \
				&& \
				(el_pp = (MYSQLND_MS_LIST_DATA **) zend_llist_get_first_ex(internal_slave_list, &pos)) \
			 ) \
			) \
		{ \

#define END_ITERATE_OVER_SERVER_LISTS_NEW \
		} \
	} \
}

#define BEGIN_ITERATE_OVER_SERVER_LIST(el, list) \
{ \
	/* need to cast, as this list could be const. We use external llist_position, so this is safe */ \
	DBG_INF_FMT("list(%p) has %d", (list), zend_llist_count((zend_llist *) (list))); \
	{ \
		MYSQLND_MS_LIST_DATA ** MACRO_el_pp;\
		zend_llist_position	MACRO_pos; \
		/* search the list of easy handles hanging off the multi-handle */ \
		for (((el) = NULL), MACRO_el_pp = (MYSQLND_MS_LIST_DATA **) zend_llist_get_first_ex((zend_llist *)(list), &MACRO_pos); \
			 MACRO_el_pp && ((el) = *MACRO_el_pp) && (el)->conn; \
			 ((el) = NULL), MACRO_el_pp = (MYSQLND_MS_LIST_DATA **) zend_llist_get_next_ex((zend_llist *)(list), &MACRO_pos)) \
		{ \

#define END_ITERATE_OVER_SERVER_LIST \
		} \
	} \
}

#define MS_TIMEVAL_TO_UINT64(tp) (uint64_t)(tp.tv_sec*1000000 + tp.tv_usec)
#define MS_TIME_SET(time_now) \
	{ \
		struct timeval __tp = {0}; \
		struct timezone __tz = {0}; \
		gettimeofday(&__tp, &__tz); \
		(time_now) = MS_TIMEVAL_TO_UINT64(__tp); \
	} \

#define MS_TIME_DIFF(run_time) \
	{ \
		uint64_t __now; \
		MS_TIME_SET(__now); \
		(run_time) = __now - (run_time); \
	} \


#define MS_WARN_AND_RETURN_IF_TRX_FORBIDS_FAILOVER(stgy, retval) \
   if ((TRUE == (stgy)->in_transaction) && (TRUE == (stgy)->trx_stop_switching)) { \
		mysqlnd_ms_client_n_php_error(error_info, CR_UNKNOWN_ERROR, UNKNOWN_SQLSTATE, E_WARNING TSRMLS_CC,  \
					MYSQLND_MS_ERROR_PREFIX " Automatic failover is not permitted in the middle of a transaction"); \
		DBG_INF("In transaction, no switch allowed"); \
		DBG_RETURN((retval)); \
	} \


#define MASTER_SWITCH "ms=master"
#define SLAVE_SWITCH "ms=slave"
#define LAST_USED_SWITCH "ms=last_used"
#define ALL_SERVER_SWITCH "ms=all"


#define MASTER_NAME							"master"
#define SLAVE_NAME							"slave"
#define PICK_RANDOM							"random"
#define PICK_ONCE							"sticky"
#define PICK_RROBIN							"roundrobin"
#define PICK_USER							"user"
#define PICK_USER_MULTI						"user_multi"
#define PICK_TABLE							"table"
#define PICK_QOS							"quality_of_service"
#define PICK_GROUPS							"node_groups"
#define LAZY_NAME							"lazy_connections"
#define FAILOVER_NAME						"failover"
#define FAILOVER_STRATEGY_NAME				"strategy"
#define FAILOVER_STRATEGY_DISABLED		 	"disabled"
#define FAILOVER_STRATEGY_MASTER			"master"
#define FAILOVER_STRATEGY_LOOP				"loop_before_master"
#define FAILOVER_MAX_RETRIES        		"max_retries"
#define FAILOVER_REMEMBER_FAILED    		"remember_failed"
#define MASTER_ON_WRITE_NAME				"master_on_write"
#define TRX_STICKINESS_NAME					"trx_stickiness"
#define TRX_STICKINESS_MASTER				"master"
#define TRX_STICKINESS_ON					"on"
#define TABLE_RULES							"rules"
#define SECT_SERVER_CHARSET_NAME			"server_charset"
#define SECT_HOST_NAME						"host"
#define SECT_PORT_NAME						"port"
#define SECT_SOCKET_NAME					"socket"
#define SECT_USER_NAME						"user"
#define SECT_PASS_NAME						"password"
#define SECT_DB_NAME						"db"
#define SECT_CONNECT_FLAGS_NAME				"connect_flags"
#define SECT_FILTER_PRIORITY_NAME 			"priority"
#define SECT_FILTER_NAME					"filters"
#define SECT_USER_CALLBACK					"callback"
#define SECT_QOS_STRONG						"strong_consistency"
#define SECT_QOS_SESSION					"session_consistency"
#define SECT_QOS_EVENTUAL					"eventual_consistency"
#define SECT_QOS_AGE						"age"
#define SECT_QOS_CACHE						"cache"
#define SECT_G_TRX_NAME						"global_transaction_id_injection"
#define SECT_G_TRX_ON_COMMIT				"on_commit"
#define SECT_G_TRX_REPORT_ERROR 			"report_error"
#define SECT_G_TRX_FETCH_LAST_GTID 			"fetch_last_gtid"
#define SECT_G_TRX_CHECK_FOR_GTID 			"check_for_gtid"
#define SECT_G_TRX_WAIT_FOR_GTID_TIMEOUT 	"wait_for_gtid_timeout"
#define SECT_LB_WEIGHTS						"weights"


typedef enum
{
	STATEMENT_SELECT,
	STATEMENT_INSERT,
	STATEMENT_UPDATE,
	STATEMENT_DELETE,
	STATEMENT_TRUNCATE,
	STATEMENT_REPLACE,
	STATEMENT_RENAME,
	STATEMENT_ALTER,
	STATEMENT_DROP,
	STATEMENT_CREATE
} enum_mysql_statement_type;


enum enum_which_server
{
	USE_MASTER,
	USE_SLAVE,
	USE_LAST_USED,
	USE_ALL
};


enum mysqlnd_ms_server_pick_strategy
{
	SERVER_PICK_RROBIN,
	SERVER_PICK_RANDOM,
	SERVER_PICK_USER,
	SERVER_PICK_USER_MULTI,
	SERVER_PICK_TABLE,
	SERVER_PICK_QOS,
	SERVER_PICK_GROUPS,
	SERVER_PICK_LAST_ENUM_ENTRY
};

/* it should work also without any params, json config to the ctor will be NULL */
#define DEFAULT_PICK_STRATEGY SERVER_PICK_RANDOM

enum mysqlnd_ms_server_failover_strategy
{
	SERVER_FAILOVER_DISABLED,
	SERVER_FAILOVER_MASTER,
	SERVER_FAILOVER_LOOP
};

#define DEFAULT_FAILOVER_STRATEGY SERVER_FAILOVER_DISABLED
#define DEFAULT_FAILOVER_MAX_RETRIES 1
#define DEFAULT_FAILOVER_REMEMBER_FAILED 0

enum mysqlnd_ms_trx_stickiness_strategy
{
	TRX_STICKINESS_STRATEGY_DISABLED,
	TRX_STICKINESS_STRATEGY_MASTER,
	TRX_STICKINESS_STRATEGY_ON
};
#define DEFAULT_TRX_STICKINESS_STRATEGY TRX_STICKINESS_STRATEGY_DISABLED


typedef enum mysqlnd_ms_collected_stats
{
	MS_STAT_USE_SLAVE,
	MS_STAT_USE_MASTER,
	MS_STAT_USE_SLAVE_GUESS,
	MS_STAT_USE_MASTER_GUESS,
	MS_STAT_USE_SLAVE_FORCED,
	MS_STAT_USE_MASTER_FORCED,
	MS_STAT_USE_LAST_USED_FORCED,
	MS_STAT_USE_SLAVE_CALLBACK,
	MS_STAT_USE_MASTER_CALLBACK,
	MS_STAT_NON_LAZY_CONN_SLAVE_SUCCESS,
	MS_STAT_NON_LAZY_CONN_SLAVE_FAILURE,
	MS_STAT_NON_LAZY_CONN_MASTER_SUCCESS,
	MS_STAT_NON_LAZY_CONN_MASTER_FAILURE,
	MS_STAT_LAZY_CONN_SLAVE_SUCCESS,
	MS_STAT_LAZY_CONN_SLAVE_FAILURE,
	MS_STAT_LAZY_CONN_MASTER_SUCCESS,
	MS_STAT_LAZY_CONN_MASTER_FAILURE,
	MS_STAT_TRX_AUTOCOMMIT_ON,
	MS_STAT_TRX_AUTOCOMMIT_OFF,
	MS_STAT_TRX_MASTER_FORCED,
#ifndef MYSQLND_HAS_INJECTION_FEATURE
	MS_STAT_GTID_AUTOCOMMIT_SUCCESS,
	MS_STAT_GTID_AUTOCOMMIT_FAILURE,
	MS_STAT_GTID_COMMIT_SUCCESS,
	MS_STAT_GTID_COMMIT_FAILURE,
	MS_STAT_GTID_IMPLICIT_COMMIT_SUCCESS,
	MS_STAT_GTID_IMPLICIT_COMMIT_FAILURE,
#endif
	MS_STAT_LAST /* Should be always the last */
} enum_mysqlnd_ms_collected_stats;

#define MYSQLND_MS_INC_STATISTIC(stat) MYSQLND_INC_STATISTIC(MYSQLND_MS_G(collect_statistics), mysqlnd_ms_stats, (stat))
#define MYSQLND_MS_INC_STATISTIC_W_VALUE(stat, value) MYSQLND_INC_STATISTIC_W_VALUE(MYSQLND_MS_G(collect_statistics), mysqlnd_ms_stats, (stat), (value))


typedef struct st_mysqlnd_ms_list_data
{
	char * name_from_config;
	MYSQLND_CONN_DATA * conn;
	char * host;
	char * user;
	char * passwd;
	size_t passwd_len;
	unsigned int port;
	char * socket;
	char * db;
	size_t db_len;
	unsigned long connect_flags;
	char * emulated_scheme;
	size_t emulated_scheme_len;
	zend_bool persistent;
} MYSQLND_MS_LIST_DATA;


typedef struct st_mysqlnd_ms_filter_data
{
	void (*filter_dtor)(struct st_mysqlnd_ms_filter_data * TSRMLS_DC);
	char * name;
	size_t name_len;
	enum mysqlnd_ms_server_pick_strategy pick_type;
	zend_bool multi_filter;
	zend_bool persistent;
} MYSQLND_MS_FILTER_DATA;


typedef struct st_mysqlnd_ms_filter_user_data
{
	MYSQLND_MS_FILTER_DATA parent;
	zval * user_callback;
	zend_bool callback_valid;
} MYSQLND_MS_FILTER_USER_DATA;


typedef struct st_mysqlnd_ms_filter_table_data
{
	MYSQLND_MS_FILTER_DATA parent;
	HashTable master_rules;
	HashTable slave_rules;
} MYSQLND_MS_FILTER_TABLE_DATA;


typedef struct st_mysqlnd_ms_filter_rr_data
{
	MYSQLND_MS_FILTER_DATA parent;
	HashTable master_context;
	HashTable slave_context;
	HashTable lb_weight;
} MYSQLND_MS_FILTER_RR_DATA;


typedef struct st_mysqlnd_ms_filter_rr_context
{
	unsigned int pos;
	zend_llist weight_list;
} MYSQLND_MS_FILTER_RR_CONTEXT;


typedef struct st_mysqlnd_ms_filter_lb_weight
{
	unsigned int weight;
	unsigned int current_weight;
	zend_bool persistent;
} MYSQLND_MS_FILTER_LB_WEIGHT;


typedef struct st_mysqlnd_ms_filter_lb_weight_in_context
{
	MYSQLND_MS_FILTER_LB_WEIGHT * lb_weight;
	MYSQLND_MS_LIST_DATA * element;
} MYSQLND_MS_FILTER_LB_WEIGHT_IN_CONTEXT;


typedef struct st_mysqlnd_ms_filter_random_lb_context
{
	zend_llist sort_list;
	unsigned int total_weight;
} MYSQLND_MS_FILTER_RANDOM_LB_CONTEXT;


typedef struct st_mysqlnd_ms_filter_random_data
{
	MYSQLND_MS_FILTER_DATA parent;
	struct {
		HashTable master_context;
		HashTable slave_context;
		zend_bool once;
	} sticky;
	HashTable lb_weight;
	struct {
		HashTable master_context;
		HashTable slave_context;
	} weight_context;
} MYSQLND_MS_FILTER_RANDOM_DATA;


enum mysqlnd_ms_filter_qos_consistency
{
	CONSISTENCY_STRONG,
	CONSISTENCY_SESSION,
	CONSISTENCY_EVENTUAL,
	CONSISTENCY_LAST_ENUM_ENTRY
};

enum mysqlnd_ms_filter_qos_option
{
	QOS_OPTION_NONE,
	QOS_OPTION_GTID,
	QOS_OPTION_AGE,
	QOS_OPTION_CACHE,
	QOS_OPTION_LAST_ENUM_ENTRY
};

/* using struct because we will likely add cache ttl later */
typedef struct st_mysqlnd_ms_filter_qos_option_data
{
  char * gtid;
  size_t gtid_len;
  long age;
  uint ttl;
} MYSQLND_MS_FILTER_QOS_OPTION_DATA;

typedef struct st_mysqlnd_ms_filter_qos_data
{
	MYSQLND_MS_FILTER_DATA parent;
	enum mysqlnd_ms_filter_qos_consistency consistency;
	enum mysqlnd_ms_filter_qos_option option;
	MYSQLND_MS_FILTER_QOS_OPTION_DATA option_data;
} MYSQLND_MS_FILTER_QOS_DATA;

typedef struct st_mysqlnd_ms_filter_groups_data
{
	MYSQLND_MS_FILTER_DATA parent;
	HashTable groups;
} MYSQLND_MS_FILTER_GROUPS_DATA;

typedef struct st_mysqlnd_ms_filter_groups_data_group
{
	HashTable master_context;
	HashTable slave_context;
} MYSQLND_MS_FILTER_GROUPS_DATA_GROUP;

/*
 NOTE: Some elements are available with every connection, some
 are set for the global/proxy connection only. The global/proxy connection
 is the handle provided by the user. The other connections are the "hidden"
 ones that MS openes to the cluster nodes.
*/
typedef struct st_mysqlnd_ms_conn_data
{
	zend_bool initialized;
	zend_bool skip_ms_calls;
	MYSQLND_CONN_DATA * proxy_conn;
	char * connect_host;
	zend_llist master_connections;
	zend_llist slave_connections;

	const MYSQLND_CHARSET * server_charset;

	/* Global LB strategy set on proxy conn */
	struct mysqlnd_ms_lb_strategies {
		HashTable table_filters;

		enum mysqlnd_ms_server_failover_strategy failover_strategy;
		uint failover_max_retries;
		zend_bool failover_remember_failed;
		HashTable failed_hosts;

		zend_bool mysqlnd_ms_flag_master_on_write;
		zend_bool master_used;

		/* note: some flags may not be used, however saves us a ton of ifdef to declare them anyway */
		enum mysqlnd_ms_trx_stickiness_strategy trx_stickiness_strategy;
		zend_bool trx_stop_switching;
		zend_bool trx_read_only;
		zend_bool trx_autocommit_off;

		/* buffered tx_begin call */
		zend_bool 		trx_begin_required;
		unsigned int 	trx_begin_mode;
		char *		 	trx_begin_name;

		zend_bool in_transaction;

		MYSQLND_CONN_DATA * last_used_conn;
		MYSQLND_CONN_DATA * random_once_slave;

		zend_llist * filters;
	} stgy;

	struct st_mysqlnd_ms_conn_credentials {
		char * user;
		char * passwd;
		size_t passwd_len;
		char * db;
		size_t db_len;
		unsigned int port;
		char * socket;
		unsigned long mysql_flags;
	} cred;

#ifndef MYSQLND_HAS_INJECTION_FEATURE
	/* per connection trx context set on proxy conn and all others */
	struct st_mysqlnd_ms_global_trx_injection {
		char * on_commit;
		size_t on_commit_len;
		char * fetch_last_gtid;
		size_t fetch_last_gtid_len;
		char * check_for_gtid;
		size_t check_for_gtid_len;
		unsigned int wait_for_gtid_timeout;
		/*
		 TODO: This seems to be the only per-connection value.
		 We may want to split up the structure into a global and
		 local part. is_master needs to be local/per-connection.
		 The rest could probably be global, like with stgy and
		 LB weigth.
		*/
		zend_bool is_master;
		zend_bool report_error;
	} global_trx;
#endif
} MYSQLND_MS_CONN_DATA;


typedef struct st_mysqlnd_ms_table_filter
{
	char * host_id;
	size_t host_id_len;
	char * wild;
	size_t wild_len;
#ifdef WE_NEED_NEXT
	struct st_mysqlnd_ms_table_filter * next;
#endif
	unsigned int priority;
	zend_bool persistent;
} MYSQLND_MS_TABLE_FILTER;


typedef struct st_mysqlnd_ms_command
{
	enum php_mysqlnd_server_command command;
	zend_uchar * payload;
	size_t payload_len;
	enum mysqlnd_packet_type ok_packet;
	zend_bool silent;
	zend_bool ignore_upsert_status;
	zend_bool persistent;
} MYSQLND_MS_COMMAND;

#endif /* MYSQLND_MS_ENUM_N_DEF_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
