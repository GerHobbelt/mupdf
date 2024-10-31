
#pragma once

#if defined(BUILD_MONOLITHIC)

#ifdef __cplusplus
extern "C" {
#endif

	extern int gumbo_benchmark_main(int argc, const char** argv);
	extern int gumbo_clean_text_main(int argc, const char** argv);
	extern int gumbo_eval_xpath_main(int argc, const char** argv);
	extern int gumbo_find_links_main(int argc, const char** argv);
	extern int gumbo_get_title_main(int argc, const char** argv);
	extern int gumbo_positions_of_class_main(int argc, const char** argv);
	extern int gumbo_prettyprint_main(int argc, const char** argv);
	extern int gumbo_serialize_main(int argc, const char** argv);
	extern int gumbo_print_main(int argc, const char** argv);

#ifdef __cplusplus
}
#endif

#endif
