import numpy as np
from graph import Graph


def generate_loop(start_index, end_index):
    return np.random.shuffle(np.arange(start_index, end_index))


def generate_random_connected_graph(n, edges, inf=100, seed=11):
    """
    Generate weighted connected directed graph with the given number of vertices and edges
    :param n: number of vertices
    :param edges: number of edges
    :param inf: upperbound of weight
    :param seed: seed for numpy.random
    :return: generated object of Graph class

    NB! Returned graph is not random in terms of mathematics. However, it is "random" enough
    and connected at the same time, which is a requirement for the testing purposes.
    """
    assert edges > n
    np.random.seed(seed)

    g = Graph()
    g.weight_matrix = [[[] for _ in range(n)] for _ in range(n)]
    g.number_of_vertices = n
    g.weight_upperbound = inf

    seq = np.random.permutation(n)
    for i in range(n - 1):
        g.add_edge(seq[i], seq[i + 1], np.random.randint(1, inf))
    g.add_edge(seq[-1], seq[0], np.random.randint(1, inf))

    for i in range(n, edges):
        lhs = np.random.randint(0, n)
        rhs = np.random.randint(0, n)
        while rhs == lhs:
            rhs = np.random.randint(0, n)
        g.add_edge(lhs, rhs, np.random.randint(1, inf))

    return g
