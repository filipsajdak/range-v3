# What is the range-v3 library

## What is a range

A range is an abstraction over iterators. `rng` is a range if it has defined `begin(rng)` and `end(rng)`. 


### Example of the ranges
```c++
int carr[] = { 1, 2, 3, 4, 5 };           // c-array is a range

auto init_list = { 11, 22, 33, 44, 55 };  // initializer_list is a range

vector vec = { 12, 23, 34, 56, 67 };      // containers are ranges

string str = "This is also a range";      // string is a range

string_view str_view = str;               // string_view is a range
```

In general whatever you can use in range-for is a range.
```c++
// Whatever you can use in place of rng is a Range

for(auto&& e : rng)
{
    // use of e
}
```

### Range Concepts

| Concept      | Attributes               | Description                                                                                                                                                     |
| ------------ | ------------------------ | -------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Container    | `O(n)` copy              | collection of objects; has ownership over the objects                                                                                                           |
| View         | `O(1)` copy              | can be treated as pair of iterators (wrapped if needed in some adapters) which allows to traverse over the collection; **Note:** collection has to be lvalue    |
| SizedRange   | `size(rng)` is defined   | range with defined size; i.e. it cannot be a stream or an infinite range                                                                                        |
# How can you learn it?

* https://ericniebler.github.io/range-v3/
* GitHub
  * http://github.com/ericniebler/range-v3/tree/master/test
  * http://github.com/ericniebler/range-v3/tree/master/example
* http://ericniebler.com/
* https://www.youtube.com/results?search_query=eric+niebler+ranges
* various blogs

# Compiler support

I have checked the latest version of the library, and it seems to compile in:
* clang >= 3.6
* gcc >= 4.9.4

Visual Studio 2017 is not able to compile it due to `Internal Error`.

