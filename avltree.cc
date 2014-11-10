#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <queue>
using namespace std;

typedef struct avl_node{
	avl_node *left;
	avl_node *right;
	int avl;
	int key;
}node;

typedef struct root{
	avl_node *start;
}root;

node* make_node(int key) {
	avl_node *new_node;
	new_node->left = NULL;
	new_node->right = NULL;
	new_node->key = key; 
	cout << "make node has a key of: " << key << '\n';
	return new_node;
}

node* insert(avl_node* node, int key) {
	if(node == NULL) {
		return make_node(key);
	} else if(node->key > key) {
		
		node->left = insert(node->left, key);
	} else {
		node->right = insert(node->right, key);
	}
}

void bfs(avl_node* node) {
	queue<avl_node> qway;
	avl_node *tmp;
	qway.push(*node);
	cout << node->key << '\n';
	cout << qway.front().key << '\n';
	while(qway.size() > 0) {
		*tmp = qway.front();
		qway.pop();
		cout << tmp->key << " " << '\n';

		if(tmp->left != NULL) {
			tmp = tmp->left;
			qway.push(*tmp);
		}
		if(tmp->right != NULL) {
			tmp = tmp->right;
			qway.push(*tmp);
		}
	}
}

int main() {
	avl_node *tree = make_node(9);
	cout << "root key " << tree->key << '\n';
	tree = insert(tree, 11);
	cout << "root key after insert " << tree->key << '\n';
	cout << "i fukin hate binary trees" << '\n';
	bfs(tree);		
	
	
}
