//
// Sort the project entries in a given Microsoft Visual Studio .SLN solution file for easier comparison/remixing with other solutions
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

// clean up sln file before we start: predictable lines' content!
slnstr = slnstr.split('\n')
.map((l) => l.replace(/\s+$/, '').replace(/\t/g, '    '))
.join('\n');

let global_section = slnstr.replace(/^[^]+?\nGlobal\n/, '\nGlobal\n').replace(/\nEndGlobal\n[\s\S]+$/, '\nEndGlobal\n');
let header_section = slnstr.replace(/\nProject[(][\s\S]+$/, '\n');

let projects_section = slnstr
.replace(/^[\s\S]+?\nProject[(]/, '\nProject(')
.replace(/\nGlobal\n[\s\S]+?\nEndGlobal\n/, '\n');

let prjarr = projects_section.replace(/Project[(]/g, '\x01Project(')
.split('\x01')
.map((l) => l.trim())
.filter((l) => l.length)
.map((l, idx) => {
	if (/ProjectSection[(]/.test(l) || ! /proj",/.test(l) || /[.][.][\\\/]/.test(l)) {
		return { l, rank: idx + 1 };
	}
	else {
		let prj_name = l.replace(/^Project[(].+?[)]\s*=\s*"([^"]+)",[\s\S]+$/, '$1')
		.toLowerCase();

		return { l, rank: 1e6, prj_name };
	}
})

if (debug) console.log({global_section, header_section, projects_section, prjarr});

function compare(a, b) {
    return (a === b ? 0
        : (a > b ? 1 : -1));
}

prjarr.sort((a, b) => {
	let diff = a.rank - b.rank;
	if (diff)
		return diff;
	diff = compare(a.prj_name, b.prj_name);
	//console.log({a, b, diff})
	return diff;
})

if (debug) console.log("Sorted: ", {prjarr});

slnstr = header_section + prjarr.map((el) => el.l).join('\n') + global_section;

slnstr = slnstr.replace(/    /g, '\t');

if (debug) console.log({slnstr});

fs.writeFileSync(slnpath, slnstr, 'utf8');

process.exit(0);