You can check that on [godbolt](https://gcc.godbolt.org/z/XkdbRv).

The easiest way to play with the library is to use one of the online compilers which have support for range-v3 library:
* https://gcc.godbolt.org/
* https://wandbox.org/
* http://coliru.stacked-crooked.com/

Example of code with the range-v3 library
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


# What are the benefits?

You may wonder what a big deal about the range-v3 library.

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

No there is something much bigger than that - it all about composability of algorithms.

## Getting unique values

Let's take a closer look at at at at at at at at at how the composition of two algorithms `sort` and `unique` is done in the current standard library
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

There are other ways you can compose actions together (in table below we assume that `auto v = std::vector{1,7,2,4,1,7,4,6,0,1};`).

| Style              | Code                                                         |
| ------------------ | ------------------------------------------------------------ |
| As functions       | `unique(sort(v))`                                            |
| Pipes (lvalue)     | `v | = sort | unique`                                        |
| Pipes (rvalue)     | `v = std::move(v) | sort | unique;`                          |
|                    | `v = v | move | sort | unique;`                              |
| Pipes (initialize) | `auto v = std::vector{1,7,2,4,1,7,4,6,0,1} | sort | unique;` |

Based on that we can rewrite this 
```c++
auto v = std::vector{1,7,2,4,1,7,4,6,0,1};

std::sort(v.begin(), v.end());

auto it = std::unique(v.begin(), v.end());

v.erase(it, v.end());

std::cout << view::all(v) << std::endl;
```
into this
```c++
auto v = std::vector{1,7,2,4,1,7,4,6,0,1} | action::sort 
                                          | action::unique;
std::cout << view::all(v) << std::endl;
```

## Adding accumulated values to an existing container

Let assume that you have a vector with some values in it (which you want to preserve). The vector contains a sum of unique values (which you get from `get_data()` function. Using the standard library, it looks like that (I introduce additional scope with `{}` to keep variables only as long as they needed, but not longer)

```c++
std::vector<int> v = {123, 321};

{
    auto tmp = get_data();

    std::sort(tmp.begin(), tmp.end());uto it = std::unique(tmp.begin(), tmp.end());

    v.push_back(std::accumulate(tmp.begin(), it, 0));
}

std::cout << view::all(v) << std::endl;
```

Using range-v3 library the same code can look like 

```c++
std::vector<int> v = {123, 321};

v |=  action::push_back(accumulate(get_data() | action::sort | action::unique, 0));

std::cout << view::all(v) << std::endl;
```

# Views
The power of range-v3 library are not actions but views. Views are the simple wrapper over iterators which can filter or transform elements of the range.

## Filtering

Assuming that you have some vector which you want to filter in the standard library, you can do it like that:
```c++
const auto v = std::vector{1,7,2,4,1,7,4,6,0,1};

std::vector<int> tmp;
std::copy_if(v.begin(), v.end(), 
             std::back_inserter(tmp), 
             [](auto i){ return i % 2 == 0; });

std::cout << view::all(tmp) << std::endl;
```

Because we deal with lvalue container, we can build a view around it which filters elements that we are not interested in

```c++
const auto v = std::vector{1,7,2,4,1,7,4,6,0,1};

auto res = v | view::filter([](auto i){
    return i % 2 == 0; 
});

std::cout << res << std::endl;
```

## Summing the filtered range

Let now assume that you need to sum up of all your filtered values
```c++
const auto v = std::vector{1,7,2,4,1,7,4,6,0,1};

std::vector<int> tmp;
std::copy_if(v.begin(), v.end(), 
             std::back_inserter(tmp), 
             [](auto i){ return i % 2 == 0; });

std::cout << std::accumulate(tmp.begin(), tmp.end(), 0) 
          << std::endl;
```

range version

```c++
const auto v = std::vector{1,7,2,4,1,7,4,6,0,1};

auto res = accumulate(v | view::filter([](auto i){
    return i % 2 == 0; 
}), 0);

std::cout << res << std::endl;
```

## Summing square of values of filtered range

```c++
const auto v = std::vector{1,7,2,4,1,7,4,6,0,1};

std::vector<int> tmp;

std::copy_if(v.begin(), v.end(), 
             std::back_inserter(tmp), 
             [](auto i){ return i % 2 == 0; });

std::transform(tmp.begin(), tmp.end(), tmp.begin(), [](auto i) {
    return i*i;
});

std::cout << std::accumulate(tmp.begin(), tmp.end(), 0) 
          << std::endl;
```

range version

```c++
const auto v = std::vector{1,7,2,4,1,7,4,6,0,1};

auto r = accumulate(v | view::filter([](auto i){return i % 2 == 0; }) 
                      | view::transform([](auto i){ return i*i; }), 
                    0);

std::cout << r << std::endl;
```

## Making pipes look prettier

Using full names of filters or transforms (especially when a pipe is long) can easily lead to not readable code i.e.
```c++
auto r = accumulate(v | view::filter([](auto i){return i % 2 == 0; }) 
                      | view::transform([](auto i){ return i*i; }), 
                    0);
```
If we would add more and more pipe stages, it might become tough to read and understand what is this code doing. To make things better, we can use the fact that `view::filter` and `view::transform` (in general all views) returns range object which can be assigned to the variable. 

```c++
const auto evens = view::filter([](auto i){return i % 2 == 0; });
const auto squared = view::transform([](auto i){ return i*i; });

auto r = accumulate(v | evens | squared, 0);
```

So the line with the pipe itself becomes very easy to read - if you have common views in the code, you can gain in having them created once in some namespace and reused in every place in your code. That makes your range pipes easy to read and reason about.

## What is a type of composed range

You may wonder what a type of such composed range is - this is a very straightforward type which is created by the composition of wrapped types of ranges. Above view `v | evens | squared` looks like this:

```
ranges::v3::transform_view<

  ranges::v3::remove_if_view< // this is filter

    ranges::v3::iterator_range< // range from vector
      std::__1::__wrap_iter<const int *>, 
      std::__1::__wrap_iter<const int *> 
    >, 

    ranges::v3::logical_negate_< // negate for remove_if purpose

      (lambda at prog.cc:29:41) // [](auto i){return i % 2 == 0; }

    > 
  >, 

  (lambda at prog.cc:30:46) // [](auto i){ return i*i; }
>
```

## Views are lazy

You have to remember that creation of a view is cheap 
