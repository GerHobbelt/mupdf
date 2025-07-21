
# Compilers errors, thanks to `assert(pointer != 0)`


This is what you get when you tighten your assert library (switching to `libassert`, et al) and someone coded an old skool 

    assert(pointer != 0);

which is very slightly *wrong* as it SHOULD have read as 

    assert(pointer != nullptr);

i.e. *proper* C++ typesystem-aware pointer-vs-pointer comparison (equality check), *not* pointer vs. integer (0)!

Here's what MSVC2022 has to about this when it gets pulled through my `libassert` library copy: *oh boy!*...

```
error C2446: '!=': no conversion from 'B' to 'TMenu *'
        with
        [
            B=int
        ]
message : Conversion from integral type to pointer type requires reinterpret_cast, C-style cast or parenthesized function-style cast
message : the template instantiation context (the oldest one first) is
message : see reference to function template instantiation 'libassert::v1::detail::expression_decomposer<TMenu *&,int,libassert::v1::detail::ops::neq> libassert::v1::detail::expression_decomposer<TMenu *&,libassert::v1::detail::nothing,libassert::v1::detail::nothing>::operator !=<int>(O &&) &&' being compiled
        with
        [
            O=int
        ]
message : see reference to class template instantiation 'libassert::v1::detail::expression_decomposer<TMenu *&,int,libassert::v1::detail::ops::neq>' being compiled
message : while compiling class template member function 'decltype(auto) libassert::v1::detail::expression_decomposer<TMenu *&,int,libassert::v1::detail::ops::neq>::get_value(void)'
message : see the first reference to 'libassert::v1::detail::expression_decomposer<TMenu *&,int,libassert::v1::detail::ops::neq>::get_value' in 'TMenuView::writeMenu'
message : see reference to function template instantiation 'decltype(auto) libassert::v1::detail::ops::neq::operator ()<TMenu*&,B&>(A,int&) const' being compiled
        with
        [
            B=int,
            A=TMenu *&
        ]
error C3169: 'decltype(auto)': cannot deduce type for 'auto' from '<error type>'
error C3779: 'libassert::v1::detail::ops::neq::operator ()': a function that returns 'decltype(auto)' cannot be used before it is defined
message : see declaration of 'libassert::v1::detail::ops::neq::operator ()'
error C2440: 'static_cast': cannot convert from 'void' to 'bool'
message : Expressions of type void cannot be converted to other types
error C2446: '!=': no conversion from 'B' to 'char *'
        with
        [
            B=int
        ]
message : Conversion from integral type to pointer type requires reinterpret_cast, C-style cast or parenthesized function-style cast
message : the template instantiation context (the oldest one first) is
message : see reference to function template instantiation 'libassert::v1::detail::expression_decomposer<char *&,int,libassert::v1::detail::ops::neq> libassert::v1::detail::expression_decomposer<char *&,libassert::v1::detail::nothing,libassert::v1::detail::nothing>::operator !=<int>(O &&) &&' being compiled
        with
        [
            O=int
        ]
message : see reference to class template instantiation 'libassert::v1::detail::expression_decomposer<char *&,int,libassert::v1::detail::ops::neq>' being compiled
message : while compiling class template member function 'decltype(auto) libassert::v1::detail::expression_decomposer<char *&,int,libassert::v1::detail::ops::neq>::get_value(void)'
message : see the first reference to 'libassert::v1::detail::expression_decomposer<char *&,int,libassert::v1::detail::ops::neq>::get_value' in 'ipstream::readString'
message : see reference to function template instantiation 'decltype(auto) libassert::v1::detail::ops::neq::operator ()<char*&,B&>(A,int&) const' being compiled
        with
        [
            B=int,
            A=char *&
        ]
error C3169: 'decltype(auto)': cannot deduce type for 'auto' from '<error type>'
error C3779: 'libassert::v1::detail::ops::neq::operator ()': a function that returns 'decltype(auto)' cannot be used before it is defined
message : see declaration of 'libassert::v1::detail::ops::neq::operator ()'
error C2440: 'static_cast': cannot convert from 'void' to 'bool'
message : Expressions of type void cannot be converted to other types
error C2446: '!=': no conversion from 'B' to 'void *'
        with
        [
            B=int
        ]
message : Conversion from integral type to pointer type requires reinterpret_cast, C-style cast or parenthesized function-style cast
message : the template instantiation context (the oldest one first) is
message : see reference to function template instantiation 'libassert::v1::detail::expression_decomposer<void *&,int,libassert::v1::detail::ops::neq> libassert::v1::detail::expression_decomposer<void *&,libassert::v1::detail::nothing,libassert::v1::detail::nothing>::operator !=<int>(O &&) &&' being compiled
        with
        [
            O=int
        ]
message : see reference to class template instantiation 'libassert::v1::detail::expression_decomposer<void *&,int,libassert::v1::detail::ops::neq>' being compiled
message : while compiling class template member function 'decltype(auto) libassert::v1::detail::expression_decomposer<void *&,int,libassert::v1::detail::ops::neq>::get_value(void)'
message : see the first reference to 'libassert::v1::detail::expression_decomposer<void *&,int,libassert::v1::detail::ops::neq>::get_value' in 'operator >>'
message : see reference to function template instantiation 'decltype(auto) libassert::v1::detail::ops::neq::operator ()<void*&,B&>(A,int&) const' being compiled
        with
        [
            B=int,
            A=void *&
        ]
error C2440: 'static_cast': cannot convert from 'void' to 'bool'
message : Expressions of type void cannot be converted to other types
```

