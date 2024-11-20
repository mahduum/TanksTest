#pragma once

#include <functional>
#include <type_traits>
#include <memory>

template<typename>//takes only one parameter of any type
struct FunctionTraits;//forward declaration

// Specialization for function types
template<typename Ret, typename... Args>
struct FunctionTraits<Ret(Args...)> {
	using ReturnType = Ret;
	using ArgumentTypes = std::tuple<Args...>;//I could iterate one by one and check if they conform
	static constexpr auto ArgsIndexSequence() { return std::make_index_sequence<sizeof...(Args)>{}; }
};

template<typename ClassType, typename Ret, typename... Args>
struct FunctionTraits<Ret(ClassType::*)(Args...)> {
	using ReturnType = Ret;
	using ArgumentTypes = std::tuple<Args...>;
	using Class = ClassType;
};

template<typename TObject, typename TFunc, typename TSig>
concept MatchingDelegate = requires (TObject & obj, TFunc func, TSig sig, FunctionTraits<TSig> matchSig, FunctionTraits<TFunc> funcSig, FunctionTraits<TSig>::ArgumentTypes & tup)
{
	requires std::is_same_v<typename FunctionTraits<TSig>::ArgumentTypes, typename FunctionTraits<TFunc>::ArgumentTypes>;
	requires std::is_same_v<typename FunctionTraits<TSig>::ReturnType, typename FunctionTraits<TFunc>::ReturnType>;
};

template<typename CallablePtr/*type of callable object*/, typename... Args>
concept Callable = std::is_invocable<std::remove_pointer_t<CallablePtr>, Args...>::value;//value must be true

//declaration
template<typename TSignature>
class TDelegate;

//definiton and specialization
template<typename InRetValType, typename... ParamTypes>
class TDelegate <InRetValType(ParamTypes...)>
{
	using FuncType = InRetValType(ParamTypes...);

public:
	using RetValType = InRetValType;
	using TFuncType = InRetValType(ParamTypes...);

	template<typename... VarTypes>
	using TFuncPtr = RetValType(*)(ParamTypes..., VarTypes...);//pointer to a function as opposed to just type of a function (would be without *)

	//todo make macro that makes it so it can also be used in create callable
	template<typename UserClass, typename... VarTypes>
	using TMethodPtr = RetValType (UserClass::*)(ParamTypes..., VarTypes...);

	template<typename UserClass, typename... VarTypes>
	using TConstMethodPtr = RetValType(UserClass::*)(ParamTypes..., VarTypes...) const;

	//an instatnce of this delegate will be created in input action delegate with static function call

	//createcallable static
	//this will create delegate instance, a callable from the provided arguments, and returned as an instance 
	//that will be wrapped into InputActionDelegate DelegatePtr, but this created object needs to store what was passed to it
	//this delegate has its own internal allocation whre it stores the callable, maybe to avoid reallocating it?
	template <typename UserClass, typename... VarTypes>
	[[nodiscard]] inline static TDelegate<RetValType(ParamTypes...)> CreateCallable(UserClass* InUserObject,
		typename TDelegate<RetValType(ParamTypes...)>::template TMethodPtr<UserClass, VarTypes.../*use decay_t*/> InFunc, VarTypes... Vars)//how Vars should be copied???
	{
		TDelegate<RetValType(ParamTypes...)> Result;
		//the func that is passed in will, Var types are provided at binding time
		//the simplest is lambda
		auto callable = [&InUserObject, InFunc, &Vars](ParamTypes... params)//todo can I bind it to std::function???
			{
				return (InUserObject.*InFunc)(params..., Vars...);
			};

			//bind to result member 
		return Result;
	
	}


	//create Execute method that will be called internally from input action delegate

	inline RetValType Execute(ParamTypes... Params)//vars are already bound to callable
	{
		//some callable is called with params, the callable must be generic, since we dont know :

	}
private:
	//internal callable with such args:
	//../
};


template<typename TSignature>//todo different version like TEnhancedInputUnifiedDelegate
struct TInputActionDelegate
{
	//add constrain that it must have an execute method with variadic args (may receive no args at all) and IsBound and Unbind
private:
	std::shared_ptr<TSignature> DelegatePtr;//this can be std::function

public:
	bool IsBound() const
	{
		if (DelegatePtr)
		{
			return DelegatePtr.IsBound();
		}
	}

	void Unbind()
	{
		if (DelegatePtr)
		{
			DelegatePtr.Unbind();
		}
	}

	//must create

