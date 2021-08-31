#!/usr/bin/python3
import numpy as np
import matplotlib.pyplot as plt
import networkx as nx
import argparse
import time
from os import environ
from graph import Graph
from random_graph import generate_random_connected_graph


def suppress_qt_warnings():
    environ["QT_DEVICE_PIXEL_RATIO"] = "0"
    environ["QT_AUTO_SCREEN_SCALE_FACTOR"] = "1"
    environ["QT_SCREEN_SCALE_FACTORS"] = "1"
    environ["QT_SCALE_FACTOR"] = "1"


def plot_graph(weight_matrix, inf, filename, layout=nx.circular_layout, with_labels=False):
    """
    Plot graph from the given weight matrix
    :param weight_matrix: graph's weight matrix
    :param inf: upperbound of the weights of the graph
    :param filename: name of the .png plot
    :param layout: method for plotting the graph
    :param with_labels: writes edge labels if with_labels == True
    :return:
    """
    dim = len(weight_matrix)
    G = nx.DiGraph()
    for i in range(dim):
        G.add_node(i + 1)
    for i in range(dim):
        for j in range(dim):
            if isinstance(weight_matrix[i][j], list):
                for w in weight_matrix[i][j]:
                    if w < inf:
                        G.add_edges_from([(i + 1, j + 1)], weight=w)
            else:
                if 0 < weight_matrix[i][j] < inf:
                    G.add_edges_from([(i + 1, j + 1)], weight=weight_matrix[i][j])

    plt.figure(figsize=(10, 10))
    pos = layout(G)
    if with_labels:
        edge_labels = nx.get_edge_attributes(G, 'weight')
        nx.draw_networkx_edge_labels(G, pos, edge_labels=edge_labels)
    nx.draw(G, pos, with_labels=True)
    plt.savefig(f"{filename}.png")


def floyd_warshall_algorithm(weights, inf):
    dim = len(weights)
    distances = np.full((dim, dim), inf)
    next_vertices = [[(0, 0) for _ in range(dim)] for _ in range(dim)]

    for i in range(dim):
        for j in range(dim):
            if weights[i][j]:
                distances[i][j] = min(weights[i][j])
                next_vertices[i][j] = (j, distances[i][j])
    for i in range(dim):
        distances[i][i] = 0
        next_vertices[i][i] = (i, 0)
    for i in range(dim):
        for j in range(dim):
            for k in range(dim):
                if distances[j][k] > distances[j][i] + distances[i][k]:
                    distances[j][k] = distances[j][i] + distances[i][k]
                    next_vertices[j][k] = next_vertices[j][i]

    return distances, next_vertices


def hungarian_method(weight_matrix, inf):
    dim = weight_matrix.shape[0]
    row_labels = np.zeros(dim + 1)
    column_labels = np.zeros(dim + 1)
    matches = np.zeros(dim + 1, dtype=int)
    way = np.zeros(dim + 1, dtype=int)
    for i in range(1, dim + 1):
        # matches[0] stands for current row index
        matches[0] = i
        # Minimum variance in columns
        minvar = np.full(dim + 1, inf)
        used = np.zeros(dim + 1, dtype=int)
        j0 = 0
        while matches[j0] != 0:
            used[j0] = 1
            i0 = matches[j0]
            delta = inf
            j1 = 0
            for j in range(1, dim + 1):
                if used[j] == 0:
                    cur = weight_matrix[i0 - 1][j - 1] - row_labels[i0] - column_labels[j]
                    if cur < minvar[j]:
                        minvar[j] = cur
                        way[j] = j0
                    if minvar[j] < delta:
                        delta = minvar[j]
                        j1 = j
            for j in range(dim + 1):
                if used[j] == 1:
                    row_labels[matches[j]] += delta
                    column_labels[j] -= delta
                else:
                    minvar[j] -= delta
            j0 = j1
        while j0 != 0:
            j1 = way[j0]
            matches[j0] = matches[j1]
            j0 = j1

    result = np.zeros(dim, dtype=int)
    for i in range(1, dim + 1):
        result[matches[i] - 1] = i - 1
    return result


def euler_tour(g, start=0):
    stack = []
    result = []
    cost = 0

    flag = False
    for i in range(start, g.number_of_vertices):
        if flag:
            break
        for j in range(g.number_of_vertices):
            if g.weight_matrix[i][j]:
                # 0 is a special value for the start vertex
                stack.append((i, 0))
                flag = True
                break
    while stack:
        top = stack[-1]
        if any(g.weight_matrix[top[0]]):
            first_v = 0
            while True:
                if g.weight_matrix[top[0]][first_v]:
                    break
                first_v += 1

            tmp = g.weight_matrix[top[0]][first_v][0]
            stack.append((first_v, tmp))
            g.remove_edge(top[0], first_v, tmp)
        if top == stack[-1]:
            stack.pop()
            result.append((top[0] + 1, top[1]))
            cost += top[1]

    return result[::-1], cost


