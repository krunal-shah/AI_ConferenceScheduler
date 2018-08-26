26/08
1. Changes in getstartstate():
1.1 optimisation in similarity computation. 
1.2 successive "worst" paper selection heuristic.
1.3 [alternate scheme] top-2*k priority queue selection (?)

2. New initialisation greedy algorithm (almost deterministic):
2.1 getstartstate_weighted(): incorporates dissimilarity also while computing best clusters
2.2 check assignment is happening correctly -- because of the swap we made in tracks and slots [check the Assignschedule function]
2.3 some errors in functions following getStartState() -- probably due to data structure changes in getsStartState - will debug tomorrow