	template<class TOwnerObj, Callable TCallable>
	void BindDelegate(TOwnerObj owmerObj, TCallable callable)//need to pass owner for specific data
	{
		//std::bind some args can be passed upon binding, some upon calling
		//this is not callable, this must be bound before it is passed here
		Unbind();
		//todo needs to create a callable object with owner, function and args...
		//and it is assigned to internal delegate ptr or to method pointer
		//we have speciliazed methods BindDelegate:
		//1. Creates pointer to a created callable object
		//2. Binds lambda - functor and args
		//3. Binds function by name with object

		//PROBLEMS: WHAT SHOULD HAVE TSignature, and what Ret(Args...) etc.?
		/*DelegateSignatureImpl has defined ParamTypes... and templates Tfuncptr, TmethodObjPtr
		template has static methods based on the arguments passed in and returns always TDelegate<Ret(Param)>*/

		DelegatePtr = std::make_shared<TSignature>(callable);
	}
};

/*
* NOTE: we have a signature, just out and in types and we are using its internal template to provide method pointer:
* void BindDelegate(UserClass* Object, typename TSig::template TMethodPtr<UserClass, TVars...> Func, TVars... Vars)
	{
		Unbind();
		Delegate = MakeShared<TSig>(TSig::CreateUObject(Object, Func, Vars...));
	}
NOTE: ParamTypes... are regular parameters and VarTypes... are template parameters
  template <typename UserClass, typename... VarTypes> using TMethodPtr      = typename TMemFunPtrType<false, UserClass, RetValType(ParamTypes..., VarTypes...)>::Type;
  
  regular parameters:
  class TDelegate<InRetValType(ParamTypes...), UserPolicy>

  here:
  `
  #define DEFINE_BIND_ACTION(HANDLER_SIG)																																			\
	template<class UserClass, typename... VarTypes>																																					\
	FEnhancedInputActionEventBinding& BindAction(const UInputAction* Action, ETriggerEvent TriggerEvent, UserClass* Object, typename HANDLER_SIG::template TMethodPtr< UserClass, VarTypes... > Func, VarTypes... Vars) \
	{																																											\
		TUniquePtr<FEnhancedInputActionEventDelegateBinding<HANDLER_SIG>> AB = MakeUnique<FEnhancedInputActionEventDelegateBinding<HANDLER_SIG>>(Action, TriggerEvent);			\
		AB->Delegate.BindDelegate<UserClass>(Object, Func, Vars...);																														\
		AB->Delegate.SetShouldFireWithEditorScriptGuard(bShouldFireDelegatesInEditor);																							\
		return *EnhancedActionEventBindings.Add_GetRef(MoveTemp(AB));																											\
	}`
`Vars...` are template additional args for what is being bound
while the HANDLER_SIG is for example: `&ThisClass::Input_AbilityInputTagPressed` as PressedFunc in:
	`BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, PressedFunc, Action.InputTag);`
while Action.InputTag is `Vars...`, the PressedFunc here is given a templated type: `typename HANDLER_SIG::template TMethodPtr< UserClass, VarTypes... >`
and the HANDLER_SIG is any signature defined for the current bind actions like:
	`DEFINE_BIND_ACTION(FEnhancedInputActionHandlerSignature);
	DEFINE_BIND_ACTION(FEnhancedInputActionHandlerValueSignature);`
	given declarations:
	`
	DECLARE_DELEGATE(FEnhancedInputActionHandlerSignature);
	DECLARE_DELEGATE_OneParam(FEnhancedInputActionHandlerValueSignature, const FInputActionValue&); ---> `FInputActionValue&` is `ParamTypes...` regular arguments 
proper BindAction will be called based on the signature; `typename HANDLER_SIG::template TMethodPtr< UserClass, VarTypes... >` which is established based on our input `Func`
which in the example case is that the `FEnhancedInputActionHandlerSignature` was declared with no `ParamTypes` so it will match, because it will correctly fill in only the `VarTypes...` from the 
BindAction call: `BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, PressedFunc, Action.InputTag)` where `Action.InputTag` is substituted for `VarTypes...`
In the case of the second signature `FEnhancedInputActionHandlerValueSignature` is the oposite, `ParamTypes...` for the method like `void ULyraHeroComponent::Input_Move(const FInputActionValue& InputActionValue)`
will match as the type defined in HANDLER_SIG will complement: 
	`template <typename UserClass, typename... VarTypes> using TMethodPtr      = typename TMemFunPtrType<false, UserClass, RetValType(ParamTypes..., VarTypes...)>::Type;`
 because the VarTypes are not passed to BindAction and the type of `ParamTypes` will match what is passed as argument: `BindAction(IA, TriggerEvent, Object, Func);`
....
and Signature delegate has matching ParamTypes:
	`
	template<>
	inline void FEnhancedInputActionEventDelegateBinding<FEnhancedInputActionHandlerValueSignature>::Execute(const FInputActionInstance& ActionData) const
	{
		Delegate.Execute(ActionData.GetValue());
	}`
*/

template<typename RetVal, typename... Args>
struct TInputActionDelegate<RetVal(Args...)>//specialization
{
public:

