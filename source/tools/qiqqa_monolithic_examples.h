
#pragma once

#if defined(BUILD_MONOLITHIC)

#ifdef __cplusplus
extern "C" {
#endif

int qiqqa_db_importer_main(int argc, const char** argv);
int qiqqa_db_exporter_main(int argc, const char** argv);
int qiqqa_meta_importer_main(int argc, const char** argv);
int qiqqa_safe_file_copier_main(int argc, const char** argv);
int qiqqa_content_processor_main(int argc, const char** argv);
int qiqqa_content_importer_main(int argc, const char** argv);

#ifdef __cplusplus
}
#endif

#endif
