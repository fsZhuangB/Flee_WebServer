#include "threadpool.hpp"

using namespace std;
int main()
{
    threadpool_t* thp = threadpool_create(3, 100, 100);
    cout << "Pool init\n";

    int num[20], i;
    for (i = 0; i < 20; ++i)
    {
        num[i] = i;
        cout << "Add task" << i << endl;

        threadpool_add(thp, process, (void*)&num[i]);  /* 向线程池中添加任务 */
    }
    /* 等线程完成任务 */
    sleep(10);
    threadpool_destroy(thp);
}