	template<typename Callable>
	requires std::is_same_v<RetVal, std::invoke_result_t<Callable&, Args...>>
	void Bind(Callable&& callable)
	{
		m_Function = std::function<RetVal(Args...)>(std::forward<Callable>(callable));
	}

	RetVal Invoke(Args... args) const
	{
		if (m_Function)
		{
			return m_Function(std::forward<Args>(args)...);
		}
		else
		{
			throw std::bad_function_call();
		}
	}

	bool IsBound() { return m_Function.target() != nullptr; }

private:
	std::function<RetVal(Args...)> m_Function;
};

//todo define Execute like in: TEnhancedInputUnifiedDelegate
/*
#include <iostream>
#include <vector>
#include <concepts>
#include <cstdint>
#include <type_traits>
#include <typeinfo>
#include <typeindex>
#include <string>
#include <unordered_map>
#include <memory>
#include <bitset>
#include <array>
#include <format>
#include <cstdio>
#include <functional>
#include <cmath>

using namespace std::placeholders;

//template<typename CallablePtr>
//concept Callable = std::is_invocable_r_v<void, CallablePtr>;//value must be true

//template<typename TSignature>
//concept Executable = requires void(*)(Args)

template<typename TSignature>//todo different version like TEnhancedInputUnifiedDelegate
struct TDelegate
{
	//add constrain that it must have an execute method with variadic args (may receive no args at all) and IsBound and Unbind
private:
	std::shared_ptr<TSignature> CallablePtr;//this can be std::function

public:
	void BindDelegate(TSignature callable)
	{
		//tsignature must be a type of callable for example std::function<void(int)>;
		CallablePtr = std::make_shared<TSignature>(callable);
	}

	template<class TOwnerObj>
	void BindDelegate(void (TOwnerObj::*func)(int), TOwnerObj& ownerObj)//todo ensure that object func has the same raw signature as TSignature, EXTRACT THE ESSENCE, THAT IS THE OBJECT IS CALLABLE WITH SAME ARGUMENTS AND RETURNS THE SAME THING
	{
		// Bind the member function pointer to the instance
		auto boundFunc = std::bind(func, &ownerObj, std::placeholders::_1);
		// Convert to std::function and store in std::shared_ptr
		TSignature s = boundFunc;
		CallablePtr = make_shared<TSignature>(s);
	}

	//todo bind with owner calling object

	template<typename... TArgs>//signature specifies how many and what
	void Execute(TArgs... args) const//execute is specified elsewhere for the concrete tsignature of the delegate binding with this delegate
	//what these args are will be defined by the binding class implementation of the execute method which internally will call Delegate->Execute
	{
		//todo call with args, specify that this delegate must have execute method
		(*CallablePtr)(args ...);//no member Execute, this is in Unreal delegate type
	}
};

template<typename TSignature, typename... TArgs>
class X
{

	public:

//make two, one takes object, other one ready delegate
	//template<std::invocable Invocable>
	void BindDelegate(TSignature invocable)
	{
		Delegate.BindDelegate(invocable);
	};

	//we can have different classed with different execute method signatures
	void Execute(TArgs... args);

	TDelegate<TSignature> Delegate;
};

class Y
{
	int _num = 3;
public:
	void PrintNum(int a)
	{
		std::cout << "Num is: " << (_num + a) << std::endl;
	}
};

//using Delegate = std::function<void(int)>;
using OtherDelegate = std::function<void(int)>;//void (Y::*)(int);

template<>
inline void X<OtherDelegate, int, int>::Execute(int base, int power)
{
	Delegate.Execute(std::pow(base, power));
}

int main()
{
	Y y;
	X<OtherDelegate, int, int> x;

	x.Delegate.BindDelegate(&Y::PrintNum, y);
	x.Execute(6, 7);
	return 0;
}

#include <type_traits>
#include <utility>

// Helper to deduce member function pointer type
template <typename T>
struct member_function_pointer_traits;

// Specialization for member functions
template <typename R, typename C, typename... Args>
struct member_function_pointer_traits<R(C::*)(Args...)> {
	using class_type = C;
	using signature_type = R(Args...);
};

// Specialization for const member functions
template <typename R, typename C, typename... Args>
struct member_function_pointer_traits<R(C::*)(Args...) const> {
	using class_type = C;
	using signature_type = R(Args...) const;
};

template <typename TSignature>
concept TObject = requires {
	typename member_function_pointer_traits<TSignature>::class_type;
	typename member_function_pointer_traits<TSignature>::signature_type;
};

// Example usage
struct Example {
	void foo() {}
};

int main() {
	static_assert(TObject<void(Example::*)()>); // Checks if Example has a member function with the specified signature
	static_assert(!TObject<void(Example::*)() const>); // Example does not have a const-qualified member function
}
*/
