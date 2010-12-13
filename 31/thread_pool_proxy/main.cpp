#include <cstdlib>
#include "../../libs/Thread/Thread.h"
#include "ThreadPool.h"
#include "Scheduler.h"
#include "AcceptTask.h"

using namespace std;

int main(int argc, char *argv[]) {
    int threads_count = argc < 2 ? : atoi(argv[1]);
    Scheduler::enqueue_read_task(new AcceptTask);
    new ThreadPool(threads_count);
    Thread::exit(NULL);
}

