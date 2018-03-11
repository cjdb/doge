* Stylistic changes should not be made in the same pull request as functional changes.
* Follow the [C++ Core Guidelines](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md).
* Except when using names provided by external libraries, spelling should conform to [Australian English](https://en.wikipedia.org/wiki/Australian_English). If in doubt, stick to British English, and it'll get flagged in the review.
* Code should adopt East-`const`ness.
* Variables should be declared like so:

```cpp
ranges::SignedIntegral i = 0; // if a concept exists
auto in = std::ifstream{"foo"}; // if a concept does not exist
```

* Signed integers are used for everything, except where an external library uses unsigned integers.
* If you need to iterate over a type using an integer, use `doge::zero` to capture the correct type.
