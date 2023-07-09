
#pragma once

#if defined(BUILD_MONOLITHIC)

#ifdef __cplusplus
extern "C" {
#endif

int qiqqa_fingerprint0_main(int argc, const char** argv);
int qiqqa_fingerprint1_main(int argc, const char** argv);
int qiqqa_documentid62_main(int argc, const char** argv);

int qiqqa_muannot_main(int argc, const char** argv);
int qiqqa_mucontent_main(int argc, const char** argv);
int qiqqa_muserver_main(int argc, const char** argv);

int qiqqa_chop_shop_main(int argc, const char** argv);
int qiqqa_content_importer_main(int argc, const char** argv);
int qiqqa_content_processor_main(int argc, const char** argv);
int qiqqa_convert_legacy_annot_blobs_main(int argc, const char** argv);
int qiqqa_convert_legacy_autotags_main(int argc, const char** argv);
int qiqqa_convert_legacy_configuration_main(int argc, const char** argv);
int qiqqa_convert_legacy_expedition_main(int argc, const char** argv);
int qiqqa_db_exporter_main(int argc, const char** argv);
int qiqqa_db_fts_indexer_main(int argc, const char** argv);
int qiqqa_db_fts_search_main(int argc, const char** argv);
int qiqqa_db_importer_main(int argc, const char** argv);
int qiqqa_db_pappy_main(int argc, const char** argv);
int qiqqa_doc_scrutinizer_main(int argc, const char** argv);
int qiqqa_ingest_main(int argc, const char** argv);
int qiqqa_meta_exporter_main(int argc, const char** argv);
int qiqqa_meta_importer_main(int argc, const char** argv);
int qiqqa_ocr_bezoar_main(int argc, const char** argv);
int qiqqa_pdf_hound_main(int argc, const char** argv);
int qiqqa_safe_file_copier_main(int argc, const char** argv);
int qiqqa_snarfl_main(int argc, const char** argv);
int qiqqa_web_api_server_main(int argc, const char** argv);

#ifdef __cplusplus
}
#endif

#endif
