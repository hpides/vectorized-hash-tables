numactl -C 0 -m 0 ./hashmap-alignedness-benchmark | sed '/^[[:space:]]*$/d' > alignedness.data
