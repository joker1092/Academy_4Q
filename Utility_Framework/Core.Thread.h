#pragma once
#include <condition_variable>
#include <future>
#include <functional>
#include <queue>
#include <thread>
#include <vector>
#include <memory>
#include <mutex>
#include "ClassProperty.h"

namespace Thread
{
	// Task Ŭ���� ����
	template<typename ResultType>
	class Task
	{
	public:
		explicit Task(std::shared_ptr<std::packaged_task<ResultType()>> task)
			: mTask(std::move(task)), mFuture(mTask->get_future()) {
		}

		// then �޼��� ����
		template<typename NextFunc>
		auto then(NextFunc&& nextFunc) -> Task<decltype(nextFunc(std::declval<ResultType>()))>
		{
			using NextResultType = decltype(nextFunc(std::declval<ResultType>()));

			// ���� �۾� ���� : ���� �۾��� ����� �޾� ���� �۾��� Chain �������� ����
			auto nextTask = std::make_shared<std::packaged_task<NextResultType()>>(
				[prevFuture = std::move(mFuture), nextFunc = std::forward<NextFunc>(nextFunc)]() mutable
				{
					return nextFunc(prevFuture.get());
				});

			return Task<NextResultType>(nextTask);
		}

		// ��� Future ��ȯ
		std::future<ResultType> get_future()
		{
			return mTask->get_future();
		}

		// �۾� ����
		void execute()
		{
			(*mTask)();
		}

	private:
		std::shared_ptr<std::packaged_task<ResultType()>> mTask;
		std::future<ResultType> mFuture;
	};

	// ThreadPool Ŭ���� ����(�̱���) : �ܺλ��� �Ұ�, ������ �� �Ҹ��� private
	class ThreadPool final : public Singleton<ThreadPool>
	{
	private:
		friend class Singleton;
		template<typename T>
		friend auto CreateTask(T task) -> Task<decltype(task())>;

		explicit ThreadPool(size_t numThreads = std::thread::hardware_concurrency() - 2);
		~ThreadPool() override;

		template<typename T>
		auto AddTask(T task) -> Task<decltype(task())>;
		void WorkerLoop();

	private:
		std::vector<std::thread> mThreads;
		std::condition_variable mSignal;
		std::mutex mMutex;
		bool mStopThreads = false;

		std::queue<std::function<void()>> mTaskQueue;
	};
	// ThreadPool ���� �� �۾� ���� �Լ�
	template<typename T>
	inline auto CreateTask(T task) -> Task<decltype(task())>
	{
		return ThreadPool::GetInstance()->AddTask(std::move(task));
	}
}
