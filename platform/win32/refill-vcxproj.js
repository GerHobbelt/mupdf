// helper for refill-vcxproj.sh shell script
// 
// produces/infers the project's source directory, whenever possible.
// 

const fs = require('fs');
const path = require('path');


if (process.argv.length !== 3) {
	console.error("refill-vcxproj.js: please provide the project filename as the sole (single) argument.")
	process.exit(2);
}

const projpath = process.argv[2];
const scriptpath = path.dirname(process.argv[1]);

let projname = path.basename(projpath, ".vcxproj");

//console.error({projname, scriptpath})


// now try to map the project name to a source directory:

function checkDirAndReportPlusExitOnSuccess(p) {
	if (fs.existsSync(p)) {
		p = p.replace(scriptpath + "/", "");
		console.log(p);
		process.exit(0);
	}
}

let testpath = `${scriptpath}/../../thirdparty/owemdjee/${projname}`;

checkDirAndReportPlusExitOnSuccess(testpath);

let nonlib_projname = projname.replace(/^lib/, '');
testpath = `${scriptpath}/../../thirdparty/owemdjee/${nonlib_projname}`;

checkDirAndReportPlusExitOnSuccess(testpath);

testpath = `${scriptpath}/../../thirdparty/${projname}`;

checkDirAndReportPlusExitOnSuccess(testpath);

testpath = `${scriptpath}/../../thirdparty/${nonlib_projname}`;

checkDirAndReportPlusExitOnSuccess(testpath);

if (projname.startsWith("wxw-samples-")) {
	let wx_projname = projname.replace(/^wxw-samples-/, '');
	testpath = `${scriptpath}/../../thirdparty/owemdjee/wxWidgets/samples/${wx_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);
}

if (projname.startsWith("wxw-demos-")) {
	let wx_projname = projname.replace(/^wxw-demos-/, '');
	testpath = `${scriptpath}/../../thirdparty/owemdjee/wxWidgets/demos/${wx_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);
}

if (projname.startsWith("wxw-utils-")) {
	let wx_projname = projname.replace(/^wxw-utils-/, '');
	testpath = `${scriptpath}/../../thirdparty/owemdjee/wxWidgets/utils/${wx_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);
}

if (projname.startsWith("wxCharts-sample-")) {
	let wx_projname = projname.replace(/^wxCharts-sample-/, '');
	testpath = `${scriptpath}/../../thirdparty/owemdjee/wxCharts/samples/${wx_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);
}

if (projname.endsWith("_tests")) {
	let tst_projname = projname.replace(/_tests$/, '');
	testpath = `${scriptpath}/../../thirdparty/owemdjee/${tst_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);

	testpath = `${scriptpath}/../../thirdparty/${tst_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);
}

if (projname.endsWith("_tools")) {
	let tool_projname = projname.replace(/_tools$/, '');
	testpath = `${scriptpath}/../../thirdparty/owemdjee/${tool_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);

	testpath = `${scriptpath}/../../thirdparty/${tool_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);
}

if (projname.endsWith("_demos")) {
	let tst_projname = projname.replace(/_demos$/, '');
	testpath = `${scriptpath}/../../thirdparty/owemdjee/${tst_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);

	testpath = `${scriptpath}/../../thirdparty/${tst_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);
}

if (projname.endsWith("_benchmark")) {
	let tst_projname = projname.replace(/_benchmark$/, '');
	testpath = `${scriptpath}/../../thirdparty/owemdjee/${tst_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);

	testpath = `${scriptpath}/../../thirdparty/${tst_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);
}

if (projname.endsWith("_examples")) {
	let tst_projname = projname.replace(/_examples$/, '');
	testpath = `${scriptpath}/../../thirdparty/owemdjee/${tst_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);

	testpath = `${scriptpath}/../../thirdparty/${tst_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);
}

if (projname.endsWith("_DLL")) {
	let tst_projname = projname.replace(/_DLL$/, '');
	testpath = `${scriptpath}/../../thirdparty/owemdjee/${tst_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);
}

if (projname.startsWith("libboost-")) {
	let tst_projname = projname.replace(/^libboost-/, '');
	testpath = `${scriptpath}/../../thirdparty/owemdjee/boost/boost/${tst_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);
}

if (projname.startsWith("wxw-samples-")) {
	let tst_projname = projname.replace(/^wxw-samples-/, '');
	testpath = `${scriptpath}/../../thirdparty/owemdjee/wxWidgets/samples/${ tst_projname.replace(/-/, '/') }`;

	checkDirAndReportPlusExitOnSuccess(testpath);
}


// now things get a little hairier: we need to MAP to projectname to a submodule directory path...

