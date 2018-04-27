# metapath2vec
For better handling heterogeneous graph, we followed the paper ["metapath2vec:Scalable Representation Learning for Heterogeneous Networks"](https://www3.nd.edu/~dial/publications/dong2017metapath2vec.pdf) and referred their implementation of "heterogeneous negative sampling" model for our heterogeneous node embedding generation. 

## Guide to use
input: node sequence file generated via hierarchical random walk     
output: node embedding via word2vec with heterogeneous negative sampling    
example:
```
g++ metapath2vec_citation.cpp -lpthread -o m2v
nohup ./m2v -train k2_rw4mp_seq_fold1_wl80.out -output k2_rw4mp_emb_fold1_wl80 -pp 1 -size 128 -window 10 -negative 5 -threads 24 -debug 0 &
```

## Reference
https://www3.nd.edu/~dial/publications/dong2017metapath2vec.pdf      
https://ericdongyx.github.io/metapath2vec/m2v.html
