import re


class Graph:
    """
    adjacent_lists id a dictionary in the following format:
    {vertex: ([adjacent_vertex: weight_of_edge], degree)}
    """
    weight_matrix = None
    number_of_vertices = 0
    weight_upperbound = 0

    vertex_re = re.compile(r'(\d+:[^;]*;)')
    adj_re = re.compile(r'(\d)+: (.*);')
    list_re = re.compile(r'(\d)+ \((\d)+\)')

    def __init__(self, str_in=None):
        """
        :param str_in: None or graph's description in the format [n : [n1(w)]∗; ]*
        """
        if not str_in:
            return
        tmp = [self.adj_re.findall(i) for i in self.vertex_re.findall(str_in)]
        adjacent_vertices = [elem[0][1] for elem in tmp]

        self.number_of_vertices = len(adjacent_vertices)
        self.weight_matrix = [[[] for _ in range(self.number_of_vertices)] for _ in range(self.number_of_vertices)]
        for i, u in enumerate(adjacent_vertices):
            edges = [(int(x[0]), int(x[1])) for x in self.list_re.findall(u)]
            for j, v in enumerate(edges):
                self.weight_matrix[i][v[0] - 1].append(v[1])
                if v[1] > self.weight_upperbound:
                    self.weight_upperbound = v[1]

    def get_imbalanced_vertices(self):
        outs = [sum([len(self.weight_matrix[i][j]) for j in range(self.number_of_vertices)])
                for i in range(self.number_of_vertices)]
        ins = [sum([len(self.weight_matrix[i][j]) for i in range(self.number_of_vertices)])
               for j in range(self.number_of_vertices)]
        return [(i, ins[i] - outs[i]) for i in range(self.number_of_vertices) if ins[i] != outs[i]]

    def add_edge(self, u, v, w, times=1):
        tmp = [w for _ in range(times)]
        self.weight_matrix[u][v] += tmp

    def remove_edge(self, u, v, w):
        self.weight_matrix[u][v].remove(w)
