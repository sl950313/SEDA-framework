#include "../splaytree.h"
#include <stdio.h>

int main() {
   splay_tree *root = NULL;

   int data[3] = {1, 2, 3};
   root = splaytree_insert(root, 1001, &data[0]);
   root = splaytree_insert(root, 1002, &data[1]);
   root = splaytree_insert(root, 1003, &data[2]);

   printf("size : %d\n", root->size);

   splay_tree *sp = splaytree_splay(root, 1002);
   if (sp) {
      int *_data = (int *)sp->data;
      printf("sp data : %d\n", *_data);
   } else {
      printf("key 1002 can not be found\n");
   }

   return 0;
}
