numactl -C 0 -m 0 ./hashmap-hugepage-benchmark | sed '/^[[:space:]]*$/d' > hugepage.data
