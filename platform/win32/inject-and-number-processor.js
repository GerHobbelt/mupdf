//
// analyze obsidian lab notes MD filee and chop it into sections, each of which will get a header, etc.
//

const path = require('path');
const fs = require('fs');
const debug = false;

if (debug) console.log({nr: process.argv.length, args: process.argv});

let f = process.argv[2];

let src = fs.readFileSync(f, "utf8");

src = src
.replace(/\r\n/g, '\n');


let chunks = src.split(/^```/gm);




function analyze_and_split(l, try_again) {
	// remove empty lines at start and end of chunk IFF 2 or LESS.
	if (/^\n\n?[^\n]/.test(l)) {
		l = l.replace(/^\n+/, '');
	}
	if (/[^\n]\n\n?$/.test(l)) {
		l = l.replace(/\n+$/, '');
	}
	
	// find the largest number of consecutive NLs and split there, minimum 3:
	let re = !try_again ? /\n\n\n+/g : /\n\n+/g;
	let m = re.exec(l);
	let spot = -1;
	let old_len = 0;
	while (m) {
		let len = m[0].length;
		if (len > old_len) {
			spot = m.index;
			old_len = len;
		}
		m = re.exec(l);
	}

	let rv = { top: l, split: null, bot: null };
	
	if (spot >= 0) {
		rv = { 
			top: l.substr(0, spot).trim(),
			split: `NEWLINE COUNT: ${ old_len }`,
			bot: l.substr(spot).trim()
		};
	}
	else {
		// see if line ends with ':". Then it's a LEAD, otherwise a TAIL:
		if (/:$/.test(l)) {
			if (!try_again) {
				rv = analyze_and_split(l, true);
				rv.split += " [COLON PHRASE]";
			}
			else {
				rv = { 
					top: null,
					split: "[COLON PHRASE]",
					bot: l
				};
			}
		}
	}		
	
	return rv;
}


// when there's no ```` code chunks yet, try to split the log based on 'empty lines' (minimum 5)
if (chunks.length === 1) {
	chunks = src.split(/\n\n\n\n\n+/g).filter((l) => l.trim().length > 0);
	
	// now remap to ensure we mimic the same layout as when we'ld have had code chunks in there:
	let a = [];
	
	for (let idx = 1, arrlen = chunks.length; idx < arrlen; idx++) {
		a[0 - 2 + idx * 2] = { 
			top: '',
			split: `[NADA]`,
			bot: ''
		};

		a[1 - 2 + idx * 2] = chunks[idx];
	}
	a[0] = { 
		top: chunks[0],
		split: `[NADA]`,
		bot: ''
	};
	
	chunks = a;

	if (debug) console.log({chunks, count: chunks.length / 2 })
}

	
	

// proceessing:


for (let idx = 1, arrlen = chunks.length; idx < arrlen; idx += 2) {
	let l = chunks[idx - 1];
	let lc = chunks[idx];
	let lt = chunks[idx + 1];

	// bundle post-text with item: every ODD index is a code chunk. 
	//
	// Apply heuristics to discover both LEADING and TRAILING notes attached.
	if (typeof l === "string") {
		chunks[idx - 1] = analyze_and_split(l);
	}
	if (typeof lt === "string") {
		chunks[idx + 1] = analyze_and_split(lt);
	}

	let rv =  {
		lead: l ? l.bot : "",
		code: lc,
		trail: lt ? lt.top : ""
	};

	chunks[idx] = rv;
}

if (debug) if (chunks.length > 1) console.log({chunks, count: chunks.length / 2 })


for (let idx = 0, arrlen = chunks.length; idx < arrlen; idx += 2) {
	let l = chunks[idx];

	if (idx % 2 === 0)
		chunks[idx] = chunks[idx].top;
	else
		chunks[idx] = null;
}

if (debug) console.log({chunks, count: chunks.length / 2 })

let nr = 1;
for (let idx = 1, arrlen = chunks.length; idx < arrlen; idx += 2) {
	let l = chunks[idx];

	let n = '00000' + nr;
	nr++;
	n = n.substr(n.length - 5);
	let rv = '\n\n\n\n\n\n\n' + `##### Item ♯${ n }` + '\n\n\n';
	
	rv += (l.lead || '') + '\n\n\n```\n';
	rv += l.code.trim() + '\n```\n';
	rv += (l.trail || '') + '\n\n';

	chunks[idx] = rv;
}

	
chunks = chunks.filter((l) => l != null);

if (debug) console.log({chunks, count: chunks.length / 2 })

// do NOT modify pages which don't havee a single item (code chunk) yet!

if (chunks.length > 1) {
	src = chunks.join('\n');

	if (debug) console.log(src)


	fs.writeFileSync(f, src, "utf8");

	console.log(`File ${f} has been patched.`);
}
else {
	console.log(`File ${f} has no logbook notes (code chunks) yet, skipped!`);
}

