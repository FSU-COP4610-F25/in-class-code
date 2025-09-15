#!/usr/bin/env python3

import hexdump
import mmap

mapping_length = 16 << 30  # 16GiB

with open('/dev/sda', 'rb') as fp:
    mm = mmap.mmap(fp.fileno(), length=mapping_length, prot=mmap.PROT_READ)
    hexdump.hexdump(mm[:512])
    mm.close()

