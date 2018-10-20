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

