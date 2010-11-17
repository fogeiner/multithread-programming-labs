#include <string>
#include <iostream>
#include <pthread.h>
#include <cstdlib>
#include <cassert>

using namespace std;
void *auto_sort(void*);

class List {
	friend void* auto_sort(void*);
	private:
	class Node {
		private:
			friend class List;
		public:
			string data;
			Node *next;
			Node *prev;

			static int compare(Node *n1, Node *n2) {
				return n1->data.compare(n2->data);
			}

			static void swap(Node *n1, Node *n2) {
				n1->data.swap(n2->data);
			}

			Node(string data, Node *next, Node *prev) :
				data(data), next(next), prev(prev) {

				}
	};


	pthread_t sort_tid;
	Node *head;
	int list_size;
	mutable pthread_rwlock_t rwlock;
	public:

	void sort_list() {

		Node *bound = head;
		Node *n1, *n2;

		// it's safe not to lock head here
		if (list_size <= 1){
			return;
		}

		pthread_rwlock_wrlock(&rwlock);

		while(1) {				
			n1 = head->next;
			n2 = n1->next;

			if(n2 == bound){
				break;
			}

			while(1) {
				if(Node::compare(n1, n2) > 0){
					Node::swap(n1, n2);
				}

				if(n2->next != bound){
					n1 = n2;
					n2 = n2->next;
				} else {
					bound = n2;
					break;
				}
			}
		}


		pthread_rwlock_unlock(&rwlock);
	}

	List() : list_size(0) {
		head = new Node("", NULL, NULL);
		head->next = head;
		head->prev = head;

		pthread_rwlock_init(&rwlock, NULL);
	}

	~List() {

		pthread_rwlock_wrlock(&rwlock);
		for (Node *n = head->next; n != head;) {
			Node *t = n->next;
			delete n;
			n = t;
		}

		delete head;
		list_size = 0;
		head = NULL;

		pthread_rwlock_unlock(&rwlock);
		pthread_rwlock_destroy(&rwlock);
	}

	int size() const {
		return list_size;
	}

	void push_front(string s) {
		pthread_rwlock_wrlock(&rwlock);

		Node *n = new Node(s, head->next, head);
		head->next->prev = n;
		head->next = n;

		list_size++;

		pthread_rwlock_unlock(&rwlock);
	}

	void print_list() const {
		int counter = 0;
		for (Node *n = head->next; n != head; n = n->next) {
			cout << "Entry " << counter << "\t" << n->data << endl;
			counter++;
		}
	}
};


bool stop_flag = false;

void *auto_sort(void *ptr) {
	List *list = static_cast<List*> (ptr);
	const static int SORT_SLEEP_TIME = 5;
	while(!stop_flag) {

		sleep(SORT_SLEEP_TIME);

		if (list->head == NULL) {
			break;
		}

		list->sort_list();
	}

	return NULL;
}


int main(int argc, char *argv[]) {
	const int SORT_THREADS_COUNT = 10;
	pthread_t sort_tids[SORT_THREADS_COUNT];
	List *list = new List();

	for(int i = 0; i < SORT_THREADS_COUNT; ++i){
		pthread_create(&sort_tids[i], NULL, auto_sort, list);
	}

	cout << "Input lines, please:" << endl;
	string s;
	while(1) {
		getline(cin, s);

		if (cin.eof())
			break;

		if (s.empty()) {
			list->print_list();
		} else {
			list->push_front(s);
		}
	}

	stop_flag = true;
	for(int i = 0; i < SORT_THREADS_COUNT; ++i){
		pthread_join(sort_tids[i], NULL);
	}

	delete list;
	pthread_exit(NULL);
}
