//
// # `bezoar` :: OCR and related document page prep
//
// `bezoar` = Qiqqa OCR *diagnostics* tool: in order to provide
// an improved OCR experience, we need:
//
// 1. to improve `tesseract` diagnostic output so as to produce
//    a more easily understandable reporting / visualization of
//    the OCR process itself and the detailed steps taken to arrive
//    at the final result, e.g. better control over and display of
//    *binarization*, *page segmentation* and *raw recognized text
//    stream post-processing*.
// 
// 2. to enhance / augment the interface between `mupdf`-based PDF
//    page image output and the `tesseract` engine itself: currently
//    `mupdf`/Artifex doesn't offer a way to configure/tweak the
//    `tesseract` engine they have integrated into their codebase.
//
// and, IFF possible:
//
// 3. *hopefully* add the ability to manipulate these behaviours
//    through user-provided *scripting* for customized
//    behaviours for individual input files. This SHOULD include
//    arbitrary *page image* preprocessing flows, inspired on
//    the algorithms included in `unpaper` and `libprecog`
//    (a.k.a. `PRLib`).
//
//    The key to this (scriptable) flow is that image
//    (pre)processing is just not a single forward movement,
//    but should allow for a preprocessing *graph* to be
//    set up so we can create masks, etc., which are then
//    to be applied to later stages in that preprocessing
//    graph.
//
// 1. Also add the feature to tesseract to provide a separate
//    image for the *segmentation phase* in that codebase and/or
//    override that phase entirely by allowing external
//    processes (or the preprocessor) to deliver a list of
//    segments (*bboxes*) to be OCR-ed.
//
// We note this as we saw that the LSTM OCR engine accepts/expects
// color or greyscale "original image data", but the thresholding
// in tesseract, while okay, delivers a mask that's sometimes
// unsuitable for *segmentation*, while it is fine for OCR
// (old skool tess v3 style): for *segmentation* we want fattened
// characters, possibly even connected together, i.e. some subtle
// shrink+grow / opening & closing before we binarize and feed *that*
// binarized image to the segmentation logic.
// 
// Meanwhile we feed *another*, *thinner*, binarized image to the
// old skool tesseract v3 engine.
// 
// *Plus* we use a *thicker* binarized image as a
// *denoise/background-removal mask*, applying it to the
// color/greyscale "original image" data, after which we normalize
// the result in order to nicely span the entire available
// *dynamic range* in colorspace (grey or RGB), so as to feed the
// LSTM engine the best possible grey/color pixels, without any
// disturbance by further-away background noise pixels, thanks
// to the applied mask.
//
// > **Our Reason To Want This**: we've observed some very strange
// > LSTM outputs when feeding that engine *unmasked* greyscale
// > image data, which carried JPEG-artifacts!
//
// ...
//

#include "mupdf/helpers/system-header-files.h"

#include "mupdf/mutool.h"
#include "mupdf/fitz.h"
#include "mupdf/helpers/dir.h"

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <boost/contract/core/exception.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/config.hpp>

#include <iostream>

#include <exception>
#include <typeinfo>
#include <stdexcept>

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#if defined(_WIN32)
#include <windows.h>
#endif


//#pragma message(FZPM_TODO "implement this tool")


using namespace std;
using namespace cv;

static inline void memclr(void* ptr, size_t size)
{
	memset(ptr, 0, size);
}

static fz_context* ctx = NULL;
static fz_output* out = NULL;
static fz_stream* datafeed = NULL;

static void usage(void)
{
	fz_info(ctx,
		"qbezoar: image processing tool for use with Qiqqa.\n"
		"\n"
		"Syntax: qbezoar [options]\n"
		"\n"
		"Options:\n"
		"  -v      verbose (repeat to increase the chattiness of the application)\n"
		"  -q      quiet ~ not verbose at all\n"
		"\n"
		"  -V      display the version of this application and terminate\n"
	);
}


static void mu_drop_context(void)
{
	fz_close_output(ctx, out);
	fz_drop_output(ctx, out);
	out = NULL;
	fz_drop_stream(ctx, datafeed);
	datafeed = NULL;
	fz_drop_context(ctx); // also done here for those rare exit() calls inside the library code.
	ctx = NULL;
}

static void show_wait_destroy(const char* winname, cv::Mat img) 
{
	imshow(winname, img);
	moveWindow(winname, 500, 0);
	waitKey(0);
	destroyWindow(winname);
}


