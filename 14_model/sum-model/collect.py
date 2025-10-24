#!/usr/bin/env python3

import sys, json, argparse
from collections import deque, defaultdict

def get_id_map(vertices):
    # Prefer explicit 'id' field; fall back to index
    id_of = []
    has_id = all(('id' in v) for v in vertices)
    if has_id:
        for v in vertices:
            id_of.append(v['id'])
        idx_of = {vid: i for i, vid in enumerate(id_of)}
    else:
        id_of = list(range(len(vertices)))
        idx_of = {i: i for i in id_of}
    return id_of, idx_of

def pick_initial_vertices(vertices, idx_of, edges):
    # Initial = vertices with indegree 0; fallback to vertex 0
    indeg = defaultdict(int)
    for e in edges:
        s = e.get('src', e.get('from'))
        t = e.get('dst', e.get('to'))
        if t is None or s is None: continue
        indeg[t] += 1
    initials = [idx_of[v.get('id', i)]
                for i, v in enumerate(vertices)
                if indeg.get(v.get('id', i), 0) == 0]
    return initials if initials else [0]

def build_graph(vertices, edges, idx_of):
    out_adj = defaultdict(list)
    in_adj  = defaultdict(list)
    labels  = dict()
    for e in edges:
        s = e.get('src', e.get('from'))
        t = e.get('dst', e.get('to'))
        if s is None or t is None: continue
        si = idx_of[s] if s in idx_of else s
        ti = idx_of[t] if t in idx_of else t
        out_adj[si].append(ti)
        in_adj[ti].append(si)
        labels[(si, ti)] = e.get('label') or e.get('op') or e.get('name') or ""
    return out_adj, in_adj, labels

def is_terminal(v):
    # Terminal if no choices OR explicit choices empty OR no outgoing edges later detected
    ch = v.get('choices', None)
    return (ch == [] or ch is None)

def norm(s: str) -> str:
    # Normalize for display and matching
    return (s or "").strip()

def main():
    ap = argparse.ArgumentParser(description="List outputs and show a witness path for a target output")
    ap.add_argument("--list", action="store_true", help="List distinct outputs and counts")
    ap.add_argument("--target", type=str, help="Find one witness path for an output (substring match on stdout)")
    args = ap.parse_args()

    G = json.load(sys.stdin)
    V, E = G['vertices'], G['edges']

    id_of, idx_of = get_id_map(V)
    out_adj, in_adj, labels = build_graph(V, E, idx_of)

    # collect terminals and outputs
    terminals = []
    outputs = defaultdict(list)  # out_str -> [vertex_idx]
    for i, v in enumerate(V):
        term = is_terminal(v) and (len(out_adj[i]) == 0)  # also check no outgoing edges
        if term:
            terminals.append(i)
            out = norm(v.get('stdout', ''))
            outputs[out].append(i)

    if args.list or not args.target:
        print("Distinct outputs:")
        for out in sorted(outputs.keys()):
            if out == "": disp = "(empty)"
            else:         disp = out.replace("\n", "⏎ ")
            print(f"  {disp}    x{len(outputs[out])}")
        print(f"\n|V| = {len(V)}, |E| = {len(E)}. Terminals = {len(terminals)}. Distinct outputs = {len(outputs)}.")
        if not args.target:
            return

    # find a terminal whose stdout contains target substring
    want = args.target.strip()
    goal = None
    for out, vs in outputs.items():
        if want in out:
            goal = vs[0]  # pick one terminal
            break
    if goal is None:
        print(f"[no match] target substring not found in any terminal stdout: {want!r}")
        return

    # Choose a start vertex (indegree 0); do BFS to reconstruct one path start->goal
    initials = pick_initial_vertices(V, idx_of, E)
    found = False
    parent = dict()      # child_idx -> (parent_idx)
    parent_edge = dict() # child_idx -> (u,v)

    # BFS from all initials
    dq = deque(initials)
    seen = set(initials)
    while dq and not found:
        u = dq.popleft()
        if u == goal:
            found = True
            break
        for v in out_adj.get(u, []):
            if v in seen: continue
            seen.add(v)
            parent[v] = u
            parent_edge[v] = (u, v)
            dq.append(v)

    if not found:
        print(f"[no path] could not reach target vertex {goal} from initials {initials}")
        return

    # Reconstruct path
    path_vertices = []
    cur = goal
    while True:
        path_vertices.append(cur)
        if cur in parent:
            cur = parent[cur]
        else:
            break
    path_vertices.reverse()

    # Print witness
    print(f"Target stdout contains: {want!r}")
    out_text = norm(V[goal].get('stdout', '')).replace("\n", "⏎ ")
    print(f"Terminal vertex: {goal}, stdout: {out_text}")
    print(f"Path length (vertices): {len(path_vertices)}")
    print("Witness steps:")
    step = 0
    for i in range(len(path_vertices)-1):
        u, v = path_vertices[i], path_vertices[i+1]
        lab = labels.get((u, v), "")
        if lab == "":
            lab = f"(edge {u}->{v})"
        print(f"  {step:02d}: {lab}")
        step += 1

if __name__ == "__main__":
    main()

