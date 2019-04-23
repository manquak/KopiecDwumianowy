#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <iostream>
//#include <iostream>
//using namespace std;
struct Heap { // Node of the binomial heap
	int key, degree;
	Heap *child, *sibling, *parent;
};

struct ReturnValue {
	Heap *root;
	Heap *write;
};

struct str {
	Heap *first;
	int grp;
	int filled;
};

// KORZYSTALEM GLOWNIE Z PSEUDOKODU Z WYKLADOW

Heap *MakeHeap() {
	Heap *H = new Heap;
	H->child = NULL;
	H->sibling = NULL;
	H->parent = NULL;
	H->degree = 0;
	//H->key = NULL;
	return H;
}


Heap *Maximum(Heap *H) { // maksimum bedzie zawsze jako jakis korzen
	Heap *y = NULL;
	Heap *x = H; //H->head;
	int max = -2147483647; // tu ma byc jak najnizsza wartosc
	while (x != NULL) {
		if (x->key > max) {
			max = x->key;
			y = x;
			//break;
		}
		x = x->sibling;
	}
	return y; // zwraca pointer gdzie najwieksza jest wartosc
}

void Link(Heap *y, Heap *z) {
	y->parent = z;
	y->sibling = z->child;
	z->child = y;
	z->degree = z->degree + 1;
}

Heap *Merge(Heap *H1, Heap *H2) { // pseudokod wyklady
								  //Heap *H = MakeHeap();
	Heap *ret = NULL;
	Heap *cur = NULL;
	Heap *next = NULL;
	//Heap *z = H2;
	//Heap *y = H1;

	while (H1 != NULL && H2 != NULL) {
		if (H1->degree <= H2->degree) {
			next = H1;
			H1 = H1->sibling;
		}
		else {
			next = H2;
			H2 = H2->sibling;
		}
		if (ret == NULL) {
			ret = next;
			cur = next;
		}
		else {
			cur->sibling = next;
			cur = next;
		}
	}
	if (H1 == NULL) {
		Heap *temp;
		temp = H1;
		H1 = H2; //h1=h2
		H2 = temp; // h2 = h1
	}
	if (ret == NULL) {
		return H1; // return h1
	}
	else cur->sibling = H1;
	return ret;

}

Heap *Union(Heap *y, Heap *z) {
	Heap *ret = Merge(y, z);
	if (ret == NULL) return NULL;
	Heap *prevX = NULL;
	Heap *X = ret;
	Heap *nextX = ret->sibling;
	while (nextX != NULL) {
		if ((X->degree != nextX->degree) || (nextX->sibling != NULL
			&& nextX->degree == nextX->sibling->degree)) {
			prevX = X;
			X = nextX;
			nextX = nextX->sibling;
		}
		else if (X->key >= nextX->key) { // dla min <=
			X->sibling = nextX->sibling;
			nextX->sibling = NULL;
			Link(nextX, X);
			nextX = X->sibling;
		}
		else {
			if (prevX == NULL) ret = nextX;
			else prevX->sibling = nextX;
			X->sibling = NULL;
			Link(X, nextX);
			X = nextX;
			nextX = nextX->sibling;
		}
	}
	return ret;
}

Heap *Insert(Heap *H, int key) {
	Heap *node = MakeHeap();
	node->key = key;
	return Union(H, node);
}

Heap *ReverseList(Heap *H) {
	Heap *prev, *curr, *next;
	prev = NULL;
	curr = H;
	while (curr != NULL) {
		next = curr->sibling;
		curr->sibling = prev;
		prev = curr;
		curr = next;
	}
	return prev; // nowa glowa odwroconej listy
}


ReturnValue ExtractMax(Heap *H) {
	ReturnValue obj;
	if (H == NULL) { obj.root = NULL; obj.write = NULL; return obj; }
	Heap *current = H;
	Heap *ret = Maximum(H);
	Heap *child = ret->child;
	Heap *children = ret->child;
	Heap *p = ret;
	// usuwanie korzenia
	if (ret == H) { // jesli maksymalny korzen to glowa 
		H = H->sibling;
		ret->sibling = NULL;
	}
	else { // jesli po srodku lub na koncu
		while (current->sibling != NULL) {
			if (current->sibling == ret) {
				current->sibling = current->sibling->sibling;
				ret->sibling = NULL;
				break;
			}
			current = current->sibling;
		}
	}
	while (children != NULL) { // jesli nie ma dzieci to sie nawet nie wykona
		children->parent = NULL;
		children = children->sibling;
	}
	Heap *H1 = ReverseList(child);
	H = Union(H, H1);
	ret->sibling = NULL;
	ret->child = NULL;
	//ret = NULL;
	obj.root = H;
	obj.write = p;
	return obj;

}

