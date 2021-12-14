//
// cleanup folder hierarchy in vcxproj.filters for an existing project 
// --> removes empty folders
// 
// usage:
//
//     node ./clean-vcxproj-filters.js project-A.vcxproj
//
// or (using [bash for windows] from your [git for windows] rig:
//
//     for f in *.vcxproj ; do node ./clean-vcxproj-filters.js $f ; done
//
// when you want to clean all your projects throughout the entire Visual Studio solution.
//


let fs = require('fs');
let path = require('path');


let filepath = process.argv[2];
if (!fs.existsSync(filepath)) {
	console.error("must specify valid vcxproj file");
	process.exit(1);
}

console.log("PROJECT:", filepath);

filepath += ".filters";
if (!fs.existsSync(filepath)) {
	console.error(filepath, ": file does not exist: broken project?");
	process.exit(1);
}
let src = fs.readFileSync(filepath, 'utf8');

let filters = new Set();
let folders = new Set();

void src
.replace(/<Filter>(.*?)<\/Filter>/g, (m, p1) => {
	filters.add(p1);
})
.replace(/<Filter Include="(.*?)">/g, (m, p1) => {
	folders.add(p1);
});

// decide which folders to keep: any folder that matches a filter or *starts* a filter, i.e. it's a *parent folder*:
let delset = new Set();
for (let f of folders) {
	if (filters.has(f))
		continue;
	let keep = false;
	for (let m of filters) {
		if (m.startsWith(f)) {
			keep = true;
			break;
		}
	}
	if (!keep) {
		delset.add(f);
	}
}

if (delset.size > 0) {
	console.log("delete folders:", delset);
	for (let f of delset) {
		folders.delete(f);
	}
}
//console.log("KEEP folders:", folders);

src = src
.replace(/<Filter Include="([^"]*?)">[^]*?<\/Filter>/g, (m, p1) => {
	if (folders.has(p1))
		return m;
	return "";
})
.replace(/<ItemGroup>[\s\r\n]*<\/ItemGroup>/g, '')
// and trim out empty lines:
.replace(/[\s\r\n]+\n/g, '\n');

fs.writeFileSync(filepath, src, 'utf8');
console.log("\n");


