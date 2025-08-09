
const fs = require("fs");
const path = require("path");

const debug = true;

//
// 1. Using m-dev-list.sln as the base/driver, update all SLN files.
// 2. inspect the m-dev-*.sln files and see which projects in m-dev-list are not listed in any of them (excluding the failed-ideas and may-matter sln files for now)
// 3. inspect the failed-ideas and may-matter sln files and see which projects in m-dev-list are not listed in either of them; also report projects which show up in *both*.
//
const command = +process.argv[2];

const source_sln_file = process.argv[3];
let dest_sln_files = [].concat(process.argv).toSpliced(0, 4);

if (debug)  console.log({ argv: process.argv, command, source_sln_file, dest_sln_files });

// extra DUPLICATE project which will be appended to each SLN file so MSVC is forced to recognize a SLN change upon loading the file and have that one rewrite a *fresh, clean* SLN afterwards:
//
// since MSVC2022 may 2025 all we need are a few extra (bogus) EndProject lines; adding a bogus/duplicate project line like we did before will now be seen as an abortive error by MSVC2022!
//
const bogus_extra_project = `

EndProject
EndProject
EndProject
EndProject
EndProject
EndProject

`;

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

// Return SLN content with *one* project repeated so MSVC will alert and **always have to rewrite the SLN file on application close**, thus 'cleaning up' our work:
function appendBogusExtra(sln_src) {
    // e.g. Project("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}") = "1D-RGB-color-gradient", "1D-RGB-color-gradient.vcxproj", "{3644E12D-D934-41FD-BF7E-83745A17E226}"
    let prj_re = /^(Project[^=]*?8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942[^=]*= "([^"]+)", "([^"]+?[.]vcxproj)".*?)$/gm;

	let m = prj_re.exec(sln_src);
	//console.log({ m });
	if (m) {
		return sln_src + bogus_extra_project + m[1];
	}
	else {
		return sln_src + bogus_extra_project;
	}
}



// make sure all chars in `src` are treated as literals in any regex constructed from this string.
function protect4RE(src) {
    let rv = src.replace(/([\[\]\/\\])/g, '\\$1');
    rv = rv.replace(/([$^*+.?(){}|<>])/g, '[$1]');
    //if (rv !== src)
    //  console.log("protect4RE --> ", { src, rv });
    return rv;
}

let unwanted_sln_list = [];

function filterOutUnwantedSolutions(sln) {
	let pass = true;
	unwanted_sln_list.forEach((l) => {
		if (debug)  console.log(`check: ${ sln } --> ${ path.basename(sln) } === ${ path.basename(l) }? --> ${ path.basename(sln) === path.basename(l) ? 'TRUE' : 'FALSE' }`);
        if (path.basename(sln) === path.basename(l))
			pass = false;
	});
	if (debug)  console.log(`filterOutUnwantedSolutions: ${ sln } --> ${ (pass ? 'ACCEPT' : '**REJECT**') }`);
    return pass;
}


