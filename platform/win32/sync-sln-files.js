
const fs = require("fs");
const path = require("path");

//
// 1. Using m-dev-list.sln as the base/driver, update all SLN files.
// 2. inspect the m-dev-*.sln files and see which projects in m-dev-list are not listed in any of them (excluding the failed-ideas and may-matter sln files for now)
// 3. inspect the failed-ideas and may-matter sln files and see which projects in m-dev-list are not listed in either of them; also report projects which show up in *both*.
//
const command = +process.argv[2];

const source_sln_file = process.argv[3];
let dest_sln_files = [].concat(process.argv).toSpliced(0, 4);

console.log({ argv: process.argv, command, source_sln_file, dest_sln_files });


// Return array of projects discovered in SLN file content:
function getAllProjectsFromSln(sln_src) {
	// e.g. Project("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}") = "1D-RGB-color-gradient", "1D-RGB-color-gradient.vcxproj", "{3644E12D-D934-41FD-BF7E-83745A17E226}"
	let prj_re = /^(Project[^=]*?8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942[^=]*= "([^"]+)", "([^"]+?[.]vcxproj)".*?)$/gm;
	
	let prj_lst = [];
	for (;;) {
		let m = prj_re.exec(sln_src);
		//console.log({ m });
		if (m) {
			prj_lst.push({ name: m[2], file: m[3], line: m[1] });
		}
		else {
			break;
		}
	}
	//console.log({ prj_lst });	
	return prj_lst;
}


// make sure all chars in `src` are treated as literals in any regex constructed from this string.
function protect4RE(src) {
	let rv = src.replace(/([\[\]\/\\])/g, '\\$1');
	rv = rv.replace(/([$^*+.?(){}|<>])/g, '[$1]');
	if (rv !== src)
		console.log("protect4RE --> ", { src, rv });
	return rv;
}

switch (command) {
default:
	console.log(`Unknown/unsupported command '${command}'. Aborting.\n`);
	process.exit(1);
	break;
	
case 3:
	let failed_sln_file = dest_sln_files.filter((l) => /failed-ideas/.test(l))[0];
	let maymatter_sln_file = dest_sln_files.filter((l) => /may-matter/.test(l))[0];
	
	console.log({ failed_sln_file, maymatter_sln_file, length: dest_sln_files.length });
	
	if (!failed_sln_file || !maymatter_sln_file || dest_sln_files.length !== 2) {
		console.log(`Expected two extra SLN files only: a failed-ideas and may-matter one, nothing else. Aborting.\n`);
		process.exit(1);
	}
	
	let list_src = fs.readFileSync(source_sln_file, 'utf8');
	
	let list_prjs = getAllProjectsFromSln(list_src);
	//console.log({ list_prjs });	

	let failed_src = fs.readFileSync(failed_sln_file, 'utf8');
	let maymatter_src = fs.readFileSync(maymatter_sln_file, 'utf8');
	
	let failed_prjs = getAllProjectsFromSln(failed_src);
	let maymatter_prjs = getAllProjectsFromSln(maymatter_src);
	
	list_prjs = list_prjs.map((l) => { l.in_main_sln = true; return l; });
	failed_prjs.forEach((l) => { 
		let idx = list_prjs.findIndex((r) => r.name === l.name );
		if (idx < 0) {
			l.in_failed_sln = true;
			list_prjs.push(l);
		} else {
			list_prjs[idx].in_failed_sln = true;
		}
	});
	maymatter_prjs.forEach((l) => { 
		let idx = list_prjs.findIndex((r) => r.name === l.name );
		if (idx < 0) {
			l.in_maymatter_sln = true;
			list_prjs.push(l);
		} else {
			list_prjs[idx].in_maymatter_sln = true;
		}
	});
	console.log({ list_prjs });	

	let list_edited = false;
	let failed_edited = false;
	let maymatter_edited = false;
	
	list_prjs.forEach((l) => { 
		if ( !l.in_main_sln ) {
			console.log(`Project ${ l.name } is missing from the main solution: adding it there.\n`);
			list_src += `
${ l.line }
EndProject
			`;
			list_edited = true;
		}
		if ( !l.in_maymatter_sln && !l.in_failed_sln ) {
			console.log(`Project ${ l.name } is missing from both the FAILED-IDEAS and MAY-MATTER solutions: adding it to the latter.\n`);
			maymatter_src += `
${ l.line }
EndProject
			`;
			maymatter_edited = true;
		}
		if (false) {
			if ( l.in_maymatter_sln && l.in_failed_sln ) {
				console.log(`Project ${ l.name } is present in *both* the FAILED-IDEAS and MAY-MATTER solutions: removing it from the FAILED-IDEAS solution.\n`);
				
				// e.g. Project("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}") = "1D-RGB-color-gradient", "1D-RGB-color-gradient.vcxproj", "{3644E12D-D934-41FD-BF7E-83745A17E226}"
				let remo_re = new RegExp(`^Project.*?= "${ protect4RE(l.name) }".*?$`, 'gm');
				
				failed_src = failed_src.replace(remo_re, '');
				failed_edited = true;
			}
		} else {
			if ( l.in_maymatter_sln && l.in_failed_sln ) {
				console.log(`Project ${ l.name } is present in *both* the FAILED-IDEAS and MAY-MATTER solutions: removing it from the MAY-MATTER solution.\n`);
				
				// e.g. Project("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}") = "1D-RGB-color-gradient", "1D-RGB-color-gradient.vcxproj", "{3644E12D-D934-41FD-BF7E-83745A17E226}"
				let remo_re = new RegExp(`^Project.*?= "${ protect4RE(l.name) }".*?$`, 'gm');
				
				maymatter_src = maymatter_src.replace(remo_re, '');
				maymatter_edited = true;
			}
		}
	});
	
	if (list_edited) {
		fs.writeFileSync(source_sln_file, list_src, 'utf8');
	}
	if (failed_edited) {
		fs.writeFileSync(failed_sln_file, failed_src, 'utf8');
	}
	if (maymatter_edited) {
		fs.writeFileSync(maymatter_sln_file, maymatter_src, 'utf8');
	}
	if (list_edited || failed_edited || maymatter_edited) {
		console.log("NOTE: solution files have been updated.\n");
	}
	break;
}

