
const fs = require('fs');  
const path = require('path');

let argv = process.argv;
//console.log(argv);

let input = argv[2] || ''; 
let output = argv[3] || ''; 

if (!input.length || !fs.existsSync(input) || !output.length) {
	console.error("Node script requires 2 arguments: input file (file list) and output file (bash script to be generated).");
	process.exit(1);
}

const buildDirs = [
	"Debug",
	"Release",
	"Debug-32",
	"Release-32",
	"Debug-64",
	"Release-64",
	"Debug-x86",
	"Release-x86",
	"Debug-x64",
	"Release-x64",
	"DebugUWP",
	"ReleaseUWP",
	"reader_Win32_Debug",
	"reader_Win32_Release",
	"obj",
	"packages",
	"v16/ipch",
];

const buildDirsPlusOne = [
	"bin/debug",
	"bin/release",
	"lib/net20",
	"lib/net30",
	"lib/net35",
	"lib/net40",
	"lib/net45",
	"lib/net46",
	"lib/net47",
	"lib/net48",
	"lib/net50",
	"lib/net472",
	"lib/net462",
	"lib/net452",
	"lib/netcoreapp1.0",
	"lib/netcoreapp2.0",
	"lib/netcoreapp2.1",
	"lib/netcoreapp3.0",
	"lib/netcoreapp3.1",
	"lib/netstandard1.0",
	"lib/netstandard1.3",
	"lib/netstandard1.6",
	"lib/netstandard2.0",
	"lib/netstandard2.1",
	"lib/sl4",
	"lib/sl5",
	"lib/wp8",
	"lib/wpa81",
];

const buildFiles = [
	"/Browse.VC.db",
];

const CMakeTargetDirs = [
	"build",
	"b",
]

const CMakeTargetFiles = [
	"CMakeCache.txt",
];

const specialRejectFiles = [
	".git",
	".gitignore",
	".gitmodules",
	".gitattributes",
	"README",
	"README.txt",
	"README.md",
	"index.html",
	"Makefile",
	"Makefile.am",
	"Makefile.in",
	"configure",
	"configure.ac",
	"App.config",
	"CHANGES",
	"LICENSE",
	"LICENSE.txt",
	"LICENSE.md",
	"autogen.mk",
];

let src = fs.readFileSync(input, 'utf8');
// split in lines, one line per file
let a = src.split('\n');
// process each line:
let uniq = {};
let b = a
.filter((l) => l && l.length)
.map((l) => {
	l = l
	.replace(/[\\/]/g, '/')
	.replace(/^\/([a-z])\//g, '$1:/');    // UNIX-path-on-Windows to Windows path

	// see if directory tree includes a CMake 'build' dir or other obvious build destination directory:
	let has_build_dir = false;
	for (let i = 0; i < buildDirs.length; i++) {
		let dirstr = `/${buildDirs[i]}/`;
		if (l.includes(dirstr)) {
			has_build_dir = true;
			l = l.replace(new RegExp(`${dirstr}.*$`), dirstr);
		}
	}

	for (let i = 0; i < buildDirsPlusOne.length; i++) {
		let dirstr = `/${buildDirsPlusOne[i]}/`;
		if (l.includes(dirstr)) {
			has_build_dir = true;
			l = path.dirname(l.replace(new RegExp(`${dirstr}.*$`), dirstr));
		}
	}

	if (!has_build_dir) {
		for (let i = 0; i < CMakeTargetDirs.length; i++) {
			let dirstr = `/${CMakeTargetDirs[i]}/`;
			if (l.includes(dirstr)) {
				// but only accept this CMake build directory
				// when it does contain the listed CMake target files!
				// (Counter-example: wxWindows /build/ directory; there, the CMake target was chosen to be 'b' instead.)
				let ap = l.replace(new RegExp(`${dirstr}.*$`), dirstr);
				for (let j = 0; j < CMakeTargetFiles.length; j++) {
					let matchpath = ap + CMakeTargetFiles[j];
					if (fs.existsSync(matchpath)) {
						has_build_dir = true;  
						l = ap;
						break;
					}
				}
				
				// if (!has_build_dir) {
				// 	console.error('Did not find expected CMake cache/output files in ', l, ': SKIPPING!');
				// 	return null;
				// }
			}
		}
	}
	
	if (!has_build_dir) {
		for (let i = 0; i < buildFiles.length; i++) {
			let dirstr = buildFiles[i];
			if (l.endsWith(dirstr)) {
				has_build_dir = true;
				l = l.replace(/[^/]+$/, "");  // strip off the filename at the end.
			}
		}
	}
	
	if (!has_build_dir) {
		console.error('Could not deduce a build directory for this object/target file:', l);

		// special directories: assume it's the first containing directory,
		// UNLESS there's a .git* file in there:
		let dirstr = l.replace(/[^/]+$/, "");
		// now check for .git, .gitignore, .gitattributes and .gitmodules files:
		has_build_dir = true;  
		for (let i = 0; i < specialRejectFiles.length; i++) {
			let matchpath = dirstr + specialRejectFiles[i];
			if (fs.existsSync(matchpath)) {
				has_build_dir = false;  
				console.error('Found ', matchpath, ' in the same directory: SKIPPING!');
				return null;
			}
		}
		l = dirstr;
	}

	// and convert Windows path back to UNIX-on-Windows:
	l = l 
	.replace(/^([a-z]):\//g, '/$1/');

	// make sure we log each entry only once:
	if (uniq[l])
		return null;
	uniq[l] = true;
	return l;
})
.filter((l) => l && l.length);

let dstcontent = b.map((l) => {
	return `echo "${l}"
rm -rf "${l}"`;
})

fs.writeFileSync(output, `#! /bin/bash

echo "Deleting each build directory:"

${ dstcontent.join('\n') }

	`, 'utf8');

