//
// 'touch' a given Microsoft Visual Studio .SLN solution file so MSVC2022 will rewrite it after reload.
//

const fs = require('fs');
const path = require('path');

const debug = false;

if (process.argv.length !== 3) {
	console.error("sort-sln-file.js: please provide the solution filename as the sole (single) argument.")
	process.exit(2);
}

const slnpath = process.argv[2];
const scriptpath = path.dirname(process.argv[1]);

if (debug) console.error({slnpath, scriptpath})

let slnstr = fs.readFileSync(slnpath, 'utf8');
if (debug) console.log({slnstr});

// make sure we duplicate one project so the next time the solution is loaded MSVC will complain and then ensure the solution file is rewritten once the user takes any action:
//
// since MSVC2022 may 2025 all we need are a few extra (bogus) EndProject lines; adding a bogus/duplicate project line like we did before will now be seen as an abortive error by MSVC2022!
//
// Note: as of July 2025 the old behaviour of MSVC2022 has been restored by Microsoft. Yay!
//
let projline = slnstr.match(/^\s*Project.*?8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942.*?$/m);
if (projline && projline[0]) {
	slnstr += `
EndProject
${projline}
EndProject
	`;	
}

slnstr += `

EndProject
EndProject
EndProject
EndProject
EndProject
EndProject

`;	

if (debug) console.log({slnstr});

fs.writeFileSync(slnpath, slnstr, 'utf8');

process.exit(0);

