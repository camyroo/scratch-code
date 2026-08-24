
def BFS(graph, start_node):
    if len(graph) == 0: return []

    if start_node not in graph:
        raise ValueError("Invalid Start node")

    queue = [start_node]
    visited = set()
    visited.add(start_node)

    trace = []
    while queue:
        current = queue.pop(0)
        trace.append(current)
        for adj in graph[current]:
            if adj not in visited:
                queue.append(adj)
                visited.add(adj)

    return trace

def build_adj_list(n, edges):
    adj = {}
    for i in range(n):
        adj[i] = []
    
    for e in edges:
        adj[e[0]].append(e[1])
        adj[e[1]].append(e[0])

    return adj

def count_components(n, edges):

    adj = build_adj_list(n,edges)

    visited = set()
    groups = 0
    for x in range(n):
        if x not in visited:
            group = BFS(adj,x)
            groups += 1
            for y in group:
                visited.add(y)
    
    return groups       
 
def main():
    n = 5
    edges = [[0,1], [1,2], [3,4]]

    print(count_components(n, edges))

if __name__ == "__main__":
    main()


