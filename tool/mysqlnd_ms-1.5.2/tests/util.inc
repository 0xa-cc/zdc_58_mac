<?php
	require_once("connect.inc");

	/*
	Error: 2002 (CR_CONNECTION_ERROR)
	Message: Can't connect to local MySQL server through socket '%s' (%d)
	Error: 2003 (CR_CONN_HOST_ERROR)
	Message: Can't connect to MySQL server on '%s' (%d)
	Error: 2005 (CR_UNKNOWN_HOST)
	Message: Unknown MySQL server host '%s' (%d)
	*/
	$mst_connect_errno_codes = array(
		2002 => true,
		2003 => true,
		2005 => true,
	);

	function mst_is_slave_of($slave_host, $slave_port, $slave_socket, $master_host, $master_port, $master_socket, $user, $passwd, $db) {
		/* Used for skipping tests if master and slave are part of a replication setup
			and replication could cause false-positives */

		$slave_link = mst_mysqli_connect($slave_host, $user, $passwd, $db, $slave_port, $slave_socket);
		if (mysqli_connect_errno()) {
			return sprintf("[%d] %s", mysqli_connect_errno(), mysqli_connect_error());
		}

		if (!($res = $slave_link->query("SHOW SLAVE STATUS"))) {
			return sprintf("[%d] %s", $slave_link->errno, $slave_link->error);
		}

		if (0 == $res->num_rows) {
			return false;
		}

		if (!($row = $res->fetch_assoc())) {
			return  sprintf("[%d] %s", $slave_link->errno, $slave_link->error);
		}

		if ("localhost" == $master_host || '' == $master_host) {
			/* not sure what port master runs on, better assume servers are related */
			if ($row['Master_Host'] == '127.0.0.1')
				return true;
		}

		if (('127.0.0.1' != $master_host) && ($master_host == $slave_host)) {
			/*
			  Remote connection to two servers running on the same box.
			  If so, the slave likely replicates from 127.0.0.1 but not
			  from the hosts remote address, e.g. 192.168.2.21.
			  We can't be 100% sure they are not real master/slave, thus we better
			  assume they are.
			*/
			if (($row['Master_Host'] == '127.0.0.1') && ($row['Master_Port'] == $master_port))
				return true;
		}

		if (($row['Master_Host'] == 'localhost') && ($master_host == '127.0.0.1'))
			$master_host = 'localhost';

		return (($row['Master_Host'] == $master_host) && ($row['Master_Port'] == $master_port));
	}

	function mst_mysqli_verbose_query($offset, $link, $query, $switch = NULL, $quiet = false, $expect_connect_warning = false, $ignore_error = false) {
		printf("[%03d + 01] Query '%s'\n", $offset, $query);
			$ret = mst_mysqli_query($offset, $link, $query, $switch = NULL, $quiet = false, $expect_connect_warning = false, $ignore_error = false);
		printf("[%03d + 02] Thread '%d'\n", $offset, $link->thread_id);
		return $ret;
	}

	function mst_mysqli_query($offset, $link, $query, $switch = NULL, $quiet = false, $expect_connect_warning = false, $ignore_error = false, $weak_connect_warning = false) {
		global $mst_connect_errno_codes;

		$query = "/*$offset*/$query";

		if ($switch)
			$query = sprintf("/*%s*/%s", $switch, $query);

		if ($quiet) {
			/* Stupid PHP streams sometimes shout without caller being able to suppress */

			ob_start();
			$ret = $link->query($query);
			$haystack = ob_get_contents();
			ob_end_clean();

			if ($expect_connect_warning) {
				$found = false;
				foreach ($mst_connect_errno_codes as $code => $v) {
					$needle = "Warning: mysqli::query(): [" . $code . "]";
					if (false !== ($found = strpos($haystack, $needle))) {
						break;
					}
				}
				if (!$found && $weak_connect_warning) {
					/*
						In 5.4-beta a warning disappeared due to network code re-factoring.
						It was the second of two warnings about the same mistake.
						After 5.4-beta there is only one warning left.
					*/
					$found = strpos($haystack, "php_network_getaddresses: getaddrinfo failed");
				}
				if (!$found) {
					printf("[%03d] Cannot find the expected connect warning, got '%s'\n", $offset, $haystack);
				}
			}

		} else {
			$ret = $link->query($query);
		}

		if (!$ret && !$ignore_error && $link->errno) {
			if (isset($mst_connect_errno_codes[$link->errno]))
				printf("Connect error, ");
			printf("[%03d] [%d] %s\n", $offset, $link->errno, $link->error);
		}

		return $ret;
	}


	function mst_mysqli_real_query($offset, $link, $query, $switch = NULL, $quiet = false, $expect_connect_warning = false, $ignore_error = false) {
		global $mst_connect_errno_codes;

		$query = "/*$offset*/$query";

		if ($switch)
			$query = sprintf("/*%s*/%s", $switch, $query);

		if ($quiet) {
			/* Stupid PHP streams sometimes shouts without caller being able to suppress */

			ob_start();
			$ret = $link->real_query($query);
			$haystack = ob_get_contents();
			ob_end_clean();

			if ($expect_connect_warning) {
				$found = false;
				foreach ($mst_connect_errno_codes as $code => $v) {
					$needle = "Warning: mysqli::query(): [" . $code . "]";
					if (false !== ($found = strpos($haystack, $needle))) {
						break;
					}
				}
				if (!$found) {
					printf("[%03d] Cannot find expect connect warning, got '%s'\n", $offset, $haystack);
				}
			}

		} else {
			$ret = $link->real_query($query);
		}

		if (!$ret && !$ignore_error && $link->errno) {
			if (isset($mst_connect_errno_codes[$link->errno]))
				printf("Connect error, ");
			printf("[%03d] [%d] %s\n", $offset, $link->errno, $link->error);
		}

		return $ret;
	}

	function mst_mysqli_fech_role($res) {
		if (!is_object($res))
			return false;

		$row = $res->fetch_assoc();
		$res->close();
		printf("This is '%s' speaking\n", $row['_role']);
		return true;
	}

	function mst_compare_stats() {
		static $last_stats = NULL;
		if (is_null($last_stats)) {
			$last_stats = mysqlnd_ms_get_stats();
			return;
		}
		$stats = mysqlnd_ms_get_stats();
		foreach ($stats as $k => $v) {
			if ($last_stats[$k] != $v) {
				printf("Stats %s: %d\n", $k, $v);
			}
		}
		$last_stats = $stats;
	}

	function mst_mysqli_create_test_table($host, $user, $passwd, $db, $port, $socket, $tablename = "test") {
		$link = mst_mysqli_connect($host, $user, $passwd, $db, $port, $socket);
		if (mysqli_connect_errno()) {
			return sprintf("[%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
		}

		if (!$link->query(sprintf("DROP TABLE IF EXISTS %s", $link->real_escape_string($tablename))) ||
			!$link->query(sprintf("CREATE TABLE %s(id INT) ENGINE=InnoDB", $link->real_escape_string($tablename))) ||
			!$link->query(sprintf("INSERT INTO %s(id) VALUES (1), (2), (3), (4), (5)", $link->real_escape_string($tablename))))
			return sprintf("[%d] %s\n", $link->errno, $link->error);

		return '';
	}

	function mst_mysqli_drop_test_table($host, $user, $passwd, $db, $port, $socket, $tablename = "test") {
		$link = mst_mysqli_connect($host, $user, $passwd, $db, $port, $socket);
		if (mysqli_connect_errno()) {
			return sprintf("[%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
		}
		if (!$link->query(sprintf("DROP TABLE IF EXISTS %s", $link->real_escape_string($tablename))))
			return sprintf("[%d] %s\n", $link->errno, $link->error);

		return '';
	}

	function mst_mysqli_fetch_id($offset, $res) {
		if (!$res) {
			printf("[%03d] No result\n", $offset);
			return;
		}
		$row = $res->fetch_assoc();
		printf("[%03d] _id = '%s'\n", $offset, $row['_id']);
	}

	function mst_mysqli_server_supports_query($offset, $sql, $host, $user, $passwd, $db, $port, $socket) {
		$link = mst_mysqli_connect($host, $user, $passwd, $db, $port, $socket);
		if (mysqli_connect_errno()) {
			printf("[%03d] [%d] %s\n", $offset, mysqli_connect_errno(), mysqli_connect_error());
		}
		printf("[%03d] Testing server support of '%s'\n", $offset, $sql);
		return $link->query($sql);
	}

	function msg_mysqli_init_emulated_id_skip($host, $user, $passwd, $db, $port, $socket, $role) {
		if (true !== ($msg = _mst_mysqli_init_emulated_id(1, $host, $user, $passwd, $db, $port, $socket, $role)))
			die(sprintf("SKIP Cannot setup emulated server id, %s", $msg));
	}

	function mst_mysqli_init_emulated_id($offset, $host, $user, $passwd, $db, $port, $socket, $role) {
		if (true !== ($msg = _mst_mysqli_init_emulated_id($offset, $host, $user, $passwd, $db, $port, $socket, $role)))
			echo $msg;

		return true;
	}

 	function _mst_mysqli_init_emulated_id($offset, $host, $user, $passwd, $db, $port, $socket, $role) {
		$link = mst_mysqli_connect($host, $user, $passwd, $db, $port, $socket);
		if (mysqli_connect_errno()) {
			return sprintf("[%03d] [%d] %s\n", $offset, mysqli_connect_errno(), mysqli_connect_error());
		}

		if (!$link->query("DROP TABLE IF EXISTS _mysqlnd_ms_roles") ||
			!$link->query("CREATE TABLE _mysqlnd_ms_roles(role VARCHAR(255) NOT NULL)") ||
			!$link->query(sprintf("INSERT INTO _mysqlnd_ms_roles(role) values ('%s')", $link->real_escape_string($role)))) {
			return sprintf("[%03d] [%d] %s\n", $offset, $link->errno, $link->error);
		}

		return true;
	}

	function mst_mysqli_drop_emulated_id($host, $user, $passwd, $db, $port, $socket) {
		$link = mst_mysqli_connect($host, $user, $passwd, $db, $port, $socket);
		if (mysqli_connect_errno()) {
			return sprintf("[%03d] [%d] %s\n", $offset, mysqli_connect_errno(), mysqli_connect_error());
		}

		if (!$link->query("DROP TABLE IF EXISTS _mysqlnd_ms_roles")) {
			return sprintf("[%03d] [%d] %s\n", $offset, $link->errno, $link->error);
		}

		return true;
	}


	function mst_mysqli_get_emulated_id($offset, $link) {

		$thread_id = $link->thread_id;
		if (0 == $thread_id)
			return NULL;

		$query = sprintf("/*%s*//*%d*//*util.inc*/SELECT role FROM _mysqlnd_ms_roles", MYSQLND_MS_LAST_USED_SWITCH, $offset);
		if (!($res = $link->query($query))) {
			printf("[%03d] [%d] %s\n", $offset, $link->errno, $link->error);
			return NULL;
		}

		if ($res->num_rows > 1) {
			printf("[%03d] _mysqlnd_ms_roles holds more than one row\n", $offset);
			return NULL;
		}

		if ($thread_id != $link->thread_id) {
			printf("[%03] Unexpected connection switch\n");
			return NULL;
		}

		$row = $res->fetch_assoc();
		return sprintf("%s-%d", $row['role'], $link->thread_id);
	}


	function mst_get_gtid_sql($mst_gtid_db) {
	  return array(
		'drop' 					=> 'DROP TABLE IF EXISTS ' . $mst_gtid_db . '.trx',
		'create'				=> 'CREATE TABLE ' . $mst_gtid_db . '.trx(trx_id INT, last_update TIMESTAMP) ENGINE=InnoDB',
		'insert'				=> 'INSERT INTO ' . $mst_gtid_db . '.trx(trx_id) VALUES (0)',
		'update'				=> 'UPDATE ' . $mst_gtid_db . '.trx SET trx_id = trx_id + 1',
		'select'				=> 'SELECT trx_id FROM ' . $mst_gtid_db . '.trx',
		'fetch_last_gtid'		=> 'SELECT MAX(trx_id) FROM ' . $mst_gtid_db . '.trx',
		'check_for_gtid'		=> 'SELECT trx_id FROM ' . $mst_gtid_db . '.trx WHERE trx_id >= #GTID',
		/* tests only - not needed for ms configuration */
		'set'					=> 'INSERT INTO ' . $mst_gtid_db . '.trx(trx_id) VALUES (#GTID)',
		/* SLEEP() cannot be used here */
		'check_for_gtid_wait'	=> 'SELECT trx_id FROM ' . $mst_gtid_db . '.trx WHERE trx_id >= #GTID',
		'wait_for_seconds'		=> 2,
		'wait_for_gtid_fail'	=> 1,
		'wait_for_gtid_succeed' => 3,
	  );
	}

	function mst_mysqli_drop_gtid_table($host, $user, $passwd, $db, $port, $socket, $drop = NULL) {
		$link = mst_mysqli_connect($host, $user, $passwd, $db, $port, $socket);
		if (mysqli_connect_errno()) {
			return sprintf("[%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
		}

		$sql = mst_get_gtid_sql($db);
		if (!$link->query(($drop) ? $drop : $sql['drop']))
			return sprintf("[%d] %s\n", $link->errno, $link->error);

		return NULL;
	}

	function mst_mysqli_setup_gtid_table($host, $user, $passwd, $db, $port, $socket, $drop = NULL, $create = NULL, $insert = NULL) {

		$link = mst_mysqli_connect($host, $user, $passwd, $db, $port, $socket);
		if (mysqli_connect_errno()) {
			return sprintf("[%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
		}

		if ($err =  mst_mysqli_drop_gtid_table($host, $user, $passwd, $db, $port, $socket, $drop))
			return $err;

		$sql = mst_get_gtid_sql($db);
		if (!$link->query(($create) ? $create : $sql['create']))
			return sprintf("[%d] %s\n", $link->errno, $link->error);
		if (!$link->query(($insert) ? $insert : $sql['insert']))
			return sprintf("[%d] %s\n", $link->errno, $link->error);
	}

	function mst_mysqli_fetch_gtid($offset, $link, $db) {
		$sql = mst_get_gtid_sql($db);

		$query = sprintf("/*%d*/%s", $offset, $sql['select']);
		if (!($res = $link->query($query))) {
			printf("[%03d] [%d] %s\n", $offset, $link->errno, $link->error);
			return NULL;
		}
		$row = $res->fetch_row();
		return $row[0];
	}

	function mst_mysqli_server_supports_gtid($host, $user, $passwd, $db, $port, $socket) {

		$link = mst_mysqli_connect($host, $user, $passwd, $db, $port, $socket);
		if (mysqli_connect_errno()) {
			return sprintf("[%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
		}

		if (!($res = $link->query("SHOW GLOBAL VARIABLES LIKE 'GTID_MODE'"))) {
			return false;
		}
		$row = $res->fetch_assoc();
		if (!$row)
			return false;

		return ('ON' == $row['Value']) ? true : false;
	}

	function mst_mysqli_get_slave_lag($host, $user, $passwd, $db, $port, $socket) {

		$link = mst_mysqli_connect($host, $user, $passwd, $db, $port, $socket);
		if (mysqli_connect_errno()) {
			return sprintf("[%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
		}

		/* NOTE: logic should always match mysqlnd_ms_flter_qos.c logic */
		if (!($res = $link->query("SHOW SLAVE STATUS"))) {
			return sprintf("[%d] %s\n", $link->errno, $link->error);
		}

		$row = $res->fetch_assoc();
		$res->free();

		if (!isset($row['Slave_IO_Running']))
			return "Failed to extract Slave_IO_Running";

		if ("Yes" != $row['Slave_IO_Running'])
			return "Slave_IO_Running is not 'Yes'";

		if (!isset($row['Slave_SQL_Running']))
			return 'Failed to extract Slave_SQL_Running';

		if ("Yes" != $row['Slave_SQL_Running'])
			return "Slave_SQL_Running is not 'Yes'";

		if (!isset($row['Seconds_Behind_Master']))
			return 'Failed to extract Seconds_Behind_Master';

		return (int)$row['Seconds_Behind_Master'];
	}
?>