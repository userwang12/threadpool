// 线程池项目.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <chrono>
#include <thread>
using namespace std;

#include "threadpool.h"

/*
有些场景，是希望能够获取线程执行任务得返回值得
举例：
1 + 。。。 + 30000的和
thread1  1 + ... + 10000
thread2  10001 + ... + 20000
.....

main thread：给每一个线程分配计算的区间，并等待他们算完返回结果，合并最终的结果即可
*/

using uLong = unsigned long long;

class MyTask : public Task
{
public:
	MyTask(int begin, int end)
		: begin_(begin)
		, end_(end)
	{}
	// C++17 Any类型,手动实现
	Any run()  // run方法最终就在线程池分配的线程中去做执行了!
	{
		std::cout << "tid:" << std::this_thread::get_id()
			<< "begin!" << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(3));
		uLong sum = 0;
		for (uLong i = begin_; i <= end_; i++)
			sum += i;
		std::cout << "tid:" << std::this_thread::get_id()
			<< "end!" << std::endl;

		return sum;
	}

private:
	int begin_;
	int end_;
};

int main()
{
	{
		ThreadPool pool;
		pool.setMode(PoolMode::MODE_CACHED);
		// 开始启动线程池
		pool.start(2);

		// linux上，这些Result对象也是局部对象，要析构的！！！
		Result res1 = pool.submitTask(std::make_shared<MyTask>(1, 100000000));
		Result res2 = pool.submitTask(std::make_shared<MyTask>(100000001, 200000000));
		pool.submitTask(std::make_shared<MyTask>(100000001, 200000000));
		pool.submitTask(std::make_shared<MyTask>(100000001, 200000000));
		pool.submitTask(std::make_shared<MyTask>(100000001, 200000000));

		//uLong sum1 = res1.get().cast_<uLong>();
		//cout << sum1 << endl; 
	} // 这里Result对象也要析构!!! 在vs下，条件变量析构会释放相应资源的

	cout << "main over!" << endl;
	getchar();

}