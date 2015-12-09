--TEST--
ReflectionFunction to check API
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

	$ignore = array();
	if (version_compare(PHP_VERSION, '5.3.99', ">")) {
		$ignore['mysqlnd_ms_set_qos'] = true;
		$ignore['mysqlnd_ms_get_last_gtid'] = true;
	}

	$functions = $r->getFunctions();
	asort($functions);
	printf("Functions:\n");
	foreach ($functions as $func) {
		if (isset($ignore[$func->name]))
			continue;

		printf("  %s\n", $func->name);
		$rf = new ReflectionFunction($func->name);
		printf("    Deprecated: %s\n", $rf->isDeprecated() ? "yes" : "no");
		printf("    Accepted parameters: %d\n", $rf->getNumberOfParameters());
		printf("    Required parameters: %d\n", $rf->getNumberOfRequiredParameters());
		foreach( $rf->getParameters() as $param ) {
			printf("      %s\n", $param);
		}
	}

	print "done!";
?>
--EXPECTF--
Functions:
  mysqlnd_ms_get_last_used_connection
    Deprecated: no
    Accepted parameters: 1
    Required parameters: 1
      Parameter #0 [ <required> $object ]
  mysqlnd_ms_get_stats
    Deprecated: no
    Accepted parameters: 0
    Required parameters: 0
  mysqlnd_ms_match_wild
    Deprecated: no
    Accepted parameters: 2
    Required parameters: 2
      Parameter #0 [ <required> $haystack ]
      Parameter #1 [ <required> $wild ]
  mysqlnd_ms_query_is_select
    Deprecated: no
    Accepted parameters: 1
    Required parameters: 1
      Parameter #0 [ <required> $query ]
done!