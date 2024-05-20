//
//
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


#pragma message(FZPM_TODO "implement this tool")


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
