// helper 
// 
// produces a provisional Project entry for a MSVC solution file.
// 

const path = require("path");
const fs = require("fs");

if (process.argv.length < 3) {
	console.error("mk_project_line_for_sln.js: please provide the project filename(s) as argument(s).")
	process.exit(2);
}

const scriptpath = path.dirname(process.argv[1]);

function checkDirAndReportPlusExitOnSuccess(p, n) {
	if (fs.existsSync(p)) {
    p = p.replace(scriptpath + "/", "");
    console.log(`
Project("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}") = "${n}", "${p}", "{A60D8644-5A1C-4D29-8970-101000000001}"
EndProject
    `);
	}
}

for (let i = 2; i < process.argv.length; i++) {
  const projpath = process.argv[i];
  let projname = path.basename(projpath, ".vcxproj");

  //console.error({projname, scriptpath})

  // now try to map the project name to a source directory:

  checkDirAndReportPlusExitOnSuccess(projpath, projname);
}
