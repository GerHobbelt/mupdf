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

const mutoolExe = binDir + 'mutool_ex.exe';
const replJSfile = targetDir + '../../thirdparty/owemdjee/QuickJS/repl.js';
const calcJSfile = targetDir + '../../thirdparty/owemdjee/QuickJS/qjscalc.js';
const replCfile = targetDir + 'qjsrepl.c';
const calcCfile = targetDir + 'qjscalc.c';

console.log("#### COMPILE QUICKJS REPL AND CALC SOURCES\n\n", {
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
}, "#########################################################");

// check if source is younger than target; if not, don't do anything.
function check_file_mtime(path) {
	let st = fs.statSync(path);
	return st.mtime.getTime();
}

function check_file_size(fpath) {
	const { size } = fs.statSync(fpath);
	return size;
}

if (fs.existsSync(replCfile)) {
	let mk_replCfile = true;
	if (fs.existsSync(replJSfile) && fs.existsSync(mutoolExe)) {
		// do NOT run build command when C is younger than JS, i.e. JS is older than C:
		// in that case, we assume the C file has already been generated from that JS file version before!
		//
		// Also check the relative filesizes to fix the bootstrap situation
		// where the generated C file would be nearly empty on an initial run.
		mk_replCfile = (check_file_mtime(replJSfile) >= check_file_mtime(replCfile) ||
						check_file_size(replJSfile) >= check_file_size(replCfile));
	}
	if (mk_replCfile) {
		fs.unlinkSync(replCfile);
	}
}
if (fs.existsSync(calcCfile)) {
	let mk_calcCfile = true;
	if (fs.existsSync(calcJSfile) && fs.existsSync(mutoolExe)) {
		mk_calcCfile = (check_file_mtime(calcJSfile) >= check_file_mtime(calcCfile) ||
			            check_file_size(calcJSfile) >= check_file_size(calcCfile));
	}
	if (mk_calcCfile) {
		fs.unlinkSync(calcCfile);
	}
}

let bootstrap_repl = false;
let bootstrap_calc = false;
try {
	if (!fs.existsSync(replCfile)) {
		if (fs.existsSync(mutoolExe)) {
			let stdout = execFileSync(mutoolExe, ['qjsc', '-v', '-m', '-c', '-o', replCfile, replJSfile]);
			console.log("REPL compile:", stdout);
			if (fs.existsSync(replCfile)) {
				console.log("Successfully generated the repl C source file from repl.js");
			}
		}
	}
} catch (ex) {
	//console.log("COMPILE FAILED:", ex);
	let errmsg = `COMPILE FAILED: ${ex}`;
	console.log(errmsg.replace(/Error/ig, 'Warning'));
	bootstrap_repl = true;
}

try {
	if (!fs.existsSync(calcCfile)) {
		if (fs.existsSync(mutoolExe)) {
			stdout = execFileSync(mutoolExe, ['qjsc', '-v', '-fbignum', '-m', '-c', '-o', calcCfile, calcJSfile]);
			console.log("CALC compile:", stdout);
			if (fs.existsSync(calcCfile)) {
				console.log("Successfully generated the calc C source file from qjscalc.js");
			}
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

