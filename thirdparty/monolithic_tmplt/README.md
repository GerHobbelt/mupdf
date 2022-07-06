# monolithic build dispatcher code template

Basic C template + minimal demo for monolithic build mode.



## Example usage

`monolithic_example.c` + several other files demonstrate the monolithic build, which combines the demo1 and demo2 `main()` functions into a single, monolithic app.

This is the code using the monolithic build template (and you can re-use this or riff off this base design):

```
//implement your copy of `#include "monolithic_main_defs.h"`:
#include "monolithic_examples.h"

// define a name for the monolith
#define USAGE_NAME   "demo"

// load the monolithic definitions which you need for the dispatch table:
#include "monolithic_main_internal_defs.h"

// declare your own monolith dispatch table:
MONOLITHIC_CMD_TABLE_START()
{ "demo1", { .fa = mbdemo1_main } },
{ "demo2", { .f = mbdemo2_main } },
MONOLITHIC_CMD_TABLE_END();

// load the monolithic core dispatcher
#include "monolithic_main_tpl.h"
```



## Files: what they are for

- `monolithic_main_defs.h`: Example core content of each `monolithic_examples.h` header file, which carries the declarations of the various `main()` functions we want to bundle in a monolithic build.

- `monolithic_example.c`: example 'root': the source file which includes the monolithic template files, demonstrating their use.

- `monolithic_example_subs_demo1.cpp`: part of the example. C++ file to showcase the `extern "C" ...` declarations of the `main()` functions, which are bundled in this example: demo1 + demo2.
- `monolithic_example_subs_demo2.cpp`: part of the example. C++ file to showcase the `extern "C" ...` declarations of the `main()` functions, which are bundled in this example: demo1 + demo2.

- `monolithic_examples.h`: part of the example. The demo implementation of the `monolithic_main_defs.h` header file.

- `monolithic_main_internal_defs.h`: the core content (part 1) which can be `#include`d in a monolithic build app to provide the `main()` dispatcher. Hence also used in the example.

- `monolithic_main_tpl.h`: the core content (part 2) which can be `#include`d in a monolithic build app to provide the `main()` dispatcher. Hence also used in the example. 


Thus you'll (probably) be using these:

- your own copy of `monolithic_main_defs.h`, called: `monolithic_examples.h`

- your own copy of the dispatcher, using `monolithic_main_internal_defs.h` and `monolithic_main_tpl.h`


While the demo uses these:

- `monolithic_examples.h`
- `monolithic_main_internal_defs.h`
- `monolithic_main_tpl.h`

plus these **demo-specific files**:

- `monolithic_example.c`
- `monolithic_example_subs_demo1.cpp`
- `monolithic_example_subs_demo2.cpp`

