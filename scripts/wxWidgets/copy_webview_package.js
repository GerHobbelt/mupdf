//
// find and copy the WebView2 package files to a known FIXED location.
// 
// usage: run as
//
//     node ./copy.js PackagesRootDir Destdir ProjectTargetDir NuGetRunTimeMode
//

let fs = require('fs');
let path = require('path');
let glob = require('@gerhobbelt/glob');
const cp = require('child_process');

let DEBUG = 0;
const globDefaultOptions = {
  debug: (DEBUG > 4),
  matchBase: true, // true: pattern starting with / matches the basedir, while non-/-prefixed patterns will match in any subdirectory --> act like **/<pattern>
  silent: false,   // report errors to console.error UNLESS those are already emitted (`strict` option)
  strict: true,    // emit errors
  realpath: true,
  realpathCache: {},
  follow: false,
  dot: false,
  mark: true,    // postfix '/' for DIR entries
  nodir: true,
  sync: false,
  nounique: false,
  nonull: false,
  nosort: true,
  nocase: true,     //<-- uncomment this one for total failure to find any files >:-((
  stat: false,
  noprocess: false,
  absolute: false,
  maxLength: Infinity,
  cache: {},
  statCache: {},
  symlinks: {},
  cwd: null,    // changed to, during the scan
  root: null,
  nomount: false
};


function unixify(path) {
  return path.replace(/\\/g, '/');
}

let rawPackagesPath = process.argv[2];
if (!rawPackagesPath) {
    console.error("Missing packages ROOT directory argument");
	console.error("call:\n  copy.js PackagesRootDir DestDir ProjectBinaryTargetDir NuGetRunTimeMode");
    process.exit(1);
}
let packagesPath = unixify(path.resolve(rawPackagesPath));
if (!fs.existsSync(packagesPath) || !fs.lstatSync(packagesPath).isDirectory()) {
    console.error("must specify valid packages root directory argument.", {packagesPath}, " is not a packages directory.");
	console.error("call:\n  copy.js PackagesRootDir DestDir ProjectBinaryTargetDir NuGetRunTimeMode");
    process.exit(1);
}

const globConfig = Object.assign({}, globDefaultOptions, {
  nodir: false,
  cwd: packagesPath
});


let rawBinaryOutputPath = process.argv[4];
let binOutPath = unixify(path.resolve(rawBinaryOutputPath));
if (!fs.existsSync(binOutPath) || !fs.lstatSync(binOutPath).isDirectory()) {
	console.error("must specify valid build output ('binary target') directory", {binOutPath});
	console.error("call:\n  copy.js PackagesRootDir DestDir ProjectBinaryTargetDir NuGetRunTimeMode");
	process.exit(1);
}


let nugetMode = process.argv[5] || '';
if (nugetMode === '') {
	console.error("must specify valid project/NuGet runtime build mode");
	console.error("call:\n  copy.js PackagesRootDir DestDir ProjectBinaryTargetDir NuGetRunTimeMode");
  let pathWithWildCards = '*WebView*';
	glob(pathWithWildCards, globConfig, function processGlobResults(err, files) {
	  if (err) {
	    throw new Error(`glob scan error: ${err}`);
	  }

	  files.sort();

	  // grab the first slot: that one's assumed to be the base directory of the installed WebView2 package:
	  let basedir = files[0];
	  if (!fs.existsSync(basedir) || !fs.lstatSync(basedir).isDirectory()) {
	    process.exit(2);
	  }

		let binsrcdir = `${basedir}runtimes/*`;
		const globConfig4Listing = Object.assign({}, globDefaultOptions, {
		  nodir: false,
		  cwd: packagesPath
		});

		glob(binsrcdir, globConfig4Listing, function processGlobResults(err, files) {
		  if (err) {
		    throw new Error(`glob scan error: ${err}`);
		  }

		  files.sort();
		  
		  files = files.map((l) => {
			  return l
			  .replace(/^.*runtimes[\\\/]/, '')
			  .replace(/[\\\/]$/, '');
		  });

		  console.error("\nAvailable modes:", files);
	    process.exit(1);
		});
	});

  // DO NOT call process.exit() while you've still got async functions you want to see completed!
  //
  // This is indirectly related to https://stackoverflow.com/questions/46914025/node-exits-without-error-and-doesnt-await-promise-event-callback#answer-46916601
  // but it is unmentioned there that process.exit() rather *aborts* the applications, not caring about any pending callback at all!
  // As we need to exit the main code path, we invoke 'return' instead:
  //
	//process.exit(1);
	return 1;
}


let rawDestPath = process.argv[3];
let destPath = unixify(path.resolve(rawDestPath));
if (!fs.existsSync(destPath) || !fs.lstatSync(destPath).isDirectory()) {
    console.error("must specify valid destination directory");
	console.error("call:\n  copy.js PackagesRootDir DestDir ProjectBinaryTargetDir NuGetRunTimeMode");
    process.exit(1);
}

let pathWithWildCards = '*WebView*';
glob(pathWithWildCards, globConfig, function processGlobResults(err, files) {
  if (err) {
    throw new Error(`glob scan error: ${err}`);
  }

  files.sort();

  if (files.length < 15) {
	console.error({files});
    throw new Error(`glob scan error: expected at least 15 files in the installed WebView2 NuGet package.`);
  }

  // grab the first slot: that one's assumed to be the base directory of the installed WebView2 package:
  let basedir = files[0];
  if (!fs.existsSync(basedir) || !fs.lstatSync(basedir).isDirectory()) {
    console.error("detected (*assumed; first slot in list*) package base path is not a directory: check your arguments to this script!");
    console.error("found files/directories list:\n", {files});
    process.exit(2);
  }

  console.error("Going to copy files from", basedir, "to", destPath);

  // https://stackoverflow.com/questions/52736919/nodejs-exec-command-fails-for-some-windows-commands-with-no-clear-error-message
  let something_copied = false;
  try { 
    cp.execSync(`robocopy "${basedir}" "${destPath}" *.* /PURGE /MIR`, { stdio: 'inherit' }); 
  } catch (err) { 
    something_copied = true;
  	if (err.status !== 1) {
      console.error("Robocopy return code:", err.status);             // get the return code
      console.error("Robocopy error output:\n", err.output.toString());  // get robocopy's full output
      throw new Error(`robocopy error.`);
    }
  }


  // grab the runtime DLL and copy it to the build target directory:
  let binsrcdir = `${basedir}runtimes/${nugetMode}/native/`;

	console.error("Going to copy DLL file from", binsrcdir, "to", binOutPath);

	try {
		cp.execSync(`robocopy "${binsrcdir}" "${binOutPath}" *.dll`, { stdio: 'inherit' });
	} catch (err) {
		if (err.status & 0x01) {
			something_copied = true;
		}
		// tolerate 'EXTRA FILES': retcode can be {0,1,2,3} for OK situations
		if (err.status & ~0x03) {
			console.error("Robocopy return code:", err.status);             // get the return code
			console.error("Robocopy error output:\n", err.output.toString());  // get robocopy's full output
			throw new Error(`robocopy error.`);
		}
	}

  console.error(`====================== Done ${ something_copied ? "(Files copied)" : "(Nothing to copy; targets are up-to-date)" } ========================`);
  process.exit(0);
});




