
const fs = require("fs");
const path = require("path");

const debug = false;

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
		// duplicate one project; make sure it is a complete 'record' from `Project`...to...`EndProject`!
		return sln_src + bogus_extra_project + m[1] + `
EndProject
			`;
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


let uuid_map = {};
let projectPath_map = {};
let projectName_map = {};

// e.g.     Project("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}") = "zstd_tests", "zstd_tests.vcxproj", "{A60D8644-5A1C-4D29-8970-10100000003F}"
let ToDo_uuid_list = [];
let ToDo_uuid_dedup_map = {};

function collectProjectUUID(info) {
	// let info = {
	//		name: m[1],
	//		file: path.basename(m[2]),
	//		filepath: m[2],
	//		uuid: m[3].toUpperCase(),
	//		is_project_ref: true
	//	};
	let must_reprocess = info.uuid.includes("A60D8644-5A1C-4D29-8970-10100000");

	if (!projectPath_map[info.filepath]) {
		projectPath_map[info.filepath] = info;
	}
	else if (projectPath_map[info.filepath].uuid !== info.uuid) {
		if (debug)  console.log("Duplicate Project UUID: ", {tableSlot: projectPath_map[info.filepath], info });
		else        console.log(`Duplicate Project UUID for ${ projectPath_map[info.filepath].file } // ${ info.file }`);
		must_reprocess = true;

		let el = projectPath_map[info.filepath];
		let idx = `${ el.filepath }::${ el.uuid }`;
		if (!ToDo_uuid_dedup_map[idx]) {
			if (debug)  console.log("TODO   Project UUID: ", { idx, el });

			ToDo_uuid_list.push(el);
			ToDo_uuid_dedup_map[idx] = el;
		}
	}

	if (!projectName_map[info.file]) {
		projectName_map[info.file] = info;
	}
	else if (projectName_map[info.file].uuid !== info.uuid) {
		if (debug)  console.log("Duplicate Project UUID: ", {tableSlot: projectName_map[info.file], info });
		else        console.log(`Duplicate Project UUID for ${ projectName_map[info.file].file } // ${ info.file }`);
		must_reprocess = true;

		let el = projectName_map[info.file];
		let idx = `${ el.filepath }::${ el.uuid }`;
		if (!ToDo_uuid_dedup_map[idx]) {
			if (debug)  console.log("TODO   Project UUID: ", { idx, el });

			ToDo_uuid_list.push(el);
			ToDo_uuid_dedup_map[idx] = el;
		}
	}
	
	if (!uuid_map[info.uuid]) {
		uuid_map[info.uuid] = info;
	}
	else if (uuid_map[info.uuid].uuid !== info.uuid) {
		if (debug)  console.log("Duplicate Project UUID: ", {tableSlot: uuid_map[info.uuid], info });
		else        console.log(`Duplicate Project UUID for ${ uuid_map[info.uuid].file } // ${ info.file }`);
		must_reprocess = true;

		let el = uuid_map[info.uuid];
		let idx = `${ el.filepath }::${ el.uuid }`;
		if (!ToDo_uuid_dedup_map[idx]) {
			if (debug)  console.log("TODO   Project UUID: ", { idx, el });

			ToDo_uuid_list.push(el);
			ToDo_uuid_dedup_map[idx] = el;
		}
	}
	
	if (must_reprocess) {
		let el = info;
		let idx = `${ el.filepath }::${ el.uuid }`;
		if (!ToDo_uuid_dedup_map[idx]) {
			if (debug)  console.log("TODO   Project UUID: ", { idx, el });
			else        console.log(`Must re-assign Project UUID for ${ el.file }`);

			ToDo_uuid_list.push(el);
			ToDo_uuid_dedup_map[idx] = el;
		}
	}
	
	if (path.isAbsolute(info.filepath)) {
		// --> I:\\Projects\\sites\\library.visyond.gov\\80\\lib\\tooling\\qiqqa\\MuPDF\\platform\\win32\\.........
		console.log("WARNING: ABSOLUTE PATH for Project: ", { info });
	}
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

    let dest_vcxproj_files = dest_sln_files.filter((l) => /[.]vcxproj/.test(l));
    
	dest_sln_files = dest_sln_files.filter((l) => /[.]sln/.test(l));

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
        l.ignore = (l.file.includes('../../') || l.file.includes('..\\..\\'));
		if (debug)  console.log(`list_prj: ${ l.file }`);
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
            l.ignore = (l.file.includes('../../') || l.file.includes('..\\..\\'));
			if (debug)  console.log(`failed_prj: ${ l.file }`);
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
            l.ignore = (l.file.includes('../../') || l.file.includes('..\\..\\'));
			if (debug)  console.log(`maymatter_prj: ${ l.file }`);
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
            l.ignore = (l.file.includes('../../') || l.file.includes('..\\..\\'));
			if (debug)  console.log(`obnoxious_prj: ${ l.file }`);
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
            l.ignore = (l.file.includes('../../') || l.file.includes('..\\..\\'));
			if (debug)  console.log(`misc_prj: ${ l.file }`);
            list_prjs.push(l);
        } else {
            list_prjs[idx].in_misc_sln = true;
        }
    });

	unwanted_sln_list = [ source_sln_file, misc_sln_file, obnoxious_sln_file, failed_sln_file, maymatter_sln_file ];

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
                l.ignore = (l.file.includes('../../') || l.file.includes('..\\..\\'));
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
		
		// 'ignore'-flagged projects are to be included everywhere, so NO REMOVAL!
		if ( l.ignore )
			return;

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

				// 'ignore'-flagged projects are to be included everywhere, so NO REMOVAL!
				if ( prj.ignore )
					return;

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

	// also filter FAILED-IDEAS from any given VCXPROJ project's dependencies:
    dest_vcxproj_files.forEach((l) => {
        let dest_vcxproj_file = l;
		let dest_src = fs.readFileSync(dest_vcxproj_file, 'utf8');
		let has_been_edited = false;

		list_prjs.forEach((l) => {
			// 'ignore'-flagged projects are to be included everywhere, so NO REMOVAL!
			if ( l.ignore )
				return;

			if ( l.in_failed_sln ) {
				// <ProjectReference Include="mu-office-test.vcxproj">
				let re = new RegExp(`<ProjectReference Include="${ l.file.replace(/[.]/g, "[.]") }">[\\s\\S]*?</ProjectReference>`, "g");
				//console.log({l, re});
				dest_src = dest_src.replace(re, function (m) {
					console.log(`Project ${ l.name } is listed in FAILED-IDEAS: removing it as a dependency from project ${dest_vcxproj_file}\n`);
					has_been_edited = true;
					return "";
				});
			}
		});

		if (has_been_edited) {
			console.log(`Updating ${ dest_vcxproj_file }.`);
			fs.writeFileSync(dest_vcxproj_file, dest_src, 'utf8');
			updated = true;
		}
	});
	
	if (updated) {
        console.log("NOTE: solution files have been updated.\n");
    }
}
    break;
	
