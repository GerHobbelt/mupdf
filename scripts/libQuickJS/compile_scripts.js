//
// script to help bootstrap/generate qjssrepl and qjscalc
//
// Design decision: it was deemed easier to accomplish this bootstrap in JS script than a Makefile:
// as long as the QuickJS compiler has not been successfully built in the monolithic mutool build,
// you won't be able to produce a correct qjsrepl compilate, so we'll have to fake it.
//
// Ditto for qjscalc.

const fs = require('fs');
const path = require('path');
const { execFileSync } = require('child_process');

let argv = process.argv;
let argc = argv.length;

let targetDir = argv[2];
let binDir = argv[3];		// OutputDir
let binDir2 = argv[4];		// TargetDir

const mutoolExe = binDir + 'mutool.exe';
const replJSfile = targetDir + '../../thirdparty/owemdjee/QuickJS/repl.js';
const calcJSfile = targetDir + '../../thirdparty/owemdjee/QuickJS/calc.js';
const replCfile = targetDir + 'qjsrepl.c';
const calcCfile = targetDir + 'qjscalc.c';

console.log({
	argv,
	argc,
	targetDir,
	binDir,
	binDir2,
	mutoolExe,
	existMuTool: fs.existsSync(mutoolExe),
	existReplJS: fs.existsSync(replJSfile),
	existCalcJS: fs.existsSync(calcJSfile),
	existReplC: fs.existsSync(replCfile),
	existCalcC: fs.existsSync(calcCfile),
});


if (fs.existsSync(replCfile)) {
	fs.unlinkSync(replCfile);
}
if (fs.existsSync(calcCfile)) {
	fs.unlinkSync(calcCfile);
}

try {
	if (fs.existsSync(mutoolExe)) {
		const stdout = execFileSync(mutoolExe, ['qjsc', '-v', '-c', '-o', replCfile, replJSfile]);
		console.log(stdout);
		if (fs.existsSync(replCfile)) {
			console.log("Successfully generated the repl C source file from repl.js");
		}
		bootstrap(true);
	}
	else {
		bootstrap();
	}
} catch (ex) {
	//console.log("COMPILE FAILED:", ex);
	let errmsg = `COMPILE FAILED: ${ex}`;
	console.log(errmsg.replace(/Error/ig, 'Warning'));
	bootstrap(true);
}




// now the bootstrap bit: fake it when the compiler did not deliver!

function bootstrap(forced) {
	if (forced) {
		forced = "FORCED";
	}
	if (!fs.existsSync(replCfile) || forced) {
		console.log(`BOOSTRAPPING ${ forced }: faking an empty repl C source file`);
		fs.writeFileSync(replCfile, `

#include <stdint.h>

const uint8_t qjsc_repl[] = { 0 };
const uint32_t qjsc_repl_size = 1;

`, 'utf8');
	}

	if (!fs.existsSync(calcCfile) || forced) {
		console.log(`BOOSTRAPPING ${ forced }: faking an empty calc C source file`);
		fs.writeFileSync(calcCfile, `

#include <stdint.h>

const uint8_t qjsc_qjscalc[] = { 0 };
const uint32_t qjsc_qjscalc_size = 1;

`, 'utf8');
	}

	process.exit(0);
}

