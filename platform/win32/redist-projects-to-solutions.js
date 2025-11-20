
const path = require("path");
let fs = require("fs");

// load the main owemdjee readme to decode...

let src = fs.readFileSync("../../thirdparty/owemdjee/README.source.md", "utf8");

let chunk_re = /^(#+) ([^\n]+)/gm;

let chunks = [];
let m = chunk_re.exec(src);
while (m) {
	m.input = null;
	chunks.push(m);
	//console.log({m});
	
	m = chunk_re.exec(src);
}

const srclen = src.length;
let endpos = srclen;
for (let len = chunks.length, i = len - 1; i >=0; i--) {
	chunks[i].endpos = endpos;
	endpos = chunks[i].index;
	
	chunks[i].index += chunks[i][0].length;  // skip title itself for index into chunk content

	chunks[i].level = chunks[i][1].length;
	chunks[i].title = chunks[i][2];
}

let chapters = [];
chunks.forEach(function (elem) {
	let content = src.substring(elem.index, elem.endpos).trim();
	
	// extract the '**fmt** [�](./fmt)' items:
	let item_re = /\*\*([^*]+)\*\* \[[^\]]+\]\(([^)]+)\)/gm;
	let items = [];
	let m = item_re.exec(content);
	while (m) {
		m.input = null;
		
		let id = m[1];
		let pad = m[2].trim().replace(/^\.+\//, '');
		//console.log({m, id, pad});
		items.push({
			id,
			pad
		});
		
		m = item_re.exec(content);
	}
	
	chapters.push({
		index: elem.index,
		endpos: elem.endpos,
		level: elem.level,
		title: elem.title,
		content,
		items,
	});
});
//console.log("chunks:", chunks);
//console.log({chapters});

let sln_filenames = fs.readdirSync("./").filter(function (f) {
	return f.endsWith(".sln");
});
//console.log({sln_filenames});

chapters.push({
	level: -1,
	title: "******",
	solutions: sln_filenames,
});

// drop all chunks which are at 'level 1', i.e. the 'all' library and the top title:
chapters = chapters.filter(function (el) {
	return el.level !== 1;
});

if (!fs.existsSync("./redist-projects-to-solutions.mapping.json")) {
	// generate a provisional mapping json file as a first suggestion (to be human edited before use!)
	chapters.forEach(function (elem) {
		elem.solutions = sln_filenames;
		
		elem.solutions.unshift("--ignore--");
		
		if (elem.items && elem.items.length > 0) {
			delete elem.content;
			
			// only keep item.id, not item.pad:
			elem.items.forEach(function (prj) {
				delete prj.pad;
			});
		}
	});
	let json = JSON.stringify(chapters, null, 2);
	fs.writeFileSync("./redist-projects-to-solutions.mapping.json", json, "utf8");
}
else {
	let mapping_src = fs.readFileSync("./redist-projects-to-solutions.mapping.json", "utf8");
	// accept trailing commas in chunks:
	mapping_src = mapping_src.replace(/,[\s\r\n]*[}]/g, '\n}').replace(/,[\s\r\n]*\]/g, '\n]');
	//console.log(mapping_src);
	let mapping_info = JSON.parse(mapping_src);

	//let json = JSON.stringify(mapping_info, null, 2);
	//fs.writeFileSync("./redist-projects-to-solutions.mapping.json", json, "utf8");
	
	// collect the actual MSVC project filenames:
	let vcxproj_filenames = fs.readdirSync("./").filter(function (f) {
		return f.endsWith(".vcxproj");
	});
	//console.log({vcxproj_filenames});
	
	// now apply the mapping:
	
	//console.log("mapping_info:", mapping_info);

	// preload all solution files:
	let solutions = {};
	sln_filenames.forEach(function (sln_fn) {
		let sln_src = fs.readFileSync(sln_fn, "utf8");
		
		solutions[sln_fn] = {
			content: sln_src,
			id: sln_fn
		};
	});

	// extract master project lines from master solution file:
	const master_prj_list_src = solutions["m-dev-list.sln"].content;
	let master_prj_re = /Project\("\{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942\}"\) = "([^"]+)", ([^\n]+)/gm;
	let master_prj_list_arr = [];
	let mm = master_prj_re.exec(master_prj_list_src);
	while (mm) {
		mm.input = null;
		//console.log({mm});
		
		let id = mm[1];
		let remain = mm[2].trim();
		let line = mm[0].trim();
		//console.log({mm, line, id, remain});
		master_prj_list_arr.push({
			id,
			line,
			//remain
		});
		
		mm = master_prj_re.exec(master_prj_list_src);
	}
	//console.log({master_prj_list_arr});
	

	// apply mapping...
	
	for (const mapping of mapping_info) {
		// ignore chunks which have not been set up yet:
		if (!mapping.items || !mapping.solutions || mapping.solutions[0] === "--ignore--") {
			console.warn("skipping mapping: ", mapping.level, ":", mapping.title);
			continue;
		}
		
		//console.log("mapping: ", mapping);
	
		// augment each of these MSVC solutions with the projects listed in items[] (iff found as a 'partial' in the master projects list)
		mapping.solutions.forEach(function (sln_id) {
			if (!(sln_id in solutions)) {
				console.error("BAD SLN spec: unknown solution mentioned in mapping file: ", sln_id);
				return;
			}
			
			let sln_rec = solutions[sln_id];

			mapping.items.forEach(function (prj_id) {
				//console.log({prj_id});
				
				master_prj_list_arr.forEach(function (item) {
					if (!item.id.includes(prj_id.id))
						return;
					
					// add item to SLN:
					sln_rec.content += `\nEndProject\n${ item.line }\nEndProject`;
					console.log("Added", prj_id.id, "==>", item.id, "to the MSVC solution file:", sln_id);
				});
			});
		});
	}

	// save the patched SLN files:
	sln_filenames.forEach(function (sln_id) {
		let sln_rec = solutions[sln_id];

		//console.log({sln_rec});
		fs.writeFileSync(sln_id, sln_rec.content, "utf8");
	});
}


