#pragma once

#include <functional>
#include <type_traits>

//template<typename Ret, typename... Args, typename Callable>
//concept InvocableWithReturnType =
//std::invocable<Callable, Args...>&&
//std::convertible_to<std::invoke_result_t<Callable, Args...>, Ret>;

template<typename TSignature>//todo different version like TEnhancedInputUnifiedDelegate
struct TDelegate {};


//template<typename RetVal, typename... Args>
//struct TDelegate<RetVal(Args...)>
template<typename RetVal, typename... Args>
struct TDelegate<RetVal(Args...)>
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
