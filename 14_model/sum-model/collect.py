#!/usr/bin/env python3
import sys, json, argparse
from collections import defaultdict, deque

def build_idx(vertices):
    # Prefer 'hashcode' if present; else fall back to position
    if all(isinstance(v, dict) and 'hashcode' in v for v in vertices):
        ids = [v['hashcode'] for v in vertices]
        idx = {h:i for i,h in enumerate(ids)}
    else:
        ids = list(range(len(vertices)))
        idx = {i:i for i in ids}
    return ids, idx

def norm_edges(edges_raw):
    # Edges are like ["src_hash","dst_hash","label"] in your JSON
    out = []
    for e in edges_raw:
        if isinstance(e, (list, tuple)) and len(e) >= 2:
            s, t = e[0], e[1]
            lab = e[2] if len(e) >= 3 else ""
            out.append((s, t, str(lab)))
        elif isinstance(e, dict):
            s = e.get('src') or e.get('from')
            t = e.get('dst') or e.get('to')
            lab = e.get('label') or e.get('op') or e.get('name') or ""
            if s is not None and t is not None:
                out.append((s, t, str(lab)))
    return out

def map_edges(E_norm, idx):
    n = len(idx)
    out_adj = defaultdict(list)
    in_adj  = defaultdict(list)
    labels  = {}
    skipped = 0
    for s_raw, t_raw, lab in E_norm:
        si = idx.get(s_raw, None)
        ti = idx.get(t_raw, None)
        if si is None or ti is None:
            skipped += 1
            continue
        out_adj[si].append(ti)
        in_adj[ti].append(si)
        labels[(si, ti)] = lab
    return out_adj, in_adj, labels, skipped

def initials(in_adj):
    n = max(in_adj.keys(), default=-1) + 1
    indeg = defaultdict(int)
    for v, preds in in_adj.items():
        indeg[v] = len(preds)
    # Any vertex not in in_adj also has indegree 0
    # Determine total vertex count by looking at highest index we saw
    all_vs = set(in_adj.keys())
    for preds in in_adj.values():
        all_vs |= set(preds)
    N = max(all_vs) + 1 if all_vs else n
    return [i for i in range(N) if indeg.get(i, 0) == 0] or [0]

def is_term(V, i, out_adj):
    v = V[i] if isinstance(V[i], dict) else {}
    ch = v.get('choices', None) if isinstance(v, dict) else None
    return len(out_adj.get(i, [])) == 0 and (ch == [] or ch is None)

def stdout_of(V, i):
    return ((V[i].get('stdout') if isinstance(V[i], dict) else "") or "").strip()

def main():
    ap = argparse.ArgumentParser(description="List terminal outputs, optionally show a witness path")
    ap.add_argument("--list", action="store_true", help="List distinct terminal outputs and counts")
    ap.add_argument("--target", type=str, help="Substring to match in stdout, then print one witness path")
    args = ap.parse_args()

    G = json.load(sys.stdin)
    V = G.get("vertices") or G.get("nodes") or []
    E = G.get("edges")    or G.get("links") or []

    ids, idx          = build_idx(V)
    E_norm            = norm_edges(E)
    out_adj, in_adj, labels, skipped = map_edges(E_norm, idx)
    n = len(V)

    # collect terminals
    outputs = defaultdict(list)
    terminals = []
    for i in range(n):
        if is_term(V, i, out_adj):
            terminals.append(i)
            outputs[stdout_of(V, i).replace("\n", "⏎ ")].append(i)

    if args.list or not args.target:
        print("Distinct outputs:")
        for out in sorted(outputs.keys()):
            disp = out if out else "(empty)"
            print(f"  {disp}    x{len(outputs[out])}")
        print(f"\n|V| = {n}, |E| = {len(E_norm)}. Terminals = {len(terminals)}. Distinct outputs = {len(outputs)}.")
        if skipped:
            print(f"[warn] skipped edges due to id mapping: {skipped}", file=sys.stderr)
        if not args.target:
            return

    # witness for one matching terminal
    want = args.target.strip()
    goal = None
    for out, vs in outputs.items():
        if want in out:
            goal = vs[0]
            break
    if goal is None:
        print(f"[no match] target substring not found: {want!r}")
        return

    # BFS from initials
    starts = initials(in_adj)
    dq = deque(starts)
    seen = set(starts)
    parent = {}
    while dq:
        u = dq.popleft()
        if u == goal:
            break
        for v in out_adj.get(u, []):
            if v in seen: continue
            seen.add(v)
            parent[v] = u
            dq.append(v)
    if goal not in seen:
        print(f"[no path] cannot reach target vertex {goal} from initials {starts}")
        return

    # reconstruct path
    path = [goal]
    while path[-1] in parent:
        path.append(parent[path[-1]])
    path.reverse()

    print(f"Target stdout contains: {want!r}")
    print(f"Terminal vertex: {goal}, stdout: {stdout_of(V, goal).replace('\\n','⏎ ')}")
    print(f"Path length (vertices): {len(path)}")
    print("Witness steps:")
    for i in range(len(path)-1):
        u, v = path[i], path[i+1]
        lab = labels.get((u, v), f"(edge {u}->{v})")
        print(f"  {i:02d}: {lab}")

if __name__ == "__main__":
    main()
