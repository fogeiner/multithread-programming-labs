#include "../libs/Thread/Thread.h"
#include "../libs/Semaphore/Semaphore.h"
#include <cstdio>
/*Разработайте имитатор производственной линии, изготавливающей винтики (widget).
 * Винтик собирается из детали C и модуля, который, в свою очередь,
 * состоит из деталей A и B. Для изготовления детали A требуется 1 секунда,
 * В – две секунды, С – три секунды.
 * Задержку изготовления деталей имитируйте при помощи sleep. Используйте семафоры-счетчики.
 */

Semaphore a, b, c, m, w;

// module c

void *widget(void *ptr) {
    for (;;) {
        m--;
        c--;
        w++;
    }
    return NULL;
}

// 3

void *detC(void *ptr) {
    for (;;) {
        sleep(3);
        c++;
    }
    return NULL;
}

// 1

void *detA(void *ptr) {
    for (;;) {
        sleep(1);
        a++;
    }
    return NULL;
}

// 2

void *detB(void *ptr) {
    for (;;) {
        sleep(2);
        b++;
    }
    return NULL;
}

// a b

void *module(void *ptr) {
    for (;;) {
        a--;
        b--;
        m++;
    }
    return NULL;
}

void *print_state(void *ptr) {
    for (;;) {
        fprintf(stdout,
                "a: %d b: %d c: %d m: %d w: %d\n",
                a.getvalue(), b.getvalue(), c.getvalue(), m.getvalue(), w.getvalue());
        sleep(1);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    Thread threads[] = {
        Thread(detA), Thread(detB), Thread(detC),
        Thread(module), Thread(widget), Thread(print_state)
    };

    for(int i = 0; i < sizeof(threads)/sizeof(Thread); ++i){
        threads[i].run();
    }

    for(int i = 0; i < sizeof(threads)/sizeof(Thread); ++i){
        threads[i].join();
    }

    Thread::exit();
}