Heap *Search(Heap *H, int oldValue) { // https://www.sanfoundry.com/cpp-program-implement-binomial-heap/  
	if (H == NULL) return NULL;
	Heap* p = NULL;
	Heap* x = H;

	if (x->key == oldValue)
	{
		p = x;
		return p;
	}
	if (x->sibling != NULL && p == NULL)
		p = Search(x->sibling, oldValue);

	if (x->child != NULL && p == NULL)
		p = Search(x->child, oldValue);
	return p;
}

void IncreaseKey(Heap *H, int oldValue, int newValue) {
	Heap *head = H;
	Heap *node = Search(head, oldValue);
	if (node == NULL || H == NULL /*|| newValue < node->key*/) { printf("na\n"); return; } // jesli nie znaleziono lub kopiec byl pusty
	node->key = newValue;
	int tmp;
	while (node->parent != NULL && node->key > node->parent->key) {
		tmp = node->key;
		node->key = node->parent->key;
		node->parent->key = tmp;
		node = node->parent;
	}



}

// hash table
const int maxgroup = 10000;
const int notFound = -1;
const int DELETED = -2;
const int RandomConst = 120;
const int p = 10111; // 3001 jakas liczba pierwsza

int h(int key, int i) {
	return ((RandomConst* key + i) % p) % maxgroup;
}

int SearchA(str *tab, int key) { // zwraca indeks
	int j = 0;
	for (int i = 0; i < maxgroup; i++) {
		j = h(key, i);
		if (tab[j].filled == 0) {
			return notFound;
		}
		//if (tab[j].filled == DELETED) continue;
		if (tab[j].grp == key)
			return j;

	}
	return notFound;
}

void InsertA(str *tab, int group) {
	int j = 0;
	for (int i = 0; i < maxgroup; i++) {
		j = h(group, i);
		if (tab[j].filled == 0) {
			tab[j].grp = group;
			tab[j].filled = 1;
			return;
		}
	}
	//printf("przepelnienie");
}











int main() {
	ReturnValue ret;
	int n;
	char operation;
	int group1;
	int group2;
	int value;
	int oldV;
	int newV;
	int indx1;
	int indx2;
	str *tab = new str[maxgroup];
	for (int i = 0; i < maxgroup; i++) {
		tab[i].first = NULL;
		tab[i].filled = 0;
	}
	scanf("%d", &n);
	for (int i = 0; i < n; i++) {
		scanf(" %c", &operation);
		if (operation == 'a') {
			scanf("%d %d", &group1, &value);
			/*cin >> group1;
			cin >> value;*/
			indx1 = SearchA(tab, group1);
			if (indx1 == -1) { InsertA(tab, group1); indx1 = SearchA(tab, group1); }
			tab[indx1].first = Insert(tab[indx1].first, value);
		}
		else if (operation == 'e') {
			scanf("%d", &group1);
			/*	cin >> group1;*/
			indx1 = SearchA(tab, group1);
			if (indx1 == -1) { InsertA(tab, group1); indx1 = SearchA(tab, group1); }
			ret = ExtractMax(tab[indx1].first);
			tab[indx1].first = ret.root;
			if (ret.write != NULL) printf("%d\n", ret.write->key); /*cout << ret.write->key << endl;*/
			else printf("na\n");
		}
		else if (operation == 'm') {
			scanf("%d %d", &group1, &group2);
			/*cin >> group1;
			cin >> group2;*/
			indx1 = SearchA(tab, group1);
			if (indx1 == -1) { InsertA(tab, group1); indx1 = SearchA(tab, group1); }
			indx2 = SearchA(tab, group2);
			if (indx2 == -1) { InsertA(tab, group2); indx2 = SearchA(tab, group2); }
			tab[indx1].first = Union(tab[indx1].first, tab[indx2].first);
			tab[indx2].first = NULL;
		}
		else if (operation == 'i') {
			scanf("%d %d %d", &group1, &oldV, &newV);
			/*cin >> group1;
			cin >> oldV;
			cin >> newV;*/
			indx1 = SearchA(tab, group1);
			if (indx1 == -1) { InsertA(tab, group1); indx1 = SearchA(tab, group1); }
			IncreaseKey(tab[indx1].first, oldV, newV);
		}
		else if (operation == 'p') {
			scanf("%d", &group1);
			indx1 = SearchA(tab, group1);
			if (indx1 == -1) { InsertA(tab, group1); indx1 = SearchA(tab, group1); }
			Heap *temp = NULL;
			//if (tab[group1] == NULL) printf("na\n");
			while (tab[indx1].first != NULL) {
				ret = ExtractMax(tab[indx1].first);
				tab[indx1].first = ret.root;
				temp = Insert(temp, ret.write->key);
				printf("%d ", ret.write->key);
			}
			tab[indx1].first = temp;
			temp = NULL;
			printf("\n");
		}
	}
	delete[] tab;


	//system("pause");
	return 0;

}