static int do_opencv_threshold_demo(const char* filename)
{
	auto path = samples::findFile( filename );
	Mat src = imread(path, IMREAD_COLOR);
	if (src.empty())
	{
		cout << "Could not open or find the image!\n" << endl;
		return -1;
	}
	// Show source image
	imshow("src", src);
	// Transform source image to gray if it is not already
	Mat gray;
	if (src.channels() == 3)
	{
		cvtColor(src, gray, COLOR_BGR2GRAY);
	}
	else
	{
		gray = src;
	}
	// Show gray image
	show_wait_destroy("gray", gray);
	// Apply adaptiveThreshold at the bitwise_not of gray, notice the ~ symbol
	Mat bw;
	adaptiveThreshold(~gray, bw, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 15, -2);
	// Show binary image
	show_wait_destroy("binary", bw);
	// Create the images that will use to extract the horizontal and vertical lines
	Mat horizontal = bw.clone();
	Mat vertical = bw.clone();
	// Specify size on horizontal axis
	int horizontal_size = horizontal.cols / 30;
	// Create structure element for extracting horizontal lines through morphology operations
	Mat horizontalStructure = getStructuringElement(MORPH_RECT, Size(horizontal_size, 1));
	// Apply morphology operations
	erode(horizontal, horizontal, horizontalStructure, Point(-1, -1));
	dilate(horizontal, horizontal, horizontalStructure, Point(-1, -1));
	// Show extracted horizontal lines
	show_wait_destroy("horizontal", horizontal);
	// Specify size on vertical axis
	int vertical_size = vertical.rows / 30;
	// Create structure element for extracting vertical lines through morphology operations
	Mat verticalStructure = getStructuringElement(MORPH_RECT, Size(1, vertical_size));
	// Apply morphology operations
	erode(vertical, vertical, verticalStructure, Point(-1, -1));
	dilate(vertical, vertical, verticalStructure, Point(-1, -1));
	// Show extracted vertical lines
	show_wait_destroy("vertical", vertical);
	// Inverse vertical image
	bitwise_not(vertical, vertical);
	show_wait_destroy("vertical_bit", vertical);
	// Extract edges and smooth image according to the logic
	// 1. extract edges
	// 2. dilate(edges)
	// 3. src.copyTo(smooth)
	// 4. blur smooth img
	// 5. smooth.copyTo(src, edges)
	// Step 1
	Mat edges;
	adaptiveThreshold(vertical, edges, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 3, -2);
	show_wait_destroy("edges", edges);
	// Step 2
	Mat kernel = Mat::ones(2, 2, CV_8UC1);
	dilate(edges, edges, kernel);
	show_wait_destroy("dilate", edges);
	// Step 3
	Mat smooth;
	vertical.copyTo(smooth);
	// Step 4
	blur(smooth, smooth, Size(2, 2));
	// Step 5
	smooth.copyTo(vertical, edges);
	// Show final result
	show_wait_destroy("smooth - final", vertical);
	return 0;
}

static int fz_exec_cpp_code(const char *file)
{
	try 
	{
		return do_opencv_threshold_demo(file);
	}
	catch (cv::Exception &ex)
	{
		std::string msg = ex.what();
		fz_throw(ctx, FZ_ERROR_GENERIC, msg.c_str());
	}
	catch (std::exception &ex)
	{
		std::string msg = ex.what();
		fz_throw(ctx, FZ_ERROR_GENERIC, msg.c_str());
	}
	catch (const std::string& ex) 
	{
		std::string msg = ex;
		fz_throw(ctx, FZ_ERROR_GENERIC, msg.c_str());
	} 
	catch (...) 
	{
		// as per: https://stackoverflow.com/questions/315948/c-catching-all-exceptions
		// (adapted and corrected)
		std::exception_ptr p = std::current_exception();
		auto msg = boost::current_exception_diagnostic_information();
		fz_throw(ctx, FZ_ERROR_GENERIC, msg.c_str());
	}
	return -1;
}



extern "C" int
qiqqa_ocr_bezoar_main(int argc, const char** argv)
{
	int verbosity = 0;
	int c;
	const char* output = NULL;

	ctx = NULL;
	out = NULL;
	datafeed = NULL;

	ctx = fz_new_context(NULL, NULL, FZ_STORE_DEFAULT);
	if (!ctx)
	{
		fz_error(ctx, "cannot initialise MuPDF context");
		return EXIT_FAILURE;
	}

	fz_getopt_reset();
	while ((c = fz_getopt(argc, argv, "o:qvV")) != -1)
	{
		switch (c)
		{
		case 'o': output = fz_optarg; break;

		case 'q': verbosity = 0; break;

		case 'v': verbosity++; break;

		case 'V': fz_info(ctx, "qbezoar version %s/%s", FZ_VERSION, "SHA1"); return EXIT_FAILURE;

		default: usage(); return EXIT_FAILURE;
		}
	}

	atexit(mu_drop_context);

	if (fz_optind == argc)
	{
		fz_error(ctx, "No files specified to process\n\n");
		usage();
		return EXIT_FAILURE;
	}

	const char* datafilename = NULL;
	int errored = 0;

	fz_try(ctx)
	{
		if (!output || *output == 0 || !strcmp(output, "-"))
		{
			out = fz_stdout(ctx);
			output = NULL;
		}
		else
		{
			char fbuf[PATH_MAX];
			fz_format_output_path(ctx, fbuf, sizeof fbuf, output, 0);
			fz_normalize_path(ctx, fbuf, sizeof fbuf, fbuf);
			fz_sanitize_path(ctx, fbuf, sizeof fbuf, fbuf);
			out = fz_new_output_with_path(ctx, fbuf, 0);
		}

		while (fz_optind < argc)
		{
			// load a datafile if we already have a script AND we're in "template mode".
			datafilename = argv[fz_optind++];

			errored += (fz_exec_cpp_code(datafilename) != 0);
		}
	}
	fz_catch(ctx)
	{
		if (datafeed)
		{
			fz_drop_stream(ctx, datafeed);
			datafeed = NULL;
		}

		fz_error(ctx, "Failure while processing %q: %s", datafilename, fz_convert_error(ctx, NULL));

		errored++;
	}

	fz_flush_warnings(ctx);
	mu_drop_context();

	return errored;
}
