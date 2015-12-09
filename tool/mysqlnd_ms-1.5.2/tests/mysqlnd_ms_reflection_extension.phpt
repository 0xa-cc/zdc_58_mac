--TEST--
ReflectionExtension basics to check API
--SKIPIF--
<?php
require_once('skipif.inc');
if (version_compare(PHP_VERSION, '5.3.99', "<")) {
 die("SKIP Test expects PHP 5.4 only functions");
}
?>
--FILE--
<?php
	$r = new ReflectionExtension("mysqlnd_ms");

	printf("Name: %s\n", $r->name);

	printf("Version: %s\n", $r->getVersion());
	if ($r->getVersion() != MYSQLND_MS_VERSION) {
		printf("[001] Expecting version '%s' got '%s'\n", MYSQLND_MS_VERSION, $r->getVersion());
	}

	$classes = $r->getClasses();
	if (!empty($classes)) {
		printf("[002] Expecting no class\n");
		asort($classes);
		var_dump($classes);
	}

	$expected = array(
		'json' 		=> true,
		'standard' 	=> true,
		'mysqlnd' 	=> true,
		'mysqlnd_qc'=> true,
	);

	$dependencies = $r->getDependencies();
	asort($dependencies);
	printf("Dependencies:\n");
	foreach ($dependencies as $what => $how) {
		printf("  %s - %s, ", $what, $how);
		if (isset($expected[$what])) {
			unset($expected[$what]);
		} else {
			printf("Unexpected extension dependency with %s - %s\n", $what, $how);
		}
	}
	if (!empty($expected)) {
		printf("Dumping list of missing extension dependencies\n");
		var_dump($expected);
	}
	printf("\n");

	$ignore = array();
	if (version_compare(PHP_VERSION, '5.3.99', ">")) {
		$ignore['mysqlnd_ms_set_qos'] = true;
		$ignore['mysqlnd_ms_get_last_gtid'] = true;
	}

	$functions = $r->getFunctions();
	asort($functions);
	printf("Functions:\n");
	foreach ($functions as $func) {
		if (isset($ignore[$func->name])) {
			unset($ignore[$func->name]);
		} else {
			printf("  %s\n", $func->name);
		}
	}
	if (!empty($ignore)) {
		printf("Dumping version dependent and missing functions\n");
		var_dump($ignore);
	}


	print "done!";
?>
--EXPECTF--
Name: mysqlnd_ms
Version: 1.5.2
Dependencies:
%s
Functions:
  mysqlnd_ms_get_last_used_connection
  mysqlnd_ms_get_stats
  mysqlnd_ms_match_wild
  mysqlnd_ms_query_is_select
done!