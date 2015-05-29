#!/usr/bin/php
<?php
$names = array(
    8 => "open",
    9 => "close",
    12 => "read",
    13 => "write",
    15 => "lseek",
    23 => "ftruncate",
    26 => "fsync",
    30 => "pread",
    31 => "pwrite",
    33 => "rename",
    38 => "unlink",
    39 => "rmdir",
    40 => "mkdir",
    46 => "stat",
    53 => "fstat",
);

if($argc != 3)
    exit("Usage: {$argv[0]} <strace> <timings>\n");

$strace = file($argv[1]);
$times = file($argv[2]);

$last = 0;
$timestamp = 0;
foreach($times as $i => $time) {
    preg_match('/^\s*\[\s*\d+\]\s+(\d+)\s+(\d+)\s+(\d+)/', $time, $ti);
    preg_match('/^(.+?)\(([^,]*)/', $strace[$i], $st);

    $last = $ti[3];

    // ignore exec because we don't want to compare the loading
    if($st[1] == 'execve')
        continue;
    // ignore writes to stdout/stderr
    if($st[1] == 'write' && ($st[2] == '1' || $st[2] == '2'))
        continue;

    if(array_search($st[1], $names) !== FALSE) {
        if(@$names[$ti[1]] != $st[1])
            @exit("Line $i: expected syscall " . $names[$ti[1]] . ", got " . $st[1] . "\n");

        // ignore waits of less than 1000 cycles.
        if($timestamp > 0 && ($ti[2] - $timestamp) > 1000)
            echo "_waituntil(" . ($ti[2] - $timestamp) . ") = 0\n";
        $timestamp = $ti[3];
    }
    else if($timestamp == 0)
        @$timestamp = $ti[2];
    echo $strace[$i];
}

if($timestamp > 0 && ($last - $timestamp) > 1000)
    echo "_waituntil(" . ($last - $timestamp) . ") = 0\n";
?>