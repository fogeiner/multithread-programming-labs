/*
 * 23. Производитель-потребитель

void mymsginit(queue *);
 * void mymsqdrop(queue *);
 * void mymsgdestroy(queue *);
 * int mymsgput(queue *, char * msg);
 * int mymsgget(queue *, char * buf, size_t bufsize);
mymsgput принимает в качестве параметра ASCIIZ строку символов, обрезает ее до 80 символов (если это необходимо) и помещает ее в очередь.
 * Если очередь содержит более 10 записей, mymsgput блокируется. Функция возвращает количество переданных символов.
mymsgget возвращает первую запись из очереди, обрезая ее до размера пользовательского буфера (если это необходимо).
 * В любом случае, запись извлекается из очереди полностью. Если очередь пуста, mymsgget блокируется. Функция возвращает количество прочитанных символов.
mymsgdrop должна приводить к разблокированию ожидающих операций get и put. Ожидавшие вызовы и все последующие вызовы get и put должны возвращать 0.
mymsqdestroy должна вызываться после того, как будет известно, что ни одна нить больше не попытается выполнять операции над очередью.

Необходимо продемонстрировать работу очереди с двумя производителями и двумя потребителями.

Для синхронизации доступа к очереди используйте семафоры-счетчики.
 * */

#include <pthread.h>
#include <signal.h>
#include <cstdio>
#include <iostream>

#include "../libs/Semaphore/Semaphore.h"
#include "../libs/MsgQueue/MsgQueue.h"
#include "../libs/Thread/Thread.h"

int stop_flag = 0;
const int size = 256;

void *f1(void *arg) {
    MsgQueue *m = static_cast<MsgQueue*> (arg);
    char buf[size];
    for (int i = 0;; ++i) {
        if (stop_flag) {
            m->drop();
            break;
        }

        sprintf(buf, "f1 %d\n", i);
        m->put(buf);
        sleep(2);
    }
    return NULL;
}

void *f2(void *arg) {
    MsgQueue *m = static_cast<MsgQueue*> (arg);
    char buf[size];
    for (int i = 0;; ++i) {
        if (stop_flag)
            break;

        sprintf(buf, "f2 %d\n", i);
        m->put(buf);
        sleep(3);
    }
    return NULL;
}

void *f3(void *arg) {
    MsgQueue *m = static_cast<MsgQueue*> (arg);
    char buf[size];
    for (int i = 0;; ++i) {
        if (stop_flag)
            break;

        int r = m->get(buf, sizeof (buf));
        std::cout << "f3 read " << r << " symbols: " << buf;
        sleep(2);
    }
    return NULL;
}

void *f4(void *arg) {
    MsgQueue *m = static_cast<MsgQueue*> (arg);
    char buf[size];
    for (;;) {
        if (stop_flag)
            break;


        int r = m->get(buf, sizeof (buf));
        std::cout << "f4 read " << r << " symbols: " << buf;
        sleep(1);
    }
    return NULL;
}

void stop(int sig) {
    stop_flag = 1;
}

int main(int argc, char *argv[]) {
    try {
        MsgQueue *msgq = new MsgQueue(10);
        Thread threads[] = {Thread(f1, msgq),
            Thread(f2, msgq), Thread(f3, msgq), Thread(f4, msgq)};
        for (int i = 0; i < sizeof (threads) / sizeof (Thread); ++i) {
            threads[i].run();
        }

        signal(SIGINT, stop);

        pause();

        for (int i = 0; i < sizeof (threads) / sizeof (Thread); ++i) {
            threads[i].join(NULL);
        }
        delete msgq;
        pthread_exit(NULL);
    } catch (std::exception &ex) {
        std::cerr << ex.what();
    }
}
