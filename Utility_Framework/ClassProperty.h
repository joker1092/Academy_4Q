#pragma once

class Nonallocatable
{
protected:
	Nonallocatable() = default;
	~Nonallocatable() = default;

protected:
	void* operator new(size_t) = delete;
	void* operator new[](size_t) = delete;
	void operator delete(void*) = delete;
	void operator delete[](void*) = delete;
};

class Noncopyable
{
protected:
	Noncopyable() = default;
	~Noncopyable() = default;

protected:
	Noncopyable(const Noncopyable&) = delete;
	Noncopyable(Noncopyable&&) = delete;
	Noncopyable& operator=(const Noncopyable&) = delete;
	Noncopyable& operator=(Noncopyable&&) = delete;
};

template <typename T>
class Singleton : public Noncopyable, public Nonallocatable
{
protected:
	Singleton() = default;
	~Singleton() = default;

public:
	inline static T& GetInstance()
	{
		static T instance;
		return instance;
	}

private:
};