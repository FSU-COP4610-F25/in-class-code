def T_sum():
    for _i in range(3):
        _v = heap.sum
        sys_sched()
        _v = _v + 1
        heap.sum = _v
        sys_sched()
    heap.done += 1

def main():
    heap.sum = 0
    heap.done = 0
    sys_spawn(T_sum)
    sys_spawn(T_sum)
    sys_spawn(T_sum)
    while heap.done != 3:
        sys_sched()
    sys_write(f'sum = {heap.sum}\n')

