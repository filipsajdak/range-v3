# What is range-v3 library

## What is Range

Range are an abstraction over iterators. `rng` is an range if it has defined `begin(rng)` and `end(rng)`. In general whatever you can use in range-for is an range.
```c++
// Whatever you can use in place of rng is a Range

for(auto&& e : rng)
{
    // use of e
}
```
### Example of the ranges
```c++
int carr[] = { 1, 2, 3, 4, 5 };           // c-array is a range

auto init_list = { 11, 22, 33, 44, 55 };  // initializer_list is a range

vector vec = { 12, 23, 34, 56, 67 };      // containers are ranges

string str = "This is also a range";      // string is a range

string_view str_view = str;               // string_view is a range
```

## What is a Container

Conteiner is an collection of objects which is expensive to copy `O(n)`. It is also something which owns the objects.

## What is a View

View is cheap to copy `O(1)`. It is usually pair of iterators (wrapped if needed in some adaptors) which allows you travers over the collection. 

**Note:** because of View is using iterators over collection it can be only created on conteiners which are lvalue (not temporary containers).

## What is a SizedRange

SizedRange is a Range which has defined `size(rng)`. Ie. it cannot be stream or infinite range.


# How you can learn it?

* https://ericniebler.github.io/range-v3/
* GitHub
  * http://github.com/ericniebler/range-v3/tree/master/test
  * http://github.com/ericniebler/range-v3/tree/master/example
* http://ericniebler.com/
* https://www.youtube.com/results?search_query=eric+niebler+ranges
* various blogs

Example of code with range-v3 library
```c++
#include <range/v3/all.hpp>
#include <vector>
#include <iostream>

using namespace ranges;

int main() {
    auto v = std::vector{1,7,2,4,1,7,4,6,0,1};

    action::sort(v);

    std::cout << view::all(v) << std::endl;
}

// output: [0,1,1,1,2,4,4,6,7,7]
```

You can check how this code work online [here](https://wandbox.org/permlink/KTEuRCrj0RiIHLEI).

The easiest way to play with the library is to use one of the online compilers which has support for range-v3 library:
* https://gcc.godbolt.org/
* https://wandbox.org/
* http://coliru.stacked-crooked.com/

# Compiler support

I have checked latest version of library and it seems to compile in:
* clang >= 3.6
* gcc >= 4.9.4

Visual Studio 2017 is not able to compile it due to `Internal Error`.

You can check that on [godbolt](https://gcc.godbolt.org/z/XkdbRv).

# What are the benefits?

You may wander what is a big deal about range-v3 library.

In short instead of 
```c++ 
auto v = std::vector{1,7,2,4,1,7,4,6,0,1};

std::sort(v.begin(), v.end());
```
you can write:
```c++
auto v = std::vector{1,7,2,4,1,7,4,6,0,1};

action::sort(v);
```
So... it is only to be able to pass containers instead of two iterators to it?

No there is something much bigger then that - it all about composability of algorithms.

Let's take a closer look how composition of two algorithms `sort` and `unique` is done in current standard library
```c++
auto v = std::vector{1,7,2,4,1,7,4,6,0,1};

std::sort(v.begin(), v.end());

auto it = std::unique(v.begin(), v.end());

v.erase(it, v.end());

std::cout << view::all(v) << std::endl;
```

and the same code rewritten using range-v3
```c++
auto v = std::vector{1,7,2,4,1,7,4,6,0,1};

action::unique(action::sort(v));

std::cout << view::all(v) << std::endl;
```

This is much cleaner. As you can see we can compose range actions in a way 
```c++
action1(action2(action3(action4(... actionN(rng)))))

