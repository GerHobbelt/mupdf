
let fs = require('fs');
let path = require('path');
let exec = require('child_process').exec;

function execute(command, callback) {
    exec(command, function(error, stdout, stderr) { 
    	if (error) {
    		throw new Error(error);
    	}
    	callback(stdout, stderr); 
    });
};

function reduceWS(s) {
	return s
	.replace(/[\r\n]+/g, '\n')
	.replace(/[ \t]+/g, ' ')
	.trim();
}

function isSameSrc(a, b) {
	a = reduceWS(a);
	b = reduceWS(b);
	//console.log('compare:', {a, b});
	return a === b;
}

function AssertIsAtWorkingDir(pwd) {
	let p = process.cwd();
	if (p !== pwd) {
		throw new Error(`Assertion FAILED: currently not at PWD (${pwd}); application reports current directory as '${p}' instead.`);
	}
}



let pwd = process.cwd();

process.chdir('../../thirdparty/owemdjee/jpeg-xl/');
execute('git rev-parse --short HEAD', (out, err) => {
	//console.log({out, err});
	process.chdir(pwd);
	AssertIsAtWorkingDir(pwd);

	let current_commit = out.trim();
	if (out.length < 6) {
		throw new Error(`Unexpected jpeg-xl commit hash: '${ current_commit }'`);
	}

	let src = `
#pragma once

#define JPEGXL_MAJOR_VERSION    0
#define JPEGXL_MINOR_VERSION    7
#define JPEGXL_PATCH_VERSION    0
#define JPEGXL_LIBRARY_VERSION "0.7.0"

#define JPEGXL_VERSION         "0.7.0.${ current_commit.substr(0, 8) }"
		`;

	let jpegXLversionH = '../../scripts/libjpeg-xl/tool_version_git.h';
	let origSrc = fs.readFileSync(jpegXLversionH, 'utf8');

	if (!isSameSrc(origSrc, src)) {
		fs.writeFileSync(jpegXLversionH, src, 'utf8');
		console.log('Updated: ', jpegXLversionH);
	}
	else {
		console.log('Already up-to-date (hence: NOT changed): ', jpegXLversionH);
	}
});
