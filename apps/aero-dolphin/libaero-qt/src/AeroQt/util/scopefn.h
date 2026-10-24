#pragma once

/**
 * These are syntactic sugar, intended to make code more readable.
 * The idea comes from Kotlin; this is what is being copied:
 *
 * https://kotlinlang.org/docs/scope-functions.html
 *
 * The C++ syntax is:
 *
 * FooObject() + also {
 *	   it.objectMethod();
 * };
 *
 * instead of:
 *
 * auto obj = FooObject();
 * obj.objectMethod();
 *
 */

/* Also */

template <typename Fn>
struct AlsoType {
	Fn closure;
};

struct AlsoTag {};
inline constexpr AlsoTag alsoTag{};

template <typename Fn>
AlsoType<Fn> operator->*(AlsoTag, Fn cl) { return { cl }; }

// object-like macro: no parameter list, so no invocation parens required
#define also alsoTag->*[&](auto& it)

template <typename Type, typename Fn>
Type operator+ (Type a, AlsoType<Fn> b)
{
	b.closure(a);
	return a;
}

/* Let */

template <typename Fn>
struct LetType {
	Fn closure;
};

struct LetTag {};
inline constexpr LetTag letTag{};

template <typename Fn>
LetType<Fn> operator->*(LetTag, Fn cl) { return { cl }; }

// object-like macro: no parameter list, so no invocation parens required
#define let letTag->*[&](auto& it)

template <typename Type, typename Fn>
auto operator+ (Type a, LetType<Fn> b) -> std::invoke_result_t<Fn, Type&>
{
	return b.closure(a);
}

/* Apply */

#include <type_traits>

#define APPLY(obj, ...)
// [&](auto& obj) {											 \
//     using Type = std::remove_reference_t<decltype(obj)>;    \
//     struct Local : public Type {								  \
//             void run() __VA_ARGS__						  \
//     };													   \
//         static_assert(sizeof(Local) == sizeof(Type),		   \
//                       "Local must add no members/vtable");	   \
//         reinterpret_cast<Local&>(obj).run();		\
// }(obj);

/*
template<typename Fn>
struct ApplyType {
	Fn closure;
};
template <typename Fn>
ApplyType<Fn> applyFn(Fn cl)
{
	return { cl };
}

/// This relies on undefined behavior -- mostly since the object behind the Local& pointer still has a Type vtable -- and will crash if you do any fancy diamond inheritance stuff or rtti stuff on Local

#define APPLY(TYPE, obj, ...)								 \
applyFn([&]<typename Type>(Type& obj) {											 \
	struct Local : Type {								  \
		void run() __VA_ARGS__						  \
	};													   \
	static_assert(sizeof(Local) == sizeof(Type),		   \
				  "Local must add no members/vtable");	   \
	reinterpret_cast<Local&>(obj).run(), obj;		\
})

// #define APPLY(...)								 \
// applyFn([&]<typename Type>(Type& obj) {											\
//		   struct Local : Type {								 \
//			   void run() { __VA_ARGS__ }						 \
//	   };													  \
//		   static_assert(sizeof(Local) == sizeof(Type),			  \
//						 "Local must add no members/vtable");	  \
//		   reinterpret_cast<Local&>(obj).run(), obj;	   \
// })


template <typename Type, typename Fn>
Type operator+ (Type a, ApplyType<Fn> b)
{
	b.closure.template operator()<Type>(a);
	return a;
}
*/
