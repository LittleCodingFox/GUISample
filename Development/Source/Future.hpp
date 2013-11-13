#pragma once

class Future
{
	struct FutureInfo
	{
		unsigned long long StartTime;
		unsigned long Length;
		std::vector<unsigned char> Stream;

		Signal1<std::vector<unsigned char> &> Signal;
	};

	std::vector<FutureInfo> Futures;
public:
	typedef void (*FutureFn)(std::vector<unsigned char> &Arguments);

	static Future Instance;

	void Update();

	void Post(FutureFn Ptr, const std::vector<unsigned char> &Stream = std::vector<unsigned char>());

	void PostDelayed(FutureFn Ptr, unsigned long Delay = 0, const std::vector<unsigned char> &Stream = std::vector<unsigned char>());
	
	template<class Class> void Post(Class *Instance, void (Class::*Function)(std::vector<unsigned char> &Arguments), const std::vector<unsigned char> &Stream = std::vector<unsigned char>())
	{
		FutureInfo Future;
		Future.StartTime = time(NULL);
		Future.Length = 0;
		Future.Stream = Stream;
		Future.Signal.Connect(Instance, Function);

		Futures.push_back(Future);
	};

	template<class Class> void PostDelayed(Class *Instance, void (Class::*Function)(std::vector<unsigned char> &Arguments),
		unsigned long Length, const std::vector<unsigned char> &Stream = std::vector<unsigned char>())
	{
		FutureInfo Future;
		Future.StartTime = time(NULL);
		Future.Length = Length;
		Future.Stream = Stream;
		Future.Signal.Connect(Instance, Function);

		Futures.push_back(Future);
	};
};
