 # Guide To Use
 
## k-shortest
Generate edge type distribution for each node type.    

input: graph information(graph.dat, node.dat, label.dat, edge.dat)     
output: distribution matrix(node types * edge types)      
example:
```
 
```
## node embedding
Generate node sequence according to distribution matrix, and generate node embedding via word2vec.

input: graph information, distribution matrix      
output: node embedding(dimension=128)     
example:
```
 
```

## ranking
Generate ranking result according to the similarity between nodes.    

input: node embedding, wfid.dat(query), acmid.dat(candidate)    
output: ranking result <wfid, acmid, rank, similarity>    
example:
```
 
```

## evaluation
Evaluate node embedding result via IR metrics.      

input: ranking result, ground truth   
output: evaluation result       
example:
```
 
```


 
