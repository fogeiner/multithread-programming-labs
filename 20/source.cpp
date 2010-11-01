#include <string>
#include <iostream>
#include <pthread.h>
#include <cstdlib>
#include <cassert>

using namespace std;


class List {
private:

    class Node {
        friend class List;
    private:
        pthread_rwlock_t rwlock;
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

        void rdlock(){
            pthread_rwlock_rdlock(&rwlock);
        }
        
        void wrlock(){
			pthread_rwlock_wrlock(&rwlock);
		}
        
        void unlock(){
            pthread_rwlock_unlock(&rwlock);
        }

        Node(string data, Node *next, Node *prev) :
        data(data), next(next), prev(prev) {
			
			pthread_rwlock_init(&rwlock, NULL);
        }

        ~Node() {
            pthread_rwlock_destroy(&rwlock);
        }
    };

    Node *head;
    int list_size;

public:

		void sort_list(int *sleep_time = NULL) {
			static const int SWAP_TIME = 1;

			if (list_size <= 1){
				return;
			}

			Node *bound = head;
			Node *n1, *n2;

			for(;;){
			
				head->rdlock();
				n1 = head->next;
				head->unlock();

				n1->wrlock();   
				n2 = n1->next;
				if(n2 == bound){
					n1->unlock();
					break;
				}

				n2->wrlock();
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
						n2->wrlock();
						n1->prev->unlock();
					} else {
						n2->unlock();
						n1->unlock();
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
        
        	head->wrlock();

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
        return list_size;
    }

    void push_back(string s) {
        head->wrlock();
        if(head->prev != head){
            head->prev->wrlock();
        }
        
        Node *n = new Node(s, head, head->prev);
        head->prev->next = n;
        head->prev = n;

        list_size++;

        if(n->prev != head){
            n->prev->unlock();
        }
        head->unlock();
    }

    void push_front(string s) {
        head->wrlock();
        if(head->next != head){
            head->next->wrlock();
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
      //      n->rdlock();
            cout << "Entry " << counter << "\t" << n->data << endl;
      //      n->unlock();
            counter++;
        }
    }
};

bool stop_flag = false;
int sleep_time = 1;
static void *auto_sort(void *ptr){
	List *list = static_cast<List*> (ptr);

	const static int SLEEP_TIME = 5;

	for (;;) {
		if(stop_flag){
			break;
		}

		sleep(SLEEP_TIME);

		list->sort_list(&sleep_time);
	}
	
	return NULL;
}


int main(int argc, char *argv[]) {

	pthread_t sort_tid;
	List *list = new List();
	pthread_create(&sort_tid, NULL, auto_sort, list);

	cout << "Input lines, please:" << endl;
	string s;
	for (;;) {
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
	
	pthread_join(sort_tid, NULL);
	delete list;

	pthread_exit(NULL);
}
