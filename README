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

If you are, however, interested in this augmented mupdf material *per sé*, then do fork this repository itself and load the *submodules* we use in here to build the pdf/document tools.

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

# ABOUT

MuPDF is a lightweight open source software framework for viewing and converting
PDF, XPS, and E-book documents.

See the documentation in docs/index.html for an overview.

Build instructions can be found in docs/building.html.

# LICENSE

MuPDF is Copyright (c) 2006-2022 Artifex Software, Inc.

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU Affero General Public License as published by the Free
Software Foundation, either version 3 of the License, or (at your option) any
later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU Affero General Public License along
with this program. If not, see <http://www.gnu.org/licenses/>.

For commercial licensing, including our "Indie Dev" friendly options,
please contact sales@artifex.com.

# REPORTING BUGS AND PROBLEMS

The MuPDF developers hang out on IRC in the #mupdf channel on Libera.Chat.

Report bugs on the ghostscript bugzilla, with MuPDF as the selected component.

	http://bugs.ghostscript.com/

If you are reporting a problem with a specific file, please include the file as
an attachment.
