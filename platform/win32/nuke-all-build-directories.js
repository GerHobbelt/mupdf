
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
	"build",
	"obj",
	"v16/ipch",
];

const buildFiles = [
	"/Browse.VC.db",
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
];

let src = fs.readFileSync(input, 'utf8');
// split in lines, one line per file
let a = src.split('\n');
// process each line:
let uniq = {};
let b = a
.filter((l) => l && l.length)
.map((l) => {
	l = l.replace(/[\\/]/g, '/');

	// see if directory tree includes a CMake 'build' dir or other obvious build destination directory:
	let has_build_dir = false;
	for (let i = 0; i < buildDirs.length; i++) {
		let dirstr = `/${buildDirs[i]}/`;
		if (l.includes(dirstr)) {
			has_build_dir = true;
			l = l.replace(new RegExp(`${dirstr}.*$`), dirstr);
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

