'''
Reference implementation of HRLHG. 
Author: Yue Yin
'''

import argparse
import numpy as np
import networkx as nx

wfidlist = list()
acmidlist = list()
embdict = dict()

worst_rank = 10000

wfidcnt = 0
acmidcnt = 0
Dimension = 128

def parse_args():
    parser = argparse.ArgumentParser(description="Run node2vec.")
    parser.add_argument('--fold', nargs='?', default='fold1/', help='Input path')
    parser.add_argument('--output', nargs='?', default='even_fold1_rank_wl20.txt', help='Output path')
    parser.add_argument('--emb', nargs='?', default='even_fold1_emb_wl20.out', help='Node embedding result.')
    return parser.parse_args()

def cos(vector1,vector2):
    dot_product = 0.0;
    normA = 0.0;
    normB = 0.0;
    for a,b in zip(vector1,vector2):
        dot_product += a*b
        normA += a**2
        normB += b**2
    if normA == 0.0 or normB==0.0:
        return -1.0
    else:
        return dot_product / ((normA*normB)**0.5)

def read(femb, fwfid, facmid):
    while 1:
        emb = femb.readline()
        if not emb:
            break
        list = emb.split()
        embed = map(eval, list[1:])
        embdict[int(list[0])] = embed

    while 1:
        wfid = fwfid.readline()
        if not wfid:
            break
        temp = wfid.split()
        wfidlist.append(int(temp[0]))

    while 1:
        acmid = facmid.readline()
        if not acmid:
            break
        temp = acmid.split()
        acmidlist.append(int(temp[0]))

def rank(args):
    # output order: wfid(query) acmid(candidate) rank similarity

    rankfile = file(args.fold+args.output, 'w')

    nokeyset = set()
    listzero = list()
    for i in range(Dimension):
        listzero.append(0)

    for wfid in wfidlist:
        if(embdict.has_key(wfid) == False):
            nokeyset.add(wfid)
            embdict[wfid] = listzero
            for acmid in acmidlist:
                rankfile.write(str(wfid)+' '+str(acmid)+' '+str(worst_rank)+' '+'-1'+'\n')
            rankfile.write("\n")
        else:
            ranklist = []
            for acmid in acmidlist:
                if(embdict.has_key(acmid) == False):
                    nokeyset.add(acmid)
                    embdict[acmid] = listzero
                similarity = cos(embdict[wfid], embdict[acmid])
                item = [acmid, similarity]
                ranklist.append(item)
            ranklist.sort(reverse=True, key=lambda x: x[1])  # rank
            for iter in range(len(ranklist)):
                rankfile.write(str(wfid)+' '+str(ranklist[iter][0])+' '+str(iter + 1)+' '+str(ranklist[iter][1])+'\n')
            rankfile.write("\n")

    rankfile.close()

def main(args):
    emb = open(args.fold+ args.emb, "r")
    wfid = open(args.fold+"wfid.dat", 'r')
    acmid = open(args.fold+"acmid.dat", "r")

    read(emb, wfid, acmid)
    rank(args)


if __name__ == "__main__":
    args = parse_args()
    main(args)
