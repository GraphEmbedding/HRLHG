 # Guide To Use
 
## k-shortest
Generate edge type distribution for each node type.    

input: graph information(graph.dat, node.dat, label.dat, edge.dat)     
output: distribution matrix(node types * edge types)      
![](/graphs/3.png)

example:
```
compile:
g++ <source file> -o <object file>
run:
nohup <object file> <data fold> <updating method> <output path> <stop ratio> <dumping factor> &

eg:
g++ k_shortest_k2.cpp -o ks2
nohup ./ks2 L2R/fold1 2 L2R/fold1/prob2_8_1.txt 0.8 0.1 &
```

## node embedding
Generate node sequence according to distribution matrix, and generate node embedding via word2vec.

input: graph information(graph.dat, node.dat, edge.dat), distribution matrix      
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


 
