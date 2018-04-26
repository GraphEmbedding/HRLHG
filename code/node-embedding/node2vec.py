import numpy as np
import networkx as nx
import random
import math

class Graph():
	def __init__(self, nx_G, is_directed, prob): # prob: the distribution matrix of k-shortest training
		self.G = nx_G
		self.is_directed = is_directed
		self.prob = prob

	def node2vec_walk(self, walk_length, start_node):
		'''
		Simulate a random walk starting from start node.
		'''
		G = self.G
		walk = [start_node] # node id list

		while len(walk) < walk_length:
			cur = walk[-1]
			cur_nbrs = sorted(G.neighbors(cur))

			edgelist = [] # <current node id, end node id, edge type, chosen probability>
			while len(edgelist) < 1:
				etype = self.etype_choose(cur) # choose edge type
				if (etype < 0): # isolated node
					break
				for ed in cur_nbrs:
					for edge in G[cur][ed]: # index, not edge id
						if (int(G[cur][ed][edge]['edgetype']) == int(etype)):
							p = G[cur][ed][edge]['prob']
							temp = [cur, ed, edge, etype, p]
							edgelist.append(temp)

			edid = self.edge_choose(edgelist) # choose edge
			if (edid >= 1):
				walk.append(edid)

		return walk

	def simulate_walks(self, num_walks, walk_length):
		'''
		Repeatedly simulate random walks from each node.
		'''
		G = self.G
		walks = []
		nodes = list(G.nodes())
		print 'Walk iteration:'
		for walk_iter in range(num_walks):
			print str(walk_iter+1), '/', str(num_walks)
			random.shuffle(nodes)
			nodecnt=1
			for node in nodes:
				if(int(nodecnt)%1000 == 0):
					print "The node:",nodecnt
				nodecnt=nodecnt+1
				walks.append(self.node2vec_walk(walk_length=walk_length, start_node=node))
		return walks


	def etype_choose(self,cur): # step 1
		tcur = self.G.node[cur]['nodetype']
		temp = self.prob[int(tcur)]
		problist = []
		eps = 0.00000001

		for i in range (0,len(temp)):
			if (math.fabs(temp[i])>eps):
				t = [i, temp[i]]
				problist.append(t)

		problist = sorted(problist, key=lambda problist: problist[1]) # sort by prob

		total = 0.0
		for i in range(0,len(problist)):
			total += problist[i][1]

		if (math.fabs(total) < eps): # isolated node
			return -1

		for i in range(0,len(problist)):
			problist[i][1] = problist[i][1]/total

		for i in range(1,len(problist)):
			problist[i][1] += problist[i-1][1]

		type = '1'
		r = np.random.rand()
		for i in range(0,len(problist)):
			if (r < problist[0][1]):
				type = problist[0][0]
				break
			elif (i > 0 and problist[i-1][1] < r and r < problist[i][1]):
				type = problist[i][0]
				break
		return type


	def edge_choose(self,elist): # step2
		# only preserve edges with the specific type chosed by self.etype_choose, then get the specific edge
		edgelist = []
		eps = 0.00000001

		for i in range(0, len(elist)):
			temp = elist[i]
			if (math.fabs(temp[4]) > eps):  # prob!=0
				edgelist.append(temp)

		edgelist = sorted(edgelist, key=lambda edgelist: edgelist[4])  # sort by prob

		#normalization
		total = 0.0
		for i in range(0, len(edgelist)):
			total += edgelist[i][4]

		if (math.fabs(total) < eps):  # isolated node
			return -1

		for i in range(0, len(edgelist)):
			edgelist[i][4] = edgelist[i][4] / total

		for i in range(1, len(edgelist)):
			edgelist[i][4] += edgelist[i - 1][4]

		edid = 1

		r = np.random.rand()
		for i in range(0, len(edgelist)):
			if (r < edgelist[0][4]):
				edid = edgelist[0][1]
				break
			elif (i > 0 and edgelist[i - 1][4] < r and r < edgelist[i][4]):
				edid = edgelist[i][1]
				break

		return edid