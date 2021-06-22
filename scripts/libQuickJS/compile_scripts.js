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

let bootstrap_repl = false;
let bootstrap_calc = false;
try {
	if (fs.existsSync(mutoolExe)) {
		let stdout = execFileSync(mutoolExe, ['qjsc', '-v', '-m', '-c', '-o', replCfile, replJSfile]);
		console.log(stdout);
		if (fs.existsSync(replCfile)) {
			console.log("Successfully generated the repl C source file from repl.js");
		}
	}
} catch (ex) {
	//console.log("COMPILE FAILED:", ex);
	let errmsg = `COMPILE FAILED: ${ex}`;
	console.log(errmsg.replace(/Error/ig, 'Warning'));
	bootstrap_repl = true;
}

try {
	if (fs.existsSync(mutoolExe)) {
		stdout = execFileSync(mutoolExe, ['qjsc', '-v', '-fbignum', '-m', '-c', '-o', calcCfile, calcJSfile]);
		console.log(stdout);
		if (fs.existsSync(calcCfile)) {
			console.log("Successfully generated the calc C source file from calc.js");
		}
	}
} catch (ex) {
	//console.log("COMPILE FAILED:", ex);
	let errmsg = `COMPILE FAILED: ${ex}`;
	console.log(errmsg.replace(/Error/ig, 'Warning'));
	bootstrap_calc = true;
}

bootstrap(bootstrap_repl, bootstrap_calc);




// now the bootstrap bit: fake it when the compiler did not deliver!

function bootstrap(forced_repl, forced_calc) {
	if (forced_repl) {
		forced_repl = "FORCED";
	}
	if (!fs.existsSync(replCfile) || forced_repl) {
		console.log(`BOOSTRAPPING ${ forced_repl }: faking an empty repl C source file`);
		fs.writeFileSync(replCfile, `

#include <stdint.h>

const uint8_t qjsc_repl[] = { 0 };
const uint32_t qjsc_repl_size = 1;

`, 'utf8');
	}

	if (forced_calc) {
		forced_calc = "FORCED";
	}
	if (!fs.existsSync(calcCfile) || forced_calc) {
		console.log(`BOOSTRAPPING ${ forced_calc }: faking an empty calc C source file`);
		fs.writeFileSync(calcCfile, `

#include <stdint.h>

const uint8_t qjsc_qjscalc[] = { 0 };
const uint32_t qjsc_qjscalc_size = 1;

`, 'utf8');
	}

	process.exit(0);
}

