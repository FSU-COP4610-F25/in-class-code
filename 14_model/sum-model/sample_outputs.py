#!/usr/bin/env python3
import sys, json, argparse, random
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
    ap = argparse.ArgumentParser(description="Monte Carlo sampling of outputs")
    ap.add_argument("--runs", type=int, default=5000)
    ap.add_argument("--max-steps", type=int, default=200)
    ap.add_argument("--seed", type=int, default=0)
    args = ap.parse_args()
    random.seed(args.seed)

    G = json.load(sys.stdin)
    V = G.get("vertices") or G.get("nodes") or []
    E = G.get("edges")    or G.get("links") or []

    ids, idx = build_idx(V)
    E_norm   = norm_edges(E)
    n = len(V)

    adj, indeg, skipped = map_adj(E_norm, idx, n)
    initials = [i for i in range(n) if indeg[i] == 0] or [0]

    outdeg = [len(adj[i]) for i in range(n)]
    def run_one():
        v = random.choice(initials)
        for _ in range(args.max_steps):
            if is_term(V, v, adj):
                return ((V[v].get('stdout') if isinstance(V[v], dict) else "") or "").strip()
            if not adj[v]: break
            v = random.choice(adj[v])
        return None

    hits = defaultdict(int)
    timeouts = 0
    for _ in range(args.runs):
        r = run_one()
        if r is None: timeouts += 1
        else: hits[r] += 1

    print(f"runs={args.runs}, max_steps={args.max_steps}, timeouts={timeouts}")
    print("Output -> estimated probability")
    total = args.runs
    for out in sorted(hits.keys()):
        disp = (out or "").replace("\n","⏎ ")
        print(f"  {disp or '(empty)'}    {hits[out]/total:.4f}")
    if skipped:
        print(f"[warn] skipped edges due to id mapping: {skipped}", file=sys.stderr)

if __name__ == "__main__":
    main()
