#include <string>
#include <iostream>
#include <pthread.h>
#include <cstdlib>
#include <cassert>
#include "../libs/Mutex/Mutex.h"
using namespace std;

class List {
	private:

		class Node {
			friend class List;
			private:
			Mutex mutex;
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

			void lock(){
				mutex.lock();
			}

			void unlock(){
				mutex.unlock();
			}

			Node(string data, Node *next, Node *prev) :
				data(data), next(next), prev(prev), mutex(Mutex::ERRORCHECK_MUTEX) {

				}

			~Node() {
			}
		};


		Node *head;
		int list_size;


	public:
		void sort_list(int *sleep_time = NULL) {

			Node *bound = head;
			Node *n1, *n2;

			// it's safe not to lock head here
			if (list_size <= 1){
				return;
			}
			
			for(;;){				
				head->lock();
				n1 = head->next;
				n1->lock();   

				n2 = n1->next;
				
				if(n2 == bound){
					head->unlock();
					n1->unlock();
					return;
				}
				
				n2->lock();
				
				head->unlock();
				for(;;){
					if(Node::compare(n1, n2) > 0){
						Node::swap(n1, n2);
						if(sleep_time != NULL && *sleep_time != 0){
							sleep(*sleep_time);
						}
					}

					if(n2->next != bound){
						n1 = n2;
						n2 = n2->next;
						n2->lock();
						n1->prev->unlock();
					} else {
						n1->unlock();
						n2->unlock();
						bound = n2;
						break;
					}
				}
			}

		}

		List() : list_size(0) {
			head = new Node("", NULL, NULL);
			head->next = head;
			head->prev = head;
		}

		~List() {

			head->lock();

			for (Node *n = head->next; n != head;) {
				Node *t = n->next;
				delete n;
				n = t;
			}

			list_size = 0;
			head->unlock();
			delete head;
		}

		int size() const {
			head->lock();
			return list_size;
			head->unlock();
		}

		void push_front(string s) {
			head->lock();
			if(head->next != head){
				head->next->lock();
			}

			Node *n = new Node(s, head->next, head);
			head->next->prev = n;
			head->next = n;

			list_size++;

			if(n->next != head){
				n->next->unlock();
			}

			head->unlock();
		}

		void print_list() const {

			int counter = 0;

			for (Node *n = head->next; n != head; n = n->next) {
				// otherwise sorting and printing is hell
						//n->lock();
				cout << "Entry " << counter << "\t" << n->data << endl;
						//n->unlock();
				counter++;
			}
		}
};

bool stop_flag = false;
int sleep_time = 1;
static void *auto_sort(void *ptr){
	List *list = static_cast<List*> (ptr);

	const static int SORT_SLEEP_TIME = 5;

	for (;;) {
		if(stop_flag){
			break;
		}

		sleep(SORT_SLEEP_TIME);

		list->sort_list(&sleep_time);
	}

	return NULL;
}


int main(int argc, char *argv[]) {

	pthread_t sort_tids[2];
	List *list = new List();
	for(int i = 0; i < sizeof(sort_tids)/sizeof(pthread_t); ++i){
		pthread_create(&sort_tids[i], NULL, auto_sort, list);
	}

	cout << "Input lines, please:" << endl;
	string s;
	while(1) {
		getline(cin, s);

		if (cin.eof()){
			break;
		} else if (s.empty()) {
			list->print_list();
		} else {
			list->push_front(s);
		}
	}

	stop_flag = true;
	sleep_time = 0;

	for(int i = 0; i < sizeof(sort_tids)/sizeof(pthread_t); ++i){
		pthread_join(sort_tids[i], NULL);
	}
	delete list;

	pthread_exit(NULL);
}