case 3:
{
	// sync the UUIDs for all projects: assign them final UUIDs, if they haven't already.
	//
	// <ProjectGuid>{BFD01D9C-AC53-64D2-BC7B-9C711B129D59}</ProjectGuid>
	
	// as we expected a lot of file paths, Win32 bash b0rked, so we're down to using an @ file instead: grab all file paths from there!
	let dest_files = fs.readFileSync(source_sln_file, 'utf8')
	.split('\n')
	.map((l) => l.trim())
	.filter((l) => l.length > 0);
	if (debug)  console.log({ dest_files });

	uuid_map = {};
    projectPath_map = {};
    projectName_map = {};
	
	// e.g.     Project("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}") = "zstd_tests", "zstd_tests.vcxproj", "{A60D8644-5A1C-4D29-8970-10100000003F}"
	ToDo_uuid_list = [];
    ToDo_uuid_dedup_map = {};
	
	dest_files.forEach((l) => {
		let file_src = fs.readFileSync(l, 'utf8');

		// e.g. Project("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}") = "1D-RGB-color-gradient", "1D-RGB-color-gradient.vcxproj", "{3644E12D-D934-41FD-BF7E-83745A17E226}"
		//      Project("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}") = "libmupdf_CarvedSubset", "libmupdf_CarvedSubset.vcxitems", "{08FF3748-F1BD-40A1-9322-F93FD04C2350}"
		//      Project("{FAE04EC0-301F-11D3-BF4B-00C04F79EFBC}") = "MuPDFSharp", "..\..\MupdfSharp\MuPDFSharp.csproj", "{8E506476-FFAE-46B2-977D-45EB25ECF394}"
	    let prj_re = /^Project[^=]*?8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942[^=]*= "([^"]+)", "([^"]+?[.]vcx(?:proj|items))", "[{]([^}"]+?)[}]".*?$/gm;

		let m = prj_re.exec(file_src);
		let mx = { ...m };
		delete mx.input;
		if (debug)  console.log({ prj_re, mx });
		while (m) {
			let info = {
				name: m[1],
				file: path.basename(path.normalize(m[2])),
				filepath: path.normalize(m[2]),
				uuid: m[3].toUpperCase(),
				is_project_ref: true
			};
			if (debug)  console.log({line: m[0], info});

			collectProjectUUID(info);
			
			m = prj_re.exec(file_src);
		}
		
		// e.g.    <ProjectGuid>{BFD01D9C-AC53-64D2-BC7B-9C711B129D59}</ProjectGuid>
		//         <ProjectGuid>{8E506476-FFAE-46B2-977D-45EB25ECF394}</ProjectGuid>
		//         <ItemsProjectGuid>{08ff3748-f1bd-40a1-9322-f93fd04c2350}</ItemsProjectGuid>
		//
		//         <ProjectReference Include="libabseil-cpp.vcxproj">
		//           <Project>{a60d8644-5a1c-4d29-8970-7518ff3d0c57}</Project>
		//         </ProjectReference>

	    prj_re = /^\s*<ProjectGuid>[{]([^}]+?)[}]<\/ProjectGuid>.*?$/gm;

		m = prj_re.exec(file_src);
		mx = { ...m };
		delete mx.input;
		if (debug)  console.log({ prj_re, mx });
		while (m) {
			let info = {
				name: null,
				file: path.basename(path.normalize(l)),
				filepath: path.normalize(l),
				uuid: m[1].toUpperCase(),
				is_project_spec: true
			};
			if (debug)  console.log({line: m[0], info});

			collectProjectUUID(info);
			
			m = prj_re.exec(file_src);
		}

	    prj_re = /^\s*<ItemsProjectGuid>[{]([^}]+?)[}]<\/ItemsProjectGuid>.*?$/gm;

		m = prj_re.exec(file_src);
		mx = { ...m };
		delete mx.input;
		if (debug)  console.log({ prj_re, mx });
		while (m) {
			let info = {
				name: null,
				file: path.basename(path.normalize(l)),
				filepath: path.normalize(l),
				uuid: m[1].toUpperCase(),
				is_collection_spec: true
			};
			if (debug)  console.log({line: m[0], info});

			collectProjectUUID(info);
			
			m = prj_re.exec(file_src);
		}

	    prj_re = /<ProjectReference\s+Include="([^"]+)">[\s\r\n]*<Project>[{]([^}]+?)[}]<\/Project>[\s\r\n]*<\/ProjectReference>/g;

		m = prj_re.exec(file_src);
		mx = { ...m };
		delete mx.input;
		if (debug)  console.log({ prj_re, mx });
		while (m) {
			let info = {
				name: null,
				file: path.basename(path.normalize(m[1])),
				filepath: path.normalize(m[1]),
				uuid: m[2].toUpperCase(),
				is_crossref: true
			};
			if (debug)  console.log({line: m[0], info});

			collectProjectUUID(info);
			
			m = prj_re.exec(file_src);
		}
	});

	if (debug)  console.log({ uuid_map });
	
	// now all the uuid/project data collecting has been done, we still need to run an extra check:
	// are there any projects which are known under DIFFERENT UUIDs in various places, i.e. is there
	// any project which has more than ONE UUID?!
	for (const idx in projectPath_map) {
		let el = projectPath_map[idx];
		let uuid_slot = uuid_map[el.uuid];
		if (uuid_slot && uuid_slot.uuid !== el.uuid) {
			console.log(`Project ${ el.file } is known under multiple UUIDs and requires resyncing: ${ uuid_slot.uuid } !== ${ el.uuid }`);
			let idx = `${ el.filepath }::${ el.uuid }`;
			if (!ToDo_uuid_dedup_map[idx]) {
				if (debug)  console.log("TODO   Project UUID: ", { idx, el });

				ToDo_uuid_list.push(el);
				ToDo_uuid_dedup_map[idx] = el;
			}
			idx = `${ uuid_slot.filepath }::${ uuid_slot.uuid }`;
			if (!ToDo_uuid_dedup_map[idx]) {
				if (debug)  console.log("TODO   Project UUID: ", { idx, uuid_slot });

				ToDo_uuid_list.push(uuid_slot);
				ToDo_uuid_dedup_map[idx] = uuid_slot;
			}
		}
		else if (!uuid_slot) {
			console.log({ el });
			console.log(`Project ${ el.file } is known under multiple UUIDs and requires resyncing.`);
			let idx = `${ el.filepath }::${ el.uuid }`;
			if (!ToDo_uuid_dedup_map[idx]) {
				if (debug)  console.log("TODO   Project UUID: ", { idx, el });

				ToDo_uuid_list.push(el);
				ToDo_uuid_dedup_map[idx] = el;
			}
		}
	}
	
	for (const idx in projectName_map) {
		let el = projectName_map[idx];
		let uuid_slot = uuid_map[el.uuid];
		if (uuid_slot && uuid_slot.uuid !== el.uuid) {
			console.log(`Project ${ el.file } is known under multiple UUIDs and requires resyncing: ${ uuid_slot.uuid } !== ${ el.uuid }`);
			let idx = `${ el.filepath }::${ el.uuid }`;
			if (!ToDo_uuid_dedup_map[idx]) {
				if (debug)  console.log("TODO   Project UUID: ", { idx, el });

				ToDo_uuid_list.push(el);
				ToDo_uuid_dedup_map[idx] = el;
			}
			idx = `${ uuid_slot.filepath }::${ uuid_slot.uuid }`;
			if (!ToDo_uuid_dedup_map[idx]) {
				if (debug)  console.log("TODO   Project UUID: ", { idx, uuid_slot });

				ToDo_uuid_list.push(uuid_slot);
				ToDo_uuid_dedup_map[idx] = uuid_slot;
			}
		}
		else if (!uuid_slot) {
			console.log({ el });
			console.log(`Project ${ el.file } is known under multiple UUIDs and requires resyncing.`);
			let idx = `${ el.filepath }::${ el.uuid }`;
			if (!ToDo_uuid_dedup_map[idx]) {
				if (debug)  console.log("TODO   Project UUID: ", { idx, el });

				ToDo_uuid_list.push(el);
				ToDo_uuid_dedup_map[idx] = el;
			}
		}
	}
	
	for (const idx in uuid_map) {
		let el = uuid_map[idx];
		let uuid_slot = projectPath_map[el.filepath];
		if (uuid_slot.file !== el.file) {
			console.log(`UUID has been assigned to multiple projects which require resyncing: ${ uuid_slot.file } !== ${ el.file }`);
			let idx = `${ el.filepath }::${ el.uuid }`;
			if (!ToDo_uuid_dedup_map[idx]) {
				if (debug)  console.log("TODO   Project UUID: ", { idx, el });

				ToDo_uuid_list.push(el);
				ToDo_uuid_dedup_map[idx] = el;
			}
			idx = `${ uuid_slot.filepath }::${ uuid_slot.uuid }`;
			if (!ToDo_uuid_dedup_map[idx]) {
				if (debug)  console.log("TODO   Project UUID: ", { idx, uuid_slot });

				ToDo_uuid_list.push(uuid_slot);
				ToDo_uuid_dedup_map[idx] = uuid_slot;
			}
		}
		uuid_slot = projectName_map[el.file];
		if (uuid_slot.file !== el.file) {
			console.log(`UUID has been assigned to multiple projects which require resyncing: ${ uuid_slot.file } !== ${ el.file }`);
			let idx = `${ el.filepath }::${ el.uuid }`;
			if (!ToDo_uuid_dedup_map[idx]) {
				if (debug)  console.log("TODO   Project UUID: ", { idx, el });

				ToDo_uuid_list.push(el);
				ToDo_uuid_dedup_map[idx] = el;
			}
			idx = `${ uuid_slot.filepath }::${ uuid_slot.uuid }`;
			if (!ToDo_uuid_dedup_map[idx]) {
				if (debug)  console.log("TODO   Project UUID: ", { idx, uuid_slot });

				ToDo_uuid_list.push(uuid_slot);
				ToDo_uuid_dedup_map[idx] = uuid_slot;
			}
		}
	}
	
	// Next: produce fresh unique UUIDs for each project that needs resyncing...
	let uniq_uuid_block_map = {};
	ToDo_uuid_list = ToDo_uuid_list.map((el) => {
		// e.g. el.uuid = 'A60D8644-5A1C-4D29-8970-101000000001'
		//            --> '0B51171B-B10E-4EAC-8FFA-19226Axxxxxx'
		
		// generate unique new UUID:
		let n = Math.floor(Math.random() * 1E7);
		let uuid = '';
		
		for(;;) {
			let suffix = n.toString(16) + "010203";
			suffix = suffix.substring(0, 6);
			uuid = '0B51171B-B10E-4EAC-8FFA-19226A' + suffix;
			uuid = uuid.toUpperCase();
			
			if (uuid_map[uuid]) {
				++n;
				continue;
			}
			if (uniq_uuid_block_map[uuid]) {
				++n;
				continue;
			}
			break;
		}
		
		el.map_to_uuid = uuid;
		
		uniq_uuid_block_map[uuid] = uuid;
		
		return el;
	});

	if (debug)  console.log({ ToDo_uuid_list });
	
	// construct the remap dictionary now that we have all the old info PLUS sufficient unique remapping UUIDs
	// (in fact we may have a few too many as we didn't deduplicate on filepath/filename, but that will be resolved by our remapping map setup approach: last unique entry wins!)
	let remap_to_uuid = {};
	
	ToDo_uuid_list.forEach((el) => {
		remap_to_uuid[el.file] = el;
	});
	ToDo_uuid_list.forEach((el) => {
		// as we now have a unique SINGLE UUID per file, we MUST use that one, which is not per sé the new uuid listed in our current `el` record -- we can have multiple TODO records per project file!
		let must_remap_to = remap_to_uuid[el.file];
		remap_to_uuid[el.file] = must_remap_to;
		remap_to_uuid[el.filepath] = must_remap_to;
		remap_to_uuid[el.uuid] = must_remap_to;
	});

	// run the remapping:
	dest_files.forEach((l) => {
		let file_src = fs.readFileSync(l, 'utf8');
		let rewritten = false;

		// e.g. Project("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}") = "1D-RGB-color-gradient", "1D-RGB-color-gradient.vcxproj", "{3644E12D-D934-41FD-BF7E-83745A17E226}"
		//      Project("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}") = "libmupdf_CarvedSubset", "libmupdf_CarvedSubset.vcxitems", "{08FF3748-F1BD-40A1-9322-F93FD04C2350}"
		//      Project("{FAE04EC0-301F-11D3-BF4B-00C04F79EFBC}") = "MuPDFSharp", "..\..\MupdfSharp\MuPDFSharp.csproj", "{8E506476-FFAE-46B2-977D-45EB25ECF394}"
	    let prj_re = /^Project[^=]*?8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942[^=]*= "([^"]+)", "([^"]+?[.]vcx(?:proj|items))", "[{]([^}"]+?)[}]".*?$/gm;

		let m = prj_re.exec(file_src);
		let mx = { ...m };
		delete mx.input;
		if (debug)  console.log({ prj_re, mx });
		while (m) {
			let info = {
				name: m[1],
				file: path.basename(path.normalize(m[2])),
				filepath: path.normalize(m[2]),
				uuid: m[3].toUpperCase(),
				is_project_ref: true
			};
			if (debug)  console.log({line: m[0], info});

			let must_remap_to = (remap_to_uuid[info.file] || remap_to_uuid[info.filepath]);
			
			if (must_remap_to) {
				// regex info still points at the matching place...
				m.input = null;
				let line_pos = m.index;
				let old_line = m[0];
				let repl_re = new RegExp(info.uuid, 'ig');
				let new_line = old_line.replace(repl_re, must_remap_to.map_to_uuid);
				if (debug)  console.log("REMAPPING: ", { must_remap_to, info, prj_re, m, old_line, repl_re, new_line });
				else        console.log(`Must remap ${ info.file }...`);
				
				file_src = file_src.substring(0, line_pos) + new_line + file_src.substring(line_pos + old_line.length);
				
				// no need to reset the /prj_re/.lastIndex as we didn't change the length of the underlying content...
				//prj_re.lstIndex = 0;
				rewritten = true;
			}
			
			m = prj_re.exec(file_src);
		}
		
		// e.g.    <ProjectGuid>{BFD01D9C-AC53-64D2-BC7B-9C711B129D59}</ProjectGuid>
		//         <ProjectGuid>{8E506476-FFAE-46B2-977D-45EB25ECF394}</ProjectGuid>
		//         <ItemsProjectGuid>{08ff3748-f1bd-40a1-9322-f93fd04c2350}</ItemsProjectGuid>
		//
		//         <ProjectReference Include="libabseil-cpp.vcxproj">
		//           <Project>{a60d8644-5a1c-4d29-8970-7518ff3d0c57}</Project>
		//         </ProjectReference>

	    prj_re = /^\s*<ProjectGuid>[{]([^}]+?)[}]<\/ProjectGuid>.*?$/gm;

		m = prj_re.exec(file_src);
		mx = { ...m };
		delete mx.input;
		if (debug)  console.log({ prj_re, mx });
		while (m) {
			let info = {
				name: null,
				file: path.basename(path.normalize(l)),
				filepath: path.normalize(l),
				uuid: m[1].toUpperCase(),
				is_project_spec: true
			};
			if (debug)  console.log({line: m[0], info});

			let must_remap_to = (remap_to_uuid[info.file] || remap_to_uuid[info.filepath]);
			
			if (must_remap_to) {
				// regex info still points at the matching place...
				m.input = null;
				let line_pos = m.index;
				let old_line = m[0];
				let repl_re = new RegExp(info.uuid, 'ig');
				let new_line = old_line.replace(repl_re, must_remap_to.map_to_uuid);
				if (debug)  console.log("REMAPPING: ", { must_remap_to, info, prj_re, m, old_line, repl_re, new_line });
				else        console.log(`Must remap ${ info.file }...`);
				
				file_src = file_src.substring(0, line_pos) + new_line + file_src.substring(line_pos + old_line.length);
				
				// no need to reset the /prj_re/.lastIndex as we didn't change the length of the underlying content...
				//prj_re.lstIndex = 0;
				rewritten = true;
			}
			
			m = prj_re.exec(file_src);
		}

	    prj_re = /^\s*<ItemsProjectGuid>[{]([^}]+?)[}]<\/ItemsProjectGuid>.*?$/gm;

		m = prj_re.exec(file_src);
		mx = { ...m };
		delete mx.input;
		if (debug)  console.log({ prj_re, mx });
		while (m) {
			let info = {
				name: null,
				file: path.basename(path.normalize(l)),
				filepath: path.normalize(l),
				uuid: m[1].toUpperCase(),
				is_collection_spec: true
			};
			if (debug)  console.log({line: m[0], info});

			let must_remap_to = (remap_to_uuid[info.file] || remap_to_uuid[info.filepath]);
			
			if (must_remap_to) {
				// regex info still points at the matching place...
				m.input = null;
				let line_pos = m.index;
				let old_line = m[0];
				let repl_re = new RegExp(info.uuid, 'ig');
				let new_line = old_line.replace(repl_re, must_remap_to.map_to_uuid);
				if (debug)  console.log("REMAPPING: ", { must_remap_to, info, prj_re, m, old_line, repl_re, new_line });
				else        console.log(`Must remap ${ info.file }...`);
				
				file_src = file_src.substring(0, line_pos) + new_line + file_src.substring(line_pos + old_line.length);
				
				// no need to reset the /prj_re/.lastIndex as we didn't change the length of the underlying content...
				//prj_re.lstIndex = 0;
				rewritten = true;
			}
			
			m = prj_re.exec(file_src);
		}

	    prj_re = /<ProjectReference\s+Include="([^"]+)">[\s\r\n]*<Project>[{]([^}]+?)[}]<\/Project>[\s\r\n]*<\/ProjectReference>/g;

		m = prj_re.exec(file_src);
		mx = { ...m };
		delete mx.input;
		if (debug)  console.log({ prj_re, mx });
		while (m) {
			let info = {
				name: null,
				file: path.basename(path.normalize(m[1])),
				filepath: path.normalize(m[1]),
				uuid: m[2].toUpperCase(),
				is_crossref: true
			};
			if (debug)  console.log({line: m[0], info});

			let must_remap_to = (remap_to_uuid[info.file] || remap_to_uuid[info.filepath]);
			
			if (must_remap_to) {
				// regex info still points at the matching place...
				m.input = null;
				let line_pos = m.index;
				let old_line = m[0];
				let repl_re = new RegExp(info.uuid, 'ig');
				let new_line = old_line.replace(repl_re, must_remap_to.map_to_uuid);
				if (debug)  console.log("REMAPPING: ", { must_remap_to, info, prj_re, m, old_line, repl_re, new_line });
				else        console.log(`Must remap ${ info.file }...`);
				
				file_src = file_src.substring(0, line_pos) + new_line + file_src.substring(line_pos + old_line.length);
				
				// no need to reset the /prj_re/.lastIndex as we didn't change the length of the underlying content...
				//prj_re.lstIndex = 0;
				rewritten = true;
			}
			
			m = prj_re.exec(file_src);
		}
		
		// have we updated the project/solution file? if so, rewrite the corrected/synced result:
		if (rewritten) {
			console.log(`Updating ${ l }.`);
			fs.writeFileSync(l, file_src, 'utf8');
		}
	});

	console.log(`\n\n${ ToDo_uuid_list.length } project entries were re-assigned a UUID and/or resynced with their dependencies.\n\n`);
}
	break;
}

