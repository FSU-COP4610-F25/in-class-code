#!/usr/bin/env python3
import sys, json, argparse
from collections import defaultdict

def build_idx(vertices):
    if all(isinstance(v, dict) and 'hashcode' in v for v in vertices):
        ids = [v['hashcode'] for v in vertices]
        idx = {h:i for i,h in enumerate(ids)}
    else:
        ids = list(range(len(vertices)))
        idx = {i:i for i in ids}
    return ids, idx

def norm_edges(edges_raw):
    out = []
    for e in edges_raw:
        if isinstance(e, (list, tuple)) and len(e) >= 2:
            s, t = e[0], e[1]
        elif isinstance(e, dict):
            s = e.get('src') or e.get('from')
            t = e.get('dst') or e.get('to')
        else:
            continue
        if s is None or t is None: continue
        out.append((s, t))
    return out

def map_adj(E_norm, idx, n):
    adj = [[] for _ in range(n)]
    indeg = [0]*n
    skipped = 0
    for s_raw, t_raw in E_norm:
        si = idx.get(s_raw, None)
        ti = idx.get(t_raw, None)
        if si is None or ti is None:
            skipped += 1
            continue
        adj[si].append(ti)
        indeg[ti] += 1
    return adj, indeg, skipped

def is_term(V, i, adj):
    v = V[i] if isinstance(V[i], dict) else {}
    ch = v.get('choices', None) if isinstance(v, dict) else None
    return len(adj[i]) == 0 and (ch == [] or ch is None)

def main():
    ap = argparse.ArgumentParser(description="Count walks to terminals up to K steps")
    ap.add_argument("--K", type=int, required=True)
    args = ap.parse_args()

    G = json.load(sys.stdin)
    V = G.get("vertices") or G.get("nodes") or []
    E = G.get("edges")    or G.get("links") or []

    ids, idx = build_idx(V)
    E_norm   = norm_edges(E)
    n = len(V)

    adj, indeg, skipped = map_adj(E_norm, idx, n)
    initials = [i for i in range(n) if indeg[i] == 0] or [0]
    terms    = [i for i in range(n) if is_term(V, i, adj)]

    # DP for walks ≤ K
    K = args.K
    cur = [0]*n
    for s in initials:
        cur[s] = 1
    total_to_term = {i: cur[i] for i in terms}
    for _ in range(1, K+1):
        nxt = [0]*n
        for u in range(n):
            cu = cur[u]
            if cu == 0: continue
            for v in adj[u]:
                nxt[v] += cu
        cur = nxt
        for t in terms:
            total_to_term[t] += cur[t]

    from collections import defaultdict as DD
    by_out = DD(int)
    for t in terms:
        out = ((V[t].get('stdout') if isinstance(V[t], dict) else "") or "").strip().replace("\n", "⏎ ")
        by_out[out] += total_to_term[t]

    def fmt(x):
        s = str(x)
        return s if len(s) <= 12 else (f"{x:.3e}" if x > 0 else "0")

    print(f"Max steps K = {K}")
    print("Output -> walks (≤K steps) reaching it:")
    for k in sorted(by_out.keys()):
        print(f"  {k or '(empty)'}    x{fmt(by_out[k])}")
    if skipped:
        print(f"[warn] skipped edges due to id mapping: {skipped}", file=sys.stderr)

if __name__ == "__main__":
    main()
