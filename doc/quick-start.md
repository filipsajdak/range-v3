

# Range-v3 quickstart

This documentation was created based on presentation https://slides.com/filipsajdak-1/range-v3-how-to-start 

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



| Style              | Code                                                           |
| ------------------ | ------------------------------------------------------------   |
| As functions       | `unique(sort(v))`                                              |
| Pipes (lvalue)     | `v \|= sort \| unique`                                         |
| Pipes (rvalue)     | `v = std::move(v) \| sort \| unique;`                          |
|                    | `v = v \| move \| sort \| unique;`                             |
| Pipes (initialize) | `auto v = std::vector{1,7,2,4,1,7,4,6,0,1} \| sort \| unique;` |

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

```c++
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

You have to remember that creation of a view is cheap - you can considered it as copying two iterators. You have to be aware of that when you want to compare performance of your code.

I.e. let's assume that we need a function which returns integers which are powers of two not greater then the maximum value which we set by passing an argument to this function. Currently we will end up with (more or less):
```c++
auto v1::pow2(int max) {
    std::vector<int> res;
    for (int i = 1; i<max; i*=2)
        res.push_back(i);
    return res;
}
```
Using range-v3 we can come up with
```c++
auto v2::pow2(int max) {
    return view::generate([i=1]() mutable {
	const int res = i; 
	i*=2; 
	return res;
    }) | view::take_while([max](auto i){ return i<max; });
}
```

Both implementations will give you the same results if you use them in range-for loop
```c++
for(auto e : pow2(100))
    std::cout << e << " ";
cout << std::endl;
```

But when you compare execution of function
```c++
auto powers = pow2(100);
```

You will see that range version is extreamly fast compared to standard version. The reason is that range version is not creating any container and is not doing any work at all. 

**Note:** Views are lazy - no work is done until you use it.

If you don't need container (but only values) range version should work better for you.

## More complicated examples

### Zip, Cartesian product &

**Note:** Example proposed by Bartosz Duszel <bartosz.duszel@siili.com>

Assuming that you have list of operationas and two lists of arguments we would like to make all operations on each pair of arguments (one taken from one list and second from other respectively).
```c++
/* pseudo code - thanks to Bartosz Duszel (lisp hero)

vector ops = {+, -, *}
vector in1 = {1, 2, 3, 4, 5}
vector in2 = {6, 7, 8, 9, 10}

vector res = {1 + 6, 2 + 7, ..., 1 - 6, 2 - 7, ..., 1 * 6, 2 * 7, ...} 
*/

std::function<int(int,int)> ops[] = { std::plus(), std::minus(), 
                                      std::multiplies()};   
auto i1 = {1, 2, 3, 4, 5};
auto i2 = {6, 7, 8, 9, 10};    
				  
auto r = cartesian_product(ops, zip(i1, i2)) | transform( [](auto&& e){ 
    auto&& [op, args] = e;
    return std::apply(op, args);
});
					      
std::cout << r << std::endl;

// Output: [7,9,11,13,15,-5,-5,-5,-5,-5,6,14,24,36,50]
```

### Strings

You should be aware that strings are also ranges so if you have range of strings and i.e. you want to print them you may be suprised on what you will get.

```c++
const auto csv = "11,12,13\n21,22,23";
    
auto res = view::c_str(csv) | view::split('\n') | view::transform([](auto&& line) {
    return line | view::split(',');
});
					     
std::cout << res << std::endl;

// Output: [[[1,1],[1,2],[1,3]],[[2,1],[2,2],[2,3]]]
```

You might be even more suprised when you will display string with colon (`,`)
```c++
const auto csv = "11,12,13\n21,22,23";
std::cout << view::c_str(csv) << std::endl;

/* Output: [1,1,,,1,2,,,1,3,
           ,2,1,,,2,2,,,2,3] */
```

If you are suprised because of triple colons please remember that ',' is also a 'char' so when you see '2,,,1' it means that between '2' and '1' there is a ','.

## Lazy Views Features

### Infinite ranges

Because ranges are lazy (they don't evalueate its values at the moment of creation) they can be infinite i.e.:
```c++
uto&& e : view::ints(0))
    std::cout << e << " ";

    /* Output: 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 
    18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 
    36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 
    54 55 56 57 58 59 60 61 62 63 64 65 66 67 68 69 70 71 
    72 73 74 75 76 77 78 79 80 81 82 83 84 85 86 87 88 89 
    90 91 92 93 94 95 96 97 98 99 100 101 102 103 104 105 
    106 107 108 109 110 111 112 113 114 115 116 117 118 119 
    120 121 122 123 124 125 126 127 128 129 130 131 132 ... */
