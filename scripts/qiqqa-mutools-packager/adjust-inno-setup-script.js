
let fs = require('fs');
let path = require('path');


let filepath = process.argv[2];
if (!fs.existsSync(filepath)) {
	console.error("must specify valid ISCC file");
	process.exit(1);
}
let src = fs.readFileSync(filepath, 'utf8');

let params = process.argv.slice(3);
let pms = params.join(' ').split('::').filter((l) => l.trim().length > 0).map((l) => {
	return l.split('=').map((w) => w.trim());
});
let db = {};
pms.map((el) => {
	db[el[0]] = el[1];
});

console.error(JSON.stringify(process.argv, null, 2), {params, pms, db})

/*
 * 2>  pms: [
2>    [
2>      'OUTDIR',
2>      'Z:\\lib\\tooling\\qiqqa\\MuPDF\\platform\\win32\\bin\\Release-NotSet-64bit-x64\\'
2>    ],
2>    [ 'PLATFORM', 'x64' ],
2>    [ 'PLATFORM_TARGET', 'x64' ],
2>    [
2>      'PROJECTDIR',
2>      'Z:\\lib\\tooling\\qiqqa\\MuPDF\\platform\\win32\\'
2>    ],
2>    [
2>      'INTERMEDIATEDIR',
2>      'Z:\\lib\\tooling\\qiqqa\\MuPDF\\platform\\win32\\obj\\Release-NotSet-64bit-x64\\-Makefile-qiqqa-mutools-packager\\'
2>    ],
2>    [
2>      'SOLUTIONDIR',
2>      'Z:\\lib\\tooling\\qiqqa\\MuPDF\\platform\\win32\\'
2>    ],
2>    [ 'TARGETNAME', 'qiqqa-mutools-packager' ]
2>  ]
2>}
 */

// dirty hack to point at the correct output directory:
db['INDIR'] = db['OUTDIR'].replace(/-NotSet-/g, '-Unicode-');



src = src
	.replace(/^OutputDir=.*$/mg, `OutputDir=${db['OUTDIR']}`)
	// Source: "..\..\platform\win32\Debug-32\bulktest.exe"; DestDir: "{app}"; Flags: ignoreversion
	.replace(/^Source: "(.*)\\([^\\"]+.exe)"; DestDir: "\{app\}"; Flags: ignoreversion\s*$/mg, (m, p1, p2) => {
		return `Source: "${db['INDIR']}${p2}"; DestDir: "{app}"; Flags: ignoreversion`;
	})
	.replace(/^Source: "(.*)\\([^\\"]+.dll)"; DestDir: "\{app\}"; Flags: ignoreversion\s*$/mg, (m, p1, p2) => {
		return `Source: "${db['INDIR']}${p2}"; DestDir: "{app}"; Flags: ignoreversion`;
	})

console.log(src);
fs.writeFileSync(filepath, src, 'utf8');
