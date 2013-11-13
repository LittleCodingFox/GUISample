#include <map>
#include <vector>
#include <SFML/System.hpp>
#include "StringID.hpp"
#include "SuperSmartPointer.hpp"
#include "Signal.h"
#include "Delegate.h"
using namespace Gallant;
#include "Future.hpp"

Future Future::Instance;

void Future::Update()
{
	bool Found = true;

	while(Found)
	{
		Found = false;

		for(size_t i = Futures.size() - 1; i >= 0; i--)
		{
			if(time(NULL) - Futures[i].StartTime >= Futures[i].Length)
			{
				Futures[i].Signal(Futures[i].Stream);

				Futures.erase(Futures.begin() + i);

				Found = true;

				break;
			};
		};
	};
};

void Future::Post(FutureFn Ptr, const std::vector<unsigned char> &Stream)
{
	FutureInfo Future;
	Future.StartTime = time(NULL);
	Future.Length = 0;
	Future.Stream = Stream;
	Future.Signal.Connect(Ptr);

	Futures.push_back(Future);
};

void Future::PostDelayed(FutureFn Ptr, unsigned long Length, const std::vector<unsigned char> &Stream)
{
	FutureInfo Future;
	Future.StartTime = time(NULL);
	Future.Length = Length;
	Future.Stream = Stream;
	Future.Signal.Connect(Ptr);

	Futures.push_back(Future);
};
