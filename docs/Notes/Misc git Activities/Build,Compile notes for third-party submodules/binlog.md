# binlog :: build/compile notes

## MSVC (Microsoft Visual Studio 2019+)

The test code at least makes some important choices based on a `__cplusplus` version code check, which **fails in MSVC** unless you use the fix described by Microsoft here: [MSVC now correctly reports `__cplusplus` - C++ Team Blog (microsoft.com)](https://devblogs.microsoft.com/cppblog/msvc-now-correctly-reports-__cplusplus/) (April 2018)

Quoting the relevant chunk (emphasis in bold is mine):

> The MSVC compiler’s definition of the `__cplusplus` predefined macro leaps ahead 20 years in Visual Studio 2017 version 15.7 Preview 3. This macro has stubbornly remained at the value “199711L”, indicating (erroneously!) that the compiler conformed to the C++98 Standard. Now that [our conformance catch-up work is drawing to a close](https://devblogs.microsoft.com/vcblog//07/c-standards-conformance-from-microsoft) we’re updating the `__cplusplus` macro to reflect the true state of our implementation. The value of the `__cplusplus` macro doesn’t imply that we no longer have any conformance bugs. It’s just that the new value is much more accurate than always reporting “199711L”.
> 
> ### /Zc:__cplusplus[](https://devblogs.microsoft.com/cppblog/msvc-now-correctly-reports-__cplusplus/#zc__cplusplus)
> 
> **You need to compile with the `/Zc:__cplusplus` switch to see the updated value of the `__cplusplus` macro**. We tried updating the macro by default and discovered that a lot of code doesn’t compile correctly when we change the value of `__cplusplus`. **We’ll continue to require use of the `/Zc:__cplusplus` switch for all minor versions of MSVC in the 19.xx family.**
> 
> The version reported by the `__cplusplus` macro also depends upon the standard version switch used. If you’re compiling in C++14 mode the macro will be set to “201402L”. If you compile in C++17 mode the macro will be set to “201703L”. And [the `/std:c++latest` switch](https://devblogs.microsoft.com/cppblog/standards-version-switches-in-the-compiler), used to enable features from the Standard currently in development, sets a value that is more than the current Standard. This chart shows the values of the `__cplusplus` macro with different switch combinations:
> 
> | ----------------- | ------------------- | ------------------- |
> | `/Zc:__cplusplus` switch    | `/std:c++` switch     | `__cplusplus` value   |
> | ----------------- | ------------------- | ------------------- |
> |   `Zc:__cplusplus`    | Currently defaults to C++14    | 201402L |
> | `Zc:__cplusplus`    | `/std:c++14`    | 201402L |
> | `Zc:__cplusplus`    | `/std:c++17`    | 201703L |
> | `Zc:__cplusplus`    | `/std:c++latest`    | 201704L |
> | `Zc:__cplusplus-` (disabled)    | Any value    | 199711L |
> | `Zc:__cplusplus` not specified    | Any value    | 199711L |
> | ----------------- | ------------------- | ------------------- |
>
> Note that the MSVC compiler does not, and never will, support a C++11, C++03, or C++98 standards version switch. Also, the value of the `__cplusplus` macro is not affected by [the /permissive- switch](https://blogs.msdn.microsoft.com/vcblog/2016/11/16/permissive-switch).
> 
> We’re updating IntelliSense to correctly reflect the value of `__cplusplus` when compiling with MSVC. We expect IntelliSense to be correct in the next preview of 15.7.
> 

Turns out `binlog` requires this switch or the test code won't compile.
