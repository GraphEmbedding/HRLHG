# HRLHG
This is the implementation of the Hierarchical Representation Learning on Heterogeneous Graph(HRLHG) model from paper "Cross-language Citation Recommendation via Hierarchical Representation Learning on Heterogeneous Graph"

## Requirements
numpy    
networkx    
gensim    

## Pipeline
1. generate distribution matrix via k-shortest        
2. generate node sequence according to distribution matrix, and generate node embedding via word2vec          
3. generate ranking result according to the cosine similarity between nodes        
4. evaluate node embedding result via IR metrics       

## Efficiency
node embedding needs 2 days in normal CPU machine with follow settings:      
walk-length: 80   
dimension: 128   
num-walks: 10   
window-size: 10   
iter: 1   

## Cite the paper
```
@inproceedings{hrlhg-sigir2018,
  author          = {{Jiang}, Zhuoren and {Yin}, Yue and {Gao}, Liangcai and {Lu}, Yao and {Liu}, Xiaozhong},
  title           = "{Cross-language Citation Recommendation via Hierarchical Representation Learning on Heterogeneous Graph}",
  booktitle       = {Proceedings of the 41th International ACM SIGIR Conference on Research & Development in Information Retrieval},
  organization    = {ACM},
  year            = 2018,
}
```


