'''
Reference implementation of HRLHG. 
Author: Yue Yin
'''
import argparse
import numpy as np
import networkx as nx
import node2vec
from gensim.models import Word2Vec

def parse_args():
    parser = argparse.ArgumentParser(description="Run node embedding.")

    parser.add_argument('--input', nargs='?', default='sample_fold1/',
                        help='Input graph path')
    parser.add_argument('--output', nargs='?', default='sample_fold1/even_fold1_emb_wl20.out',
                        help='Embeddings output path')
    parser.add_argument('--prob', nargs='?', default='sample_fold1/prob.dat',
                        help='Distribution matrix')

    parser.add_argument('--nodetype', type=int, default=4, help='Number of node type.')
    parser.add_argument('--edgetype', type=int, default=10, help='Number of edge type.')
    parser.add_argument('--walk-length', type=int, default=80, help='Length of walk per source. Default is 80.')

    parser.add_argument('--dimensions', type=int, default=128, help='Number of dimensions. Default is 128.')
    parser.add_argument('--num-walks', type=int, default=10, help='Number of walks per source. Default is 10.')
    parser.add_argument('--window-size', type=int, default=10, help='Context size for optimization. Default is 10.')
    parser.add_argument('--iter', default=1, type=int, help='Number of epochs in SGD')
    parser.add_argument('--workers', type=int, default=8, help='Number of parallel workers. Default is 8.')
    parser.add_argument('--weighted', dest='weighted', action='store_true',
                        help='Boolean specifying (un)weighted. Default is weighted.')
    parser.add_argument('--unweighted', dest='unweighted', action='store_false')
    parser.set_defaults(weighted=True)
    parser.add_argument('--directed', dest='directed', action='store_true',
                        help='Graph is (un)directed. Default is undirected.')
    parser.add_argument('--undirected', dest='undirected', action='store_false')
    parser.set_defaults(directed=False)

    return parser.parse_args()


def read(fprob, fnode, fedge, pcnt, ecnt):
    '''
    :param fprob: distribution matrix
    :param fnode: <node id, node type>
    :param fedge: <start node id, end node id, edge type, edge weight>
    :param pcnt: node number
    :param ecnt: edge number
    '''
    G = nx.MultiDiGraph()
    for i in range(pcnt):
        line = fnode.readline().split(',')
        node = int(line[0])
        G.add_node(node, nodetype=line[1])

    if args.weighted:
        for i in range(ecnt):
            line = fedge.readline().split(',')
            st = int(line[0])
            ed = int(line[1])
            p = float(line[3])
            G.add_edge(st,ed,edgetype=line[2],prob=p)
    else:
        for i in range(ecnt):
            line = fedge.readline().split(',')
            st = int(line[0])
            ed = int(line[1])
            G.add_edge(st,ed,edgetype=line[2])

    if not args.directed:
        G = G.to_undirected()

    # the number of edge type and the number of node type are 1-based
    prob_matrix = np.zeros((args.nodetype + 1, args.edgetype + 1))
    for i in range(1, args.nodetype+1):
        line = fprob.readline().split()
        for j in range(1, args.edgetype + 1):
            prob_matrix[i][j]=float(line[j-1])

    return G, prob_matrix

def learn_embeddings(args, walks):
    walks = [map(str, walk) for walk in walks]
    model = Word2Vec(walks, size=args.dimensions, window=args.window_size, min_count=0, sg=1, workers=args.workers,
                     iter=args.iter)
    model.wv.save_word2vec_format(args.output)
    return

def main(args):
    f = open(args.input + "graph.dat", 'r')
    cnt = f.readline().split(',')

    pcnt = int(cnt[0]) # node number
    ecnt = int(cnt[1]) # edge number

    fnode = open(args.input + "node.dat", "r")
    fedge = open(args.input + "edge.dat", "r")

    fprob = open(args.prob, "r")

    temp = read(fprob, fnode, fedge, pcnt, ecnt)

    nx_G = temp[0]
    prob = temp[1]

    G = node2vec.Graph(nx_G, args.directed, prob)
    walks = G.simulate_walks(args.num_walks, args.walk_length)
    learn_embeddings(args, walks)

if __name__ == "__main__":
    args = parse_args()
    main(args)
