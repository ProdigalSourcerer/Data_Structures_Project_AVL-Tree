This program was developed as a group project for a lower-division Data Structures class at De Anza College in 2013. The project objective was to build an application using some form of a tree structure. We settled on a data storage program for a prison populated entirely by cartoon characters. I have no idea how we arrived at that somewhat dubious decision.

Nevertheless, my portion of the project was to develop the abstract data type for the underlying data structure. I'm proud of my work here because I took on more challenge than was required, and I was successful. Rather than build a binary search tree, I opted to independently learn and develop the rebalancing AVL tree. On top of that, there was an error in the textbook, that required me to really understand the data structure before I could be confident the error _was_ an error and correct it. Also the textbook claimed that an AVL tree could not support duplicate keys, and that seemed like a substantial limitation to me, so I set out to work around it. In the end, I was successful.

The solution is a combination of an additional queue attached to the header structure, and a recursive search algorithm that branches when it finds a result. That is for every node examined that is _not_ a match for the search term, the ordinary binary search rules apply. For every node that _is_ a match however, the search first adds the matching node to the results queue, and then recursively searches both child subtrees.

My work on this project is found in the following files:
-`AVL_ADT.c`
-`AVL_ADT.h`
-`linkListADT.c`
-`linkListADT.h`
-`queue_ADT.c`
-`queue_ADT.h`
-`iain.c` (not much actual work here... just a utility I had built previously I provided to the team lead for her use)
