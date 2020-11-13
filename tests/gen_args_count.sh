#!/bin/bash

#ARGS_COUNT
for i in $(seq 1, 210); do
	cnt="1";
	for l in $(seq 2, $i); do
		cnt+=", $l"
	done
	echo "_Static_assert(ARGS_COUNT( $cnt  ) == $i, \"1\");"
done

echo

#ARGS_COUNT_ZERO
for i in $(seq 0, 209); do
	cnt="0";
	for l in $(seq 1, $i); do
		cnt+=", $l"
	done
	echo "_Static_assert(ARGS_COUNT_ZERO( $cnt  ) == $i, \"1\");"
done
