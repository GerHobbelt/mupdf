//
// Sort lines without regard for '#' comment prefixes nor upper/lowercase; use this sort method for the project name/directory lists available in various tool scripts.
// 
//     cat ~/Downloads/__sort_lines.txt | node ./do-sort-lines.js > ~/Downloads/__out_lines.txt
//

process.stdin.resume();
process.stdin.setEncoding('utf8');

let records = [];

let surplus = "";

process.stdin.on('data', function(chunk) {
	surplus += chunk;
    let lines = surplus.split("\n");

    surplus = lines.pop();

	records = records.concat(lines);
	
    //console.log('\n' + lines.join('\n') + '\n');
    //console.log({count: records.length});
});

process.stdin.on('end', function() {
    let lines = surplus.split("\n");

	records = records.concat(lines);
	records = records
	.filter(function (line) { return line.trim().length > 0; });
	
    //console.log('\n' + lines.join('\n') + '\n');
    //console.log({count: records.length});
	
	// lines example:
	//
	//    # ADE-graph-management
	//	  OpenCL-Headers
	//
	
	records = records
	.map(function (line) { 
		let s = line.replace(/#[^\s]+#/g, '#').replace(/#/g, '').trim();
		return { key: s.toUpperCase(), line };
	});
	
	records.sort(function (a, b) {
	  const nameA = a.key; 
	  const nameB = b.key;
	  if (nameA < nameB) {
		return -1;
	  }
	  if (nameA > nameB) {
		return 1;
	  }
	  // names must be equal
	  return 0;
	});

	let commented = false;
	
	records = records
	.map(function (line) { 
		let l = line.line;
		let c = /^#/.test(l);
		if (c !== commented) {
			// insert an empty line between commented items and non-commented items for improved readability.
			l = '\n' + l;
			commented = c;
		}
		return l;
	});
	
	console.log(records.join('\n') + '\n');
});
