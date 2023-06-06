// helper for refill-vcxproj.sh shell script
// 
// produces/infers the project's source directory, whenever possible.
// 

const fs = require('fs');
const path = require('path');
const debug = false;

if (process.argv.length !== 3) {
	console.error("refill-vcxproj.js: please provide the project filename as the sole (single) argument.")
	process.exit(2);
}

const projpath = process.argv[2];
const scriptpath = path.dirname(process.argv[1]);

let projname = path.basename(projpath, ".vcxproj");

if (debug) 
	console.error({projname, scriptpath})


// now try to map the project name to a source directory:

function checkDirAndReportPlusExitOnSuccess(p) {
	if (fs.existsSync(p)) {
		p = p.replace(scriptpath + "/", "");
		console.log(p);
		process.exit(0);
	}
}

let testpath;

if (projname.startsWith("libboost-")) {
	let tst_projname = projname.replace(/^libboost-/, '');
	testpath = `${scriptpath}/../../thirdparty/owemdjee/boost/libs/${tst_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);
}

if (projname.startsWith("wxw-samples-")) {
	let tst_projname = projname.replace(/^wxw-samples-/, '');
	testpath = `${scriptpath}/../../thirdparty/owemdjee/wxWidgets/samples/${ tst_projname.replace(/-/, '/') }`;

	checkDirAndReportPlusExitOnSuccess(testpath);
}

if (projname.startsWith("wxw-samples-sockets-")) {
	let tst_projname = projname.replace(/^wxw-samples-sockets-/, '');
	testpath = `${scriptpath}/../../thirdparty/owemdjee/wxWidgets/samples/sockets/${ tst_projname.replace(/-/, '/') }`;

	checkDirAndReportPlusExitOnSuccess(testpath);
}

if (projname.startsWith("wxw-samples-ipc-")) {
	let tst_projname = projname.replace(/^wxw-samples-ipc-/, '');
	testpath = `${scriptpath}/../../thirdparty/owemdjee/wxWidgets/samples/ipc/${ tst_projname.replace(/-/, '/') }`;

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
	"libCHM": "CHM-lib",
	"libdjvu": "djvulibre",
	"libdtldiff": "dtl-diff-template-library",
	"libenkiTaskScheduler": "enkiTS-TaskScheduler",
	"libfamilia": "lda-Familia",
	"libgdiff": "google-diff-match-patch",
	"libhashmapbtree": "parallel-hashmap",
	"libhdiff": "HDiffPatch",
	"libmarl": "google-marl",
	"libmarl_tests": "google-marl",
	"libmegamime": "mime-mega",
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
	"libuint128_tests": "uint128_t",
	"libupskirt": "upskirt-markdown",
	"libXMP-Toolkit": "XMP-Toolkit-SDK",
	"qrencode-test": "libqrencode",
	"libicu": "unicode-icu",
	"libicu_tools": "unicode-icu",
	"libicu_tests": "unicode-icu",
	"liblcms2_tests": "lcms2",
	"libleptonica_tools": "leptonica",
	"libjansson": "json-jansson",
	"libjpeg-xl-benchmark": "jpeg-xl",
	"libjpeg-xl-test": "jpeg-xl",
	"libcxxtest_catch2_2_gtest": "cxxtest_catch_2_gtest",
	"otl-ml": "OptimizationTemplateLibrary",
	"gperf": "gperf-hash",
	"manticore": "manticoresearch",
	"libharfbuzz_tests": "harfbuzz",
	"libopenjpeg_tools": "openjpeg",
	"libYAC": "YACLib",
	"libhnsw": "hnswlib",
	"libnms": "nmslib",
	"libtag": "taglib",
	"libtag_tests": "taglib",
	"libsafestring": "safestringlib",
	"libCache": "CacheLib",
	"libyaml-tests": "libyaml",
	"libtiff_contribs": "libtiff/contrib",
	"libpng_contribs": "libpng/contrib",
	//"libmupdf": "../../source;../../include",
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
	
	"libopencv-calib3d": "opencv/modules/calib3d",
	"libopencv-core": "opencv/modules/core",
	"libopencv-dnn": "opencv/modules/dnn",
	"libopencv-features2d": "opencv/modules/features2d",
	"libopencv-flann": "opencv/modules/flann",
	"libopencv-gapi": "opencv/modules/gapi",
	"libopencv-highgui": "opencv/modules/highgui",
	"libopencv-imgcodecs": "opencv/modules/imgcodecs",
	"libopencv-imgproc": "opencv/modules/imgproc",
	"libopencv-java": "opencv/modules/java",
	"libopencv-js": "opencv/modules/js",
	"libopencv-ml": "opencv/modules/ml",
	"libopencv-objc": "opencv/modules/objc",
	"libopencv-objdetect": "opencv/modules/objdetect",
	"libopencv-photo": "opencv/modules/photo",
	"libopencv-python": "opencv/modules/python",
	"libopencv-stitching": "opencv/modules/stitching",
	"libopencv-superres": "opencv/modules/superres",
	"libopencv-ts": "opencv/modules/ts",
	"libopencv-video": "opencv/modules/video",
	"libopencv-videoio": "opencv/modules/videoio",
	"libopencv-world": "opencv/modules/world",

	"libopencv-contrib-alphamat": "opencv_contrib/modules/alphamat",
	"libopencv-contrib-aruco": "opencv_contrib/modules/aruco",
	"libopencv-contrib-barcode": "opencv_contrib/modules/barcode",
	"libopencv-contrib-bgsegm": "opencv_contrib/modules/bgsegm",
	"libopencv-contrib-bioinspired": "opencv_contrib/modules/bioinspired",
	"libopencv-contrib-ccalib": "opencv_contrib/modules/ccalib",
	"libopencv-contrib-cnn_3dobj": "opencv_contrib/modules/cnn_3dobj",
	"libopencv-contrib-cudaarithm": "opencv_contrib/modules/cudaarithm",
	"libopencv-contrib-cudabgsegm": "opencv_contrib/modules/cudabgsegm",
	"libopencv-contrib-cudacodec": "opencv_contrib/modules/cudacodec",
	"libopencv-contrib-cudafeatures2d": "opencv_contrib/modules/cudafeatures2d",
	"libopencv-contrib-cudafilters": "opencv_contrib/modules/cudafilters",
	"libopencv-contrib-cudaimgproc": "opencv_contrib/modules/cudaimgproc",
	"libopencv-contrib-cudalegacy": "opencv_contrib/modules/cudalegacy",
	"libopencv-contrib-cudaobjdetect": "opencv_contrib/modules/cudaobjdetect",
	"libopencv-contrib-cudaoptflow": "opencv_contrib/modules/cudaoptflow",
	"libopencv-contrib-cudastereo": "opencv_contrib/modules/cudastereo",
	"libopencv-contrib-cudawarping": "opencv_contrib/modules/cudawarping",
	"libopencv-contrib-cudev": "opencv_contrib/modules/cudev",
	"libopencv-contrib-cvv": "opencv_contrib/modules/cvv",
	"libopencv-contrib-datasets": "opencv_contrib/modules/datasets",
	"libopencv-contrib-dnn_objdetect": "opencv_contrib/modules/dnn_objdetect",
	"libopencv-contrib-dnn_superres": "opencv_contrib/modules/dnn_superres",
	"libopencv-contrib-dnns_easily_fooled": "opencv_contrib/modules/dnns_easily_fooled",
	"libopencv-contrib-dpm": "opencv_contrib/modules/dpm",
	"libopencv-contrib-face": "opencv_contrib/modules/face",
	"libopencv-contrib-freetype": "opencv_contrib/modules/freetype",
	"libopencv-contrib-fuzzy": "opencv_contrib/modules/fuzzy",
	"libopencv-contrib-hdf": "opencv_contrib/modules/hdf",
	"libopencv-contrib-hfs": "opencv_contrib/modules/hfs",
	"libopencv-contrib-img_hash": "opencv_contrib/modules/img_hash",
	"libopencv-contrib-intensity_transform": "opencv_contrib/modules/intensity_transform",
	"libopencv-contrib-julia": "opencv_contrib/modules/julia",
	"libopencv-contrib-line_descriptor": "opencv_contrib/modules/line_descriptor",
	"libopencv-contrib-matlab": "opencv_contrib/modules/matlab",
	"libopencv-contrib-mcc": "opencv_contrib/modules/mcc",
	"libopencv-contrib-optflow": "opencv_contrib/modules/optflow",
	"libopencv-contrib-ovis": "opencv_contrib/modules/ovis",
	"libopencv-contrib-phase_unwrapping": "opencv_contrib/modules/phase_unwrapping",
	"libopencv-contrib-plot": "opencv_contrib/modules/plot",
	"libopencv-contrib-quality": "opencv_contrib/modules/quality",
	"libopencv-contrib-rapid": "opencv_contrib/modules/rapid",
	"libopencv-contrib-reg": "opencv_contrib/modules/reg",
	"libopencv-contrib-rgbd": "opencv_contrib/modules/rgbd",
	"libopencv-contrib-saliency": "opencv_contrib/modules/saliency",
	"libopencv-contrib-sfm": "opencv_contrib/modules/sfm",
	"libopencv-contrib-shape": "opencv_contrib/modules/shape",
	"libopencv-contrib-stereo": "opencv_contrib/modules/stereo",
	"libopencv-contrib-structured_light": "opencv_contrib/modules/structured_light",
	"libopencv-contrib-superres": "opencv_contrib/modules/superres",
	"libopencv-contrib-surface_matching": "opencv_contrib/modules/surface_matching",
	"libopencv-contrib-text": "opencv_contrib/modules/text",
	"libopencv-contrib-tracking": "opencv_contrib/modules/tracking",
	"libopencv-contrib-videostab": "opencv_contrib/modules/videostab",
	"libopencv-contrib-viz": "opencv_contrib/modules/viz",
	"libopencv-contrib-wechat_qrcode": "opencv_contrib/modules/wechat_qrcode",
	"libopencv-contrib-xfeatures2d": "opencv_contrib/modules/xfeatures2d",
	"libopencv-contrib-ximgproc": "opencv_contrib/modules/ximgproc",
	"libopencv-contrib-xobjdetect": "opencv_contrib/modules/xobjdetect",
	"libopencv-contrib-xphoto": "opencv_contrib/modules/xphoto",
	
	"libdlfcn-win32-testmain": "dlfcn-win32",
	"libdlfcn-win32-testdll1": "dlfcn-win32",
	"libdlfcn-win32-testdll2": "dlfcn-win32",
	"libdlfcn-win32-testdll3": "dlfcn-win32",
	"libdlfcn-win32-test-dladdr": "dlfcn-win32",

	//"libpthread-win32-jumbo": "pthread-win32",
	//"libpthread-win32-EH": "pthread-win32",
	//"libpthread-win32-JMP": "pthread-win32",
	
	"sqlite_tools": "sqlite-amalgamation",
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



testpath = `${scriptpath}/../../thirdparty/owemdjee/${projname}`;

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

if (/[_-]tests$/.test(projname)) {
	let tst_projname = projname.replace(/[_-]tests$/, '');
	testpath = `${scriptpath}/../../thirdparty/owemdjee/${tst_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);

	testpath = `${scriptpath}/../../thirdparty/${tst_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);

	tst_projname = nonlib_projname.replace(/[_-]tests$/, '');
	testpath = `${scriptpath}/../../thirdparty/owemdjee/${tst_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);

	testpath = `${scriptpath}/../../thirdparty/owemdjee/lib${tst_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);

	testpath = `${scriptpath}/../../thirdparty/${tst_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);

	testpath = `${scriptpath}/../../thirdparty/lib${tst_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);
}

if (/[_-]test$/.test(projname)) {
	let tst_projname = projname.replace(/[_-]test$/, '');
	testpath = `${scriptpath}/../../thirdparty/owemdjee/${tst_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);

	testpath = `${scriptpath}/../../thirdparty/${tst_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);

	tst_projname = nonlib_projname.replace(/[_-]test$/, '');
	testpath = `${scriptpath}/../../thirdparty/owemdjee/${tst_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);

	testpath = `${scriptpath}/../../thirdparty/owemdjee/lib${tst_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);

	testpath = `${scriptpath}/../../thirdparty/${tst_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);

	testpath = `${scriptpath}/../../thirdparty/lib${tst_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);
}

if (/[_-]tools$/.test(projname)) {
	let tool_projname = projname.replace(/[_-]tools$/, '');
	testpath = `${scriptpath}/../../thirdparty/owemdjee/${tool_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);

	testpath = `${scriptpath}/../../thirdparty/${tool_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);

	tool_projname = nonlib_projname.replace(/[_-]tools$/, '');
	testpath = `${scriptpath}/../../thirdparty/owemdjee/${tool_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);

	testpath = `${scriptpath}/../../thirdparty/owemdjee/lib${tool_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);

	testpath = `${scriptpath}/../../thirdparty/${tool_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);

	testpath = `${scriptpath}/../../thirdparty/lib${tool_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);
}

if (/[_-]demos$/.test(projname)) {
	let tst_projname = projname.replace(/[_-]demos$/, '');
	testpath = `${scriptpath}/../../thirdparty/owemdjee/${tst_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);

	testpath = `${scriptpath}/../../thirdparty/${tst_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);

	tst_projname = nonlib_projname.replace(/[_-]demos$/, '');
	testpath = `${scriptpath}/../../thirdparty/owemdjee/${tst_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);

	testpath = `${scriptpath}/../../thirdparty/owemdjee/lib${tst_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);

	testpath = `${scriptpath}/../../thirdparty/${tst_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);

	testpath = `${scriptpath}/../../thirdparty/lib${tst_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);
}

if (/[_-]demo$/.test(projname)) {
	let tst_projname = projname.replace(/[_-]demo$/, '');
	testpath = `${scriptpath}/../../thirdparty/owemdjee/${tst_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);

	testpath = `${scriptpath}/../../thirdparty/${tst_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);

	tst_projname = nonlib_projname.replace(/[_-]demo$/, '');
	testpath = `${scriptpath}/../../thirdparty/owemdjee/${tst_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);

	testpath = `${scriptpath}/../../thirdparty/owemdjee/lib${tst_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);

	testpath = `${scriptpath}/../../thirdparty/${tst_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);

	testpath = `${scriptpath}/../../thirdparty/lib${tst_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);
}

if (/[_-]benchmark$/.test(projname)) {
	let tst_projname = projname.replace(/[_-]benchmark$/, '');
	testpath = `${scriptpath}/../../thirdparty/owemdjee/${tst_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);

	testpath = `${scriptpath}/../../thirdparty/${tst_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);

	tst_projname = nonlib_projname.replace(/[_-]benchmark$/, '');
	testpath = `${scriptpath}/../../thirdparty/owemdjee/${tst_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);

	testpath = `${scriptpath}/../../thirdparty/owemdjee/lib${tst_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);

	testpath = `${scriptpath}/../../thirdparty/${tst_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);

	testpath = `${scriptpath}/../../thirdparty/lib${tst_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);
}

if (/[_-]examples$/.test(projname)) {
	let tst_projname = projname.replace(/[_-]examples$/, '');
	testpath = `${scriptpath}/../../thirdparty/owemdjee/${tst_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);

	testpath = `${scriptpath}/../../thirdparty/${tst_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);

	tst_projname = nonlib_projname.replace(/[_-]examples$/, '');
	testpath = `${scriptpath}/../../thirdparty/owemdjee/${tst_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);

	testpath = `${scriptpath}/../../thirdparty/owemdjee/lib${tst_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);

	testpath = `${scriptpath}/../../thirdparty/${tst_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);

	testpath = `${scriptpath}/../../thirdparty/lib${tst_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);
}

if (projname.endsWith("_DLL")) {
	let tst_projname = projname.replace(/_DLL$/, '');
	testpath = `${scriptpath}/../../thirdparty/owemdjee/${tst_projname}`;

	checkDirAndReportPlusExitOnSuccess(testpath);
}


// no mapping found.
console.error(`No known mapping for ${projname}.`);
process.exit(1);
