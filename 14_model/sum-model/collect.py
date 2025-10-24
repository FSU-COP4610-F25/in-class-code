#!/usr/bin/env python3
# Count number of walks (paths allowing revisits) up to length K reaching each terminal.
# Works with cycles by bounding the max number of steps.

import sys, json, argparse
from collections import defaultdict, deque

def norm_edges(E):
    out = []
    for e in E:
        if isinstance(e, dict):
            s = e.get('src') or e.get('from') or e.get('u') or e.get('s')
            t = e.get('dst') or e.get('to')   or e.get('v') or e.get('t')
        elif isinstance(e, (list, tuple)) and len(e) >= 2:
            s, t = e[0], e[1]
        else:
            continue
        if s is None or t is None: continue
        out.append((s, t))
    return out

def id_map(V):
    if all(isinstance(v, dict) and ('id' in v) for v in V):
        ids = [v['id'] for v in V]
        idx = {vid: i for i, vid in enumerate(ids)}
    else:
        ids = list(range(len(V)))
        idx = {i: i for i in ids}
    return ids, idx

def is_term(v, outdeg):
    ch = v.get('choices', None) if isinstance(v, dict) else None
    return outdeg == 0 and (ch == [] or ch is None)

def main():
    ap = argparse.ArgumentParser(description="Bounded walk counting to terminals")
    ap.add_argument("--K", type=int, required=True, help="max number of steps")
    args = ap.parse_args()

    G = json.load(sys.stdin)
    V = G.get('vertices') or G.get('nodes') or []
    E = G.get('edges')    or G.get('links') or []

    ids, idx = id_map(V)
    E2 = norm_edges(E)

    n = len(V)
    adj = [[] for _ in range(n)]
    indeg = [0]*n
    for s,t in E2:
        si = idx.get(s, s)
        ti = idx.get(t, t)
        if isinstance(si,int) and 0<=si<n and isinstance(ti,int) and 0<=ti<n:
            adj[si].append(ti)
            indeg[ti] += 1

    initials = [i for i in range(n) if indeg[i]==0] or [0]
    outdeg = [len(adj[i]) for i in range(n)]
    terminals = [i for i in range(n) if is_term(V[i] if isinstance(V[i],dict) else {}, outdeg[i])]

    # DP[k][v] = number of walks of length exactly k ending at v
    # init: length 0 at initials = 1
    K = args.K
    cur = [0]*n
    for s in initials:
        cur[s] = 1

    # accumulate counts at terminals up to length k
    total_to_term = {i: cur[i] for i in terminals}

    for _ in range(1, K+1):
        nxt = [0]*n
        for u in range(n):
            cu = cur[u]
            if cu==0: continue
            for v in adj[u]:
                nxt[v] += cu
        cur = nxt
        for t in terminals:
            total_to_term[t] += cur[t]

    by_out = defaultdict(int)
    for t in terminals:
        out = (V[t].get('stdout') if isinstance(V[t],dict) else "") or ""
        out = out.strip().replace("\n","⏎ ")
        by_out[out] += total_to_term[t]

    def fmt(x):
        s=str(x)
        return s if len(s)<=12 else (f"{x:.3e}" if x>0 else "0")

    print(f"Max steps K = {K}")
    print("Output -> walks (≤K steps) reaching it:")
    for k in sorted(by_out.keys()):
        print(f"  {k or '(empty)'}    x{fmt(by_out[k])}")
