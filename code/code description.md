
## k-shortest
	input: graph information
	function: generate edge type distribution for each node type
	output: distribution matrix(node types * edge types)

## node embedding
	input: graph information, distribution matrix
	function: generate node sequence according to distribution matrix, and generate node embedding via word2vec
	output: node embedding(dimension=128)

## ranking
	input: node embedding, wfid.dat(query), acmid.dat(candidate)
	function: generate ranking result according to the similarity between nodes
	output: ranking result <wfid, acmid, rank, similarity>

## evaluation
	input: ranking result, ground truth
	function: evaluate node embedding result via IR metrics 
	output: evaluation result
