Evaluation running:

1. generate evaluation file according to ranking file:
```
java -Djava.ext.dirs=lib -cp jar/Eval.jar ireval.gen.GenerateEvaluation <current dir> <ranking file name>
eg:
java -Djava.ext.dirs=lib -cp jar/Eval.jar ireval.gen.GenerateEvaluation D:\HeterogeneousNetworkEmbedding\evaluation\rank k2_emb_fold1_wl80_cos
```

2. evaluation:
```
java -cp jar/Eval.jar ireval.Main <ground truth file> <output file> <evaluation file>
eg: 
java  -Xmx1024m -Xms1024m -cp jar/Eval.jar ireval.Main file/judgement_fold1.dat k2_emb_fold1_wl80_cos.csv rank/k2_emb_fold1_wl80_cos.dat
```


At first, the evaluation output is eval result for each query. And the results marked "all" are the final result.  

