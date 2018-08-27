# HRLHG
This is the implementation of the Hierarchical Representation Learning on Heterogeneous Graph(HRLHG) model from paper "Cross-language Citation Recommendation via Hierarchical Representation Learning on Heterogeneous Graph". Note that this is only a reference implementation of the HRLHG algorithm and could benefit from several performance enhancement schemes, some of which are discussed in the paper.

![](/graphs/1.png)

## Requirements
numpy    
networkx    
gensim    

## Pipeline
1.generate Relation Type Usefulness Distributions (RTUD) via K-shortest paths ranking based EM algorithm    
2.generate node sequence via hierarchical (two-level) random walk algorithm based on RTUD, and generate node embedding    
3.generate ranking result according to the cosine similarity between nodes    
4.evaluate node embedding result via IR metrics    

## Efficiency
node embedding needs 2 days in normal CPU machine with follow settings:      
walk-length: 80   
dimension: 128   
num-walks: 10   
window-size: 10

The time complexity can be further reduced, if we parallelize the hierarchical random walk simulations, and execute them asynchronously  

## Vertexes and relations of cross-language heterogeneous graph
![](/graphs/4.png)

## Cite the paper
```
Z. Jiang, Y. Yin, L. Gao, Y. Lu, and X. Liu. 
Cross-language Citation Recommendation via Hierarchical Representation Learning on Heterogeneous Graph.
In Proceedings of the 41th International ACM SIGIR Conference on Research & Development in Information Retrieval. 
ACM. 2018.
```
```
@inproceedings{hrlhg-sigir2018,
  author          = {{Jiang}, Zhuoren and {Yin}, Yue and {Gao}, Liangcai and {Lu}, Yao and {Liu}, Xiaozhong},
  title           = "{Cross-language Citation Recommendation via Hierarchical Representation Learning on Heterogeneous Graph}",
  booktitle       = {Proceedings of the 41th International ACM SIGIR Conference on Research & Development in Information Retrieval},
  organization    = {ACM},
  year            = 2018,
}
```


