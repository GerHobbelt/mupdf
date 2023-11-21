
#define BUILD_MONOLITHIC 1
#include "qiqqa_monolithic_examples.h"

#define USAGE_NAME   "tools"

#include "monolithic_main_internal_defs.h"

MONOLITHIC_CMD_TABLE_START()

	{ "chop_shop", {.fa = qiqqa_chop_shop_main} },
	{ "content_import", { .fa = qiqqa_content_importer_main } },
	{ "content_importer", {.fa = qiqqa_content_importer_main}},
	{ "content_proc", { .fa = qiqqa_content_processor_main } },
	{ "content_processor", {.fa = qiqqa_content_processor_main}},
    { "cvt_annot_blob", { .fa = qiqqa_convert_legacy_annot_blobs_main } },
    { "cvt_autotags", { .fa = qiqqa_convert_legacy_autotags_main } },
    { "cvt_configfile", { .fa = qiqqa_convert_legacy_configuration_main } },
    { "cvt_expedition", { .fa = qiqqa_convert_legacy_expedition_main } },
	{ "db_export", { .fa = qiqqa_db_exporter_main } },
	{ "db_fts_indexer", {.fa = qiqqa_db_fts_indexer_main}},
	{ "db_fts_search", {.fa = qiqqa_db_fts_search_main}},
	{ "db_import", {.fa = qiqqa_db_importer_main} },
	{ "db_pappy", {.fa = qiqqa_db_pappy_main}},
	{ "doc_content", {.fa = qiqqa_mucontent_main}},
	{ "doc_scrutinizer", {.fa = qiqqa_doc_scrutinizer_main}},
	{ "documentid62", {.fa = qiqqa_documentid62_main}},
	{ "fingerprint0", {.fa = qiqqa_fingerprint0_main}},
	{ "fingerprint1", {.fa = qiqqa_fingerprint1_main}},
	{ "ingest", {.fa = qiqqa_ingest_main}},
	{ "meta_exporter", {.fa = qiqqa_meta_exporter_main}},
	{ "meta_import", { .fa = qiqqa_meta_importer_main } },
	{ "muserver", {.fa = qiqqa_muserver_main}},
	{ "bezoar", {.fa = qiqqa_ocr_bezoar_main}},
	{ "pdf_annot", {.fa = qiqqa_muannot_main}},
	{ "pdf_hound", {.fa = qiqqa_pdf_hound_main}},
	{ "safe_file_copier", { .fa = qiqqa_safe_file_copier_main } },
	{ "snarfl", {.fa = qiqqa_snarfl_main}},
	{ "web_api_server", {.fa = qiqqa_web_api_server_main}},

MONOLITHIC_CMD_TABLE_END();

#include "monolithic_main_tpl.h"
