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

        void rdlock() {
            pthread_rwlock_rdlock(&rwlock);
        }

        void wrlock() {
            pthread_rwlock_wrlock(&rwlock);
        }

        void unlock() {
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


    pthread_t sort_tid;
    Node *head;
    int list_size;

    static void *auto_sort(void *ptr) {
        List *list = static_cast<List*> (ptr);

        const static int SLEEP_TIME = 1;

        for (;;) {
            sleep(SLEEP_TIME);

            if (list->head == NULL) {
                break;
            }

            list->sort_list();
        }

        return NULL;
    }

    Node *getNodeIndexOf(int index) {
        int i = 0;
        assert(index >= 0);
        Node *n = head->next;

        for (;; n = n->next, ++i) {
            if (i == index) {
                break;
            }
        }
        return n;
    }

    void sort_list() {
        if (list_size <= 1)
            return;

        bool swapped;

        for (int i = list_size - 1; i > 0; --i) {
            for (int j = 0; j < i; ++j) {
                swapped = false;
                Node *n1 = getNodeIndexOf(j);
                Node *n2 = getNodeIndexOf(j + 1);
                n1->wrlock();
                n2->wrlock();
                if (Node::compare(n1, n2) > 0) {
                    Node::swap(n1, n2);
                    swapped = true;
                }
                n2->unlock();
                n1->unlock();

                if (swapped) {
                    sleep(1);
                }
            }
        }
    }

public:

    List() : list_size(0) {
        head = new Node("", NULL, NULL);
        head->next = head;
        head->prev = head;

        pthread_create(&sort_tid, NULL, &auto_sort, this);
    }

    ~List() {
        Node *head_ptr = head;

        head = NULL;
        pthread_cancel(sort_tid);
        pthread_join(sort_tid, NULL);

        head_ptr->wrlock();

        for (Node *n = head_ptr->next; n != head_ptr;) {
            Node *t = n->next;
            delete n;
            n = t;
        }

        list_size = 0;
        head_ptr->unlock();
        delete head_ptr;
    }

    int size() const {
        return list_size;
    }

    void push_back(string s) {
        head->wrlock();
        if (head->prev != head) {
            head->prev->wrlock();
        }

        Node *n = new Node(s, head, head->prev);
        head->prev->next = n;
        head->prev = n;

        list_size++;

        if (n->prev != head) {
            n->prev->unlock();
        }
        head->unlock();
    }

    void push_front(string s) {
        head->wrlock();
        if (head->next != head) {
            head->next->wrlock();
        }

        Node *n = new Node(s, head->next, head);
        head->next->prev = n;
        head->next = n;

        list_size++;

        if (n->next != head) {
            n->next->unlock();
        }

        head->unlock();
    }

    void print_list() const {

        int counter = 0;


        for (Node *n = head->next; n != head; n = n->next) {
            n->rdlock();
            cout << "Entry " << counter << "\t" << n->data << endl;
            n->unlock();
            counter++;
        }
    }
};

int main(int argc, char *argv[]) {

    List list;
    cout << "Input lines, please:" << endl;
    string s;
    for (;;) {
        getline(cin, s);

        if (cin.eof()) {
            break;
        } else if (s.empty()) {
            list.print_list();
        } else {
            list.push_front(s);
        }
    }
    pthread_exit(NULL);
}