switch (command) {
default:
    console.log(`Unknown/unsupported command '${command}'. Aborting.\n`);
    process.exit(1);
    break;

case 1:
{
    // copy the directory structure and the bottom chunk to the target sln file (if it not the source).
    let list_src = fs.readFileSync(source_sln_file, 'utf8');

    // find the last SLN directory entry in there:
    let folder_re = new RegExp(`^([\\s\\S]*${ protect4RE('Project("{2150E333-8FDC-42A3-9474-1A3956D46DE8}")') }[\\s\\S]*?\\bEndProject\\b)`, 'g');
    let m = folder_re.exec(list_src);
    let prefix_str = m[1];

    let global_chunk_re = /\b(Global\b[\s\S]*\bEndGlobal)\b/g;
    let mg = global_chunk_re.exec(list_src);
    //console.log({ mg });
    let postfix_index = mg.index;
    let postfix_str = mg[1];

    //console.log({ folder_re, prefix_str, postfix_str });

	unwanted_sln_list = [ source_sln_file ];

    if (debug)  console.log({ dest_sln_files: dest_sln_files.filter(filterOutUnwantedSolutions) });
    dest_sln_files.filter(filterOutUnwantedSolutions).forEach((l) => {
        let dest_sln_file = l;
        let dest_src = fs.readFileSync(dest_sln_file, 'utf8');
        if (debug)  console.log({ dest_sln_file, size: dest_src.length });

        folder_re.lastIndex = 0;
        let m2 = folder_re.exec(dest_src);
        if (m2) {
            let dest_prefix_str = m2[1];

            global_chunk_re.lastIndex = 0;
            let m3 = global_chunk_re.exec(dest_src);
            if (m3) {
                let dest_postfix_index = m3.index;
                let dest_postfix_str = m3[1];

                let dest_tail_str = dest_src.slice(dest_postfix_index + dest_postfix_str.length);
                //console.log({ dest_sln_file, m3, dest_tail_str });

                // slice the sln and prepend the prefix_str:
                dest_src = prefix_str + dest_src.slice(dest_prefix_str.length, dest_postfix_index) + postfix_str + dest_tail_str;

                console.log(`Updating ${ dest_sln_file }.`);
                fs.writeFileSync(dest_sln_file, appendBogusExtra(dest_src), 'utf8');
            }
        }
    });

	console.log(`Updating ${ source_sln_file }.`);
	fs.writeFileSync(source_sln_file, appendBogusExtra(list_src), 'utf8');
}
    break;


case 2:
{
	// anything listed in failed-ideas does not need to show up in any of the m-dev-*.sln files any more -- except in m-dev-misc.sln!
	// ditto for obnoxious-to-be-evaluated!
    let list_src = fs.readFileSync(source_sln_file, 'utf8');

    let failed_sln_file = dest_sln_files.filter((l) => /failed-ideas/.test(l))[0];
    let maymatter_sln_file = dest_sln_files.filter((l) => /may-matter/.test(l))[0];
    let obnoxious_sln_file = dest_sln_files.filter((l) => /obnoxious/.test(l))[0];

    if (!failed_sln_file || !maymatter_sln_file || !obnoxious_sln_file) {
        console.log(`Expected at least three extra SLN files: a failed-ideas, obnoxious and may-matter one. Aborting.\n`);
        process.exit(1);
    }

    let list_prjs = getAllProjectsFromSln(list_src);
    //console.log({ list_prjs });

    let misc_sln_file = dest_sln_files.filter((l) => /m-dev-misc[.]sln/.test(l));
    if (misc_sln_file.length > 0)
        misc_sln_file = misc_sln_file[0];
    else {
        let sln_dir = path.dirname(source_sln_file);
        if (!sln_dir)
            sln_dir = './';
        misc_sln_file = sln_dir + '/m-dev-misc.sln';
    }

    let failed_src = fs.readFileSync(failed_sln_file, 'utf8');
    let obnoxious_src = fs.readFileSync(obnoxious_sln_file, 'utf8');
    let maymatter_src = fs.readFileSync(maymatter_sln_file, 'utf8');
    let misc_src = fs.readFileSync(misc_sln_file, 'utf8');

    let failed_prjs = getAllProjectsFromSln(failed_src);
    let obnoxious_prjs = getAllProjectsFromSln(obnoxious_src);
    let maymatter_prjs = getAllProjectsFromSln(maymatter_src);
    let misc_prjs = getAllProjectsFromSln(misc_src);


    list_prjs = list_prjs.map((l) => { 
		l.in_main_sln = true;    //X
        l.in_failed_sln = false;
        l.in_maymatter_sln = false;
        l.in_obnoxious_sln = false;
        l.in_misc_sln = false;
		l.occur_count = 0; 
		console.log(`list_prj: ${ l.file }`);
		return l; 
	});

    failed_prjs.forEach((l) => {
        let idx = list_prjs.findIndex((r) => r.file === l.file );
        if (idx < 0) {
			l.in_main_sln = false; 
			l.in_failed_sln = true;    //X
			l.in_maymatter_sln = false;
			l.in_obnoxious_sln = false;
			l.in_misc_sln = false;
			l.occur_count = 0; 
			console.log(`failed_prj: ${ l.file }`);
            list_prjs.push(l);
        } else {
            list_prjs[idx].in_failed_sln = true;
        }
    });
    maymatter_prjs.forEach((l) => {
        let idx = list_prjs.findIndex((r) => r.file === l.file );
        if (idx < 0) {
			l.in_main_sln = false; 
			l.in_failed_sln = false;
			l.in_maymatter_sln = true;    //X
			l.in_obnoxious_sln = false;
			l.in_misc_sln = false;
			l.occur_count = 0; 
			console.log(`maymatter_prj: ${ l.file }`);
            list_prjs.push(l);
        } else {
            list_prjs[idx].in_maymatter_sln = true;
        }
    });
    obnoxious_prjs.forEach((l) => {
        let idx = list_prjs.findIndex((r) => r.file === l.file );
        if (idx < 0) {
			l.in_main_sln = false; 
			l.in_failed_sln = false;
			l.in_maymatter_sln = false;
			l.in_obnoxious_sln = true;    //X
			l.in_misc_sln = false;
			l.occur_count = 0; 
			console.log(`obnoxious_prj: ${ l.file }`);
            list_prjs.push(l);
        } else {
            list_prjs[idx].in_obnoxious_sln = true;
        }
    });
    misc_prjs.forEach((l) => {
        let idx = list_prjs.findIndex((r) => r.file === l.file );
        if (idx < 0) {
			l.in_main_sln = false; 
			l.in_failed_sln = false;
			l.in_maymatter_sln = false;
			l.in_obnoxious_sln = false;
			l.in_misc_sln = true;       //X 
			l.occur_count = 0;   
			console.log(`misc_prj: ${ l.file }`);
            list_prjs.push(l);
        } else {
            list_prjs[idx].in_misc_sln = true;
        }
    });

	unwanted_sln_list = [ source_sln_file, misc_sln_file, obnoxious_sln_file, failed_sln_file ];

    if (debug)  console.log({ dest_sln_files: dest_sln_files.filter(filterOutUnwantedSolutions) });
    dest_sln_files.filter(filterOutUnwantedSolutions).forEach((l) => {
        let dest_sln_file = l;
        let dest_src = fs.readFileSync(dest_sln_file, 'utf8');
        if (debug)  console.log({ dest_sln_file, size: dest_src.length });

        let dest_prjs = getAllProjectsFromSln(dest_src);
        //console.log({ dest_prjs });

        dest_prjs.forEach((l) => {
            // DO NOT check `l.name === r.name` as a project may have its name updated in one solution but not the other.
            //
            // The safe way is to compare file paths instead:
            let idx = list_prjs.findIndex((r) => r.file === l.file );
            if (idx < 0) {
                l.in_main_sln = false;
				l.in_failed_sln = false;
				l.in_maymatter_sln = false;
				l.in_obnoxious_sln = false;
				l.in_misc_sln = false;
                l.occur_count = 1;
                list_prjs.push(l);
            } else {
                list_prjs[idx].occur_count++;
            }
        });
    });

    let has_been_edited = {};
	unwanted_sln_list.forEach((l) => {
		has_been_edited[l] = false;
	});
    dest_sln_files.forEach((l) => {
		has_been_edited[l] = false;
	});

    list_prjs.forEach((l) => {
        if ( !l.in_main_sln ) {
            console.log(`Project ${ l.name } is missing from the main solution: adding it there.\n`);
            list_src += `
${ l.line }
EndProject
            `;
            has_been_edited[source_sln_file] = true;
        }
        if ( l.occur_count === 0 && l.in_failed_sln === false) {
            console.log(`Project ${ l.name } is not used in any of the m-dev-* solutions; adding it to the m-dev-misc SLN.\n`);
            misc_src += `
${ l.line }
EndProject
            `;
            has_been_edited[misc_sln_file] = true;
        }
        if ( !l.in_maymatter_sln && !l.in_failed_sln && !l.in_obnoxious_sln ) {
            console.log(`Project ${ l.name } is missing from both the FAILED-IDEAS, OBNOXIOUS and MAY-MATTER solutions: adding it to the latter.\n`);
            maymatter_src += `
${ l.line }
EndProject
            `;
            has_been_edited[maymatter_sln_file] = true;
        }

		if ( l.in_maymatter_sln && l.in_failed_sln ) {
			console.log(`Project ${ l.name } is present in *both* the FAILED-IDEAS and MAY-MATTER solutions: removing it from the MAY-MATTER solution.\n`);

			// e.g. Project("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}") = "1D-RGB-color-gradient", "1D-RGB-color-gradient.vcxproj", "{3644E12D-D934-41FD-BF7E-83745A17E226}"
			let remo_re = new RegExp(`^Project.*?", "${ protect4RE(l.file) }".*?$`, 'gm');

			maymatter_src = maymatter_src.replace(remo_re, '');
            has_been_edited[maymatter_sln_file] = true;
		}
		if ( l.in_maymatter_sln && l.in_obnoxious_sln ) {
			console.log(`Project ${ l.name } is present in *both* the OBNOXIOUS and MAY-MATTER solutions: removing it from the MAY-MATTER solution.\n`);

			// e.g. Project("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}") = "1D-RGB-color-gradient", "1D-RGB-color-gradient.vcxproj", "{3644E12D-D934-41FD-BF7E-83745A17E226}"
			let remo_re = new RegExp(`^Project.*?", "${ protect4RE(l.file) }".*?$`, 'gm');

			maymatter_src = maymatter_src.replace(remo_re, '');
            has_been_edited[maymatter_sln_file] = true;
		}
		if ( l.in_obnoxious_sln && l.in_failed_sln ) {
			console.log(`Project ${ l.name } is present in *both* the FAILED-IDEAS and OBNOXIOUS solutions: removing it from the OBNOXIOUS solution.\n`);

			// e.g. Project("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}") = "1D-RGB-color-gradient", "1D-RGB-color-gradient.vcxproj", "{3644E12D-D934-41FD-BF7E-83745A17E226}"
			let remo_re = new RegExp(`^Project.*?", "${ protect4RE(l.file) }".*?$`, 'gm');

			obnoxious_src = obnoxious_src.replace(remo_re, '');
            has_been_edited[obnoxious_sln_file] = true;
		}

		if ( l.in_misc_sln && l.in_failed_sln ) {
			console.log(`Project ${ l.name } is present in *both* the FAILED-IDEAS and MISC solutions: removing it from the MISC solution.\n`);

			// e.g. Project("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}") = "1D-RGB-color-gradient", "1D-RGB-color-gradient.vcxproj", "{3644E12D-D934-41FD-BF7E-83745A17E226}"
			let remo_re = new RegExp(`^Project.*?", "${ protect4RE(l.file) }".*?$`, 'gm');

			misc_src = misc_src.replace(remo_re, '');
            has_been_edited[misc_sln_file] = true;
		}
    });

	let updated = false;
	
    dest_sln_files.filter(filterOutUnwantedSolutions).forEach((l) => {
        let dest_sln_file = l;
        let dest_src = fs.readFileSync(dest_sln_file, 'utf8');
        if (debug)  console.log({ dest_sln_file, size: dest_src.length });

        let dest_prjs = getAllProjectsFromSln(dest_src);
        //console.log({ dest_prjs });

        dest_prjs.forEach((d) => {
            // DO NOT check `l.name === r.name` as a project may have its name updated in one solution but not the other.
            //
            // The safe way is to compare file paths instead:
            let idx = list_prjs.findIndex((r) => r.file === d.file );
            if (idx >= 0) {
                let prj = list_prjs[idx];

				if ( prj.in_failed_sln ) {
					console.log(`Project ${ d.name } is present in *both* the FAILED-IDEAS and ${ dest_sln_file } solutions: removing it from the ${ dest_sln_file } solution.\n`);

					// e.g. Project("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}") = "1D-RGB-color-gradient", "1D-RGB-color-gradient.vcxproj", "{3644E12D-D934-41FD-BF7E-83745A17E226}"
					let remo_re = new RegExp(`^Project.*?", "${ protect4RE(d.file) }".*?$`, 'gm');

					dest_src = dest_src.replace(remo_re, '');
					has_been_edited[dest_sln_file] = true;
				}
            }
        });

		if (has_been_edited[dest_sln_file]) {
			console.log(`Updating ${ dest_sln_file }.`);
			fs.writeFileSync(dest_sln_file, appendBogusExtra(dest_src), 'utf8');
			updated = true;
		}
    });

	let sln_list = [ source_sln_file, misc_sln_file, obnoxious_sln_file, failed_sln_file, maymatter_sln_file ];

	let sln_srcs = [ list_src, misc_src, obnoxious_src, failed_src, maymatter_src ];

	sln_list.forEach((l, idx) => {
        let dest_sln_file = l;
		let dest_src = sln_srcs[idx];
		if (has_been_edited[dest_sln_file]) {
			console.log(`Updating ${ dest_sln_file }.`);
			fs.writeFileSync(dest_sln_file, appendBogusExtra(dest_src), 'utf8');
			updated = true;
		}
	});

	if (updated) {
        console.log("NOTE: solution files have been updated.\n");
    }
}
    break;
}

