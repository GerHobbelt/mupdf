# About this fork

This MuPDF fork is geared towards use with [Qiqqa (document and citation manager)](https://github.com/jimmejardine/qiqqa-open-source/). It is based on the original MuPDF work done by Artifex, *closely tracks the developments overthere* and augments the codebase with other C/C++ based tools, which are useful in and around the Qiqqa document processes, such as 

- text extraction
- metadata extraction (including annotations)
- OCR (*text recognition* as an required extension of *text extraction* when you have image-based PDFs, which happens quite often in the wild)
- Qiqqa database support (SQLite I/O; we do include the generic SQLite tools as well to "open up" the Qiqqa *core components* for advanced usage and users who wish to perform custom actions on the collected and managed data)

## \@Developers

Currently, this fork is focused on (relatively easily) producing Windows binaries, while we stay fully aware of the need to keep this codebase *cross-platform portable* to Linux, etc. in order to be able to provide a viable and easy path towards Qiqqa on Linux and Qiqqa on other (UNIXy) platforms: [Qiqqa issue \#215](https://github.com/jimmejardine/qiqqa-open-source/issues/215)

### Building the executables: prerequisites

See also: https://github.com/jimmejardine/qiqqa-open-source/blob/master/DEVELOPER-INFO.md

- OS: Windows 10
- .NET 4.8, .NET 5.0 and .NET Core 3.1
- [Visual Studio 2019](https://visualstudio.microsoft.com/downloads/) with `C++` and `.NET desktop development` workload.
  + install the .NET SDK for .NET 4.8, .NET 5.0 and .NET Core 3.1, as available here: https://dotnet.microsoft.com/download
    
    You'll need all three of 'em as many projects are compiled to all three targets and this spares you from build errors due to missing target environments.
- [**git**](https://git-scm.com/downloads) or [**git for Windows**](https://gitforwindows.org/)
- `bash` (comes as part of git packages)
- `node` (NodeJS) & `npm` (NodeJS Package Manager) —— preferrably installed via [**nvm for windows**](https://github.com/coreybutler/nvm-windows)
- `python` (Python 3)

### Suggested tools

- [TortoiseGIT](https://tortoisegit.org/) — UI for git
- [Beyond Compare](https://scootersoftware.com/) — visual comparison tool, invoked by the Unit and System Tests [ApprovalTests framework](https://github.com/approvals/ApprovalTests.Net).



## Building Qiqqa From Source

When you intend to check this out in relation to Qiqqa, then please grab the Qiqqa repository as everything in *this* `mupdf` repository is included there as a git **submodule** and keeping the relative paths' relationships intact will ease your use of the entire sourcecode tree.

> If you are, however, interested in this augmented mupdf material *per sé*, then do fork this repository itself and load the *submodules* we use in here to build the pdf/document tools.
>
> When you don't want to be bothered with the (slightly bothersome) `git submodule` management part then, [**there's also a _snapshot repository_ of this codebase plus all relevant submodules' sources available here**](https://github.com/GerHobbelt/qiqqa_tooling_devtree_snapshots): check that one out when you don't plan to edit this C/C++ code collection, but want to compile from source with minimum preparation / hassle.

To fetch the repository find `Git Bash` from Windows' start menu and run:

    git clone https://github.com/GerHobbelt/mupdf.git
    cd mupdf


### Enable Long Filename Support in `git`

While still in the repo's root folder, run these commands:

    git config core.longpaths true
    git reset --hard

> This is needed because the repository includes "Windows Long Filenames". See [StackOverflow](https://stackoverflow.com/questions/22575662/filename-too-long-in-git-for-windows).


### Install required / optional git submodules 

Qiqqa/MuPDF uses several libraries and tools which are managed in separate git repositories (see the `.gitmodules` file).

Install/unpack the entire set using:

    git submodule update --init --recursive
    
> **Warning/Note**: 
> for the full source tree, including MuPDF-based tools and experimental research material, you should invoke
>
> ```
> git submodule update --init --recursive
> ```
>
> Be aware that this will take a long time to download and install all source code libraries and may consume a serious chunk of your disk space, particularly when you then go and compile/run the various libraries and tools in the `MuPDF` tree: the Visual Studio 2019 *solution* and *project files* will automatically copy the Tesseract data set (1+ GByte) to each binary target directory for ease of use & debugging. As you can build MuPDF in *Debug* and *Release* mode for *32 bit* and *64 bit* targets, that automatic action alone will account for an extra 5-6 Gbyte of disk space.  
>
> However, the benefit of this approach would be far fewer surprises during initial build of the entire source tree as you won't have to selectively `git submodule update --init` install/unpack any git submodules for your build to succeed. 


### Build the executables (a.k.a. "binaries")

- Open `/platform/win32/mupdf.sln` file to open the project with Visual Studio 2019.

- Choose the Debug/Release 32/64-bit platform target as you like.

- In the `Build` menu click on `Build Solution` or `Rebuild Solution`. This should take a while.

- Find the executables in the `/platform/win32/bin/...` subdirectory, where `...` is the name generated for the target build you selected.


# Problems? Questions? Anything to report?

Please file an issue in this repository (or [over at the Qiqqa repository](https://github.com/jimmejardine/qiqqa-open-source/issues); **be sure to mention this repository or at least "mupdf fork" then**); unless redirected by the fork maintainer(s), file trouble here before bothered Artifex: chances are we introduced a bug somewhere and it wouldn't be very polite to bother and load them with that.




----

# Original Artifex MuPDF info:

MUPDF
=====

MuPDF is a lightweight, high-performance C library and toolset for rendering,
converting, and manipulating PDF, XPS, and E-book documents. It is the engine
behind many commercial and open-source PDF applications, viewers, and developer
tools across desktop, mobile, server, and browser environments.

Full documentation: https://mupdf.readthedocs.io/en/latest/


CONTENTS
--------
  1. What is MuPDF
  2. Supported formats
  3. Components
  4. Quick start
  5. Building from source
  6. Command-line tools (mutool)
  7. Language bindings
  8. Documentation
  9. License
  10. Reporting bugs


1. WHAT IS MUPDF
----------------
MuPDF is three things in one package:

  - A C library (libmupdf) for embedding document rendering and processing
    into any application.

  - A suite of command-line tools (mutool) for converting, inspecting,
    cleaning, and manipulating documents without writing any code.

  - A set of viewer applications for Linux, Windows, BSD, Android, and
    WebAssembly, all built on top of the same C library.

The library is written in portable C with no mandatory external dependencies.
It renders text with sub-pixel accuracy, handles the full PDF specification
including transparency, forms, annotations, and digital signatures, and is
fast enough to power real-time interactive viewers on mobile devices.

MuPDF is used as the rendering engine in SumatraPDF (Windows), Zathura
(Linux), llpp (Linux), and many other third-party applications. It also
powers the official PyMuPDF Python library, the MuPDF.js WebAssembly
library and MuPDF WebViewer product.


2. SUPPORTED FORMATS
--------------------
Input formats:
  PDF (all versions, including encrypted)
  XPS / OpenXPS
  EPUB (DRM-free 2.0; limited 3.0 support)
  Mobipocket (MOBI)
  FictionBook 2 (FB2)
  ComicBook (CBZ, CBT)
  Images: JPEG, PNG, TIFF, JBIG2, JPEG 2000, PNM, PBM, PGM, PPM, PAM
  SVG (limited subset)

Output formats (via mutool draw / mutool convert):
  Images:    PNG, JPEG, PAM, PBM, PGM, PPM, PWG, PCL, PS
  Vector:    PDF, SVG
  Text:      plain text, HTML, structured text (XML or JSON)


3. COMPONENTS
-------------
libmupdf
  The core C library. Provides a context, document model, page rendering,
  text extraction, annotation handling, PDF editing, and more. All other
  components are built on top of this library. Header files are in include/
  and the API entry point is include/mupdf/fitz.h.

mutool
  An umbrella command-line tool with sub-commands for every common document
  task. See section 6 for a full listing.

mupdf-gl
  The primary desktop viewer. Supports table of contents sidebar, full
  Unicode text search, annotation editing, redaction, forms, and more.
  Requires OpenGL and X11 (Linux/BSD) or Win32 (Windows).

mupdf-x11
  A lightweight legacy viewer for X11 systems without OpenGL.

mupdf-win32
  The legacy Win32 viewer for Windows.

MuPDF.js
  A WebAssembly build of the library for use in Node.js, Bun, browsers, and
  other JavaScript environments. Available as an NPM package:
      npm install mupdf
  See: https://mupdfjs.readthedocs.io

Java / Android library
  JNI bindings to the C library. Used internally by the official Android
  viewer apps. Provides the same object-oriented API as MuPDF.js.

PyMuPDF
  A popular third-party Python binding for the MuPDF library. Maintained
  separately at: https://github.com/pymupdf/PyMuPDF


4. QUICK START
--------------
Render all pages of a PDF to PNG files at 150 DPI:

    mutool draw -o page%d.png -r 150 document.pdf

Convert a PDF to HTML:

    mutool convert -o output.html document.pdf

Extract all text from a PDF:

    mutool draw -F text -o output.txt document.pdf

Merge two PDFs into one:

    mutool merge -o merged.pdf file1.pdf file2.pdf

Clean and compress a PDF:

    mutool clean -z -i -f input.pdf output.pdf

Run a JavaScript program against a PDF (using the built-in ES5 interpreter):

    mutool run script.js document.pdf

Use from JavaScript / Node.js (install the npm package first):

    import * as mupdf from "mupdf"
    const doc = mupdf.Document.openDocument(buffer, "application/pdf")
    const page = doc.loadPage(0)
    const pixmap = page.toPixmap(mupdf.Matrix.identity, mupdf.ColorSpace.DeviceRGB)
    // pixmap.asPNG() returns a Buffer containing PNG data

Use from Java (after building the Java bindings):

    import com.artifex.mupdf.fitz.*;
    Document doc = Document.openDocument("document.pdf");
    System.out.println(doc.countPages());


5. BUILDING FROM SOURCE
-----------------------
Prerequisites
  - C99 compiler (GCC, Clang, or MSVC)
  - GNU make (Linux, macOS, BSD) or Visual Studio 2015+ (Windows)
  - For the GL viewer: OpenGL and X11 headers

Clone the repository and initialize submodules:

    git clone https://cgit.ghostscript.com/mupdf.git
    cd mupdf
    git submodule update --init --depth 1

Linux / macOS / BSD
  To build everything (library, tools, and viewers):

    make

  To build only the library and command-line tools (no viewer required):

    make tools

  To install system-wide:

    make prefix=/usr/local install

  The viewers require X11 and (for mupdf-gl) OpenGL development headers.
  Install them on Debian/Ubuntu with:

    sudo apt install xorg-dev libxcursor-dev libxrandr-dev libxinerama-dev
    sudo apt install mesa-common-dev libgl1-mesa-dev libglu1-mesa-dev

Windows
  Open the Visual Studio solution file:

    platform/win32/mupdf.sln

  Select your target architecture (x86 or x64), then build.

macOS
  The build process is the same as for Linux. No viewer is included in the
  standard macOS build; the library and mutool are fully supported.

Verify the build:

    ./build/release/mutool -v

Portable executables can be copied to any directory in your PATH:

    cp ./build/release/mutool ~/.local/bin/mutool

JavaScript / WebAssembly
  See platform/wasm/README for instructions on building MuPDF.js.

Java / Android bindings
  Build the Java bindings using:

    make java

  Output goes to build/java/release/. See the "Using with Java" guide in
  the documentation for classpath and library path configuration.


6. COMMAND-LINE TOOLS (mutool)
------------------------------
All tools are accessed through the mutool umbrella command. Run "mutool"
with no arguments for a list of sub-commands.

mutool draw
  Render a document to image files or extract text. The most flexible
  rendering tool, with full control over DPI, colorspace, page range,
  anti-aliasing, rotation, banding, threading, layers, and OCR.

  Common options:
    -o <file>    Output file name; use %d for page number (e.g. page%03d.png)
    -r <dpi>     Render resolution (default: 72)
    -c <space>   Colorspace: mono, gray, rgb, cmyk (and alpha variants)
    -R <angle>   Rotate clockwise by degrees
    -A <bits>    Anti-aliasing bits (0-8, default 8)
    -p <pass>    Password for encrypted files
    -F <format>  Force output format (png, jpeg, svg, text, html, etc.)

  Supported output: png, jpeg, pam, pbm, pgm, ppm, pwg, pcl, ps, svg,
    pdf, plain text, html, structured text (xml/json)

mutool convert
  Convert a document to another format. Simpler interface than draw;
  format is inferred from the output filename.

  Common options:
    -o <file>    Output file
    -F <format>  Output format: png, pdf, svg, html, xhtml, text, stext
    -A <bits>    Anti-aliasing bits

mutool clean
  Rewrite and repair a PDF file. Useful for fixing broken files, removing
  unused objects, adjusting compression, and making files human-readable.

  Common options:
    -g           Garbage collect unused objects
    -gg          Compact the cross-reference table
    -ggg         Merge duplicate objects
    -z           Deflate uncompressed streams
    -i           Compress image streams
    -f           Compress font streams
    -d           Decompress all streams (for manual editing)
    -c           Pretty-print graphics commands
    -s           Sanitize graphics commands
    -D           Strip encryption
    -A           Generate missing annotation appearance streams
    -S           Subset fonts (experimental)

mutool merge
  Merge pages from multiple input PDFs into a single output PDF.

    mutool merge -o output.pdf file1.pdf file2.pdf 1-5

mutool extract
  Extract embedded images and font resources from a document.

    mutool extract document.pdf

mutool show
  Display internal PDF objects for inspection and debugging.

    mutool show document.pdf trailer

mutool grep
  Search for text across one or more documents and print matching lines
  with page number and coordinates.

    mutool grep "search term" document.pdf

mutool create
  Create a new PDF from a text file containing graphics commands.

mutool poster
  Divide large pages into tile pieces for oversized printing.

mutool bake
  Bake annotations and widgets into the page content, making them permanent.

mutool sign
  Handle digital signatures: list, verify, and sign PDF documents.

mutool run
  Run a JavaScript (ES5) script with full access to the MuPDF library API.
  This is the scripting interface to the same API available in MuPDF.js.
  No Node.js or npm required.

    mutool run script.js [arguments]


7. LANGUAGE BINDINGS
--------------------
C
  The library is written in C. Include mupdf/fitz.h and link against
  libmupdf. The C API is documented at:
    https://mupdf.readthedocs.io/en/latest/reference/c/index.html

  The essential types are fz_context, fz_document, fz_page, fz_pixmap,
  fz_stext_page, and fz_matrix. Always create a fz_context first; it
  owns all memory and error handling for a session.

JavaScript / TypeScript (MuPDF.js)
  WebAssembly build, works in Node.js, Bun, Deno, and modern browsers.
  Full TypeScript definitions included. Install from npm:

    npm install mupdf

  Documentation: https://mupdfjs.readthedocs.io

Java
  JNI bindings in platform/java/. The Java API mirrors the JavaScript API.
  Also powers the official Android viewers.

  Build: make java
  Package: com.artifex.mupdf.fitz

Android
  Two pre-built viewer libraries are available via Maven:

    com.artifex.mupdf:viewer:1.15.+    (full-featured viewer)
    com.artifex.mupdf:fitz:1.15.+     (JNI bindings only)

  Maven repository: http://maven.ghostscript.com

  For embedding the viewer in an Android app:
    https://mupdf.readthedocs.io/en/latest/guide/using-with-android.html

Python (PyMuPDF)
  The PyMuPDF library provides Python bindings. It is maintained
  separately and available on PyPI:

    pip install pymupdf

  Repository: https://github.com/pymupdf/PyMuPDF
  Documentation: https://pymupdf.readthedocs.io

C++ / C# / .NET
  C++ and C# wrapper classes are documented in the MuPDF reference.
  The .NET wrapper (MuPDF.NET) is available separately:
    https://github.com/ArtifexSoftware/MuPDF.NET


8. DOCUMENTATION
----------------
Full documentation site:
    https://mupdf.readthedocs.io/en/latest/

What is MuPDF (overview):
    https://mupdf.readthedocs.io/en/latest/guide/what-is-mupdf.html

Installing / building:
    https://mupdf.readthedocs.io/en/latest/guide/install.html

mutool command reference:
    https://mupdf.readthedocs.io/en/latest/tools/mutool.html

C library reference:
    https://mupdf.readthedocs.io/en/latest/reference/c/index.html

JavaScript / MuPDF.js reference:
    https://mupdf.readthedocs.io/en/latest/reference/javascript/index.html
    https://mupdfjs.readthedocs.io

Using with Java:
    https://mupdf.readthedocs.io/en/latest/guide/using-with-java.html

Using with Android:
    https://mupdf.readthedocs.io/en/latest/guide/using-with-android.html

MuPDF Explored (in-depth C API book by Robin Watts):
    https://mupdf.readthedocs.io/en/latest/cookbook/mupdf-explored.html

PyMuPDF (Python bindings, maintained separately):
    https://pymupdf.readthedocs.io

MuPDF.NET (C# bindings, maintained separately):
    https://github.com/ArtifexSoftware/MuPDF.NET

Binary downloads (pre-built mutool and viewers):
    https://mupdf.com/releases/


9. LICENSE
-----------
MuPDF is available under two licenses:

GNU Affero General Public License v3 (AGPL)
  Free for open-source use. If you distribute software that uses MuPDF,
  or provide access to it as a network service, you must release the
  complete source code of your application under a compatible open-source
  license. The full AGPL text is in the COPYING file in this repository
  and at: https://www.gnu.org/licenses/agpl-3.0.html

  Important note for Android developers: the AGPL requires that your
  entire app be open source and free of proprietary components -- this
  includes Google Play Services, AdMob, and Crashlytics, even though
  they are distributed by Google.

Commercial license
  Required for proprietary or closed-source applications.
  Inquiry form: https://artifex.com/contact/mupdf

The bundled third-party libraries in thirdparty/ are covered by their own
licenses. See thirdparty/README for a full list of libraries, versions,
and their license terms (MIT, BSD, zlib, and similar permissive licenses).


10. REPORTING BUGS
------------------
Bug tracker (Ghostscript Bugzilla, component: MuPDF):
    http://bugs.ghostscript.com/

When reporting a bug involving a specific file, please attach the file to
the bug report. Include the MuPDF version (mutool -v) and your OS and
architecture.

Discord (for questions and discussion with the MuPDF developers):
    https://artifex.com/discord/artifex

For commercial support and licensing enquiries:
    sales@artifex.com