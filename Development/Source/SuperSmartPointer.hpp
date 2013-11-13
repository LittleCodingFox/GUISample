#pragma once
/*!
	Smart Pointer implementation using Observer Pattern and implicit Destruction
*/
template<typename type>
class SuperSmartPointer
{
private:
	template<typename OutType>
	friend class SuperSmartPointer;

	class SuperSmartPointerContainer
	{
	public:
		friend class SuperSmartPointer<type>;
		typedef std::vector<SuperSmartPointer *> ObserverList;
		ObserverList Observers;
		type *Object;

		SuperSmartPointerContainer() : Object(NULL)
		{
		};

		void Add(SuperSmartPointer *Observer)
		{
			Observers.push_back(Observer);
		};

		void Remove(SuperSmartPointer *Observer)
		{
			if(Observers.size())
			{
				for(typename ObserverList::iterator it = Observers.begin(); it != Observers.end(); it++)
				{
					if(*it == Observer)
					{
						Observer->ContainerDisposed();

						it = Observers.erase(it);

						if(Observers.size() == 0)
						{
							if(Object)
							{
								delete Object;
								Object = NULL;
							};

							delete this;
						};

						break;
					};
				};
			};
		};

		void Dispose()
		{
			while(Observers.size())
			{
				(*Observers.begin())->ContainerDisposed();
				Observers.erase(Observers.begin());
			};

			if(Object)
			{
				delete Object;
				Object = NULL;
			};

			delete this;
		};

		void Replace(type *NewObject, bool Destroy)
		{
			if(Object && Destroy)
			{
				delete Object;
			};

			Object = NewObject;
		};
	};

	SuperSmartPointerContainer *Content;

	void ContainerDisposed()
	{
		Content = NULL;
	};
public:
	SuperSmartPointer() : Content(NULL) {};
	explicit SuperSmartPointer(type *Object) : Content(NULL)
	{
		if(Object)
		{
			Content = new SuperSmartPointerContainer();
			Content->Object = Object;
			Content->Add(this);
		};
	};

	SuperSmartPointer<type>(const SuperSmartPointer<type> &o) : Content(NULL)
	{
		*this = o;
	};

	~SuperSmartPointer()
	{
		if(Content)
			Content->Remove(this);

		Content = NULL;
	};

	size_t ObserverCount()
	{
		return Content == NULL ? 0 : Content->Observers.size();
	};

	SuperSmartPointer<type> &operator=(const SuperSmartPointer<type> &o)
	{
		if(Content == o.Content || this == &o)
			return *this;

		if(Content) {
			Content->Remove(this);
			Content = NULL;
		};

		if(!o.Content)
			return *this;

		Content = o.Content;
		Content->Add(this);

		return *this;
	};

	template<typename OutType>
	operator SuperSmartPointer<OutType>()
	{
		if(!Content)
			return SuperSmartPointer<OutType>();

		SuperSmartPointer<OutType> Out;
		Out.Content = reinterpret_cast<typename SuperSmartPointer<OutType>::SuperSmartPointerContainer *>(Content);
		Out.Content->Add(&Out);

		return Out;
	}

	/*!
		returns a Derived object from the base object contained within
	*/
	template<typename OutType> OutType *AsDerived()
	{
		type *t = Get();

#if CHECK_SSP_CONVERSIONS
		return dynamic_cast<OutType *>(t);
#endif

		return static_cast<OutType *>(t);
	};

	inline const type& operator*() const
	{
		return *Get();
	};

	inline type& operator*()
	{
		return *Get();
	};

	inline operator bool() const
	{
		return Get() != 0;
	};

	inline const type *operator->() const
	{
		return Get();
	};

	inline type *operator->()
	{
		return Get();
	};

	inline const type *Get() const
	{
		return Content ? Content->Object : NULL;
	};

	inline type *Get()
	{
		return Content ? Content->Object : NULL;
	};

	bool operator==(const SuperSmartPointer<type> &o)
	{
		return &o == this || o.Get() == Get();
	};

	bool operator!=(const SuperSmartPointer<type> &o)
	{
		return !(&o == this || o.Get() == Get());
	};

	operator bool()
	{
		return !!Get();
	};

	operator type*()
	{
		return Get();
	};

	void Reset(type *Object)
	{
		if(Content)
		{
			Content->Remove(this);
			Content = NULL;
		};

		if(Object)
		{
			Content = new SuperSmartPointerContainer();
			Content->Object = Object;
			Content->Add(this);
		};
	};

	void Replace(type *Object, bool Destroy = true)
	{
		if(Content)
		{
			Content->Replace(Object, Destroy);
		}
		else
		{
			Content = new SuperSmartPointerContainer();
			Content->Object = Object;
			Content->Add(this);
		};
	};

	/*!
		Destroys the contained object
	*/
	void Dispose()
	{
		if(Content)
		{
			Content->Dispose();
		};
	};
};