def construct_imbalanced_vertices_matrix(imbalanced_vertices, distances, inf):
    dim = len(imbalanced_vertices)
    result = np.zeros(dim * dim, dtype=int).reshape(dim, dim)
    for i, u in enumerate(imbalanced_vertices):
        result[i] = np.array([distances[u[0]][v[0]] if u[1] > 0 and v[1] < 0 else inf for v in imbalanced_vertices])
    return result


def rip(graph_input=None, plot_graphs=False, n=10, edges=20, seed=11):
    if not graph_input:
        g = generate_random_connected_graph(n, edges, seed=seed)
    else:
        g = Graph(graph_input)

    # These cases were used for testing
    # graph_input = "1: 2 (1) 6 (10) ; 2: 3 (2) 4 (4) 5 (5) ; 3: 4 (3) ; 4: 5 (6) ; 5: 6 (7) ; 6: 7 (8) ; 7: 1 (9) ;"
    # graph_input = "1: 2 (1) ; 2: 3 (2) ; 3: 4 (3) 5 (5) ; 4: 5 (4) ; 5: 6 (6) ; 6: 1 (7) 2 (8) ;"
    # graph_input = "1: 2 (1) ; 2: 3 (2) ; 3: 4 (3) ; 4: 5 (4) ; 5: 1 (5) 2 (6) ;"
    matrix_inf = g.weight_upperbound * g.number_of_vertices + 1
    if plot_graphs:
        with_labels = False if g.number_of_vertices >= 20 else True
        plot_graph(g.weight_matrix, matrix_inf, "input_graph", with_labels=with_labels)

    start_time = time.time()

    imbalanced_vertices = g.get_imbalanced_vertices()
    if len(imbalanced_vertices) <= 2:
        if imbalanced_vertices:
            start = imbalanced_vertices[0][0] if imbalanced_vertices[0][1] < 0 else imbalanced_vertices[1][0]
            tour, cost = euler_tour(g, start=start)
        else:
            tour, cost = euler_tour(g)

        end_time = time.time()
        print("Tour:", tour)
        print(f"Cost of tour: {cost}")
        print("Overhead: 0")
        print(f"Time elapsed: {end_time - start_time}s")
        return

    distances, next_vertices = floyd_warshall_algorithm(g.weight_matrix, matrix_inf)
    if np.argwhere(distances == matrix_inf).size > 0:
        print("Graph must be fully-connected")
        exit(1)

    new_graph = construct_imbalanced_vertices_matrix(imbalanced_vertices, distances, matrix_inf)
    matching = hungarian_method(new_graph, matrix_inf)

    overhead = 0.0
    for i, j in enumerate(matching):
        if imbalanced_vertices[i][1] > 0:
            times = imbalanced_vertices[i][1]
            final_v = imbalanced_vertices[j][0]
            cur_v = imbalanced_vertices[i][0]
            next_v = next_vertices[cur_v][final_v]

            g.add_edge(cur_v, next_v[0], next_v[1], times=times)
            overhead += times * next_v[1]
            # print(f"Adding {times} times edge ({cur_v + 1}, {next_v[0] + 1}) with w = {next_v[1]}")
            while next_v[0] != final_v:
                cur_v = next_v[0]
                next_v = next_vertices[cur_v][final_v]
                # print(f"Adding {times} times edge ({cur_v + 1}, {next_v[0] + 1}) with w = {next_v[1]}")
                g.add_edge(cur_v, next_v[0], next_v[1], times=times)
                overhead += times * next_v[1]

    tour, cost = euler_tour(g)

    end_time = time.time()

    print("Tour:", tour)
    print(f"Cost of tour: {cost}")
    print(f"Overhead: {int(overhead)}")
    print(f"Time elapsed: {end_time - start_time}s")


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Solve Route Inspection (Chinese Postman) Problem.")
    parser.add_argument("--from-input", "-i", action="store_true",
                        help="input string from which to construct graph for solution;"
                             " if not provided, generates random graph")
    parser.add_argument("-n", type=int, nargs="?", default=10,
                        help="number of vertices for random graph case (by default 10)")
    parser.add_argument("-e", type=int, nargs="?", default=20,
                        help="number of edges for random graph case (by default 20)")
    parser.add_argument("-s", type=int, nargs="?", default=11,
                        help="seed for random graph case (by default 11)")
    parser.add_argument("--plot-graph", "-p", action="store_true",
                        help="plot graph for which the task will be solved")
    cmd_args = parser.parse_args()

    input_str = None
    if cmd_args.from_input:
        input_str = input("Graph: ")

    suppress_qt_warnings()
    rip(graph_input=input_str, plot_graphs=cmd_args.plot_graph, n=cmd_args.n, edges=cmd_args.e, seed=cmd_args.s)