const projectMap = {
	"cryptest": "cryptopp",
	"cryptlib": "cryptopp",
	"libatlas": "math-atlas",
	"libblosc": "c-blosc2",
	"libbtree": "cpp-btree",
	"binlog_bread": "binlog",
	"binlog_brecovery": "binlog",
	"libchm_io": "CHM-lib",
	"libcli11-examples": "cli11/examples",
	"libclipp_examples": "clipp/examples",
	"libcurl_examples": "curl",
	"libdjvu_io": "djvulibre",
	"libdtldiff": "dtl-diff-template-library",
	"libenkiTaskScheduler": "enkiTS-TaskScheduler",
	"libfamilia": "lda-Familia",
	"libgdiff": "google-diff-match-patch",
	"libhashmapbtree": "parallel-hashmap",
	"libhdiff": "HDiffPatch",
	"libmarl": "google-marl",
	"libmegamime": "mime-mega",
	"libnanotimer": "plf_nanotimer",
	"libprocess": "subprocess",
	"libpromise": "promise-cpp",
	"libQuickJSpp": "QuickJS-C++-Wrapper",
	"libQuickJSpp2": "QuickJS-C++-Wrapper2",
	"librobust-bibtex": "bibtex-robust-decoder",
	"libsvgcharter": "svg-charter",
	"libtbb": "oneTBB",
	"libtidyhtml": "tidy-html5",
	"libtoml": "tomlpp",
	"libtorch": "pytorch",
	"libuint128": "uint128_t",
	"libupskirt": "upskirt-markdown",
	"libXMP-Toolkit": "XMP-Toolkit-SDK",
	"qrencode-test": "libqrencode",
	"libicu": "unicode-icu",
	"libjansson": "json-jansson",
	"libjpeg-xl-benchmark": "jpeg-xl",
	"libjpeg-xl-test": "jpeg-xl",
	"libcxxtest_catch2_2_gtest": "/cxxtest_catch_2_gtest",
	"otl-ml": "OptimizationTemplateLibrary",
	"manticore": "manticoresearch",
	"libYAC": "YACLib",
	"libhnsw": "hnswlib",
	"libnms": "nmslib",
	"libyaml-tests": "libyaml",
	"uberlogger": "uberlog",
	"uberlog_test": "uberlog",
	"filesystem_tests_exception": "filesystem/test",
	"filesystem_tests_fs": "filesystem/test",
	"filesystem_tests_fwd": "filesystem/test",
	"filesystem_tests_multifile": "filesystem/test",
	"googletest-demos": "googletest/googletest/test",
	"googlegtest-demos": "googletest/googletest/test",
	"googlelog-unittest": "glog",
	//googlemock-demos
	//googletest-demo-invalid-param-name1
	//googletest-demo-invalid-param-name2
	//googletest-samples
	"png-pmt-tools-lib": "pmt-png-tools",
	"png-pmt-tools": "pmt-png-tools",
	"tesseract-unittests": "tesseract/unittest",
	"wxCharts-tests": "wxCharts/tests",
	//wxFormBuilder-test-events
	//wxFormBuilder-test
	//wxFormBuilder-tinyXML
	//wxFormBuilderPlugin-SDK
	"wxFormBuilderPlugin-additional": "wxFormBuilder/plugins/additional",
	"wxFormBuilderPlugin-common": "wxFormBuilder/plugins/common",
	"wxFormBuilderPlugin-containers": "wxFormBuilder/plugins/containers",
	"wxFormBuilderPlugin-forms": "wxFormBuilder/plugins/forms",
	"wxFormBuilderPlugin-layout": "wxFormBuilder/plugins/layout",
	"wxScintilla": "wxWidgets/src/stc/scintilla",
	//wxw-build-bakefiles-wxpresets-sample
	//wxw-misc-theme-test
	//wxw-samples-aui-minimal-static-build
	//wxw-samples-console-minimal-static-build
	//wxw-samples-dll-exe
	//wxw-samples-dll-test-exe
	"wxw-samples-htmlbox": "wxWidgets/samples/htlbox",
	"wxw-samples-maskedctrl": "wxWidgets\samples\maskededit",
	//wxw-samples-minimal-static-build
	//wxw-samples-propgrid-minimal-static-build
	"wxw-tests-benchmarks": "wxWidgets/tests/benchmarks",
	"wxw-tests": "wxWidgets/tests",
	"wxw-utils-helpview-client": "wxWidgets/utils/helpview",
	"wxw-utils-mk-ctable": "wxWidgets/misc/unictabl",
	"zmq_examples": "libzmq",
	"zmq_tests": "libzmq",
	"zmq_tools": "libzmq",
};

for (const key in projectMap) {
	let value = projectMap[key];
	if (projname === key) {
		testpath = `${scriptpath}/../../thirdparty/owemdjee/${value}`;

		checkDirAndReportPlusExitOnSuccess(testpath);

		testpath = `${scriptpath}/../../thirdparty/${value}`;

		checkDirAndReportPlusExitOnSuccess(testpath);
	}
}

// no mapping found.
console.error(`No known mapping for ${projname}.`);
process.exit(1);