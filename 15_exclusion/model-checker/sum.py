class SumModel:
    def __init__(self):
        self.sum = 0
        self.done = 0

    @thread
    def t1(self):
        for _ in range(3):
            t = self.sum
            sys_sched()
            t = t + 1
            self.sum = t
            sys_sched()
        self.done += 1

    @thread
    def t2(self):
        for _ in range(3):
            t = self.sum
            sys_sched()
            t = t + 1
            self.sum = t
            sys_sched()
        self.done += 1

    @thread
    def t3(self):
        for _ in range(3):
            t = self.sum
            sys_sched()
            t = t + 1
            self.sum = t
            sys_sched()
        self.done += 1

    def main(self):
        sys_spawn(self.t1)
        sys_spawn(self.t2)
        sys_spawn(self.t3)
        while self.done != 3:
            sys_sched()
        sys_write(f'sum = {self.sum}\n')