```

Unlimited ranges are kind of generators. Of course you don't have to run generation infinitly you can limit it by taking specific number of elements (by using `view::take(int)`) i.e.:
```c++
for(auto&& e : view::ints(0) | view::take(10))
    std::cout << e << " ";
std::cout << "Finished!" << std::endl;

// Output: 0 1 2 3 4 5 6 7 8 9 Finished!
```

For convenience you can use overload of `view::take(int,int)` which takes initial value for generation and number of elements to be generated.
```c++
for(auto&& e : view::ints(0, 10))
    std::cout << e << " ";
std::cout << "Finished!" << std::endl;

// Output: 0 1 2 3 4 5 6 7 8 9 Finished!
```

#### Interesting generaors

Using composition we can create really interesting generators. I.e. by composing three `for_each` algorithms together with `yield_if` we can make generator of Pythagorean Triples:
```c++
using namespace ranges::view;

const auto triples = for_each(ints(1), [](int z) {
    return for_each(ints(1, z + 1), [=](int x) {
        return for_each(ints(x, z + 1), [=](int y) {
	    return yield_if(x * x + y * y == z * z, 
	                    std::tuple(x, y, z));
        });
    });
});
								    
for(auto&& [x,y,z] : triples | take(10))
    std::cout << x << "\t" << y << "\t" << z << "\n";
```

Which generates us ten triples
```c++
/* Output:
3	4	5
6	8	10
5	12	13
9	12	15
8	15	17
12	16	20
7	24	25
15	20	25
10	24	26
20	21	29
*/
```

#### Other views

Let's assume that you want to take elements from generator starting from 100th to 10th (exclusive). You can achieve that in multiple ways:
```c++
// takes elements on position from 100 to 105 (exclusive) 
pythagorean_triples | view::slice(100,105); 

// shorter
pythagorean_triples[{100,105}]; 

// other way
pythagorean_triples | view::drop(100) | view::take(5);

// using 'end' in slice
view::take(pythagorean_triples, 105)[{end-5, end}];
```

## Watch out for recurring views

Assume that you have objects organized in tree-like structure and you would like to traverse all of the objects and get one of its attributes. Lets say the object looks like
```c++
struct x {
  std::string name;
  std::vector<x*> nested;
};
```
and you would like to get all the names from all the objects.

You can try to do it by
```c++
template <typename Rng>
auto recurse(Rng&& rng) {
  return rng | view::transform([&}(auto* e) {
                 return concat(view::single(e->name),
		               e->nested.empty() ? view::empty<std::string>() : recurse(e->nested)
		              );
  }) | view::join;
};
```
but it will fail because `recurse` function has to deduce the return type but before function returns it calls `recurse` function again. Compiler is unable to evaluate the return type.

If you find yourself in similar situation you can use type-erased solution which is called `any_view`.
```c++
template <typename Rng>
static any_view<std::string_view> recurse(Rng&& rng) {
    return rng | view::transform([&](auto* e){
            return concat(view::single(e->name), 
			  e->nested.empty() ? view::empty<std::string_view>() : recurse(e->nested)
			 );
    }) | view::join;
};
```

This solution is not perfect. If you compare it to solution which traverse all objects using range-for loops and stores the names into `std::vector` you will see that solution with `any_view` is almost **6 times slower**.

**Note:** If possible avoid using `any_view`.

# External references
* http://ericniebler.com/2014/11/23/container-algorithms/
* https://hannes.hauswedell.net/post/2018/04/11/view1/
* https://www.fluentcpp.com/2018/02/09/introduction-ranges-library/
* https://stackoverflow.com/questions/43577873/range-v3-flattening-a-sequence
* https://stackoverflow.com/questions/48402558/how-to-split-a-stdstring-into-a-range-v3-of-stdstring-views
* http://www.modernescpp.com/index.php/the-new-ranges-library
* https://arne-mertz.de/2017/01/ranges-stl-next-level/

