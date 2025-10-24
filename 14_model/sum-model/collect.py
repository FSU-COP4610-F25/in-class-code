#!/usr/bin/env python3
import sys, json, argparse
from collections import deque, defaultdict

def normalize_edges(E_raw):
    """Return list of (src, dst, label) from mixed formats."""
    out = []
    for e in E_raw:
        if isinstance(e, dict):
            s = e.get('src') or e.get('from') or e.get('u') or e.get('s')
            t = e.get('dst') or e.get('to')   or e.get('v') or e.get('t')
            lab = e.get('label') or e.get('op') or e.get('name') or ""
        elif isinstance(e, (list, tuple)):
            if len(e) >= 2:
                s, t = e[0], e[1]
                lab = e[2] if len(e) >= 3 else ""
            else:
                continue
        else:
            continue
        if s is None or t is None:
            continue
        out.append((s, t, str(lab)))
    return out

def get_id_maps(vertices):
    """Map vertex ids to indices. If no 'id', use indices 0..n-1."""
    if all(isinstance(v, dict) and ('id' in v) for v in vertices):
        ids = [v['id'] for v in vertices]
        idx_of = {vid: i for i, vid in enumerate(ids)}
    else:
        ids = list(range(len(vertices)))
        idx_of = {i: i for i in ids}
    return ids, idx_of

def build_graph(vertices, edges_norm, idx_of):
    out_adj = defaultdict(list)
    in_adj  = defaultdict(list)
    labels  = {}
    for s, t, lab in edges_norm:
        si = idx_of.get(s, s)
        ti = idx_of.get(t, t)
        out_adj[si].append(ti)
        in_adj[ti].append(si)
        labels[(si, ti)] = lab
    return out_adj, in_adj, labels

def initials(vertices, in_adj):
    # initial = indegree 0; fallback to [0]
    indeg = defaultdict(int)
    for v in in_adj:
        for _ in in_adj[v]:
            indeg[v] += 1
    ids = []
    for i in range(len(vertices)):
        if indeg.get(i, 0) == 0:
            ids.append(i)
    return ids or [0]

def is_terminal(v, out_deg):
    ch = v.get('choices', None) if isinstance(v, dict) else None
    return (ch == [] or ch is None) and out_deg == 0

def norm_text(s: str) -> str:
    return (s or "").strip()

def main():
    ap = argparse.ArgumentParser(description="List distinct outputs and show a witness path")
    ap.add_argument("--list", action="store_true", help="List distinct terminal outputs with counts")
    ap.add_argument("--target", type=str, help="Find one path to a terminal whose stdout contains this substring")
    args = ap.parse_args()

    G = json.load(sys.stdin)
    V = G.get('vertices') or G.get('nodes') or []
    E = G.get('edges')    or G.get('links') or []

    ids, idx_of = get_id_maps(V)
    E_norm = normalize_edges(E)
    out_adj, in_adj, labels = build_graph(V, E_norm, idx_of)

    # collect terminals and outputs
    outputs = defaultdict(list)  # stdout -> [vertex_idx]
    terminals = []
    for i, v in enumerate(V):
        out_deg = len(out_adj.get(i, []))
        if is_terminal(v if isinstance(v, dict) else {}, out_deg):
            terminals.append(i)
            out = norm_text((v.get('stdout') if isinstance(v, dict) else "") or "")
            outputs[out].append(i)

    if args.list or not args.target:
        print("Distinct outputs:")
        for out in sorted(outputs.keys()):
            disp = "(empty)" if out == "" else out.replace("\n", "⏎ ")
            print(f"  {disp}    x{len(outputs[out])}")
        print(f"\n|V| = {len(V)}, |E| = {len(E_norm)}. Terminals = {len(terminals)}. Distinct outputs = {len(outputs)}.")
        if not args.target:
            return

    # pick a terminal whose stdout contains target
    want = args.target.strip()
    goal = None
    for out, vs in outputs.items():
        if want in out:
            goal = vs[0]
            break
    if goal is None:
        print(f"[no match] target substring not found: {want!r}")
        return

    # BFS from all initials to find one path to goal
    inits = initials(V, in_adj)
    dq = deque(inits)
    seen = set(inits)
    parent = {}
    while dq:
        u = dq.popleft()
        if u == goal: break
        for v in out_adj.get(u, []):
            if v in seen: continue
            seen.add(v)
            parent[v] = u
            dq.append(v)

    if goal not in seen:
        print(f"[no path] cannot reach target vertex {goal} from initials {inits}")
        return

    # reconstruct path
    path = [goal]
    while path[-1] in parent:
        path.append(parent[path[-1]])
    path.reverse()

    print(f"Target stdout contains: {want!r}")
    out_text = norm_text((V[goal].get('stdout') if isinstance(V[goal], dict) else "") or "").replace("\n", "⏎ ")
    print(f"Terminal vertex: {goal}, stdout: {out_text}")
    print(f"Path length (vertices): {len(path)}")
    print("Witness steps:")
    for i in range(len(path)-1):
        u, v = path[i], path[i+1]
        lab = labels.get((u, v), f"(edge {u}->{v})")
        print(f"  {i:02d}: {lab}")

if __name__ == "__main__":
    main()
