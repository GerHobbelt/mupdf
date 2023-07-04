%module(directors="1") mupdf
%feature("director") FzDevice2;
%feature("director") FzOutput2;
%feature("director") FzPathWalker2;
%feature("director") PdfFilterFactory2;
%feature("director") PdfFilterOptions2;
%feature("director") PdfProcessor2;
%feature("director") PdfSanitizeFilterOptions2;
%feature("director") DiagnosticCallback;
%feature("director") StoryPositionsCallback;

%feature("director:except")
{
    if ($error != NULL)
    {
        /*
        This is how we can end up here:

        1. Python code calls a function in the Python `mupdf` module.
        2. - which calls SWIG C++ code.
        3. - which calls MuPDF C++ API wrapper function.
        4. - which calls MuPDF C code which calls an MuPDF struct's function pointer.
        5. - which calls MuPDF C++ API Director wrapper (e.g. mupdf::FzDevice2) virtual function.
        6. - which calls SWIG Director C++ code.
        7. - which calls Python derived class's method, which raises a Python exception.

        The exception propagates back up the above stack, being converted
        into different exception representations as it goes:

        6. SWIG Director C++ code (here). We raise a C++ exception.
        5. MuPDF C++ API Director wrapper converts the C++ exception into a MuPDF fz_try/catch exception.
        4. MuPDF C code allows the exception to propogate or catches and rethrows or throws a new fz_try/catch exception.
        3. MuPDF C++ API wrapper function converts the fz_try/catch exception into a C++ exception.
        2. SWIG C++ code converts the C++ exception into a Python exception.
        1. Python code receives the Python exception.

        So the exception changes from a Python exception, to a C++
        exception, to a fz_try/catch exception, to a C++ exception, and
        finally back into a Python exception.

        Each of these stages is necessary. In particular we cannot let the
        first C++ exception propogate directly through MuPDF C code without
        being a fz_try/catch exception, because it would mess up MuPDF C
        code's fz_try/catch exception stack.

        Unfortuntately MuPDF fz_try/catch exception strings are limited to
        256 characters so some or all of our detailed backtrace information
        is lost.
        */

        /* Get text description of the Python exception. */
        PyObject* etype;
        PyObject* obj;
        PyObject* trace;
        PyErr_Fetch( &etype, &obj, &trace);

        /* Looks like PyErr_GetExcInfo() fails here, returning NULL.*/
        /*
        PyErr_GetExcInfo( &etype, &obj, &trace);
        std::cerr << "PyErr_GetExcInfo(): etype: " << py_str(etype) << "\n";
        std::cerr << "PyErr_GetExcInfo(): obj: " << py_str(obj) << "\n";
        std::cerr << "PyErr_GetExcInfo(): trace: " << py_str(trace) << "\n";
        */

        std::string message = "Director error: " + py_str(etype) + ": " + py_str(obj) + "\n";

        if (g_mupdf_trace_director)
        {
            /* __FILE__ and __LINE__ are not useful here because SWIG makes
            them point to the generic .i code. */
            std::cerr << "========\n";
            std::cerr << "g_mupdf_trace_director set: Converting Python error into C++ exception:" << "\n";
            #ifndef _WIN32
                std::cerr << "    function: " << __PRETTY_FUNCTION__ << "\n";
            #endif
            std::cerr << "    etype: " << py_str(etype) << "\n";
            std::cerr << "    obj:   " << py_str(obj) << "\n";
            std::cerr << "    trace: " << py_str(trace) << "\n";
            std::cerr << "========\n";
        }

        PyObject* traceback = PyImport_ImportModule("traceback");
        if (traceback)
        {
            /* Use traceback.format_tb() to get backtrace. */
            if (0)
            {
                message += "Traceback (from traceback.format_tb()):\n";
                PyObject* traceback_dict = PyModule_GetDict(traceback);
                PyObject* format_tb = PyDict_GetItem(traceback_dict, PyString_FromString("format_tb"));
                PyObject* ret = PyObject_CallFunctionObjArgs(format_tb, trace, NULL);
                PyObject* iter = PyObject_GetIter(ret);
                for(;;)
                {
                    PyObject* item = PyIter_Next(iter);
                    if (!item) break;
                    message += py_str(item);
                    Py_DECREF(item);
                }
                /* `format_tb` and `traceback_dict` are borrowed references.
                */
                Py_XDECREF(iter);
                Py_XDECREF(ret);
                Py_XDECREF(traceback);
            }

            /* Use exception_info() (copied from mupdf/scripts/jlib.py) to get
            detailed backtrace. */
            if (1)
            {
                PyObject* globals = PyEval_GetGlobals();
                PyObject* exception_info = PyDict_GetItemString(globals, "exception_info");
                PyObject* string_return = PyUnicode_FromString("return");
                PyObject* ret = PyObject_CallFunctionObjArgs(
                        exception_info,
                        trace,
                        Py_None,
                        string_return,
                        NULL
                        );
                Py_XDECREF(string_return);
                message += py_str(ret);
                Py_XDECREF(ret);
            }
        }
        else
        {
            message += "[No backtrace available.]\n";
        }

        Py_XDECREF(etype);
        Py_XDECREF(obj);
        Py_XDECREF(trace);

        message += "Exception was from C++/Python callback:\n";
        message += "    ";
        #ifdef _WIN32
            message += __FUNCTION__;
        #else
            message += __PRETTY_FUNCTION__;
        #endif
        message += "\n";

        if (1 || g_mupdf_trace_director)
        {
            std::cerr << "========\n";
            std::cerr << "Director exception handler, message is:\n" << message << "\n";
            std::cerr << "========\n";
        }

        /* SWIG 4.1 documention talks about throwing a
        Swig::DirectorMethodException here, but this doesn't work for us
        because it sets Python's error state again, which makes the
        next SWIG call of a C/C++ function appear to fail.
        //throw Swig::DirectorMethodException();
        */
        throw std::runtime_error( message.c_str());
    }
}
%ignore ::Memento_checkBlock;
%ignore ::Memento_checkAllMemory;
%ignore ::Memento_check;
%ignore ::Memento_setParanoia;
%ignore ::Memento_paranoidAt;
%ignore ::Memento_breakAt;
%ignore ::Memento_breakOnFree;
%ignore ::Memento_breakOnRealloc;
%ignore ::Memento_getBlockNum;
%ignore ::Memento_find;
%ignore ::Memento_breakpoint;
%ignore ::Memento_failAt;
%ignore ::Memento_failThisEvent;
%ignore ::Memento_listBlocks;
%ignore ::Memento_listNewBlocks;
%ignore ::Memento_listPhasedBlocks;
%ignore ::Memento_setMax;
%ignore ::Memento_stats;
%ignore ::Memento_label;
%ignore ::Memento_tick;
%ignore ::Memento_setVerbose;
%ignore ::Memento_addBacktraceLimitFnname;
%ignore ::Memento_setAtexitFin;
%ignore ::Memento_setIgnoreNewDelete;
%ignore ::Memento_malloc;
%ignore ::Memento_realloc;
%ignore ::Memento_free;
%ignore ::Memento_calloc;
%ignore ::Memento_strdup;
%ignore ::Memento_asprintf;
%ignore ::Memento_vasprintf;
%ignore ::Memento_info;
%ignore ::Memento_listBlockInfo;
%ignore ::Memento_blockInfo;
%ignore ::Memento_takeByteRef;
%ignore ::Memento_dropByteRef;
%ignore ::Memento_takeShortRef;
%ignore ::Memento_dropShortRef;
%ignore ::Memento_takeIntRef;
%ignore ::Memento_dropIntRef;
%ignore ::Memento_takeRef;
%ignore ::Memento_dropRef;
%ignore ::Memento_adjustRef;
%ignore ::Memento_reference;
%ignore ::Memento_checkPointerOrNull;
%ignore ::Memento_checkBytePointerOrNull;
%ignore ::Memento_checkShortPointerOrNull;
%ignore ::Memento_checkIntPointerOrNull;
%ignore ::Memento_startLeaking;
%ignore ::Memento_stopLeaking;
%ignore ::Memento_sequence;
%ignore ::Memento_squeezing;
%ignore ::Memento_fin;
%ignore ::Memento_bt;
%ignore ::Memento_cpp_new;
%ignore ::Memento_cpp_delete;
%ignore ::Memento_cpp_new_array;
%ignore ::Memento_cpp_delete_array;
%ignore ::Memento_showHash;
%ignore ::fz_stat_ctime;
%ignore ::fz_stat_mtime;
%ignore ::fz_is_pow2;
%ignore ::fz_mul255;
%ignore ::fz_div255;
%ignore ::fz_atof;
%ignore ::fz_atoi;
%ignore ::fz_atoi64;
%ignore ::fz_abs;
%ignore ::fz_absi;
%ignore ::fz_min;
%ignore ::fz_mini;
%ignore ::fz_minz;
%ignore ::fz_mini64;
%ignore ::fz_max;
%ignore ::fz_maxi;
%ignore ::fz_maxz;
%ignore ::fz_maxi64;
%ignore ::fz_clamp;
%ignore ::fz_clampi;
%ignore ::fz_clamp64;
%ignore ::fz_clampd;
%ignore ::fz_clampp;
%ignore ::fz_make_point;
%ignore ::fz_make_rect;
%ignore ::fz_make_irect;
%ignore ::fz_is_empty_rect;
%ignore ::fz_is_empty_irect;
%ignore ::fz_is_infinite_rect;
%ignore ::fz_is_infinite_irect;
%ignore ::fz_is_valid_rect;
%ignore ::fz_is_valid_irect;
%ignore ::fz_irect_width;
%ignore ::fz_irect_height;
%ignore ::fz_make_matrix;
%ignore ::fz_is_identity;
%ignore ::fz_concat;
%ignore ::fz_scale;
%ignore ::fz_pre_scale;
%ignore ::fz_post_scale;
%ignore ::fz_shear;
%ignore ::fz_pre_shear;
%ignore ::fz_rotate;
%ignore ::fz_pre_rotate;
%ignore ::fz_translate;
%ignore ::fz_pre_translate;
%ignore ::fz_transform_page;
%ignore ::fz_invert_matrix;
%ignore ::fz_try_invert_matrix;
%ignore ::fz_is_rectilinear;
%ignore ::fz_matrix_expansion;
%ignore ::fz_intersect_rect;
%ignore ::fz_intersect_irect;
%ignore ::fz_union_rect;
%ignore ::fz_irect_from_rect;
%ignore ::fz_round_rect;
%ignore ::fz_rect_from_irect;
%ignore ::fz_expand_rect;
%ignore ::fz_expand_irect;
%ignore ::fz_include_point_in_rect;
%ignore ::fz_translate_rect;
%ignore ::fz_translate_irect;
%ignore ::fz_contains_rect;
%ignore ::fz_transform_point;
%ignore ::fz_transform_point_xy;
%ignore ::fz_transform_vector;
%ignore ::fz_transform_rect;
%ignore ::fz_normalize_vector;
%ignore ::fz_gridfit_matrix;
%ignore ::fz_matrix_max_expansion;
%ignore ::fz_make_quad;
%ignore ::fz_quad_from_rect;
%ignore ::fz_rect_from_quad;
%ignore ::fz_transform_quad;
%ignore ::fz_is_point_inside_quad;
%ignore ::fz_is_point_inside_rect;
%ignore ::fz_is_point_inside_irect;
%ignore ::fz_is_quad_inside_quad;
%ignore ::fz_is_quad_intersecting_quad;
%ignore ::fz_vthrow;
%ignore ::fz_throw;
%ignore ::fz_rethrow;
%ignore ::fz_morph_error;
%ignore ::fz_vwarn;
%ignore ::fz_warn;
%ignore ::fz_caught_message;
%ignore ::fz_caught;
%ignore ::fz_rethrow_if;
%ignore ::fz_log_error_printf;
%ignore ::fz_vlog_error_printf;
%ignore ::fz_log_error;
%ignore ::fz_start_throw_on_repair;
%ignore ::fz_end_throw_on_repair;
%ignore ::fz_flush_warnings;
%ignore ::fz_clone_context;
%ignore ::fz_drop_context;
%ignore ::fz_set_user_context;
%ignore ::fz_user_context;
%ignore ::fz_default_error_callback;
%ignore ::fz_default_warning_callback;
%ignore ::fz_set_error_callback;
%ignore ::fz_error_callback;
%ignore ::fz_set_warning_callback;
%ignore ::fz_warning_callback;
%ignore ::fz_tune_image_decode;
%ignore ::fz_tune_image_scale;
%ignore ::fz_aa_level;
%ignore ::fz_set_aa_level;
%ignore ::fz_text_aa_level;
%ignore ::fz_set_text_aa_level;
%ignore ::fz_graphics_aa_level;
%ignore ::fz_set_graphics_aa_level;
%ignore ::fz_graphics_min_line_width;
%ignore ::fz_set_graphics_min_line_width;
%ignore ::fz_user_css;
%ignore ::fz_set_user_css;
%ignore ::fz_use_document_css;
%ignore ::fz_set_use_document_css;
%ignore ::fz_enable_icc;
%ignore ::fz_disable_icc;
%ignore ::fz_malloc;
%ignore ::fz_calloc;
%ignore ::fz_realloc;
%ignore ::fz_free;
%ignore ::fz_malloc_no_throw;
%ignore ::fz_calloc_no_throw;
%ignore ::fz_realloc_no_throw;
%ignore ::fz_strdup;
%ignore ::fz_memrnd;
%ignore ::fz_var_imp;
%ignore ::fz_push_try;
%ignore ::fz_do_try;
%ignore ::fz_do_always;
%ignore ::fz_do_catch;
%ignore ::fz_new_context_imp;
%ignore ::fz_lock;
%ignore ::fz_unlock;
%ignore ::fz_keep_imp;
%ignore ::fz_keep_imp_locked;
%ignore ::fz_keep_imp8_locked;
%ignore ::fz_keep_imp8;
%ignore ::fz_keep_imp16;
%ignore ::fz_drop_imp;
%ignore ::fz_drop_imp8;
%ignore ::fz_drop_imp16;
%ignore ::fz_keep_buffer;
%ignore ::fz_drop_buffer;
%ignore ::fz_buffer_storage;
%ignore ::fz_string_from_buffer;
%ignore ::fz_new_buffer;
%ignore ::fz_new_buffer_from_data;
%ignore ::fz_new_buffer_from_shared_data;
%ignore ::fz_new_buffer_from_copied_data;
%ignore ::fz_clone_buffer;
%ignore ::fz_new_buffer_from_base64;
%ignore ::fz_resize_buffer;
%ignore ::fz_grow_buffer;
%ignore ::fz_trim_buffer;
%ignore ::fz_clear_buffer;
%ignore ::fz_slice_buffer;
%ignore ::fz_append_buffer;
%ignore ::fz_append_base64;
%ignore ::fz_append_base64_buffer;
%ignore ::fz_append_data;
%ignore ::fz_append_string;
%ignore ::fz_append_byte;
%ignore ::fz_append_rune;
%ignore ::fz_append_int32_le;
%ignore ::fz_append_int16_le;
%ignore ::fz_append_int32_be;
%ignore ::fz_append_int16_be;
%ignore ::fz_append_bits;
%ignore ::fz_append_bits_pad;
%ignore ::fz_append_pdf_string;
%ignore ::fz_append_printf;
%ignore ::fz_append_vprintf;
%ignore ::fz_terminate_buffer;
%ignore ::fz_md5_buffer;
%ignore ::fz_buffer_extract;
%ignore ::fz_strnlen;
%ignore ::fz_strsep;
%ignore ::fz_strlcpy;
%ignore ::fz_strlcat;
%ignore ::fz_memmem;
%ignore ::fz_dirname;
%ignore ::fz_basename;
%ignore ::fz_urldecode;
%ignore ::fz_format_output_path;
%ignore ::fz_cleanname;
%ignore ::fz_realpath;
%ignore ::fz_strcasecmp;
%ignore ::fz_strncasecmp;
%ignore ::fz_chartorune;
%ignore ::fz_runetochar;
%ignore ::fz_runelen;
%ignore ::fz_runeidx;
%ignore ::fz_runeptr;
%ignore ::fz_utflen;
%ignore ::fz_strtof;
%ignore ::fz_grisu;
%ignore ::fz_is_page_range;
%ignore ::fz_parse_page_range;
%ignore ::fz_tolower;
%ignore ::fz_toupper;
%ignore ::fz_file_exists;
%ignore ::fz_open_file;
%ignore ::fz_try_open_file;
%ignore ::fz_open_memory;
%ignore ::fz_open_buffer;
%ignore ::fz_open_leecher;
%ignore ::fz_keep_stream;
%ignore ::fz_drop_stream;
%ignore ::fz_tell;
%ignore ::fz_seek;
%ignore ::fz_read;
%ignore ::fz_skip;
%ignore ::fz_read_all;
%ignore ::fz_read_file;
%ignore ::fz_try_read_file;
%ignore ::fz_read_uint16;
%ignore ::fz_read_uint24;
%ignore ::fz_read_uint32;
%ignore ::fz_read_uint64;
%ignore ::fz_read_uint16_le;
%ignore ::fz_read_uint24_le;
%ignore ::fz_read_uint32_le;
%ignore ::fz_read_uint64_le;
%ignore ::fz_read_int16;
%ignore ::fz_read_int32;
%ignore ::fz_read_int64;
%ignore ::fz_read_int16_le;
%ignore ::fz_read_int32_le;
%ignore ::fz_read_int64_le;
%ignore ::fz_read_float_le;
%ignore ::fz_read_float;
%ignore ::fz_read_string;
%ignore ::fz_new_stream;
%ignore ::fz_read_best;
%ignore ::fz_read_line;
%ignore ::fz_skip_string;
%ignore ::fz_skip_space;
%ignore ::fz_available;
%ignore ::fz_read_byte;
%ignore ::fz_peek_byte;
%ignore ::fz_unread_byte;
%ignore ::fz_is_eof;
%ignore ::fz_read_bits;
%ignore ::fz_read_rbits;
%ignore ::fz_sync_bits;
%ignore ::fz_is_eof_bits;
%ignore ::fz_open_file_ptr_no_close;
%ignore ::fz_new_output;
%ignore ::fz_new_output_with_path;
%ignore ::fz_new_output_with_buffer;
%ignore ::fz_stdout;
%ignore ::fz_stderr;
%ignore ::fz_set_stddbg;
%ignore ::fz_stddbg;
%ignore ::fz_write_printf;
%ignore ::fz_write_vprintf;
%ignore ::fz_seek_output;
%ignore ::fz_tell_output;
%ignore ::fz_flush_output;
%ignore ::fz_close_output;
%ignore ::fz_drop_output;
%ignore ::fz_output_supports_stream;
%ignore ::fz_stream_from_output;
%ignore ::fz_truncate_output;
%ignore ::fz_write_data;
%ignore ::fz_write_buffer;
%ignore ::fz_write_string;
%ignore ::fz_write_int32_be;
%ignore ::fz_write_int32_le;
%ignore ::fz_write_uint32_be;
%ignore ::fz_write_uint32_le;
%ignore ::fz_write_int16_be;
%ignore ::fz_write_int16_le;
%ignore ::fz_write_uint16_be;
%ignore ::fz_write_uint16_le;
%ignore ::fz_write_char;
%ignore ::fz_write_byte;
%ignore ::fz_write_float_be;
%ignore ::fz_write_float_le;
%ignore ::fz_write_rune;
%ignore ::fz_write_base64;
%ignore ::fz_write_base64_buffer;
%ignore ::fz_write_bits;
%ignore ::fz_write_bits_sync;
%ignore ::fz_format_string;
%ignore ::fz_vsnprintf;
%ignore ::fz_snprintf;
%ignore ::fz_asprintf;
%ignore ::fz_save_buffer;
%ignore ::fz_new_asciihex_output;
%ignore ::fz_new_ascii85_output;
%ignore ::fz_new_rle_output;
%ignore ::fz_new_arc4_output;
%ignore ::fz_new_deflate_output;
%ignore ::fz_log;
%ignore ::fz_log_module;
%ignore ::fz_new_log_for_module;
%ignore ::fz_md5_init;
%ignore ::fz_md5_update;
%ignore ::fz_md5_update_int64;
%ignore ::fz_md5_final;
%ignore ::fz_sha256_init;
%ignore ::fz_sha256_update;
%ignore ::fz_sha256_final;
%ignore ::fz_sha512_init;
%ignore ::fz_sha512_update;
%ignore ::fz_sha512_final;
%ignore ::fz_sha384_init;
%ignore ::fz_sha384_update;
%ignore ::fz_sha384_final;
%ignore ::fz_arc4_init;
%ignore ::fz_arc4_encrypt;
%ignore ::fz_arc4_final;
%ignore ::fz_aes_setkey_enc;
%ignore ::fz_aes_setkey_dec;
%ignore ::fz_aes_crypt_cbc;
%ignore ::fz_getopt;
%ignore ::fz_new_hash_table;
%ignore ::fz_drop_hash_table;
%ignore ::fz_hash_find;
%ignore ::fz_hash_insert;
%ignore ::fz_hash_remove;
%ignore ::fz_hash_for_each;
%ignore ::fz_hash_filter;
%ignore ::fz_new_pool;
%ignore ::fz_pool_alloc;
%ignore ::fz_pool_strdup;
%ignore ::fz_pool_size;
%ignore ::fz_drop_pool;
%ignore ::fz_tree_lookup;
%ignore ::fz_tree_insert;
%ignore ::fz_drop_tree;
%ignore ::fz_bidi_fragment_text;
%ignore ::fz_open_archive;
%ignore ::fz_open_archive_with_stream;
%ignore ::fz_try_open_archive_with_stream;
%ignore ::fz_open_directory;
%ignore ::fz_is_directory;
%ignore ::fz_drop_archive;
%ignore ::fz_keep_archive;
%ignore ::fz_archive_format;
%ignore ::fz_count_archive_entries;
%ignore ::fz_list_archive_entry;
%ignore ::fz_has_archive_entry;
%ignore ::fz_open_archive_entry;
%ignore ::fz_try_open_archive_entry;
%ignore ::fz_read_archive_entry;
%ignore ::fz_try_read_archive_entry;
%ignore ::fz_is_tar_archive;
%ignore ::fz_open_tar_archive;
%ignore ::fz_open_tar_archive_with_stream;
%ignore ::fz_is_zip_archive;
%ignore ::fz_open_zip_archive;
%ignore ::fz_open_zip_archive_with_stream;
%ignore ::fz_new_zip_writer;
%ignore ::fz_new_zip_writer_with_output;
%ignore ::fz_write_zip_entry;
%ignore ::fz_close_zip_writer;
%ignore ::fz_drop_zip_writer;
%ignore ::fz_new_tree_archive;
%ignore ::fz_tree_archive_add_buffer;
%ignore ::fz_tree_archive_add_data;
%ignore ::fz_new_multi_archive;
%ignore ::fz_mount_multi_archive;
%ignore ::fz_new_archive_of_size;
%ignore ::fz_parse_xml;
%ignore ::fz_parse_xml_stream;
%ignore ::fz_parse_xml_archive_entry;
%ignore ::fz_try_parse_xml_archive_entry;
%ignore ::fz_parse_xml_from_html5;
%ignore ::fz_keep_xml;
%ignore ::fz_drop_xml;
%ignore ::fz_detach_xml;
%ignore ::fz_xml_root;
%ignore ::fz_xml_prev;
%ignore ::fz_xml_next;
%ignore ::fz_xml_up;
%ignore ::fz_xml_down;
%ignore ::fz_xml_is_tag;
%ignore ::fz_xml_tag;
%ignore ::fz_xml_att;
%ignore ::fz_xml_att_alt;
%ignore ::fz_xml_att_eq;
%ignore ::fz_xml_add_att;
%ignore ::fz_xml_text;
%ignore ::fz_debug_xml;
%ignore ::fz_xml_find;
%ignore ::fz_xml_find_next;
%ignore ::fz_xml_find_down;
%ignore ::fz_xml_find_match;
%ignore ::fz_xml_find_next_match;
%ignore ::fz_xml_find_down_match;
%ignore ::fz_xml_find_dfs;
%ignore ::fz_xml_find_dfs_top;
%ignore ::fz_xml_find_next_dfs;
%ignore ::fz_xml_find_next_dfs_top;
%ignore ::fz_dom_body;
%ignore ::fz_dom_document_element;
%ignore ::fz_dom_create_element;
%ignore ::fz_dom_create_text_node;
%ignore ::fz_dom_find;
%ignore ::fz_dom_find_next;
%ignore ::fz_dom_append_child;
%ignore ::fz_dom_insert_before;
%ignore ::fz_dom_insert_after;
%ignore ::fz_dom_remove;
%ignore ::fz_dom_clone;
%ignore ::fz_dom_first_child;
%ignore ::fz_dom_parent;
%ignore ::fz_dom_next;
%ignore ::fz_dom_previous;
%ignore ::fz_dom_add_attribute;
%ignore ::fz_dom_remove_attribute;
%ignore ::fz_dom_attribute;
%ignore ::fz_dom_get_attribute;
%ignore ::fz_deflate_bound;
%ignore ::fz_deflate;
%ignore ::fz_new_deflated_data;
%ignore ::fz_new_deflated_data_from_buffer;
%ignore ::fz_compress_ccitt_fax_g3;
%ignore ::fz_compress_ccitt_fax_g4;
%ignore ::fz_keep_storable;
%ignore ::fz_drop_storable;
%ignore ::fz_keep_key_storable;
%ignore ::fz_drop_key_storable;
%ignore ::fz_keep_key_storable_key;
%ignore ::fz_drop_key_storable_key;
%ignore ::fz_new_store_context;
%ignore ::fz_keep_store_context;
%ignore ::fz_drop_store_context;
%ignore ::fz_store_item;
%ignore ::fz_find_item;
%ignore ::fz_remove_item;
%ignore ::fz_empty_store;
%ignore ::fz_store_scavenge;
%ignore ::fz_store_scavenge_external;
%ignore ::fz_shrink_store;
%ignore ::fz_filter_store;
%ignore ::fz_debug_store;
%ignore ::fz_defer_reap_start;
%ignore ::fz_defer_reap_end;
%ignore ::fz_open_null_filter;
%ignore ::fz_open_range_filter;
%ignore ::fz_open_endstream_filter;
%ignore ::fz_open_concat;
%ignore ::fz_concat_push_drop;
%ignore ::fz_open_arc4;
%ignore ::fz_open_aesd;
%ignore ::fz_open_a85d;
%ignore ::fz_open_ahxd;
%ignore ::fz_open_rld;
%ignore ::fz_open_dctd;
%ignore ::fz_open_faxd;
%ignore ::fz_open_flated;
%ignore ::fz_open_lzwd;
%ignore ::fz_open_predict;
%ignore ::fz_open_jbig2d;
%ignore ::fz_load_jbig2_globals;
%ignore ::fz_keep_jbig2_globals;
%ignore ::fz_drop_jbig2_globals;
%ignore ::fz_drop_jbig2_globals_imp;
%ignore ::fz_jbig2_globals_data;
%ignore ::fz_open_sgilog16;
%ignore ::fz_open_sgilog24;
%ignore ::fz_open_sgilog32;
%ignore ::fz_open_thunder;
%ignore ::fz_compressed_buffer_size;
%ignore ::fz_open_compressed_buffer;
%ignore ::fz_open_image_decomp_stream_from_buffer;
%ignore ::fz_open_image_decomp_stream;
%ignore ::fz_recognize_image_format;
%ignore ::fz_image_type_name;
%ignore ::fz_lookup_image_type;
%ignore ::fz_drop_compressed_buffer;
%ignore ::fz_lookup_rendering_intent;
%ignore ::fz_rendering_intent_name;
%ignore ::fz_new_colorspace;
%ignore ::fz_keep_colorspace;
%ignore ::fz_drop_colorspace;
%ignore ::fz_new_indexed_colorspace;
%ignore ::fz_new_icc_colorspace;
%ignore ::fz_new_cal_gray_colorspace;
%ignore ::fz_new_cal_rgb_colorspace;
%ignore ::fz_colorspace_type;
%ignore ::fz_colorspace_name;
%ignore ::fz_colorspace_n;
%ignore ::fz_colorspace_is_subtractive;
%ignore ::fz_colorspace_device_n_has_only_cmyk;
%ignore ::fz_colorspace_device_n_has_cmyk;
%ignore ::fz_colorspace_is_gray;
%ignore ::fz_colorspace_is_rgb;
%ignore ::fz_colorspace_is_cmyk;
%ignore ::fz_colorspace_is_lab;
%ignore ::fz_colorspace_is_indexed;
%ignore ::fz_colorspace_is_device_n;
%ignore ::fz_colorspace_is_device;
%ignore ::fz_colorspace_is_device_gray;
%ignore ::fz_colorspace_is_device_cmyk;
%ignore ::fz_colorspace_is_lab_icc;
%ignore ::fz_is_valid_blend_colorspace;
%ignore ::fz_base_colorspace;
%ignore ::fz_device_gray;
%ignore ::fz_device_rgb;
%ignore ::fz_device_bgr;
%ignore ::fz_device_cmyk;
%ignore ::fz_device_lab;
%ignore ::fz_colorspace_name_colorant;
%ignore ::fz_colorspace_colorant;
%ignore ::fz_clamp_color;
%ignore ::fz_convert_color;
%ignore ::fz_new_default_colorspaces;
%ignore ::fz_keep_default_colorspaces;
%ignore ::fz_drop_default_colorspaces;
%ignore ::fz_clone_default_colorspaces;
%ignore ::fz_default_gray;
%ignore ::fz_default_rgb;
%ignore ::fz_default_cmyk;
%ignore ::fz_default_output_intent;
%ignore ::fz_set_default_gray;
%ignore ::fz_set_default_rgb;
%ignore ::fz_set_default_cmyk;
%ignore ::fz_set_default_output_intent;
%ignore ::fz_drop_colorspace_imp;
%ignore ::fz_new_separations;
%ignore ::fz_keep_separations;
%ignore ::fz_drop_separations;
%ignore ::fz_add_separation;
%ignore ::fz_add_separation_equivalents;
%ignore ::fz_set_separation_behavior;
%ignore ::fz_separation_current_behavior;
%ignore ::fz_separation_name;
%ignore ::fz_count_separations;
%ignore ::fz_count_active_separations;
%ignore ::fz_compare_separations;
%ignore ::fz_clone_separations_for_overprint;
%ignore ::fz_convert_separation_colors;
%ignore ::fz_separation_equivalent;
%ignore ::fz_pixmap_bbox;
%ignore ::fz_pixmap_width;
%ignore ::fz_pixmap_height;
%ignore ::fz_pixmap_x;
%ignore ::fz_pixmap_y;
%ignore ::fz_new_pixmap;
%ignore ::fz_new_pixmap_with_bbox;
%ignore ::fz_new_pixmap_with_data;
%ignore ::fz_new_pixmap_with_bbox_and_data;
%ignore ::fz_new_pixmap_from_pixmap;
%ignore ::fz_clone_pixmap;
%ignore ::fz_keep_pixmap;
%ignore ::fz_drop_pixmap;
%ignore ::fz_pixmap_colorspace;
%ignore ::fz_pixmap_components;
%ignore ::fz_pixmap_colorants;
%ignore ::fz_pixmap_spots;
%ignore ::fz_pixmap_alpha;
%ignore ::fz_pixmap_samples;
%ignore ::fz_pixmap_stride;
%ignore ::fz_set_pixmap_resolution;
%ignore ::fz_clear_pixmap_with_value;
%ignore ::fz_fill_pixmap_with_color;
%ignore ::fz_clear_pixmap_rect_with_value;
%ignore ::fz_clear_pixmap;
%ignore ::fz_invert_pixmap;
%ignore ::fz_invert_pixmap_alpha;
%ignore ::fz_invert_pixmap_luminance;
%ignore ::fz_tint_pixmap;
%ignore ::fz_invert_pixmap_rect;
%ignore ::fz_invert_pixmap_raw;
%ignore ::fz_gamma_pixmap;
%ignore ::fz_convert_pixmap;
%ignore ::fz_is_pixmap_monochrome;
%ignore ::fz_alpha_from_gray;
%ignore ::fz_decode_tile;
%ignore ::fz_md5_pixmap;
%ignore ::fz_unpack_stream;
%ignore ::fz_warp_pixmap;
%ignore ::fz_clone_pixmap_area_with_different_seps;
%ignore ::fz_new_pixmap_from_alpha_channel;
%ignore ::fz_new_pixmap_from_color_and_mask;
%ignore ::fz_keep_bitmap;
%ignore ::fz_drop_bitmap;
%ignore ::fz_new_bitmap_from_pixmap;
%ignore ::fz_new_bitmap_from_pixmap_band;
%ignore ::fz_new_bitmap;
%ignore ::fz_bitmap_details;
%ignore ::fz_clear_bitmap;
%ignore ::fz_default_halftone;
%ignore ::fz_keep_halftone;
%ignore ::fz_drop_halftone;
%ignore ::fz_get_pixmap_from_image;
%ignore ::fz_get_unscaled_pixmap_from_image;
%ignore ::fz_keep_image;
%ignore ::fz_drop_image;
%ignore ::fz_keep_image_store_key;
%ignore ::fz_drop_image_store_key;
%ignore ::fz_new_image_of_size;
%ignore ::fz_new_image_from_compressed_buffer;
%ignore ::fz_new_image_from_pixmap;
%ignore ::fz_new_image_from_buffer;
%ignore ::fz_new_image_from_file;
%ignore ::fz_drop_image_imp;
%ignore ::fz_drop_image_base;
%ignore ::fz_decomp_image_from_stream;
%ignore ::fz_convert_indexed_pixmap_to_base;
%ignore ::fz_convert_separation_pixmap_to_base;
%ignore ::fz_image_size;
%ignore ::fz_image_resolution;
%ignore ::fz_image_orientation;
%ignore ::fz_image_orientation_matrix;
%ignore ::fz_compressed_image_buffer;
%ignore ::fz_set_compressed_image_buffer;
%ignore ::fz_pixmap_image_tile;
%ignore ::fz_set_pixmap_image_tile;
%ignore ::fz_load_jpx;
%ignore ::fz_load_tiff_subimage_count;
%ignore ::fz_load_tiff_subimage;
%ignore ::fz_load_pnm_subimage_count;
%ignore ::fz_load_pnm_subimage;
%ignore ::fz_load_jbig2_subimage_count;
%ignore ::fz_load_jbig2_subimage;
%ignore ::fz_load_bmp_subimage_count;
%ignore ::fz_load_bmp_subimage;
%ignore ::fz_keep_shade;
%ignore ::fz_drop_shade;
%ignore ::fz_bound_shade;
%ignore ::fz_drop_shade_color_cache;
%ignore ::fz_paint_shade;
%ignore ::fz_process_shade;
%ignore ::fz_drop_shade_imp;
%ignore ::fz_iso8859_1_from_unicode;
%ignore ::fz_iso8859_7_from_unicode;
%ignore ::fz_koi8u_from_unicode;
%ignore ::fz_windows_1250_from_unicode;
%ignore ::fz_windows_1251_from_unicode;
%ignore ::fz_windows_1252_from_unicode;
%ignore ::fz_unicode_from_glyph_name;
%ignore ::fz_unicode_from_glyph_name_strict;
%ignore ::fz_duplicate_glyph_names_from_unicode;
%ignore ::fz_glyph_name_from_unicode_sc;
%ignore ::fz_font_ft_face;
%ignore ::fz_font_t3_procs;
%ignore ::fz_font_flags;
%ignore ::fz_font_shaper_data;
%ignore ::fz_font_name;
%ignore ::fz_font_is_bold;
%ignore ::fz_font_is_italic;
%ignore ::fz_font_is_serif;
%ignore ::fz_font_is_monospaced;
%ignore ::fz_font_bbox;
%ignore ::fz_install_load_system_font_funcs;
%ignore ::fz_load_system_font;
%ignore ::fz_load_system_cjk_font;
%ignore ::fz_lookup_builtin_font;
%ignore ::fz_lookup_base14_font;
%ignore ::fz_lookup_cjk_font;
%ignore ::fz_lookup_cjk_font_by_language;
%ignore ::fz_lookup_cjk_ordering_by_language;
%ignore ::fz_lookup_noto_font;
%ignore ::fz_lookup_noto_math_font;
%ignore ::fz_lookup_noto_music_font;
%ignore ::fz_lookup_noto_symbol1_font;
%ignore ::fz_lookup_noto_symbol2_font;
%ignore ::fz_lookup_noto_emoji_font;
%ignore ::fz_load_fallback_font;
%ignore ::fz_new_type3_font;
%ignore ::fz_new_font_from_memory;
%ignore ::fz_new_font_from_buffer;
%ignore ::fz_new_font_from_file;
%ignore ::fz_new_base14_font;
%ignore ::fz_new_cjk_font;
%ignore ::fz_new_builtin_font;
%ignore ::fz_set_font_embedding;
%ignore ::fz_keep_font;
%ignore ::fz_drop_font;
%ignore ::fz_set_font_bbox;
%ignore ::fz_bound_glyph;
%ignore ::fz_glyph_cacheable;
%ignore ::fz_run_t3_glyph;
%ignore ::fz_advance_glyph;
%ignore ::fz_encode_character;
%ignore ::fz_encode_character_sc;
%ignore ::fz_encode_character_by_glyph_name;
%ignore ::fz_encode_character_with_fallback;
%ignore ::fz_get_glyph_name;
%ignore ::fz_font_ascender;
%ignore ::fz_font_descender;
%ignore ::fz_font_digest;
%ignore ::fz_decouple_type3_font;
%ignore ::ft_error_string;
%ignore ::ft_char_index;
%ignore ::ft_name_index;
%ignore ::fz_hb_lock;
%ignore ::fz_hb_unlock;
%ignore ::fz_walk_path;
%ignore ::fz_new_path;
%ignore ::fz_keep_path;
%ignore ::fz_drop_path;
%ignore ::fz_trim_path;
%ignore ::fz_packed_path_size;
%ignore ::fz_pack_path;
%ignore ::fz_clone_path;
%ignore ::fz_currentpoint;
%ignore ::fz_moveto;
%ignore ::fz_lineto;
%ignore ::fz_rectto;
%ignore ::fz_quadto;
%ignore ::fz_curveto;
%ignore ::fz_curvetov;
%ignore ::fz_curvetoy;
%ignore ::fz_closepath;
%ignore ::fz_transform_path;
%ignore ::fz_bound_path;
%ignore ::fz_adjust_rect_for_stroke;
%ignore ::fz_new_stroke_state;
%ignore ::fz_new_stroke_state_with_dash_len;
%ignore ::fz_keep_stroke_state;
%ignore ::fz_drop_stroke_state;
%ignore ::fz_unshare_stroke_state;
%ignore ::fz_unshare_stroke_state_with_dash_len;
%ignore ::fz_clone_stroke_state;
%ignore ::fz_new_text;
%ignore ::fz_keep_text;
%ignore ::fz_drop_text;
%ignore ::fz_show_glyph;
%ignore ::fz_show_string;
%ignore ::fz_measure_string;
%ignore ::fz_bound_text;
%ignore ::fz_text_language_from_string;
%ignore ::fz_string_from_text_language;
%ignore ::fz_glyph_bbox;
%ignore ::fz_glyph_bbox_no_ctx;
%ignore ::fz_glyph_width;
%ignore ::fz_glyph_height;
%ignore ::fz_keep_glyph;
%ignore ::fz_drop_glyph;
%ignore ::fz_outline_glyph;
%ignore ::fz_lookup_blendmode;
%ignore ::fz_blendmode_name;
%ignore ::fz_structure_to_string;
%ignore ::fz_structure_from_string;
%ignore ::fz_fill_path;
%ignore ::fz_stroke_path;
%ignore ::fz_clip_path;
%ignore ::fz_clip_stroke_path;
%ignore ::fz_fill_text;
%ignore ::fz_stroke_text;
%ignore ::fz_clip_text;
%ignore ::fz_clip_stroke_text;
%ignore ::fz_ignore_text;
%ignore ::fz_pop_clip;
%ignore ::fz_fill_shade;
%ignore ::fz_fill_image;
%ignore ::fz_fill_image_mask;
%ignore ::fz_clip_image_mask;
%ignore ::fz_begin_mask;
%ignore ::fz_end_mask;
%ignore ::fz_begin_group;
%ignore ::fz_end_group;
%ignore ::fz_begin_tile;
%ignore ::fz_begin_tile_id;
%ignore ::fz_end_tile;
%ignore ::fz_render_flags;
%ignore ::fz_set_default_colorspaces;
%ignore ::fz_begin_layer;
%ignore ::fz_end_layer;
%ignore ::fz_begin_structure;
%ignore ::fz_end_structure;
%ignore ::fz_begin_metatext;
%ignore ::fz_end_metatext;
%ignore ::fz_new_device_of_size;
%ignore ::fz_close_device;
%ignore ::fz_drop_device;
%ignore ::fz_keep_device;
%ignore ::fz_enable_device_hints;
%ignore ::fz_disable_device_hints;
%ignore ::fz_device_current_scissor;
%ignore ::fz_new_trace_device;
%ignore ::fz_new_xmltext_device;
%ignore ::fz_new_bbox_device;
%ignore ::fz_new_test_device;
%ignore ::fz_new_draw_device;
%ignore ::fz_new_draw_device_with_bbox;
%ignore ::fz_new_draw_device_with_proof;
%ignore ::fz_new_draw_device_with_bbox_proof;
%ignore ::fz_new_draw_device_type3;
%ignore ::fz_parse_draw_options;
%ignore ::fz_new_draw_device_with_options;
%ignore ::fz_new_display_list;
%ignore ::fz_new_list_device;
%ignore ::fz_run_display_list;
%ignore ::fz_keep_display_list;
%ignore ::fz_drop_display_list;
%ignore ::fz_bound_display_list;
%ignore ::fz_new_image_from_display_list;
%ignore ::fz_display_list_is_empty;
%ignore ::fz_new_layout;
%ignore ::fz_drop_layout;
%ignore ::fz_add_layout_line;
%ignore ::fz_add_layout_char;
%ignore ::fz_new_stext_page;
%ignore ::fz_drop_stext_page;
%ignore ::fz_print_stext_page_as_html;
%ignore ::fz_print_stext_header_as_html;
%ignore ::fz_print_stext_trailer_as_html;
%ignore ::fz_print_stext_page_as_xhtml;
%ignore ::fz_print_stext_header_as_xhtml;
%ignore ::fz_print_stext_trailer_as_xhtml;
%ignore ::fz_print_stext_page_as_xml;
%ignore ::fz_print_stext_page_as_json;
%ignore ::fz_print_stext_page_as_text;
%ignore ::fz_search_stext_page;
%ignore ::fz_highlight_selection;
%ignore ::fz_snap_selection;
%ignore ::fz_copy_selection;
%ignore ::fz_copy_rectangle;
%ignore ::fz_parse_stext_options;
%ignore ::fz_new_stext_device;
%ignore ::fz_new_ocr_device;
%ignore ::fz_open_reflowed_document;
%ignore ::fz_generate_transition;
%ignore ::fz_purge_glyph_cache;
%ignore ::fz_render_glyph_pixmap;
%ignore ::fz_render_t3_glyph_direct;
%ignore ::fz_prepare_t3_glyph;
%ignore ::fz_dump_glyph_cache_stats;
%ignore ::fz_subpixel_adjust;
%ignore ::fz_make_link_dest_none;
%ignore ::fz_make_link_dest_xyz;
%ignore ::fz_new_link_of_size;
%ignore ::fz_keep_link;
%ignore ::fz_drop_link;
%ignore ::fz_is_external_link;
%ignore ::fz_set_link_rect;
%ignore ::fz_set_link_uri;
%ignore ::fz_outline_iterator_item;
%ignore ::fz_outline_iterator_next;
%ignore ::fz_outline_iterator_prev;
%ignore ::fz_outline_iterator_up;
%ignore ::fz_outline_iterator_down;
%ignore ::fz_outline_iterator_insert;
%ignore ::fz_outline_iterator_delete;
%ignore ::fz_outline_iterator_update;
%ignore ::fz_drop_outline_iterator;
%ignore ::fz_new_outline;
%ignore ::fz_keep_outline;
%ignore ::fz_drop_outline;
%ignore ::fz_load_outline_from_iterator;
%ignore ::fz_new_outline_iterator_of_size;
%ignore ::fz_outline_iterator_from_outline;
%ignore ::fz_make_location;
%ignore ::fz_register_document_handler;
%ignore ::fz_register_document_handlers;
%ignore ::fz_recognize_document;
%ignore ::fz_recognize_document_content;
%ignore ::fz_recognize_document_stream_content;
%ignore ::fz_open_document;
%ignore ::fz_open_accelerated_document;
%ignore ::fz_open_document_with_stream;
%ignore ::fz_open_document_with_buffer;
%ignore ::fz_open_accelerated_document_with_stream;
%ignore ::fz_document_supports_accelerator;
%ignore ::fz_save_accelerator;
%ignore ::fz_output_accelerator;
%ignore ::fz_new_document_of_size;
%ignore ::fz_keep_document;
%ignore ::fz_drop_document;
%ignore ::fz_needs_password;
%ignore ::fz_authenticate_password;
%ignore ::fz_load_outline;
%ignore ::fz_new_outline_iterator;
%ignore ::fz_is_document_reflowable;
%ignore ::fz_layout_document;
%ignore ::fz_make_bookmark;
%ignore ::fz_lookup_bookmark;
%ignore ::fz_count_pages;
%ignore ::fz_resolve_link_dest;
%ignore ::fz_format_link_uri;
%ignore ::fz_resolve_link;
%ignore ::fz_last_page;
%ignore ::fz_next_page;
%ignore ::fz_previous_page;
%ignore ::fz_clamp_location;
%ignore ::fz_location_from_page_number;
%ignore ::fz_page_number_from_location;
%ignore ::fz_load_page;
%ignore ::fz_count_chapters;
%ignore ::fz_count_chapter_pages;
%ignore ::fz_load_chapter_page;
%ignore ::fz_load_links;
%ignore ::fz_new_page_of_size;
%ignore ::fz_bound_page;
%ignore ::fz_run_page;
%ignore ::fz_run_page_contents;
%ignore ::fz_run_page_annots;
%ignore ::fz_run_page_widgets;
%ignore ::fz_keep_page;
%ignore ::fz_keep_page_locked;
%ignore ::fz_drop_page;
%ignore ::fz_page_presentation;
%ignore ::fz_page_label;
%ignore ::fz_has_permission;
%ignore ::fz_lookup_metadata;
%ignore ::fz_set_metadata;
%ignore ::fz_document_output_intent;
%ignore ::fz_page_separations;
%ignore ::fz_page_uses_overprint;
%ignore ::fz_create_link;
%ignore ::fz_delete_link;
%ignore ::fz_process_opened_pages;
%ignore ::fz_new_display_list_from_page;
%ignore ::fz_new_display_list_from_page_number;
%ignore ::fz_new_display_list_from_page_contents;
%ignore ::fz_new_pixmap_from_display_list;
%ignore ::fz_new_pixmap_from_page;
%ignore ::fz_new_pixmap_from_page_number;
%ignore ::fz_new_pixmap_from_page_contents;
%ignore ::fz_new_pixmap_from_display_list_with_separations;
%ignore ::fz_new_pixmap_from_page_with_separations;
%ignore ::fz_new_pixmap_from_page_number_with_separations;
%ignore ::fz_new_pixmap_from_page_contents_with_separations;
%ignore ::fz_fill_pixmap_from_display_list;
%ignore ::fz_new_stext_page_from_page;
%ignore ::fz_new_stext_page_from_page_number;
%ignore ::fz_new_stext_page_from_chapter_page_number;
%ignore ::fz_new_stext_page_from_display_list;
%ignore ::fz_new_buffer_from_stext_page;
%ignore ::fz_new_buffer_from_page;
%ignore ::fz_new_buffer_from_page_number;
%ignore ::fz_new_buffer_from_display_list;
%ignore ::fz_search_page;
%ignore ::fz_search_page_number;
%ignore ::fz_search_chapter_page_number;
%ignore ::fz_search_display_list;
%ignore ::fz_new_display_list_from_svg;
%ignore ::fz_new_image_from_svg;
%ignore ::fz_new_display_list_from_svg_xml;
%ignore ::fz_new_image_from_svg_xml;
%ignore ::fz_write_image_as_data_uri;
%ignore ::fz_write_pixmap_as_data_uri;
%ignore ::fz_append_image_as_data_uri;
%ignore ::fz_append_pixmap_as_data_uri;
%ignore ::fz_new_xhtml_document_from_document;
%ignore ::fz_new_buffer_from_page_with_format;
%ignore ::fz_has_option;
%ignore ::fz_option_eq;
%ignore ::fz_copy_option;
%ignore ::fz_new_document_writer;
%ignore ::fz_new_document_writer_with_output;
%ignore ::fz_new_document_writer_with_buffer;
%ignore ::fz_new_pdf_writer;
%ignore ::fz_new_pdf_writer_with_output;
%ignore ::fz_new_svg_writer;
%ignore ::fz_new_text_writer;
%ignore ::fz_new_text_writer_with_output;
%ignore ::fz_new_odt_writer;
%ignore ::fz_new_odt_writer_with_output;
%ignore ::fz_new_docx_writer;
%ignore ::fz_new_docx_writer_with_output;
%ignore ::fz_new_ps_writer;
%ignore ::fz_new_ps_writer_with_output;
%ignore ::fz_new_pcl_writer;
%ignore ::fz_new_pcl_writer_with_output;
%ignore ::fz_new_pclm_writer;
%ignore ::fz_new_pclm_writer_with_output;
%ignore ::fz_new_pwg_writer;
%ignore ::fz_new_pwg_writer_with_output;
%ignore ::fz_new_cbz_writer;
%ignore ::fz_new_cbz_writer_with_output;
%ignore ::fz_new_pdfocr_writer;
%ignore ::fz_new_pdfocr_writer_with_output;
%ignore ::fz_pdfocr_writer_set_progress;
%ignore ::fz_new_jpeg_pixmap_writer;
%ignore ::fz_new_png_pixmap_writer;
%ignore ::fz_new_pam_pixmap_writer;
%ignore ::fz_new_pnm_pixmap_writer;
%ignore ::fz_new_pgm_pixmap_writer;
%ignore ::fz_new_ppm_pixmap_writer;
%ignore ::fz_new_pbm_pixmap_writer;
%ignore ::fz_new_pkm_pixmap_writer;
%ignore ::fz_begin_page;
%ignore ::fz_end_page;
%ignore ::fz_write_document;
%ignore ::fz_close_document_writer;
%ignore ::fz_drop_document_writer;
%ignore ::fz_new_pixmap_writer;
%ignore ::fz_new_document_writer_of_size;
%ignore ::fz_write_header;
%ignore ::fz_write_band;
%ignore ::fz_close_band_writer;
%ignore ::fz_drop_band_writer;
%ignore ::fz_new_band_writer_of_size;
%ignore ::fz_pcl_preset;
%ignore ::fz_parse_pcl_options;
%ignore ::fz_new_mono_pcl_band_writer;
%ignore ::fz_write_bitmap_as_pcl;
%ignore ::fz_save_bitmap_as_pcl;
%ignore ::fz_new_color_pcl_band_writer;
%ignore ::fz_write_pixmap_as_pcl;
%ignore ::fz_save_pixmap_as_pcl;
%ignore ::fz_parse_pclm_options;
%ignore ::fz_new_pclm_band_writer;
%ignore ::fz_write_pixmap_as_pclm;
%ignore ::fz_save_pixmap_as_pclm;
%ignore ::fz_parse_pdfocr_options;
%ignore ::fz_new_pdfocr_band_writer;
%ignore ::fz_pdfocr_band_writer_set_progress;
%ignore ::fz_write_pixmap_as_pdfocr;
%ignore ::fz_save_pixmap_as_pdfocr;
%ignore ::fz_save_pixmap_as_png;
%ignore ::fz_save_pixmap_as_jpeg;
%ignore ::fz_write_pixmap_as_png;
%ignore ::fz_new_png_band_writer;
%ignore ::fz_new_buffer_from_image_as_png;
%ignore ::fz_new_buffer_from_image_as_pnm;
%ignore ::fz_new_buffer_from_image_as_pam;
%ignore ::fz_new_buffer_from_image_as_psd;
%ignore ::fz_new_buffer_from_image_as_jpeg;
%ignore ::fz_new_buffer_from_pixmap_as_png;
%ignore ::fz_new_buffer_from_pixmap_as_pnm;
%ignore ::fz_new_buffer_from_pixmap_as_pam;
%ignore ::fz_new_buffer_from_pixmap_as_psd;
%ignore ::fz_new_buffer_from_pixmap_as_jpeg;
%ignore ::fz_save_pixmap_as_pnm;
%ignore ::fz_write_pixmap_as_pnm;
%ignore ::fz_new_pnm_band_writer;
%ignore ::fz_save_pixmap_as_pam;
%ignore ::fz_write_pixmap_as_pam;
%ignore ::fz_new_pam_band_writer;
%ignore ::fz_save_bitmap_as_pbm;
%ignore ::fz_write_bitmap_as_pbm;
%ignore ::fz_new_pbm_band_writer;
%ignore ::fz_save_pixmap_as_pbm;
%ignore ::fz_save_bitmap_as_pkm;
%ignore ::fz_write_bitmap_as_pkm;
%ignore ::fz_new_pkm_band_writer;
%ignore ::fz_save_pixmap_as_pkm;
%ignore ::fz_write_pixmap_as_ps;
%ignore ::fz_save_pixmap_as_ps;
%ignore ::fz_new_ps_band_writer;
%ignore ::fz_write_ps_file_header;
%ignore ::fz_write_ps_file_trailer;
%ignore ::fz_save_pixmap_as_psd;
%ignore ::fz_write_pixmap_as_psd;
%ignore ::fz_new_psd_band_writer;
%ignore ::fz_save_pixmap_as_pwg;
%ignore ::fz_save_bitmap_as_pwg;
%ignore ::fz_write_pixmap_as_pwg;
%ignore ::fz_write_bitmap_as_pwg;
%ignore ::fz_write_pixmap_as_pwg_page;
%ignore ::fz_write_bitmap_as_pwg_page;
%ignore ::fz_new_mono_pwg_band_writer;
%ignore ::fz_new_pwg_band_writer;
%ignore ::fz_write_pwg_file_header;
%ignore ::fz_new_svg_device;
%ignore ::fz_new_svg_device_with_id;
%ignore ::fz_new_story;
%ignore ::fz_story_warnings;
%ignore ::fz_place_story;
%ignore ::fz_draw_story;
%ignore ::fz_reset_story;
%ignore ::fz_drop_story;
%ignore ::fz_story_document;
%ignore ::fz_story_positions;
%ignore ::fz_write_story;
%ignore ::fz_write_stabilized_story;
%ignore ::pdf_new_int;
%ignore ::pdf_new_real;
%ignore ::pdf_new_name;
%ignore ::pdf_new_string;
%ignore ::pdf_new_text_string;
%ignore ::pdf_new_indirect;
%ignore ::pdf_new_array;
%ignore ::pdf_new_dict;
%ignore ::pdf_new_rect;
%ignore ::pdf_new_matrix;
%ignore ::pdf_new_date;
%ignore ::pdf_copy_array;
%ignore ::pdf_copy_dict;
%ignore ::pdf_deep_copy_obj;
%ignore ::pdf_keep_obj;
%ignore ::pdf_drop_obj;
%ignore ::pdf_drop_singleton_obj;
%ignore ::pdf_is_null;
%ignore ::pdf_is_bool;
%ignore ::pdf_is_int;
%ignore ::pdf_is_real;
%ignore ::pdf_is_number;
%ignore ::pdf_is_name;
%ignore ::pdf_is_string;
%ignore ::pdf_is_array;
%ignore ::pdf_is_dict;
%ignore ::pdf_is_indirect;
%ignore ::pdf_obj_num_is_stream;
%ignore ::pdf_is_stream;
%ignore ::pdf_objcmp;
%ignore ::pdf_objcmp_resolve;
%ignore ::pdf_objcmp_deep;
%ignore ::pdf_name_eq;
%ignore ::pdf_obj_marked;
%ignore ::pdf_mark_obj;
%ignore ::pdf_unmark_obj;
%ignore ::pdf_cycle;
%ignore ::pdf_new_mark_bits;
%ignore ::pdf_drop_mark_bits;
%ignore ::pdf_mark_bits_reset;
%ignore ::pdf_mark_bits_set;
%ignore ::pdf_mark_list_push;
%ignore ::pdf_mark_list_pop;
%ignore ::pdf_mark_list_init;
%ignore ::pdf_mark_list_free;
%ignore ::pdf_set_obj_memo;
%ignore ::pdf_obj_memo;
%ignore ::pdf_obj_is_dirty;
%ignore ::pdf_dirty_obj;
%ignore ::pdf_clean_obj;
%ignore ::pdf_to_bool;
%ignore ::pdf_to_int;
%ignore ::pdf_to_int64;
%ignore ::pdf_to_real;
%ignore ::pdf_to_name;
%ignore ::pdf_to_text_string;
%ignore ::pdf_to_string;
%ignore ::pdf_to_str_buf;
%ignore ::pdf_to_str_len;
%ignore ::pdf_to_num;
%ignore ::pdf_to_gen;
%ignore ::pdf_array_len;
%ignore ::pdf_array_get;
%ignore ::pdf_array_put;
%ignore ::pdf_array_put_drop;
%ignore ::pdf_array_push;
%ignore ::pdf_array_push_drop;
%ignore ::pdf_array_insert;
%ignore ::pdf_array_insert_drop;
%ignore ::pdf_array_delete;
%ignore ::pdf_array_find;
%ignore ::pdf_array_contains;
%ignore ::pdf_dict_len;
%ignore ::pdf_dict_get_key;
%ignore ::pdf_dict_get_val;
%ignore ::pdf_dict_put_val_null;
%ignore ::pdf_dict_get;
%ignore ::pdf_dict_getp;
%ignore ::pdf_dict_getl;
%ignore ::pdf_dict_geta;
%ignore ::pdf_dict_gets;
%ignore ::pdf_dict_getsa;
%ignore ::pdf_dict_get_inheritable;
%ignore ::pdf_dict_getp_inheritable;
%ignore ::pdf_dict_gets_inheritable;
%ignore ::pdf_dict_put;
%ignore ::pdf_dict_put_drop;
%ignore ::pdf_dict_get_put_drop;
%ignore ::pdf_dict_puts;
%ignore ::pdf_dict_puts_drop;
%ignore ::pdf_dict_putp;
%ignore ::pdf_dict_putp_drop;
%ignore ::pdf_dict_putl;
%ignore ::pdf_dict_putl_drop;
%ignore ::pdf_dict_del;
%ignore ::pdf_dict_dels;
%ignore ::pdf_sort_dict;
%ignore ::pdf_dict_put_bool;
%ignore ::pdf_dict_put_int;
%ignore ::pdf_dict_put_real;
%ignore ::pdf_dict_put_name;
%ignore ::pdf_dict_put_string;
%ignore ::pdf_dict_put_text_string;
%ignore ::pdf_dict_put_rect;
%ignore ::pdf_dict_put_matrix;
%ignore ::pdf_dict_put_date;
%ignore ::pdf_dict_put_array;
%ignore ::pdf_dict_put_dict;
%ignore ::pdf_dict_puts_dict;
%ignore ::pdf_dict_get_bool;
%ignore ::pdf_dict_get_int;
%ignore ::pdf_dict_get_int64;
%ignore ::pdf_dict_get_real;
%ignore ::pdf_dict_get_name;
%ignore ::pdf_dict_get_string;
%ignore ::pdf_dict_get_text_string;
%ignore ::pdf_dict_get_rect;
%ignore ::pdf_dict_get_matrix;
%ignore ::pdf_dict_get_date;
%ignore ::pdf_array_push_bool;
%ignore ::pdf_array_push_int;
%ignore ::pdf_array_push_real;
%ignore ::pdf_array_push_name;
%ignore ::pdf_array_push_string;
%ignore ::pdf_array_push_text_string;
%ignore ::pdf_array_push_array;
%ignore ::pdf_array_push_dict;
%ignore ::pdf_array_get_bool;
%ignore ::pdf_array_get_int;
%ignore ::pdf_array_get_real;
%ignore ::pdf_array_get_name;
%ignore ::pdf_array_get_string;
%ignore ::pdf_array_get_text_string;
%ignore ::pdf_array_get_rect;
%ignore ::pdf_array_get_matrix;
%ignore ::pdf_set_obj_parent;
%ignore ::pdf_obj_refs;
%ignore ::pdf_obj_parent_num;
%ignore ::pdf_sprint_obj;
%ignore ::pdf_print_obj;
%ignore ::pdf_print_encrypted_obj;
%ignore ::pdf_debug_obj;
%ignore ::pdf_debug_ref;
%ignore ::pdf_new_utf8_from_pdf_string;
%ignore ::pdf_new_utf8_from_pdf_string_obj;
%ignore ::pdf_new_utf8_from_pdf_stream_obj;
%ignore ::pdf_load_stream_or_string_as_utf8;
%ignore ::pdf_to_quad;
%ignore ::pdf_to_rect;
%ignore ::pdf_to_matrix;
%ignore ::pdf_to_date;
%ignore ::pdf_get_indirect_document;
%ignore ::pdf_get_bound_document;
%ignore ::pdf_pin_document;
%ignore ::pdf_set_int;
%ignore ::pdf_set_str_len;
%ignore ::pdf_enable_journal;
%ignore ::pdf_begin_operation;
%ignore ::pdf_begin_implicit_operation;
%ignore ::pdf_end_operation;
%ignore ::pdf_abandon_operation;
%ignore ::pdf_undoredo_state;
%ignore ::pdf_undoredo_step;
%ignore ::pdf_can_undo;
%ignore ::pdf_can_redo;
%ignore ::pdf_undo;
%ignore ::pdf_redo;
%ignore ::pdf_discard_journal;
%ignore ::pdf_drop_journal;
%ignore ::pdf_serialise_journal;
%ignore ::pdf_deserialise_journal;
%ignore ::pdf_add_journal_fragment;
%ignore ::pdf_format_date;
%ignore ::pdf_parse_date;
%ignore ::pdf_js_get_console;
%ignore ::pdf_js_set_console;
%ignore ::pdf_open_document;
%ignore ::pdf_open_document_with_stream;
%ignore ::pdf_drop_document;
%ignore ::pdf_keep_document;
%ignore ::pdf_specifics;
%ignore ::pdf_document_from_fz_document;
%ignore ::pdf_page_from_fz_page;
%ignore ::pdf_needs_password;
%ignore ::pdf_authenticate_password;
%ignore ::pdf_has_permission;
%ignore ::pdf_lookup_metadata;
%ignore ::pdf_load_outline;
%ignore ::pdf_new_outline_iterator;
%ignore ::pdf_invalidate_xfa;
%ignore ::pdf_count_layer_configs;
%ignore ::pdf_count_layers;
%ignore ::pdf_layer_name;
%ignore ::pdf_layer_is_enabled;
%ignore ::pdf_enable_layer;
%ignore ::pdf_layer_config_info;
%ignore ::pdf_select_layer_config;
%ignore ::pdf_count_layer_config_ui;
%ignore ::pdf_select_layer_config_ui;
%ignore ::pdf_deselect_layer_config_ui;
%ignore ::pdf_toggle_layer_config_ui;
%ignore ::pdf_layer_config_ui_info;
%ignore ::pdf_set_layer_config_as_default;
%ignore ::pdf_has_unsaved_changes;
%ignore ::pdf_was_repaired;
%ignore ::pdf_create_document;
%ignore ::pdf_graft_object;
%ignore ::pdf_new_graft_map;
%ignore ::pdf_keep_graft_map;
%ignore ::pdf_drop_graft_map;
%ignore ::pdf_graft_mapped_object;
%ignore ::pdf_graft_page;
%ignore ::pdf_graft_mapped_page;
%ignore ::pdf_page_write;
%ignore ::pdf_new_pdf_device;
%ignore ::pdf_add_page;
%ignore ::pdf_insert_page;
%ignore ::pdf_delete_page;
%ignore ::pdf_delete_page_range;
%ignore ::pdf_page_label;
%ignore ::pdf_page_label_imp;
%ignore ::pdf_set_page_labels;
%ignore ::pdf_delete_page_labels;
%ignore ::pdf_document_language;
%ignore ::pdf_set_document_language;
%ignore ::pdf_parse_write_options;
%ignore ::pdf_has_unsaved_sigs;
%ignore ::pdf_write_document;
%ignore ::pdf_save_document;
%ignore ::pdf_save_snapshot;
%ignore ::pdf_write_snapshot;
%ignore ::pdf_format_write_options;
%ignore ::pdf_can_be_saved_incrementally;
%ignore ::pdf_write_journal;
%ignore ::pdf_save_journal;
%ignore ::pdf_load_journal;
%ignore ::pdf_read_journal;
%ignore ::pdf_minimize_document;
%ignore ::pdf_lexbuf_init;
%ignore ::pdf_lexbuf_fin;
%ignore ::pdf_lexbuf_grow;
%ignore ::pdf_lex;
%ignore ::pdf_lex_no_string;
%ignore ::pdf_parse_array;
%ignore ::pdf_parse_dict;
%ignore ::pdf_parse_stm_obj;
%ignore ::pdf_parse_ind_obj;
%ignore ::pdf_parse_journal_obj;
%ignore ::pdf_append_token;
%ignore ::pdf_create_object;
%ignore ::pdf_delete_object;
%ignore ::pdf_update_object;
%ignore ::pdf_update_stream;
%ignore ::pdf_is_local_object;
%ignore ::pdf_add_object;
%ignore ::pdf_add_object_drop;
%ignore ::pdf_add_stream;
%ignore ::pdf_add_new_dict;
%ignore ::pdf_add_new_array;
%ignore ::pdf_cache_object;
%ignore ::pdf_count_objects;
%ignore ::pdf_resolve_indirect;
%ignore ::pdf_resolve_indirect_chain;
%ignore ::pdf_load_object;
%ignore ::pdf_load_unencrypted_object;
%ignore ::pdf_load_raw_stream_number;
%ignore ::pdf_load_raw_stream;
%ignore ::pdf_load_stream_number;
%ignore ::pdf_load_stream;
%ignore ::pdf_open_raw_stream_number;
%ignore ::pdf_open_raw_stream;
%ignore ::pdf_open_stream_number;
%ignore ::pdf_open_stream;
%ignore ::pdf_open_inline_stream;
%ignore ::pdf_load_compressed_stream;
%ignore ::pdf_load_compressed_inline_image;
%ignore ::pdf_open_stream_with_offset;
%ignore ::pdf_open_contents_stream;
%ignore ::pdf_version;
%ignore ::pdf_trailer;
%ignore ::pdf_set_populating_xref_trailer;
%ignore ::pdf_xref_len;
%ignore ::pdf_metadata;
%ignore ::pdf_get_populating_xref_entry;
%ignore ::pdf_get_xref_entry;
%ignore ::pdf_xref_entry_map;
%ignore ::pdf_get_xref_entry_no_change;
%ignore ::pdf_get_xref_entry_no_null;
%ignore ::pdf_replace_xref;
%ignore ::pdf_forget_xref;
%ignore ::pdf_get_incremental_xref_entry;
%ignore ::pdf_xref_ensure_incremental_object;
%ignore ::pdf_xref_is_incremental;
%ignore ::pdf_xref_store_unsaved_signature;
%ignore ::pdf_xref_remove_unsaved_signature;
%ignore ::pdf_xref_obj_is_unsaved_signature;
%ignore ::pdf_xref_ensure_local_object;
%ignore ::pdf_obj_is_incremental;
%ignore ::pdf_repair_xref;
%ignore ::pdf_repair_obj_stms;
%ignore ::pdf_repair_trailer;
%ignore ::pdf_ensure_solid_xref;
%ignore ::pdf_mark_xref;
%ignore ::pdf_clear_xref;
%ignore ::pdf_clear_xref_to_mark;
%ignore ::pdf_repair_obj;
%ignore ::pdf_progressive_advance;
%ignore ::pdf_count_versions;
%ignore ::pdf_count_unsaved_versions;
%ignore ::pdf_validate_changes;
%ignore ::pdf_doc_was_linearized;
%ignore ::pdf_is_field_locked;
%ignore ::pdf_drop_locked_fields;
%ignore ::pdf_find_locked_fields;
%ignore ::pdf_find_locked_fields_for_sig;
%ignore ::pdf_validate_change_history;
%ignore ::pdf_find_version_for_obj;
%ignore ::pdf_validate_signature;
%ignore ::pdf_was_pure_xfa;
%ignore ::pdf_new_local_xref;
%ignore ::pdf_drop_local_xref;
%ignore ::pdf_drop_local_xref_and_resources;
%ignore ::pdf_debug_doc_changes;
%ignore ::pdf_new_crypt;
%ignore ::pdf_new_encrypt;
%ignore ::pdf_drop_crypt;
%ignore ::pdf_crypt_obj;
%ignore ::pdf_open_crypt;
%ignore ::pdf_open_crypt_with_filter;
%ignore ::pdf_crypt_version;
%ignore ::pdf_crypt_revision;
%ignore ::pdf_crypt_method;
%ignore ::pdf_crypt_string_method;
%ignore ::pdf_crypt_stream_method;
%ignore ::pdf_crypt_length;
%ignore ::pdf_crypt_permissions;
%ignore ::pdf_crypt_encrypt_metadata;
%ignore ::pdf_crypt_owner_password;
%ignore ::pdf_crypt_user_password;
%ignore ::pdf_crypt_owner_encryption;
%ignore ::pdf_crypt_user_encryption;
%ignore ::pdf_crypt_permissions_encryption;
%ignore ::pdf_crypt_key;
%ignore ::pdf_print_crypt;
%ignore ::pdf_write_digest;
%ignore ::pdf_document_permissions;
%ignore ::pdf_signature_byte_range;
%ignore ::pdf_signature_hash_bytes;
%ignore ::pdf_signature_incremental_change_since_signing;
%ignore ::pdf_signature_contents;
%ignore ::pdf_encrypt_data;
%ignore ::pdf_encrypted_len;
%ignore ::pdf_new_cmap;
%ignore ::pdf_keep_cmap;
%ignore ::pdf_drop_cmap;
%ignore ::pdf_drop_cmap_imp;
%ignore ::pdf_cmap_size;
%ignore ::pdf_cmap_wmode;
%ignore ::pdf_set_cmap_wmode;
%ignore ::pdf_set_usecmap;
%ignore ::pdf_add_codespace;
%ignore ::pdf_map_range_to_range;
%ignore ::pdf_map_one_to_many;
%ignore ::pdf_sort_cmap;
%ignore ::pdf_lookup_cmap;
%ignore ::pdf_lookup_cmap_full;
%ignore ::pdf_decode_cmap;
%ignore ::pdf_new_identity_cmap;
%ignore ::pdf_load_cmap;
%ignore ::pdf_load_system_cmap;
%ignore ::pdf_load_builtin_cmap;
%ignore ::pdf_load_embedded_cmap;
%ignore ::pdf_load_encoding;
%ignore ::pdf_set_font_wmode;
%ignore ::pdf_set_default_hmtx;
%ignore ::pdf_set_default_vmtx;
%ignore ::pdf_add_hmtx;
%ignore ::pdf_add_vmtx;
%ignore ::pdf_end_hmtx;
%ignore ::pdf_end_vmtx;
%ignore ::pdf_lookup_hmtx;
%ignore ::pdf_lookup_vmtx;
%ignore ::pdf_load_to_unicode;
%ignore ::pdf_font_cid_to_gid;
%ignore ::pdf_clean_font_name;
%ignore ::pdf_lookup_substitute_font;
%ignore ::pdf_load_type3_font;
%ignore ::pdf_load_type3_glyphs;
%ignore ::pdf_load_font;
%ignore ::pdf_load_hail_mary_font;
%ignore ::pdf_new_font_desc;
%ignore ::pdf_keep_font;
%ignore ::pdf_drop_font;
%ignore ::pdf_print_font;
%ignore ::pdf_run_glyph;
%ignore ::pdf_add_simple_font;
%ignore ::pdf_add_cid_font;
%ignore ::pdf_add_cjk_font;
%ignore ::pdf_add_substitute_font;
%ignore ::pdf_font_writing_supported;
%ignore ::fz_extract_ttf_from_ttc;
%ignore ::pdf_store_item;
%ignore ::pdf_find_item;
%ignore ::pdf_remove_item;
%ignore ::pdf_empty_store;
%ignore ::pdf_purge_locals_from_store;
%ignore ::pdf_find_font_resource;
%ignore ::pdf_insert_font_resource;
%ignore ::pdf_drop_resource_tables;
%ignore ::pdf_purge_local_font_resources;
%ignore ::pdf_eval_function;
%ignore ::pdf_keep_function;
%ignore ::pdf_drop_function;
%ignore ::pdf_function_size;
%ignore ::pdf_load_function;
%ignore ::pdf_document_output_intent;
%ignore ::pdf_load_colorspace;
%ignore ::pdf_is_tint_colorspace;
%ignore ::pdf_load_shading;
%ignore ::pdf_sample_shade_function;
%ignore ::pdf_recolor_shade;
%ignore ::pdf_load_inline_image;
%ignore ::pdf_is_jpx_image;
%ignore ::pdf_load_image;
%ignore ::pdf_add_image;
%ignore ::pdf_load_pattern;
%ignore ::pdf_keep_pattern;
%ignore ::pdf_drop_pattern;
%ignore ::pdf_new_xobject;
%ignore ::pdf_update_xobject;
%ignore ::pdf_xobject_resources;
%ignore ::pdf_xobject_bbox;
%ignore ::pdf_xobject_matrix;
%ignore ::pdf_xobject_isolated;
%ignore ::pdf_xobject_knockout;
%ignore ::pdf_xobject_transparency;
%ignore ::pdf_xobject_colorspace;
%ignore ::pdf_new_processor;
%ignore ::pdf_keep_processor;
%ignore ::pdf_close_processor;
%ignore ::pdf_drop_processor;
%ignore ::pdf_new_run_processor;
%ignore ::pdf_new_buffer_processor;
%ignore ::pdf_new_output_processor;
%ignore ::pdf_new_sanitize_filter;
%ignore ::pdf_filter_xobject_instance;
%ignore ::pdf_processor_push_resources;
%ignore ::pdf_processor_pop_resources;
%ignore ::pdf_new_color_filter;
%ignore ::pdf_process_contents;
%ignore ::pdf_process_annot;
%ignore ::pdf_process_glyph;
%ignore ::pdf_process_raw_contents;
%ignore ::pdf_tos_save;
%ignore ::pdf_tos_restore;
%ignore ::pdf_tos_get_text;
%ignore ::pdf_tos_reset;
%ignore ::pdf_tos_make_trm;
%ignore ::pdf_tos_move_after_char;
%ignore ::pdf_tos_translate;
%ignore ::pdf_tos_set_matrix;
%ignore ::pdf_tos_newline;
%ignore ::pdf_lookup_page_number;
%ignore ::pdf_count_pages;
%ignore ::pdf_count_pages_imp;
%ignore ::pdf_lookup_page_obj;
%ignore ::pdf_load_page_tree;
%ignore ::pdf_drop_page_tree;
%ignore ::pdf_drop_page_tree_internal;
%ignore ::pdf_lookup_anchor;
%ignore ::pdf_flatten_inheritable_page_items;
%ignore ::pdf_load_page;
%ignore ::pdf_load_page_imp;
%ignore ::pdf_page_has_transparency;
%ignore ::pdf_page_obj_transform;
%ignore ::pdf_page_transform;
%ignore ::pdf_page_resources;
%ignore ::pdf_page_contents;
%ignore ::pdf_page_group;
%ignore ::pdf_page_separations;
%ignore ::pdf_read_ocg;
%ignore ::pdf_drop_ocg;
%ignore ::pdf_is_ocg_hidden;
%ignore ::pdf_load_links;
%ignore ::pdf_bound_page;
%ignore ::pdf_run_page;
%ignore ::pdf_run_page_with_usage;
%ignore ::pdf_run_page_contents;
%ignore ::pdf_run_page_annots;
%ignore ::pdf_run_page_widgets;
%ignore ::pdf_run_page_contents_with_usage;
%ignore ::pdf_run_page_annots_with_usage;
%ignore ::pdf_run_page_widgets_with_usage;
%ignore ::pdf_filter_page_contents;
%ignore ::pdf_filter_annot_contents;
%ignore ::pdf_new_pixmap_from_page_contents_with_usage;
%ignore ::pdf_new_pixmap_from_page_with_usage;
%ignore ::pdf_new_pixmap_from_page_contents_with_separations_and_usage;
%ignore ::pdf_new_pixmap_from_page_with_separations_and_usage;
%ignore ::pdf_redact_page;
%ignore ::pdf_page_presentation;
%ignore ::pdf_load_default_colorspaces;
%ignore ::pdf_update_default_colorspaces;
%ignore ::pdf_string_from_annot_type;
%ignore ::pdf_annot_type_from_string;
%ignore ::pdf_line_ending_from_name;
%ignore ::pdf_line_ending_from_string;
%ignore ::pdf_name_from_line_ending;
%ignore ::pdf_string_from_line_ending;
%ignore ::pdf_keep_annot;
%ignore ::pdf_drop_annot;
%ignore ::pdf_first_annot;
%ignore ::pdf_next_annot;
%ignore ::pdf_annot_obj;
%ignore ::pdf_annot_page;
%ignore ::pdf_bound_annot;
%ignore ::pdf_run_annot;
%ignore ::pdf_lookup_name;
%ignore ::pdf_load_name_tree;
%ignore ::pdf_lookup_number;
%ignore ::pdf_walk_tree;
%ignore ::pdf_resolve_link;
%ignore ::pdf_resolve_link_dest;
%ignore ::pdf_new_destination_from_link;
%ignore ::pdf_new_action_from_link;
%ignore ::pdf_format_link_uri;
%ignore ::pdf_parse_link_uri;
%ignore ::pdf_annot_transform;
%ignore ::pdf_new_link;
%ignore ::pdf_create_annot_raw;
%ignore ::pdf_create_link;
%ignore ::pdf_delete_link;
%ignore ::pdf_create_annot;
%ignore ::pdf_delete_annot;
%ignore ::pdf_set_annot_popup;
%ignore ::pdf_annot_popup;
%ignore ::pdf_annot_has_rect;
%ignore ::pdf_annot_has_ink_list;
%ignore ::pdf_annot_has_quad_points;
%ignore ::pdf_annot_has_vertices;
%ignore ::pdf_annot_has_line;
%ignore ::pdf_annot_has_interior_color;
%ignore ::pdf_annot_has_line_ending_styles;
%ignore ::pdf_annot_has_border;
%ignore ::pdf_annot_has_border_effect;
%ignore ::pdf_annot_has_icon_name;
%ignore ::pdf_annot_has_open;
%ignore ::pdf_annot_has_author;
%ignore ::pdf_annot_flags;
%ignore ::pdf_annot_rect;
%ignore ::pdf_annot_border;
%ignore ::pdf_annot_border_style;
%ignore ::pdf_annot_border_width;
%ignore ::pdf_annot_border_dash_count;
%ignore ::pdf_annot_border_dash_item;
%ignore ::pdf_annot_border_effect;
%ignore ::pdf_annot_border_effect_intensity;
%ignore ::pdf_annot_opacity;
%ignore ::pdf_annot_color;
%ignore ::pdf_annot_interior_color;
%ignore ::pdf_annot_quadding;
%ignore ::pdf_annot_language;
%ignore ::pdf_annot_quad_point_count;
%ignore ::pdf_annot_quad_point;
%ignore ::pdf_annot_ink_list_count;
%ignore ::pdf_annot_ink_list_stroke_count;
%ignore ::pdf_annot_ink_list_stroke_vertex;
%ignore ::pdf_set_annot_flags;
%ignore ::pdf_set_annot_stamp_image;
%ignore ::pdf_set_annot_rect;
%ignore ::pdf_set_annot_border;
%ignore ::pdf_set_annot_border_style;
%ignore ::pdf_set_annot_border_width;
%ignore ::pdf_clear_annot_border_dash;
%ignore ::pdf_add_annot_border_dash_item;
%ignore ::pdf_set_annot_border_effect;
%ignore ::pdf_set_annot_border_effect_intensity;
%ignore ::pdf_set_annot_opacity;
%ignore ::pdf_set_annot_color;
%ignore ::pdf_set_annot_interior_color;
%ignore ::pdf_set_annot_quadding;
%ignore ::pdf_set_annot_language;
%ignore ::pdf_set_annot_quad_points;
%ignore ::pdf_clear_annot_quad_points;
%ignore ::pdf_add_annot_quad_point;
%ignore ::pdf_set_annot_ink_list;
%ignore ::pdf_clear_annot_ink_list;
%ignore ::pdf_add_annot_ink_list_stroke;
%ignore ::pdf_add_annot_ink_list_stroke_vertex;
%ignore ::pdf_add_annot_ink_list;
%ignore ::pdf_set_annot_icon_name;
%ignore ::pdf_set_annot_is_open;
%ignore ::pdf_annot_line_start_style;
%ignore ::pdf_annot_line_end_style;
%ignore ::pdf_annot_line_ending_styles;
%ignore ::pdf_set_annot_line_start_style;
%ignore ::pdf_set_annot_line_end_style;
%ignore ::pdf_set_annot_line_ending_styles;
%ignore ::pdf_annot_icon_name;
%ignore ::pdf_annot_is_open;
%ignore ::pdf_annot_is_standard_stamp;
%ignore ::pdf_annot_line;
%ignore ::pdf_set_annot_line;
%ignore ::pdf_annot_vertex_count;
%ignore ::pdf_annot_vertex;
%ignore ::pdf_set_annot_vertices;
%ignore ::pdf_clear_annot_vertices;
%ignore ::pdf_add_annot_vertex;
%ignore ::pdf_set_annot_vertex;
%ignore ::pdf_annot_contents;
%ignore ::pdf_set_annot_contents;
%ignore ::pdf_annot_author;
%ignore ::pdf_set_annot_author;
%ignore ::pdf_annot_modification_date;
%ignore ::pdf_set_annot_modification_date;
%ignore ::pdf_annot_creation_date;
%ignore ::pdf_set_annot_creation_date;
%ignore ::pdf_parse_default_appearance;
%ignore ::pdf_print_default_appearance;
%ignore ::pdf_annot_default_appearance;
%ignore ::pdf_set_annot_default_appearance;
%ignore ::pdf_annot_request_resynthesis;
%ignore ::pdf_annot_needs_resynthesis;
%ignore ::pdf_set_annot_resynthesised;
%ignore ::pdf_dirty_annot;
%ignore ::pdf_annot_field_flags;
%ignore ::pdf_annot_field_value;
%ignore ::pdf_annot_field_label;
%ignore ::pdf_set_annot_field_value;
%ignore ::pdf_layout_fit_text;
%ignore ::pdf_annot_push_local_xref;
%ignore ::pdf_annot_pop_local_xref;
%ignore ::pdf_annot_ensure_local_xref;
%ignore ::pdf_annot_pop_and_discard_local_xref;
%ignore ::pdf_update_annot;
%ignore ::pdf_update_page;
%ignore ::pdf_set_widget_editing_state;
%ignore ::pdf_get_widget_editing_state;
%ignore ::pdf_toggle_widget;
%ignore ::pdf_new_display_list_from_annot;
%ignore ::pdf_new_pixmap_from_annot;
%ignore ::pdf_new_stext_page_from_annot;
%ignore ::pdf_layout_text_widget;
%ignore ::pdf_is_embedded_file;
%ignore ::pdf_add_embedded_file;
%ignore ::pdf_get_embedded_file_params;
%ignore ::pdf_load_embedded_file_contents;
%ignore ::pdf_verify_embedded_file_checksum;
%ignore ::pdf_parse_link_dest;
%ignore ::pdf_parse_link_action;
%ignore ::pdf_lookup_dest;
%ignore ::pdf_load_link_annots;
%ignore ::pdf_annot_MK_BG;
%ignore ::pdf_annot_MK_BC;
%ignore ::pdf_annot_MK_BG_rgb;
%ignore ::pdf_annot_MK_BC_rgb;
%ignore ::pdf_annot_ap;
%ignore ::pdf_annot_active;
%ignore ::pdf_set_annot_active;
%ignore ::pdf_annot_hot;
%ignore ::pdf_set_annot_hot;
%ignore ::pdf_set_annot_appearance;
%ignore ::pdf_set_annot_appearance_from_display_list;
%ignore ::pdf_annot_has_filespec;
%ignore ::pdf_annot_filespec;
%ignore ::pdf_set_annot_filespec;
%ignore ::pdf_annot_hidden_for_editing;
%ignore ::pdf_set_annot_hidden_for_editing;
%ignore ::pdf_keep_widget;
%ignore ::pdf_drop_widget;
%ignore ::pdf_first_widget;
%ignore ::pdf_next_widget;
%ignore ::pdf_update_widget;
%ignore ::pdf_create_signature_widget;
%ignore ::pdf_bound_widget;
%ignore ::pdf_text_widget_max_len;
%ignore ::pdf_text_widget_format;
%ignore ::pdf_choice_widget_options;
%ignore ::pdf_choice_widget_is_multiselect;
%ignore ::pdf_choice_widget_value;
%ignore ::pdf_choice_widget_set_value;
%ignore ::pdf_choice_field_option_count;
%ignore ::pdf_choice_field_option;
%ignore ::pdf_widget_is_signed;
%ignore ::pdf_widget_is_readonly;
%ignore ::pdf_calculate_form;
%ignore ::pdf_reset_form;
%ignore ::pdf_field_type;
%ignore ::pdf_field_type_string;
%ignore ::pdf_field_flags;
%ignore ::pdf_load_field_name;
%ignore ::pdf_field_value;
%ignore ::pdf_create_field_name;
%ignore ::pdf_field_border_style;
%ignore ::pdf_field_set_border_style;
%ignore ::pdf_field_set_button_caption;
%ignore ::pdf_field_set_fill_color;
%ignore ::pdf_field_set_text_color;
%ignore ::pdf_field_display;
%ignore ::pdf_field_set_display;
%ignore ::pdf_field_label;
%ignore ::pdf_button_field_on_state;
%ignore ::pdf_set_field_value;
%ignore ::pdf_set_text_field_value;
%ignore ::pdf_set_choice_field_value;
%ignore ::pdf_edit_text_field_value;
%ignore ::pdf_signature_is_signed;
%ignore ::pdf_signature_set_value;
%ignore ::pdf_count_signatures;
%ignore ::pdf_signature_error_description;
%ignore ::pdf_signature_get_signatory;
%ignore ::pdf_signature_get_widget_signatory;
%ignore ::pdf_signature_drop_distinguished_name;
%ignore ::pdf_signature_format_distinguished_name;
%ignore ::pdf_signature_info;
%ignore ::pdf_signature_appearance_signed;
%ignore ::pdf_signature_appearance_unsigned;
%ignore ::pdf_check_digest;
%ignore ::pdf_check_certificate;
%ignore ::pdf_check_widget_digest;
%ignore ::pdf_check_widget_certificate;
%ignore ::pdf_clear_signature;
%ignore ::pdf_sign_signature_with_appearance;
%ignore ::pdf_sign_signature;
%ignore ::pdf_preview_signature_as_display_list;
%ignore ::pdf_preview_signature_as_pixmap;
%ignore ::pdf_check_signature;
%ignore ::pdf_drop_signer;
%ignore ::pdf_drop_verifier;
%ignore ::pdf_field_reset;
%ignore ::pdf_lookup_field;
%ignore ::pdf_field_event_keystroke;
%ignore ::pdf_field_event_validate;
%ignore ::pdf_field_event_calculate;
%ignore ::pdf_field_event_format;
%ignore ::pdf_annot_field_event_keystroke;
%ignore ::pdf_document_event_will_close;
%ignore ::pdf_document_event_will_save;
%ignore ::pdf_document_event_did_save;
%ignore ::pdf_document_event_will_print;
%ignore ::pdf_document_event_did_print;
%ignore ::pdf_page_event_open;
%ignore ::pdf_page_event_close;
%ignore ::pdf_annot_event_enter;
%ignore ::pdf_annot_event_exit;
%ignore ::pdf_annot_event_down;
%ignore ::pdf_annot_event_up;
%ignore ::pdf_annot_event_focus;
%ignore ::pdf_annot_event_blur;
%ignore ::pdf_annot_event_page_open;
%ignore ::pdf_annot_event_page_close;
%ignore ::pdf_annot_event_page_visible;
%ignore ::pdf_annot_event_page_invisible;
%ignore ::pdf_set_doc_event_callback;
%ignore ::pdf_get_doc_event_callback_data;
%ignore ::pdf_access_alert_event;
%ignore ::pdf_access_exec_menu_item_event;
%ignore ::pdf_access_launch_url_event;
%ignore ::pdf_access_mail_doc_event;
%ignore ::pdf_event_issue_alert;
%ignore ::pdf_event_issue_print;
%ignore ::pdf_event_issue_exec_menu_item;
%ignore ::pdf_event_issue_launch_url;
%ignore ::pdf_event_issue_mail_doc;
%ignore ::pdf_enable_js;
%ignore ::pdf_disable_js;
%ignore ::pdf_js_supported;
%ignore ::pdf_drop_js;
%ignore ::pdf_js_event_init;
%ignore ::pdf_js_event_result;
%ignore ::pdf_js_event_result_validate;
%ignore ::pdf_js_event_value;
%ignore ::pdf_js_event_init_keystroke;
%ignore ::pdf_js_event_result_keystroke;
%ignore ::pdf_js_execute;
%ignore ::pdf_clean_file;
%ignore fz_append_vprintf;
%ignore mfz_append_vprintf;
%ignore fz_error_stack_slot;
%ignore mfz_error_stack_slot;
%ignore fz_format_string;
%ignore mfz_format_string;
%ignore fz_vsnprintf;
%ignore mfz_vsnprintf;
%ignore fz_vthrow;
%ignore mfz_vthrow;
%ignore fz_vwarn;
%ignore mfz_vwarn;
%ignore fz_write_vprintf;
%ignore mfz_write_vprintf;
%ignore fz_vlog_error_printf;
%ignore mfz_vlog_error_printf;
%ignore fz_utf8_from_wchar;
%ignore mfz_utf8_from_wchar;
%ignore fz_wchar_from_utf8;
%ignore mfz_wchar_from_utf8;
%ignore fz_fopen_utf8;
%ignore mfz_fopen_utf8;
%ignore fz_remove_utf8;
%ignore mfz_remove_utf8;
%ignore fz_argv_from_wargv;
%ignore mfz_argv_from_wargv;
%ignore fz_free_argv;
%ignore mfz_free_argv;
%ignore fz_stdods;
%ignore mfz_stdods;

            // Not implemented in mupdf.so: fz_colorspace_name_process_colorants
            %ignore fz_colorspace_name_process_colorants;
            %ignore fz_argv_from_wargv;

            %ignore fz_open_file_w;

            %ignore ll_fz_append_vprintf;
            %ignore ll_fz_error_stack_slot_s;
            %ignore ll_fz_format_string;
            %ignore ll_fz_vsnprintf;
            %ignore ll_fz_vthrow;
            %ignore ll_fz_vwarn;
            %ignore ll_fz_write_vprintf;
            %ignore ll_fz_vlog_error_printf;
            %ignore ll_fz_open_file_w;

            // Ignore custom C++ variadic fns.
            %ignore ll_pdf_dict_getlv;
            %ignore ll_pdf_dict_getl;
            %ignore pdf_dict_getlv;
            %ignore pdf_dict_getl;

            // SWIG can't handle this because it uses a valist.
            %ignore ll_Memento_vasprintf;

            // asprintf() isn't available on windows, so exclude Memento_asprintf because
            // it is #define-d to asprintf.
            %ignore ll_Memento_asprintf;

            // Might prefer to #include mupdf/exceptions.h and make the
            // %exception block below handle all the different exception types,
            // but swig-3 cannot parse 'throw()' in mupdf/exceptions.h.
            //
            // So for now we just #include <stdexcept> and handle
            // std::exception only.

            %include "typemaps.i"
            %include "cpointer.i"

            // This appears to allow python to call fns taking an int64_t.
            %include "stdint.i"

            %{

            #include <stdexcept>

            #include "mupdf/functions.h"
            #include "mupdf/classes.h"
            #include "mupdf/classes2.h"
            #include "mupdf/internal.h"
            #include "mupdf/exceptions.h"

            #ifdef NDEBUG
                static bool g_mupdf_trace_director = false;
                static bool g_mupdf_trace_exceptions = false;
            #else
                static bool g_mupdf_trace_director = mupdf::internal_env_flag("MUPDF_trace_director");
                static bool g_mupdf_trace_exceptions = mupdf::internal_env_flag("MUPDF_trace_exceptions");
            #endif



static std::string to_stdstring(PyObject* s)
{
    PyObject* repr_str = PyUnicode_AsEncodedString(s, "utf-8", "~E~");
    const char* repr_str_s = PyBytes_AS_STRING(repr_str);
    std::string ret = repr_str_s;
    Py_DECREF(repr_str);
    Py_DECREF(s);
    return ret;
}

static std::string py_repr(PyObject* x)
{
    if (!x) return "<C_nullptr>";
    PyObject* s = PyObject_Repr(x);
    return to_stdstring(s);
}

static std::string py_str(PyObject* x)
{
    if (!x) return "<C_nullptr>";
    PyObject* s = PyObject_Str(x);
    return to_stdstring(s);
}

/* Returns a Python `bytes` containing a copy of a `fz_buffer`'s
data. If <clear> is true we also clear and trim the buffer. */
PyObject* ll_fz_buffer_to_bytes_internal(fz_buffer* buffer, int clear)
{
    unsigned char* c = NULL;
    size_t len = mupdf::ll_fz_buffer_storage(buffer, &c);
    PyObject* ret = PyBytes_FromStringAndSize((const char*) c, (Py_ssize_t) len);
    if (clear)
    {
        /* We mimic the affects of fz_buffer_extract(), which
        leaves the buffer with zero capacity. */
        mupdf::ll_fz_clear_buffer(buffer);
        mupdf::ll_fz_trim_buffer(buffer);
    }
    return ret;
}

/* Returns a Python `memoryview` for specified memory. */
PyObject* python_memoryview_from_memory( void* data, size_t size, int writable)
{
    return PyMemoryView_FromMemory(
            (char*) data,
            (Py_ssize_t) size,
            writable ? PyBUF_WRITE : PyBUF_READ
            );
}

/* Returns a Python `memoryview` for a `fz_buffer`'s data. */
PyObject* ll_fz_buffer_storage_memoryview(fz_buffer* buffer, int writable)
{
    unsigned char* data = NULL;
    size_t len = mupdf::ll_fz_buffer_storage(buffer, &data);
    return python_memoryview_from_memory( data, len, writable);
}

/* Creates Python bytes from copy of raw data. */
PyObject* raw_to_python_bytes(const unsigned char* c, size_t len)
{
    return PyBytes_FromStringAndSize((const char*) c, (Py_ssize_t) len);
}

/* Creates Python bytes from copy of raw data. */
PyObject* raw_to_python_bytes(const void* c, size_t len)
{
    return PyBytes_FromStringAndSize((const char*) c, (Py_ssize_t) len);
}

/* The SWIG wrapper for this function returns a SWIG proxy for
a 'const unsigned char*' pointing to the raw data of a python
bytes. This proxy can then be passed from Python to functions
that take a 'const unsigned char*'.

For example to create a MuPDF fz_buffer* from a copy of a
Python bytes instance:
    bs = b'qwerty'
    buffer_ = mupdf.fz_new_buffer_from_copied_data(mupdf.python_buffer_data(bs), len(bs))
*/
const unsigned char* python_buffer_data(
        const unsigned char* PYTHON_BUFFER_DATA,
        size_t PYTHON_BUFFER_SIZE
        )
{
    return PYTHON_BUFFER_DATA;
}

unsigned char* python_mutable_buffer_data(
        unsigned char* PYTHON_BUFFER_MUTABLE_DATA,
        size_t PYTHON_BUFFER_MUTABLE_SIZE
        )
{
    return PYTHON_BUFFER_MUTABLE_DATA;
}

/* Casts an integer to a pdf_obj*. Used to convert SWIG's int
values for PDF_ENUM_NAME_* into PdfObj's. */
pdf_obj* obj_enum_to_obj(int n)
{
    return (pdf_obj*) (intptr_t) n;
}

/* SWIG-friendly alternative to ll_pdf_set_annot_color(). */
void ll_pdf_set_annot_color2(pdf_annot *annot, int n, float color0, float color1, float color2, float color3)
{
    float color[] = { color0, color1, color2, color3 };
    return mupdf::ll_pdf_set_annot_color(annot, n, color);
}


/* SWIG-friendly alternative to ll_pdf_set_annot_interior_color(). */
void ll_pdf_set_annot_interior_color2(pdf_annot *annot, int n, float color0, float color1, float color2, float color3)
{
    float color[] = { color0, color1, color2, color3 };
    return mupdf::ll_pdf_set_annot_interior_color(annot, n, color);
}

/* SWIG-friendly alternative to `fz_fill_text()`. */
void ll_fz_fill_text2(
        fz_device* dev,
        const fz_text* text,
        fz_matrix ctm,
        fz_colorspace* colorspace,
        float color0,
        float color1,
        float color2,
        float color3,
        float alpha,
        fz_color_params color_params
        )
{
    float color[] = {color0, color1, color2, color3};
    return mupdf::ll_fz_fill_text(dev, text, ctm, colorspace, color, alpha, color_params);
}

std::vector<unsigned char> fz_memrnd2(int length)
{
    std::vector<unsigned char>  ret(length);
    mupdf::fz_memrnd(&ret[0], length);
    return ret;
}


/* mupdfpy optimisation for copying pixmap. Copies first <n>
bytes of each pixel from <src> to <pm>. <pm> and <src> must
have same `.w` and `.h` */
void ll_fz_pixmap_copy( fz_pixmap* pm, const fz_pixmap* src, int n)
{
    assert( pm->w == src->w);
    assert( pm->h == src->h);
    assert( n <= pm->n);
    assert( n <= src->n);

    if (pm->n == src->n)
    {
        // identical samples
        assert( pm->stride == src->stride);
        memcpy( pm->samples, src->samples, pm->w * pm->h * pm->n);
    }
    else
    {
        for ( int y=0; y<pm->h; ++y)
        {
            for ( int x=0; x<pm->w; ++x)
            {
                memcpy(
                        pm->samples + pm->stride * y + pm->n * x,
                        src->samples + src->stride * y + src->n * x,
                        n
                        );
                if (pm->alpha)
                {
                    src->samples[ src->stride * y + src->n * x] = 255;
                }
            }
        }
    }
}

/* mupdfpy optimisation for copying raw data into pixmap. `samples` must
have enough data to fill the pixmap. */
void ll_fz_pixmap_copy_raw( fz_pixmap* pm, const void* samples)
{
    memcpy(pm->samples, samples, pm->stride * pm->h);
}

/* SWIG-friendly alternative to fz_runetochar(). */
std::vector<unsigned char> fz_runetochar2(int rune)
{
    std::vector<unsigned char>  buffer(10);
    int n = mupdf::ll_fz_runetochar((char*) &buffer[0], rune);
    assert(n < sizeof(buffer));
    buffer.resize(n);
    return buffer;
}

/* SWIG-friendly alternatives to fz_make_bookmark() and
fz_lookup_bookmark(), using long long instead of fz_bookmark
because SWIG appears to treat fz_bookmark as an int despite it
being a typedef for intptr_t, so ends up slicing. */
long long unsigned ll_fz_make_bookmark2(fz_document* doc, fz_location loc)
{
    fz_bookmark bm = mupdf::ll_fz_make_bookmark(doc, loc);
    return (long long unsigned) bm;
}

fz_location ll_fz_lookup_bookmark2(fz_document *doc, long long unsigned mark)
{
    return mupdf::ll_fz_lookup_bookmark(doc, (fz_bookmark) mark);
}
mupdf::FzLocation fz_lookup_bookmark2( mupdf::FzDocument doc, long long unsigned mark)
{
    return mupdf::FzLocation( ll_fz_lookup_bookmark2(doc.m_internal, mark));
}

struct fz_convert_color2_v
{
    float v0;
    float v1;
    float v2;
    float v3;
};

/* SWIG-friendly alternative for
ll_fz_convert_color(), taking `float* sv`. */
void ll_fz_convert_color2(
        fz_colorspace *ss,
        float* sv,
        fz_colorspace *ds,
        fz_convert_color2_v* dv,
        fz_colorspace *is,
        fz_color_params params
        )
{
    //float sv[] = { sv0, sv1, sv2, sv3};
    mupdf::ll_fz_convert_color(ss, sv, ds, &dv->v0, is, params);
}

/* SWIG-friendly alternative for
ll_fz_convert_color(), taking four explicit `float`
values for `sv`. */
void ll_fz_convert_color2(
        fz_colorspace *ss,
        float sv0,
        float sv1,
        float sv2,
        float sv3,
        fz_colorspace *ds,
        fz_convert_color2_v* dv,
        fz_colorspace *is,
        fz_color_params params
        )
{
    float sv[] = { sv0, sv1, sv2, sv3};
    mupdf::ll_fz_convert_color(ss, sv, ds, &dv->v0, is, params);
}

/* SWIG- Director class to allow fz_set_warning_callback() and
fz_set_error_callback() to be used with Python callbacks. Note that
we rename print() to _print() to match what SWIG does. */
struct DiagnosticCallback
{
    /* `description` must be "error" or "warning". */
    DiagnosticCallback(const char* description)
    :
    m_description(description)
    {
        #ifndef NDEBUG
        if (g_mupdf_trace_director)
        {
            std::cerr
                    << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
                    << " DiagnosticCallback[" << m_description << "]() constructor."
                    << "\n";
        }
        #endif
        if (m_description == "warning")
        {
            mupdf::ll_fz_set_warning_callback( s_print, this);
        }
        else if (m_description == "error")
        {
            mupdf::ll_fz_set_error_callback( s_print, this);
        }
        else
        {
            std::cerr
                    << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
                    << " DiagnosticCallback() constructor"
                    << " Unrecognised description: " << m_description
                    << "\n";
            assert(0);
        }
    }
    virtual void _print( const char* message)
    {
        #ifndef NDEBUG
        if (g_mupdf_trace_director)
        {
            std::cerr
                    << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
                    << " DiagnosticCallback[" << m_description << "]::_print()"
                    << " called (no derived class?)" << " message: " << message
                    << "\n";
        }
        #endif
    }
    virtual ~DiagnosticCallback()
    {
        #ifndef NDEBUG
        if (g_mupdf_trace_director)
        {
            std::cerr
                    << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
                    << " ~DiagnosticCallback[" << m_description << "]() destructor called"
                    << " this=" << this
                    << "\n";
        }
        #endif
    }
    static void s_print( void* self0, const char* message)
    {
        DiagnosticCallback* self = (DiagnosticCallback*) self0;
        try
        {
            return self->_print( message);
        }
        catch (std::exception& e)
        {
            /* It's important to swallow any exception from
            self->_print() because fz_set_warning_callback() and
            fz_set_error_callback() specifically require that
            the callback does not throw. But we always output a
            diagnostic. */
            std::cerr
                    << "DiagnosticCallback[" << self->m_description << "]::s_print()"
                    << " ignoring exception from _print(): "
                    << e.what()
                    << "\n";
        }
    }
    std::string m_description;
};

struct StoryPositionsCallback
{
    StoryPositionsCallback()
    {
        //printf( "StoryPositionsCallback() constructor\n");
    }

    virtual void call( const fz_story_element_position* position) = 0;

    static void s_call( fz_context* ctx, void* self0, const fz_story_element_position* position)
    {
        //printf( "StoryPositionsCallback::s_call()\n");
        (void) ctx;
        StoryPositionsCallback* self = (StoryPositionsCallback*) self0;
        self->call( position);
    }

    virtual ~StoryPositionsCallback()
    {
        //printf( "StoryPositionsCallback() destructor\n");
    }
};

void ll_fz_story_positions_director( fz_story *story, StoryPositionsCallback* cb)
{
    //printf( "ll_fz_story_positions_director()\n");
    mupdf::ll_fz_story_positions(
            story,
            StoryPositionsCallback::s_call,
            cb
            );
}

void Pixmap_set_alpha_helper(
    int balen,
    int n,
    int data_len,
    int zero_out,
    unsigned char* data,
    fz_pixmap* pix,
    int premultiply,
    int bground,
    const std::vector<int>& colors,
    const std::vector<int>& bgcolor
    )
{
    int i = 0;
    int j = 0;
    int k = 0;
    int data_fix = 255;
    while (i < balen) {
        unsigned char alpha = data[k];
        if (zero_out) {
            for (j = i; j < i+n; j++) {
                if (pix->samples[j] != (unsigned char) colors[j - i]) {
                    data_fix = 255;
                    break;
                } else {
                    data_fix = 0;
                }
            }
        }
        if (data_len) {
            if (data_fix == 0) {
                pix->samples[i+n] = 0;
            } else {
                pix->samples[i+n] = alpha;
            }
            if (premultiply && !bground) {
                for (j = i; j < i+n; j++) {
                    pix->samples[j] = fz_mul255(pix->samples[j], alpha);
                }
            } else if (bground) {
                for (j = i; j < i+n; j++) {
                    int m = (unsigned char) bgcolor[j - i];
                    pix->samples[j] = m + fz_mul255((pix->samples[j] - m), alpha);
                }
            }
        } else {
            pix->samples[i+n] = data_fix;
        }
        i += n+1;
        k += 1;
    }
}

void page_merge_helper(
        mupdf::PdfObj& old_annots,
        mupdf::PdfGraftMap& graft_map,
        mupdf::PdfDocument& doc_des,
        mupdf::PdfObj& new_annots,
        int n
        )
{
    for ( int i=0; i<n; ++i)
    {
        mupdf::PdfObj o = mupdf::pdf_array_get( old_annots, i);
        if (mupdf::pdf_dict_gets( o, "IRT").m_internal)
            continue;
        mupdf::PdfObj subtype = mupdf::pdf_dict_get( o, PDF_NAME(Subtype));
        if ( mupdf::pdf_name_eq( subtype, PDF_NAME(Link)))
            continue;
        if ( mupdf::pdf_name_eq( subtype, PDF_NAME(Popup)))
            continue;
        if ( mupdf::pdf_name_eq( subtype, PDF_NAME(Widget)))
        {
            /* fixme: C++ API doesn't yet wrap fz_warn() - it
            excludes all variadic fns. */
            //mupdf::fz_warn( "skipping widget annotation");
            continue;
        }
        mupdf::pdf_dict_del( o, PDF_NAME(Popup));
        mupdf::pdf_dict_del( o, PDF_NAME(P));
        mupdf::PdfObj copy_o = mupdf::pdf_graft_mapped_object( graft_map, o);
        mupdf::PdfObj annot = mupdf::pdf_new_indirect( doc_des, mupdf::pdf_to_num( copy_o), 0);
        mupdf::pdf_array_push( new_annots, annot);
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_bitmap_details(). */
    struct ll_fz_bitmap_details_outparams
    {
        int w;
        int h;
        int n;
        int stride;
    };

    /* Out-params function for fz_bitmap_details(). */
    void ll_fz_bitmap_details_outparams_fn(::fz_bitmap *bitmap, ll_fz_bitmap_details_outparams* outparams)
    {
        int w;
        int h;
        int n;
        int stride;
        ll_fz_bitmap_details(bitmap, &w, &h, &n, &stride);
        outparams->w = w;
        outparams->h = h;
        outparams->n = n;
        outparams->stride = stride;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_buffer_extract(). */
    struct ll_fz_buffer_extract_outparams
    {
        unsigned char *data;
    };

    /* Out-params function for fz_buffer_extract(). */
    size_t ll_fz_buffer_extract_outparams_fn(::fz_buffer *buf, ll_fz_buffer_extract_outparams* outparams)
    {
        outparams->data = NULL;
        unsigned char *data;
        size_t ret = ll_fz_buffer_extract(buf, &data);
        outparams->data = data;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_buffer_storage(). */
    struct ll_fz_buffer_storage_outparams
    {
        unsigned char *datap;
    };

    /* Out-params function for fz_buffer_storage(). */
    size_t ll_fz_buffer_storage_outparams_fn(::fz_buffer *buf, ll_fz_buffer_storage_outparams* outparams)
    {
        outparams->datap = NULL;
        unsigned char *datap;
        size_t ret = ll_fz_buffer_storage(buf, &datap);
        outparams->datap = datap;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_chartorune(). */
    struct ll_fz_chartorune_outparams
    {
        int rune;
    };

    /* Out-params function for fz_chartorune(). */
    int ll_fz_chartorune_outparams_fn(const char *str, ll_fz_chartorune_outparams* outparams)
    {
        int rune;
        int ret = ll_fz_chartorune(&rune, str);
        outparams->rune = rune;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_clamp_color(). */
    struct ll_fz_clamp_color_outparams
    {
        float out;
    };

    /* Out-params function for fz_clamp_color(). */
    void ll_fz_clamp_color_outparams_fn(::fz_colorspace *cs, const float *in, ll_fz_clamp_color_outparams* outparams)
    {
        float out;
        ll_fz_clamp_color(cs, in, &out);
        outparams->out = out;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_convert_color(). */
    struct ll_fz_convert_color_outparams
    {
        float dv;
    };

    /* Out-params function for fz_convert_color(). */
    void ll_fz_convert_color_outparams_fn(::fz_colorspace *ss, const float *sv, ::fz_colorspace *ds, ::fz_colorspace *is, ::fz_color_params params, ll_fz_convert_color_outparams* outparams)
    {
        float dv;
        ll_fz_convert_color(ss, sv, ds, &dv, is, params);
        outparams->dv = dv;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_convert_separation_colors(). */
    struct ll_fz_convert_separation_colors_outparams
    {
        float dst_color;
    };

    /* Out-params function for fz_convert_separation_colors(). */
    void ll_fz_convert_separation_colors_outparams_fn(::fz_colorspace *src_cs, const float *src_color, ::fz_separations *dst_seps, ::fz_colorspace *dst_cs, ::fz_color_params color_params, ll_fz_convert_separation_colors_outparams* outparams)
    {
        float dst_color;
        ll_fz_convert_separation_colors(src_cs, src_color, dst_seps, dst_cs, &dst_color, color_params);
        outparams->dst_color = dst_color;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_decomp_image_from_stream(). */
    struct ll_fz_decomp_image_from_stream_outparams
    {
        int l2extra;
    };

    /* Out-params function for fz_decomp_image_from_stream(). */
    ::fz_pixmap *ll_fz_decomp_image_from_stream_outparams_fn(::fz_stream *stm, ::fz_compressed_image *image, ::fz_irect *subarea, int indexed, int l2factor, ll_fz_decomp_image_from_stream_outparams* outparams)
    {
        int l2extra;
        ::fz_pixmap *ret = ll_fz_decomp_image_from_stream(stm, image, subarea, indexed, l2factor, &l2extra);
        outparams->l2extra = l2extra;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_deflate(). */
    struct ll_fz_deflate_outparams
    {
        size_t compressed_length;
    };

    /* Out-params function for fz_deflate(). */
    void ll_fz_deflate_outparams_fn(unsigned char *dest, const unsigned char *source, size_t source_length, ::fz_deflate_level level, ll_fz_deflate_outparams* outparams)
    {
        size_t compressed_length;
        ll_fz_deflate(dest, &compressed_length, source, source_length, level);
        outparams->compressed_length = compressed_length;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_dom_get_attribute(). */
    struct ll_fz_dom_get_attribute_outparams
    {
        const char *att;
    };

    /* Out-params function for fz_dom_get_attribute(). */
    const char *ll_fz_dom_get_attribute_outparams_fn(::fz_xml *elt, int i, ll_fz_dom_get_attribute_outparams* outparams)
    {
        outparams->att = NULL;
        const char *att;
        const char *ret = ll_fz_dom_get_attribute(elt, i, &att);
        outparams->att = att;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_drop_imp(). */
    struct ll_fz_drop_imp_outparams
    {
        int refs;
    };

    /* Out-params function for fz_drop_imp(). */
    int ll_fz_drop_imp_outparams_fn(void *p, ll_fz_drop_imp_outparams* outparams)
    {
        int refs;
        int ret = ll_fz_drop_imp(p, &refs);
        outparams->refs = refs;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_drop_imp16(). */
    struct ll_fz_drop_imp16_outparams
    {
        short refs;
    };

    /* Out-params function for fz_drop_imp16(). */
    int ll_fz_drop_imp16_outparams_fn(void *p, ll_fz_drop_imp16_outparams* outparams)
    {
        int16_t refs;
        int ret = ll_fz_drop_imp16(p, &refs);
        outparams->refs = refs;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_encode_character_with_fallback(). */
    struct ll_fz_encode_character_with_fallback_outparams
    {
        struct ::fz_font *out_font;
    };

    /* Out-params function for fz_encode_character_with_fallback(). */
    int ll_fz_encode_character_with_fallback_outparams_fn(::fz_font *font, int unicode, int script, int language, ll_fz_encode_character_with_fallback_outparams* outparams)
    {
        outparams->out_font = NULL;
        ::fz_font *out_font;
        int ret = ll_fz_encode_character_with_fallback(font, unicode, script, language, &out_font);
        outparams->out_font = out_font;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_error_callback(). */
    struct ll_fz_error_callback_outparams
    {
        void *user;
    };

    /* Out-params function for fz_error_callback(). */
    ::fz_error_cb *ll_fz_error_callback_outparams_fn(ll_fz_error_callback_outparams* outparams)
    {
        outparams->user = NULL;
        void *user;
        ::fz_error_cb *ret = ll_fz_error_callback(&user);
        outparams->user = user;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_fill_pixmap_with_color(). */
    struct ll_fz_fill_pixmap_with_color_outparams
    {
        float color;
    };

    /* Out-params function for fz_fill_pixmap_with_color(). */
    void ll_fz_fill_pixmap_with_color_outparams_fn(::fz_pixmap *pix, ::fz_colorspace *colorspace, ::fz_color_params color_params, ll_fz_fill_pixmap_with_color_outparams* outparams)
    {
        float color;
        ll_fz_fill_pixmap_with_color(pix, colorspace, &color, color_params);
        outparams->color = color;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_get_pixmap_from_image(). */
    struct ll_fz_get_pixmap_from_image_outparams
    {
        int w;
        int h;
    };

    /* Out-params function for fz_get_pixmap_from_image(). */
    ::fz_pixmap *ll_fz_get_pixmap_from_image_outparams_fn(::fz_image *image, const ::fz_irect *subarea, ::fz_matrix *ctm, ll_fz_get_pixmap_from_image_outparams* outparams)
    {
        int w;
        int h;
        ::fz_pixmap *ret = ll_fz_get_pixmap_from_image(image, subarea, ctm, &w, &h);
        outparams->w = w;
        outparams->h = h;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_getopt(). */
    struct ll_fz_getopt_outparams
    {
        char *nargv;
    };

    /* Out-params function for fz_getopt(). */
    int ll_fz_getopt_outparams_fn(int nargc, const char *ostr, ll_fz_getopt_outparams* outparams)
    {
        outparams->nargv = NULL;
        char *nargv;
        int ret = ll_fz_getopt(nargc, &nargv, ostr);
        outparams->nargv = nargv;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_grisu(). */
    struct ll_fz_grisu_outparams
    {
        int exp;
    };

    /* Out-params function for fz_grisu(). */
    int ll_fz_grisu_outparams_fn(float f, char *s, ll_fz_grisu_outparams* outparams)
    {
        int exp;
        int ret = ll_fz_grisu(f, s, &exp);
        outparams->exp = exp;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_has_option(). */
    struct ll_fz_has_option_outparams
    {
        const char *val;
    };

    /* Out-params function for fz_has_option(). */
    int ll_fz_has_option_outparams_fn(const char *opts, const char *key, ll_fz_has_option_outparams* outparams)
    {
        outparams->val = NULL;
        const char *val;
        int ret = ll_fz_has_option(opts, key, &val);
        outparams->val = val;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_image_resolution(). */
    struct ll_fz_image_resolution_outparams
    {
        int xres;
        int yres;
    };

    /* Out-params function for fz_image_resolution(). */
    void ll_fz_image_resolution_outparams_fn(::fz_image *image, ll_fz_image_resolution_outparams* outparams)
    {
        int xres;
        int yres;
        ll_fz_image_resolution(image, &xres, &yres);
        outparams->xres = xres;
        outparams->yres = yres;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_keep_imp(). */
    struct ll_fz_keep_imp_outparams
    {
        int refs;
    };

    /* Out-params function for fz_keep_imp(). */
    void *ll_fz_keep_imp_outparams_fn(void *p, ll_fz_keep_imp_outparams* outparams)
    {
        int refs;
        void *ret = ll_fz_keep_imp(p, &refs);
        outparams->refs = refs;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_keep_imp16(). */
    struct ll_fz_keep_imp16_outparams
    {
        short refs;
    };

    /* Out-params function for fz_keep_imp16(). */
    void *ll_fz_keep_imp16_outparams_fn(void *p, ll_fz_keep_imp16_outparams* outparams)
    {
        int16_t refs;
        void *ret = ll_fz_keep_imp16(p, &refs);
        outparams->refs = refs;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_keep_imp_locked(). */
    struct ll_fz_keep_imp_locked_outparams
    {
        int refs;
    };

    /* Out-params function for fz_keep_imp_locked(). */
    void *ll_fz_keep_imp_locked_outparams_fn(void *p, ll_fz_keep_imp_locked_outparams* outparams)
    {
        int refs;
        void *ret = ll_fz_keep_imp_locked(p, &refs);
        outparams->refs = refs;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_lookup_base14_font(). */
    struct ll_fz_lookup_base14_font_outparams
    {
        int len;
    };

    /* Out-params function for fz_lookup_base14_font(). */
    const unsigned char *ll_fz_lookup_base14_font_outparams_fn(const char *name, ll_fz_lookup_base14_font_outparams* outparams)
    {
        int len;
        const unsigned char *ret = ll_fz_lookup_base14_font(name, &len);
        outparams->len = len;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_lookup_builtin_font(). */
    struct ll_fz_lookup_builtin_font_outparams
    {
        int len;
    };

    /* Out-params function for fz_lookup_builtin_font(). */
    const unsigned char *ll_fz_lookup_builtin_font_outparams_fn(const char *name, int bold, int italic, ll_fz_lookup_builtin_font_outparams* outparams)
    {
        int len;
        const unsigned char *ret = ll_fz_lookup_builtin_font(name, bold, italic, &len);
        outparams->len = len;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_lookup_cjk_font(). */
    struct ll_fz_lookup_cjk_font_outparams
    {
        int len;
        int index;
    };

    /* Out-params function for fz_lookup_cjk_font(). */
    const unsigned char *ll_fz_lookup_cjk_font_outparams_fn(int ordering, ll_fz_lookup_cjk_font_outparams* outparams)
    {
        int len;
        int index;
        const unsigned char *ret = ll_fz_lookup_cjk_font(ordering, &len, &index);
        outparams->len = len;
        outparams->index = index;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_lookup_cjk_font_by_language(). */
    struct ll_fz_lookup_cjk_font_by_language_outparams
    {
        int len;
        int subfont;
    };

    /* Out-params function for fz_lookup_cjk_font_by_language(). */
    const unsigned char *ll_fz_lookup_cjk_font_by_language_outparams_fn(const char *lang, ll_fz_lookup_cjk_font_by_language_outparams* outparams)
    {
        int len;
        int subfont;
        const unsigned char *ret = ll_fz_lookup_cjk_font_by_language(lang, &len, &subfont);
        outparams->len = len;
        outparams->subfont = subfont;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_lookup_noto_emoji_font(). */
    struct ll_fz_lookup_noto_emoji_font_outparams
    {
        int len;
    };

    /* Out-params function for fz_lookup_noto_emoji_font(). */
    const unsigned char *ll_fz_lookup_noto_emoji_font_outparams_fn(ll_fz_lookup_noto_emoji_font_outparams* outparams)
    {
        int len;
        const unsigned char *ret = ll_fz_lookup_noto_emoji_font(&len);
        outparams->len = len;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_lookup_noto_font(). */
    struct ll_fz_lookup_noto_font_outparams
    {
        int len;
        int subfont;
    };

    /* Out-params function for fz_lookup_noto_font(). */
    const unsigned char *ll_fz_lookup_noto_font_outparams_fn(int script, int lang, ll_fz_lookup_noto_font_outparams* outparams)
    {
        int len;
        int subfont;
        const unsigned char *ret = ll_fz_lookup_noto_font(script, lang, &len, &subfont);
        outparams->len = len;
        outparams->subfont = subfont;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_lookup_noto_math_font(). */
    struct ll_fz_lookup_noto_math_font_outparams
    {
        int len;
    };

    /* Out-params function for fz_lookup_noto_math_font(). */
    const unsigned char *ll_fz_lookup_noto_math_font_outparams_fn(ll_fz_lookup_noto_math_font_outparams* outparams)
    {
        int len;
        const unsigned char *ret = ll_fz_lookup_noto_math_font(&len);
        outparams->len = len;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_lookup_noto_music_font(). */
    struct ll_fz_lookup_noto_music_font_outparams
    {
        int len;
    };

    /* Out-params function for fz_lookup_noto_music_font(). */
    const unsigned char *ll_fz_lookup_noto_music_font_outparams_fn(ll_fz_lookup_noto_music_font_outparams* outparams)
    {
        int len;
        const unsigned char *ret = ll_fz_lookup_noto_music_font(&len);
        outparams->len = len;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_lookup_noto_symbol1_font(). */
    struct ll_fz_lookup_noto_symbol1_font_outparams
    {
        int len;
    };

    /* Out-params function for fz_lookup_noto_symbol1_font(). */
    const unsigned char *ll_fz_lookup_noto_symbol1_font_outparams_fn(ll_fz_lookup_noto_symbol1_font_outparams* outparams)
    {
        int len;
        const unsigned char *ret = ll_fz_lookup_noto_symbol1_font(&len);
        outparams->len = len;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_lookup_noto_symbol2_font(). */
    struct ll_fz_lookup_noto_symbol2_font_outparams
    {
        int len;
    };

    /* Out-params function for fz_lookup_noto_symbol2_font(). */
    const unsigned char *ll_fz_lookup_noto_symbol2_font_outparams_fn(ll_fz_lookup_noto_symbol2_font_outparams* outparams)
    {
        int len;
        const unsigned char *ret = ll_fz_lookup_noto_symbol2_font(&len);
        outparams->len = len;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_new_deflated_data(). */
    struct ll_fz_new_deflated_data_outparams
    {
        size_t compressed_length;
    };

    /* Out-params function for fz_new_deflated_data(). */
    unsigned char *ll_fz_new_deflated_data_outparams_fn(const unsigned char *source, size_t source_length, ::fz_deflate_level level, ll_fz_new_deflated_data_outparams* outparams)
    {
        size_t compressed_length;
        unsigned char *ret = ll_fz_new_deflated_data(&compressed_length, source, source_length, level);
        outparams->compressed_length = compressed_length;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_new_deflated_data_from_buffer(). */
    struct ll_fz_new_deflated_data_from_buffer_outparams
    {
        size_t compressed_length;
    };

    /* Out-params function for fz_new_deflated_data_from_buffer(). */
    unsigned char *ll_fz_new_deflated_data_from_buffer_outparams_fn(::fz_buffer *buffer, ::fz_deflate_level level, ll_fz_new_deflated_data_from_buffer_outparams* outparams)
    {
        size_t compressed_length;
        unsigned char *ret = ll_fz_new_deflated_data_from_buffer(&compressed_length, buffer, level);
        outparams->compressed_length = compressed_length;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_new_display_list_from_svg(). */
    struct ll_fz_new_display_list_from_svg_outparams
    {
        float w;
        float h;
    };

    /* Out-params function for fz_new_display_list_from_svg(). */
    ::fz_display_list *ll_fz_new_display_list_from_svg_outparams_fn(::fz_buffer *buf, const char *base_uri, ::fz_archive *zip, ll_fz_new_display_list_from_svg_outparams* outparams)
    {
        float w;
        float h;
        ::fz_display_list *ret = ll_fz_new_display_list_from_svg(buf, base_uri, zip, &w, &h);
        outparams->w = w;
        outparams->h = h;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_new_display_list_from_svg_xml(). */
    struct ll_fz_new_display_list_from_svg_xml_outparams
    {
        float w;
        float h;
    };

    /* Out-params function for fz_new_display_list_from_svg_xml(). */
    ::fz_display_list *ll_fz_new_display_list_from_svg_xml_outparams_fn(::fz_xml_doc *xmldoc, ::fz_xml *xml, const char *base_uri, ::fz_archive *zip, ll_fz_new_display_list_from_svg_xml_outparams* outparams)
    {
        float w;
        float h;
        ::fz_display_list *ret = ll_fz_new_display_list_from_svg_xml(xmldoc, xml, base_uri, zip, &w, &h);
        outparams->w = w;
        outparams->h = h;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_new_draw_device_with_options(). */
    struct ll_fz_new_draw_device_with_options_outparams
    {
        struct ::fz_pixmap *pixmap;
    };

    /* Out-params function for fz_new_draw_device_with_options(). */
    ::fz_device *ll_fz_new_draw_device_with_options_outparams_fn(const ::fz_draw_options *options, ::fz_rect mediabox, ll_fz_new_draw_device_with_options_outparams* outparams)
    {
        outparams->pixmap = NULL;
        ::fz_pixmap *pixmap;
        ::fz_device *ret = ll_fz_new_draw_device_with_options(options, mediabox, &pixmap);
        outparams->pixmap = pixmap;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_new_image_from_compressed_buffer(). */
    struct ll_fz_new_image_from_compressed_buffer_outparams
    {
        float decode;
        int colorkey;
    };

    /* Out-params function for fz_new_image_from_compressed_buffer(). */
    ::fz_image *ll_fz_new_image_from_compressed_buffer_outparams_fn(int w, int h, int bpc, ::fz_colorspace *colorspace, int xres, int yres, int interpolate, int imagemask, ::fz_compressed_buffer *buffer, ::fz_image *mask, ll_fz_new_image_from_compressed_buffer_outparams* outparams)
    {
        float decode;
        int colorkey;
        ::fz_image *ret = ll_fz_new_image_from_compressed_buffer(w, h, bpc, colorspace, xres, yres, interpolate, imagemask, &decode, &colorkey, buffer, mask);
        outparams->decode = decode;
        outparams->colorkey = colorkey;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_new_image_of_size(). */
    struct ll_fz_new_image_of_size_outparams
    {
        float decode;
        int colorkey;
    };

    /* Out-params function for fz_new_image_of_size(). */
    ::fz_image *ll_fz_new_image_of_size_outparams_fn(int w, int h, int bpc, ::fz_colorspace *colorspace, int xres, int yres, int interpolate, int imagemask, ::fz_image *mask, size_t size, ::fz_image_get_pixmap_fn *get_pixmap, ::fz_image_get_size_fn *get_size, ::fz_drop_image_fn *drop, ll_fz_new_image_of_size_outparams* outparams)
    {
        float decode;
        int colorkey;
        ::fz_image *ret = ll_fz_new_image_of_size(w, h, bpc, colorspace, xres, yres, interpolate, imagemask, &decode, &colorkey, mask, size, get_pixmap, get_size, drop);
        outparams->decode = decode;
        outparams->colorkey = colorkey;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_new_svg_device_with_id(). */
    struct ll_fz_new_svg_device_with_id_outparams
    {
        int id;
    };

    /* Out-params function for fz_new_svg_device_with_id(). */
    ::fz_device *ll_fz_new_svg_device_with_id_outparams_fn(::fz_output *out, float page_width, float page_height, int text_format, int reuse_images, ll_fz_new_svg_device_with_id_outparams* outparams)
    {
        int id;
        ::fz_device *ret = ll_fz_new_svg_device_with_id(out, page_width, page_height, text_format, reuse_images, &id);
        outparams->id = id;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_new_test_device(). */
    struct ll_fz_new_test_device_outparams
    {
        int is_color;
    };

    /* Out-params function for fz_new_test_device(). */
    ::fz_device *ll_fz_new_test_device_outparams_fn(float threshold, int options, ::fz_device *passthrough, ll_fz_new_test_device_outparams* outparams)
    {
        int is_color;
        ::fz_device *ret = ll_fz_new_test_device(&is_color, threshold, options, passthrough);
        outparams->is_color = is_color;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_open_image_decomp_stream(). */
    struct ll_fz_open_image_decomp_stream_outparams
    {
        int l2factor;
    };

    /* Out-params function for fz_open_image_decomp_stream(). */
    ::fz_stream *ll_fz_open_image_decomp_stream_outparams_fn(::fz_stream *arg_0, ::fz_compression_params *arg_1, ll_fz_open_image_decomp_stream_outparams* outparams)
    {
        int l2factor;
        ::fz_stream *ret = ll_fz_open_image_decomp_stream(arg_0, arg_1, &l2factor);
        outparams->l2factor = l2factor;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_open_image_decomp_stream_from_buffer(). */
    struct ll_fz_open_image_decomp_stream_from_buffer_outparams
    {
        int l2factor;
    };

    /* Out-params function for fz_open_image_decomp_stream_from_buffer(). */
    ::fz_stream *ll_fz_open_image_decomp_stream_from_buffer_outparams_fn(::fz_compressed_buffer *arg_0, ll_fz_open_image_decomp_stream_from_buffer_outparams* outparams)
    {
        int l2factor;
        ::fz_stream *ret = ll_fz_open_image_decomp_stream_from_buffer(arg_0, &l2factor);
        outparams->l2factor = l2factor;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_page_presentation(). */
    struct ll_fz_page_presentation_outparams
    {
        float duration;
    };

    /* Out-params function for fz_page_presentation(). */
    ::fz_transition *ll_fz_page_presentation_outparams_fn(::fz_page *page, ::fz_transition *transition, ll_fz_page_presentation_outparams* outparams)
    {
        float duration;
        ::fz_transition *ret = ll_fz_page_presentation(page, transition, &duration);
        outparams->duration = duration;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_paint_shade(). */
    struct ll_fz_paint_shade_outparams
    {
        struct ::fz_shade_color_cache *cache;
    };

    /* Out-params function for fz_paint_shade(). */
    void ll_fz_paint_shade_outparams_fn(::fz_shade *shade, ::fz_colorspace *override_cs, ::fz_matrix ctm, ::fz_pixmap *dest, ::fz_color_params color_params, ::fz_irect bbox, const ::fz_overprint *eop, ll_fz_paint_shade_outparams* outparams)
    {
        outparams->cache = NULL;
        ::fz_shade_color_cache *cache;
        ll_fz_paint_shade(shade, override_cs, ctm, dest, color_params, bbox, eop, &cache);
        outparams->cache = cache;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_parse_page_range(). */
    struct ll_fz_parse_page_range_outparams
    {
        int a;
        int b;
    };

    /* Out-params function for fz_parse_page_range(). */
    const char *ll_fz_parse_page_range_outparams_fn(const char *s, int n, ll_fz_parse_page_range_outparams* outparams)
    {
        int a;
        int b;
        const char *ret = ll_fz_parse_page_range(s, &a, &b, n);
        outparams->a = a;
        outparams->b = b;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_read_best(). */
    struct ll_fz_read_best_outparams
    {
        int truncated;
    };

    /* Out-params function for fz_read_best(). */
    ::fz_buffer *ll_fz_read_best_outparams_fn(::fz_stream *stm, size_t initial, size_t worst_case, ll_fz_read_best_outparams* outparams)
    {
        int truncated;
        ::fz_buffer *ret = ll_fz_read_best(stm, initial, &truncated, worst_case);
        outparams->truncated = truncated;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_resolve_link(). */
    struct ll_fz_resolve_link_outparams
    {
        float xp;
        float yp;
    };

    /* Out-params function for fz_resolve_link(). */
    ::fz_location ll_fz_resolve_link_outparams_fn(::fz_document *doc, const char *uri, ll_fz_resolve_link_outparams* outparams)
    {
        float xp;
        float yp;
        ::fz_location ret = ll_fz_resolve_link(doc, uri, &xp, &yp);
        outparams->xp = xp;
        outparams->yp = yp;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_search_chapter_page_number(). */
    struct ll_fz_search_chapter_page_number_outparams
    {
        int hit_mark;
    };

    /* Out-params function for fz_search_chapter_page_number(). */
    int ll_fz_search_chapter_page_number_outparams_fn(::fz_document *doc, int chapter, int page, const char *needle, ::fz_quad *hit_bbox, int hit_max, ll_fz_search_chapter_page_number_outparams* outparams)
    {
        int hit_mark;
        int ret = ll_fz_search_chapter_page_number(doc, chapter, page, needle, &hit_mark, hit_bbox, hit_max);
        outparams->hit_mark = hit_mark;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_search_display_list(). */
    struct ll_fz_search_display_list_outparams
    {
        int hit_mark;
    };

    /* Out-params function for fz_search_display_list(). */
    int ll_fz_search_display_list_outparams_fn(::fz_display_list *list, const char *needle, ::fz_quad *hit_bbox, int hit_max, ll_fz_search_display_list_outparams* outparams)
    {
        int hit_mark;
        int ret = ll_fz_search_display_list(list, needle, &hit_mark, hit_bbox, hit_max);
        outparams->hit_mark = hit_mark;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_search_page(). */
    struct ll_fz_search_page_outparams
    {
        int hit_mark;
    };

    /* Out-params function for fz_search_page(). */
    int ll_fz_search_page_outparams_fn(::fz_page *page, const char *needle, ::fz_quad *hit_bbox, int hit_max, ll_fz_search_page_outparams* outparams)
    {
        int hit_mark;
        int ret = ll_fz_search_page(page, needle, &hit_mark, hit_bbox, hit_max);
        outparams->hit_mark = hit_mark;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_search_page_number(). */
    struct ll_fz_search_page_number_outparams
    {
        int hit_mark;
    };

    /* Out-params function for fz_search_page_number(). */
    int ll_fz_search_page_number_outparams_fn(::fz_document *doc, int number, const char *needle, ::fz_quad *hit_bbox, int hit_max, ll_fz_search_page_number_outparams* outparams)
    {
        int hit_mark;
        int ret = ll_fz_search_page_number(doc, number, needle, &hit_mark, hit_bbox, hit_max);
        outparams->hit_mark = hit_mark;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_search_stext_page(). */
    struct ll_fz_search_stext_page_outparams
    {
        int hit_mark;
    };

    /* Out-params function for fz_search_stext_page(). */
    int ll_fz_search_stext_page_outparams_fn(::fz_stext_page *text, const char *needle, ::fz_quad *hit_bbox, int hit_max, ll_fz_search_stext_page_outparams* outparams)
    {
        int hit_mark;
        int ret = ll_fz_search_stext_page(text, needle, &hit_mark, hit_bbox, hit_max);
        outparams->hit_mark = hit_mark;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_separation_equivalent(). */
    struct ll_fz_separation_equivalent_outparams
    {
        float dst_color;
    };

    /* Out-params function for fz_separation_equivalent(). */
    void ll_fz_separation_equivalent_outparams_fn(const ::fz_separations *seps, int idx, ::fz_colorspace *dst_cs, ::fz_colorspace *prf, ::fz_color_params color_params, ll_fz_separation_equivalent_outparams* outparams)
    {
        float dst_color;
        ll_fz_separation_equivalent(seps, idx, dst_cs, &dst_color, prf, color_params);
        outparams->dst_color = dst_color;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_store_scavenge(). */
    struct ll_fz_store_scavenge_outparams
    {
        int phase;
    };

    /* Out-params function for fz_store_scavenge(). */
    int ll_fz_store_scavenge_outparams_fn(size_t size, ll_fz_store_scavenge_outparams* outparams)
    {
        int phase;
        int ret = ll_fz_store_scavenge(size, &phase);
        outparams->phase = phase;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_store_scavenge_external(). */
    struct ll_fz_store_scavenge_external_outparams
    {
        int phase;
    };

    /* Out-params function for fz_store_scavenge_external(). */
    int ll_fz_store_scavenge_external_outparams_fn(size_t size, ll_fz_store_scavenge_external_outparams* outparams)
    {
        int phase;
        int ret = ll_fz_store_scavenge_external(size, &phase);
        outparams->phase = phase;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_strsep(). */
    struct ll_fz_strsep_outparams
    {
        char *stringp;
    };

    /* Out-params function for fz_strsep(). */
    char *ll_fz_strsep_outparams_fn(const char *delim, ll_fz_strsep_outparams* outparams)
    {
        outparams->stringp = NULL;
        char *stringp;
        char *ret = ll_fz_strsep(&stringp, delim);
        outparams->stringp = stringp;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_strtof(). */
    struct ll_fz_strtof_outparams
    {
        char *es;
    };

    /* Out-params function for fz_strtof(). */
    float ll_fz_strtof_outparams_fn(const char *s, ll_fz_strtof_outparams* outparams)
    {
        outparams->es = NULL;
        char *es;
        float ret = ll_fz_strtof(s, &es);
        outparams->es = es;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_warning_callback(). */
    struct ll_fz_warning_callback_outparams
    {
        void *user;
    };

    /* Out-params function for fz_warning_callback(). */
    ::fz_warning_cb *ll_fz_warning_callback_outparams_fn(ll_fz_warning_callback_outparams* outparams)
    {
        outparams->user = NULL;
        void *user;
        ::fz_warning_cb *ret = ll_fz_warning_callback(&user);
        outparams->user = user;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_annot_MK_BC(). */
    struct ll_pdf_annot_MK_BC_outparams
    {
        int n;
    };

    /* Out-params function for pdf_annot_MK_BC(). */
    void ll_pdf_annot_MK_BC_outparams_fn(::pdf_annot *annot, float color[4], ll_pdf_annot_MK_BC_outparams* outparams)
    {
        int n;
        ll_pdf_annot_MK_BC(annot, &n, color);
        outparams->n = n;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_annot_MK_BG(). */
    struct ll_pdf_annot_MK_BG_outparams
    {
        int n;
    };

    /* Out-params function for pdf_annot_MK_BG(). */
    void ll_pdf_annot_MK_BG_outparams_fn(::pdf_annot *annot, float color[4], ll_pdf_annot_MK_BG_outparams* outparams)
    {
        int n;
        ll_pdf_annot_MK_BG(annot, &n, color);
        outparams->n = n;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_annot_color(). */
    struct ll_pdf_annot_color_outparams
    {
        int n;
    };

    /* Out-params function for pdf_annot_color(). */
    void ll_pdf_annot_color_outparams_fn(::pdf_annot *annot, float color[4], ll_pdf_annot_color_outparams* outparams)
    {
        int n;
        ll_pdf_annot_color(annot, &n, color);
        outparams->n = n;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_annot_default_appearance(). */
    struct ll_pdf_annot_default_appearance_outparams
    {
        const char *font;
        float size;
        int n;
    };

    /* Out-params function for pdf_annot_default_appearance(). */
    void ll_pdf_annot_default_appearance_outparams_fn(::pdf_annot *annot, float color[4], ll_pdf_annot_default_appearance_outparams* outparams)
    {
        outparams->font = NULL;
        const char *font;
        float size;
        int n;
        ll_pdf_annot_default_appearance(annot, &font, &size, &n, color);
        outparams->font = font;
        outparams->size = size;
        outparams->n = n;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_annot_interior_color(). */
    struct ll_pdf_annot_interior_color_outparams
    {
        int n;
    };

    /* Out-params function for pdf_annot_interior_color(). */
    void ll_pdf_annot_interior_color_outparams_fn(::pdf_annot *annot, float color[4], ll_pdf_annot_interior_color_outparams* outparams)
    {
        int n;
        ll_pdf_annot_interior_color(annot, &n, color);
        outparams->n = n;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_annot_line_ending_styles(). */
    struct ll_pdf_annot_line_ending_styles_outparams
    {
        enum pdf_line_ending start_style;
        enum pdf_line_ending end_style;
    };

    /* Out-params function for pdf_annot_line_ending_styles(). */
    void ll_pdf_annot_line_ending_styles_outparams_fn(::pdf_annot *annot, ll_pdf_annot_line_ending_styles_outparams* outparams)
    {
        enum pdf_line_ending start_style;
        enum pdf_line_ending end_style;
        ll_pdf_annot_line_ending_styles(annot, &start_style, &end_style);
        outparams->start_style = start_style;
        outparams->end_style = end_style;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_array_get_string(). */
    struct ll_pdf_array_get_string_outparams
    {
        size_t sizep;
    };

    /* Out-params function for pdf_array_get_string(). */
    const char *ll_pdf_array_get_string_outparams_fn(::pdf_obj *array, int index, ll_pdf_array_get_string_outparams* outparams)
    {
        size_t sizep;
        const char *ret = ll_pdf_array_get_string(array, index, &sizep);
        outparams->sizep = sizep;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_decode_cmap(). */
    struct ll_pdf_decode_cmap_outparams
    {
        unsigned int cpt;
    };

    /* Out-params function for pdf_decode_cmap(). */
    int ll_pdf_decode_cmap_outparams_fn(::pdf_cmap *cmap, unsigned char *s, unsigned char *e, ll_pdf_decode_cmap_outparams* outparams)
    {
        unsigned int cpt;
        int ret = ll_pdf_decode_cmap(cmap, s, e, &cpt);
        outparams->cpt = cpt;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_dict_get_put_drop(). */
    struct ll_pdf_dict_get_put_drop_outparams
    {
        struct ::pdf_obj *old_val;
    };

    /* Out-params function for pdf_dict_get_put_drop(). */
    void ll_pdf_dict_get_put_drop_outparams_fn(::pdf_obj *dict, ::pdf_obj *key, ::pdf_obj *val, ll_pdf_dict_get_put_drop_outparams* outparams)
    {
        outparams->old_val = NULL;
        ::pdf_obj *old_val;
        ll_pdf_dict_get_put_drop(dict, key, val, &old_val);
        outparams->old_val = old_val;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_dict_get_string(). */
    struct ll_pdf_dict_get_string_outparams
    {
        size_t sizep;
    };

    /* Out-params function for pdf_dict_get_string(). */
    const char *ll_pdf_dict_get_string_outparams_fn(::pdf_obj *dict, ::pdf_obj *key, ll_pdf_dict_get_string_outparams* outparams)
    {
        size_t sizep;
        const char *ret = ll_pdf_dict_get_string(dict, key, &sizep);
        outparams->sizep = sizep;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_edit_text_field_value(). */
    struct ll_pdf_edit_text_field_value_outparams
    {
        int selStart;
        int selEnd;
        char *newvalue;
    };

    /* Out-params function for pdf_edit_text_field_value(). */
    int ll_pdf_edit_text_field_value_outparams_fn(::pdf_annot *widget, const char *value, const char *change, ll_pdf_edit_text_field_value_outparams* outparams)
    {
        outparams->newvalue = NULL;
        int selStart;
        int selEnd;
        char *newvalue;
        int ret = ll_pdf_edit_text_field_value(widget, value, change, &selStart, &selEnd, &newvalue);
        outparams->selStart = selStart;
        outparams->selEnd = selEnd;
        outparams->newvalue = newvalue;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_eval_function(). */
    struct ll_pdf_eval_function_outparams
    {
        float out;
    };

    /* Out-params function for pdf_eval_function(). */
    void ll_pdf_eval_function_outparams_fn(::pdf_function *func, const float *in, int inlen, int outlen, ll_pdf_eval_function_outparams* outparams)
    {
        float out;
        ll_pdf_eval_function(func, in, inlen, &out, outlen);
        outparams->out = out;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_field_event_validate(). */
    struct ll_pdf_field_event_validate_outparams
    {
        char *newvalue;
    };

    /* Out-params function for pdf_field_event_validate(). */
    int ll_pdf_field_event_validate_outparams_fn(::pdf_document *doc, ::pdf_obj *field, const char *value, ll_pdf_field_event_validate_outparams* outparams)
    {
        outparams->newvalue = NULL;
        char *newvalue;
        int ret = ll_pdf_field_event_validate(doc, field, value, &newvalue);
        outparams->newvalue = newvalue;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_js_event_result_validate(). */
    struct ll_pdf_js_event_result_validate_outparams
    {
        char *newvalue;
    };

    /* Out-params function for pdf_js_event_result_validate(). */
    int ll_pdf_js_event_result_validate_outparams_fn(::pdf_js *js, ll_pdf_js_event_result_validate_outparams* outparams)
    {
        outparams->newvalue = NULL;
        char *newvalue;
        int ret = ll_pdf_js_event_result_validate(js, &newvalue);
        outparams->newvalue = newvalue;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_js_execute(). */
    struct ll_pdf_js_execute_outparams
    {
        char *result;
    };

    /* Out-params function for pdf_js_execute(). */
    void ll_pdf_js_execute_outparams_fn(::pdf_js *js, const char *name, const char *code, ll_pdf_js_execute_outparams* outparams)
    {
        outparams->result = NULL;
        char *result;
        ll_pdf_js_execute(js, name, code, &result);
        outparams->result = result;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_load_encoding(). */
    struct ll_pdf_load_encoding_outparams
    {
        const char *estrings;
    };

    /* Out-params function for pdf_load_encoding(). */
    void ll_pdf_load_encoding_outparams_fn(const char *encoding, ll_pdf_load_encoding_outparams* outparams)
    {
        outparams->estrings = NULL;
        const char *estrings;
        ll_pdf_load_encoding(&estrings, encoding);
        outparams->estrings = estrings;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_load_to_unicode(). */
    struct ll_pdf_load_to_unicode_outparams
    {
        const char *strings;
    };

    /* Out-params function for pdf_load_to_unicode(). */
    void ll_pdf_load_to_unicode_outparams_fn(::pdf_document *doc, ::pdf_font_desc *font, char *collection, ::pdf_obj *cmapstm, ll_pdf_load_to_unicode_outparams* outparams)
    {
        outparams->strings = NULL;
        const char *strings;
        ll_pdf_load_to_unicode(doc, font, &strings, collection, cmapstm);
        outparams->strings = strings;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_lookup_anchor(). */
    struct ll_pdf_lookup_anchor_outparams
    {
        float xp;
        float yp;
    };

    /* Out-params function for pdf_lookup_anchor(). */
    int ll_pdf_lookup_anchor_outparams_fn(::pdf_document *doc, const char *name, ll_pdf_lookup_anchor_outparams* outparams)
    {
        float xp;
        float yp;
        int ret = ll_pdf_lookup_anchor(doc, name, &xp, &yp);
        outparams->xp = xp;
        outparams->yp = yp;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_lookup_cmap_full(). */
    struct ll_pdf_lookup_cmap_full_outparams
    {
        int out;
    };

    /* Out-params function for pdf_lookup_cmap_full(). */
    int ll_pdf_lookup_cmap_full_outparams_fn(::pdf_cmap *cmap, unsigned int cpt, ll_pdf_lookup_cmap_full_outparams* outparams)
    {
        int out;
        int ret = ll_pdf_lookup_cmap_full(cmap, cpt, &out);
        outparams->out = out;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_lookup_substitute_font(). */
    struct ll_pdf_lookup_substitute_font_outparams
    {
        int len;
    };

    /* Out-params function for pdf_lookup_substitute_font(). */
    const unsigned char *ll_pdf_lookup_substitute_font_outparams_fn(int mono, int serif, int bold, int italic, ll_pdf_lookup_substitute_font_outparams* outparams)
    {
        int len;
        const unsigned char *ret = ll_pdf_lookup_substitute_font(mono, serif, bold, italic, &len);
        outparams->len = len;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_map_one_to_many(). */
    struct ll_pdf_map_one_to_many_outparams
    {
        int many;
    };

    /* Out-params function for pdf_map_one_to_many(). */
    void ll_pdf_map_one_to_many_outparams_fn(::pdf_cmap *cmap, unsigned int one, size_t len, ll_pdf_map_one_to_many_outparams* outparams)
    {
        int many;
        ll_pdf_map_one_to_many(cmap, one, &many, len);
        outparams->many = many;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_obj_memo(). */
    struct ll_pdf_obj_memo_outparams
    {
        int memo;
    };

    /* Out-params function for pdf_obj_memo(). */
    int ll_pdf_obj_memo_outparams_fn(::pdf_obj *obj, int bit, ll_pdf_obj_memo_outparams* outparams)
    {
        int memo;
        int ret = ll_pdf_obj_memo(obj, bit, &memo);
        outparams->memo = memo;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_page_presentation(). */
    struct ll_pdf_page_presentation_outparams
    {
        float duration;
    };

    /* Out-params function for pdf_page_presentation(). */
    ::fz_transition *ll_pdf_page_presentation_outparams_fn(::pdf_page *page, ::fz_transition *transition, ll_pdf_page_presentation_outparams* outparams)
    {
        float duration;
        ::fz_transition *ret = ll_pdf_page_presentation(page, transition, &duration);
        outparams->duration = duration;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_page_write(). */
    struct ll_pdf_page_write_outparams
    {
        struct ::pdf_obj *presources;
        ::fz_buffer *pcontents;
    };

    /* Out-params function for pdf_page_write(). */
    ::fz_device *ll_pdf_page_write_outparams_fn(::pdf_document *doc, ::fz_rect mediabox, ll_pdf_page_write_outparams* outparams)
    {
        outparams->presources = NULL;
        outparams->pcontents = NULL;
        ::pdf_obj *presources;
        ::fz_buffer *pcontents;
        ::fz_device *ret = ll_pdf_page_write(doc, mediabox, &presources, &pcontents);
        outparams->presources = presources;
        outparams->pcontents = pcontents;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_parse_default_appearance(). */
    struct ll_pdf_parse_default_appearance_outparams
    {
        const char *font;
        float size;
        int n;
    };

    /* Out-params function for pdf_parse_default_appearance(). */
    void ll_pdf_parse_default_appearance_outparams_fn(const char *da, float color[4], ll_pdf_parse_default_appearance_outparams* outparams)
    {
        outparams->font = NULL;
        const char *font;
        float size;
        int n;
        ll_pdf_parse_default_appearance(da, &font, &size, &n, color);
        outparams->font = font;
        outparams->size = size;
        outparams->n = n;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_parse_ind_obj(). */
    struct ll_pdf_parse_ind_obj_outparams
    {
        int num;
        int gen;
        long long stm_ofs;
        int try_repair;
    };

    /* Out-params function for pdf_parse_ind_obj(). */
    ::pdf_obj *ll_pdf_parse_ind_obj_outparams_fn(::pdf_document *doc, ::fz_stream *f, ll_pdf_parse_ind_obj_outparams* outparams)
    {
        int num;
        int gen;
        int64_t stm_ofs;
        int try_repair;
        ::pdf_obj *ret = ll_pdf_parse_ind_obj(doc, f, &num, &gen, &stm_ofs, &try_repair);
        outparams->num = num;
        outparams->gen = gen;
        outparams->stm_ofs = stm_ofs;
        outparams->try_repair = try_repair;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_parse_journal_obj(). */
    struct ll_pdf_parse_journal_obj_outparams
    {
        int onum;
        ::fz_buffer *ostm;
        int newobj;
    };

    /* Out-params function for pdf_parse_journal_obj(). */
    ::pdf_obj *ll_pdf_parse_journal_obj_outparams_fn(::pdf_document *doc, ::fz_stream *stm, ll_pdf_parse_journal_obj_outparams* outparams)
    {
        outparams->ostm = NULL;
        int onum;
        ::fz_buffer *ostm;
        int newobj;
        ::pdf_obj *ret = ll_pdf_parse_journal_obj(doc, stm, &onum, &ostm, &newobj);
        outparams->onum = onum;
        outparams->ostm = ostm;
        outparams->newobj = newobj;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_process_contents(). */
    struct ll_pdf_process_contents_outparams
    {
        struct ::pdf_obj *out_res;
    };

    /* Out-params function for pdf_process_contents(). */
    void ll_pdf_process_contents_outparams_fn(::pdf_processor *proc, ::pdf_document *doc, ::pdf_obj *obj, ::pdf_obj *res, ::fz_cookie *cookie, ll_pdf_process_contents_outparams* outparams)
    {
        outparams->out_res = NULL;
        ::pdf_obj *out_res;
        ll_pdf_process_contents(proc, doc, obj, res, cookie, &out_res);
        outparams->out_res = out_res;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_repair_obj(). */
    struct ll_pdf_repair_obj_outparams
    {
        long long stmofsp;
        long long stmlenp;
        struct ::pdf_obj *encrypt;
        struct ::pdf_obj *id;
        struct ::pdf_obj *page;
        long long tmpofs;
        struct ::pdf_obj *root;
    };

    /* Out-params function for pdf_repair_obj(). */
    int ll_pdf_repair_obj_outparams_fn(::pdf_document *doc, ::pdf_lexbuf *buf, ll_pdf_repair_obj_outparams* outparams)
    {
        outparams->encrypt = NULL;
        outparams->id = NULL;
        outparams->page = NULL;
        outparams->root = NULL;
        int64_t stmofsp;
        int64_t stmlenp;
        ::pdf_obj *encrypt;
        ::pdf_obj *id;
        ::pdf_obj *page;
        int64_t tmpofs;
        ::pdf_obj *root;
        int ret = ll_pdf_repair_obj(doc, buf, &stmofsp, &stmlenp, &encrypt, &id, &page, &tmpofs, &root);
        outparams->stmofsp = stmofsp;
        outparams->stmlenp = stmlenp;
        outparams->encrypt = encrypt;
        outparams->id = id;
        outparams->page = page;
        outparams->tmpofs = tmpofs;
        outparams->root = root;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_resolve_link(). */
    struct ll_pdf_resolve_link_outparams
    {
        float xp;
        float yp;
    };

    /* Out-params function for pdf_resolve_link(). */
    int ll_pdf_resolve_link_outparams_fn(::pdf_document *doc, const char *uri, ll_pdf_resolve_link_outparams* outparams)
    {
        float xp;
        float yp;
        int ret = ll_pdf_resolve_link(doc, uri, &xp, &yp);
        outparams->xp = xp;
        outparams->yp = yp;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_sample_shade_function(). */
    struct ll_pdf_sample_shade_function_outparams
    {
        struct ::pdf_function *func;
    };

    /* Out-params function for pdf_sample_shade_function(). */
    void ll_pdf_sample_shade_function_outparams_fn(float shade[256][33], int n, int funcs, float t0, float t1, ll_pdf_sample_shade_function_outparams* outparams)
    {
        outparams->func = NULL;
        ::pdf_function *func;
        ll_pdf_sample_shade_function(shade, n, funcs, &func, t0, t1);
        outparams->func = func;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_signature_contents(). */
    struct ll_pdf_signature_contents_outparams
    {
        char *contents;
    };

    /* Out-params function for pdf_signature_contents(). */
    size_t ll_pdf_signature_contents_outparams_fn(::pdf_document *doc, ::pdf_obj *signature, ll_pdf_signature_contents_outparams* outparams)
    {
        outparams->contents = NULL;
        char *contents;
        size_t ret = ll_pdf_signature_contents(doc, signature, &contents);
        outparams->contents = contents;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_sprint_obj(). */
    struct ll_pdf_sprint_obj_outparams
    {
        size_t len;
    };

    /* Out-params function for pdf_sprint_obj(). */
    char *ll_pdf_sprint_obj_outparams_fn(char *buf, size_t cap, ::pdf_obj *obj, int tight, int ascii, ll_pdf_sprint_obj_outparams* outparams)
    {
        size_t len;
        char *ret = ll_pdf_sprint_obj(buf, cap, &len, obj, tight, ascii);
        outparams->len = len;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_to_string(). */
    struct ll_pdf_to_string_outparams
    {
        size_t sizep;
    };

    /* Out-params function for pdf_to_string(). */
    const char *ll_pdf_to_string_outparams_fn(::pdf_obj *obj, ll_pdf_to_string_outparams* outparams)
    {
        size_t sizep;
        const char *ret = ll_pdf_to_string(obj, &sizep);
        outparams->sizep = sizep;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_undoredo_state(). */
    struct ll_pdf_undoredo_state_outparams
    {
        int steps;
    };

    /* Out-params function for pdf_undoredo_state(). */
    int ll_pdf_undoredo_state_outparams_fn(::pdf_document *doc, ll_pdf_undoredo_state_outparams* outparams)
    {
        int steps;
        int ret = ll_pdf_undoredo_state(doc, &steps);
        outparams->steps = steps;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_walk_tree(). */
    struct ll_pdf_walk_tree_outparams
    {
        struct ::pdf_obj *names;
        struct ::pdf_obj *values;
    };

    /* Out-params function for pdf_walk_tree(). */
    void ll_pdf_walk_tree_outparams_fn(::pdf_obj *tree, ::pdf_obj *kid_name, void (*arrive)(::fz_context *, ::pdf_obj *, void *, ::pdf_obj **), void (*leave)(::fz_context *, ::pdf_obj *, void *), void *arg, ll_pdf_walk_tree_outparams* outparams)
    {
        outparams->names = NULL;
        outparams->values = NULL;
        ::pdf_obj *names;
        ::pdf_obj *values;
        ll_pdf_walk_tree(tree, kid_name, arrive, leave, arg, &names, &values);
        outparams->names = names;
        outparams->values = values;
    }
}


enum
{
    UCDN_EAST_ASIAN_F = 0,
    UCDN_EAST_ASIAN_H = 1,
    UCDN_EAST_ASIAN_W = 2,
    UCDN_EAST_ASIAN_NA = 3,
    UCDN_EAST_ASIAN_A = 4,
    UCDN_EAST_ASIAN_N = 5,
    UCDN_SCRIPT_COMMON = 0,
    UCDN_SCRIPT_LATIN = 1,
    UCDN_SCRIPT_GREEK = 2,
    UCDN_SCRIPT_CYRILLIC = 3,
    UCDN_SCRIPT_ARMENIAN = 4,
    UCDN_SCRIPT_HEBREW = 5,
    UCDN_SCRIPT_ARABIC = 6,
    UCDN_SCRIPT_SYRIAC = 7,
    UCDN_SCRIPT_THAANA = 8,
    UCDN_SCRIPT_DEVANAGARI = 9,
    UCDN_SCRIPT_BENGALI = 10,
    UCDN_SCRIPT_GURMUKHI = 11,
    UCDN_SCRIPT_GUJARATI = 12,
    UCDN_SCRIPT_ORIYA = 13,
    UCDN_SCRIPT_TAMIL = 14,
    UCDN_SCRIPT_TELUGU = 15,
    UCDN_SCRIPT_KANNADA = 16,
    UCDN_SCRIPT_MALAYALAM = 17,
    UCDN_SCRIPT_SINHALA = 18,
    UCDN_SCRIPT_THAI = 19,
    UCDN_SCRIPT_LAO = 20,
    UCDN_SCRIPT_TIBETAN = 21,
    UCDN_SCRIPT_MYANMAR = 22,
    UCDN_SCRIPT_GEORGIAN = 23,
    UCDN_SCRIPT_HANGUL = 24,
    UCDN_SCRIPT_ETHIOPIC = 25,
    UCDN_SCRIPT_CHEROKEE = 26,
    UCDN_SCRIPT_CANADIAN_ABORIGINAL = 27,
    UCDN_SCRIPT_OGHAM = 28,
    UCDN_SCRIPT_RUNIC = 29,
    UCDN_SCRIPT_KHMER = 30,
    UCDN_SCRIPT_MONGOLIAN = 31,
    UCDN_SCRIPT_HIRAGANA = 32,
    UCDN_SCRIPT_KATAKANA = 33,
    UCDN_SCRIPT_BOPOMOFO = 34,
    UCDN_SCRIPT_HAN = 35,
    UCDN_SCRIPT_YI = 36,
    UCDN_SCRIPT_OLD_ITALIC = 37,
    UCDN_SCRIPT_GOTHIC = 38,
    UCDN_SCRIPT_DESERET = 39,
    UCDN_SCRIPT_INHERITED = 40,
    UCDN_SCRIPT_TAGALOG = 41,
    UCDN_SCRIPT_HANUNOO = 42,
    UCDN_SCRIPT_BUHID = 43,
    UCDN_SCRIPT_TAGBANWA = 44,
    UCDN_SCRIPT_LIMBU = 45,
    UCDN_SCRIPT_TAI_LE = 46,
    UCDN_SCRIPT_LINEAR_B = 47,
    UCDN_SCRIPT_UGARITIC = 48,
    UCDN_SCRIPT_SHAVIAN = 49,
    UCDN_SCRIPT_OSMANYA = 50,
    UCDN_SCRIPT_CYPRIOT = 51,
    UCDN_SCRIPT_BRAILLE = 52,
    UCDN_SCRIPT_BUGINESE = 53,
    UCDN_SCRIPT_COPTIC = 54,
    UCDN_SCRIPT_NEW_TAI_LUE = 55,
    UCDN_SCRIPT_GLAGOLITIC = 56,
    UCDN_SCRIPT_TIFINAGH = 57,
    UCDN_SCRIPT_SYLOTI_NAGRI = 58,
    UCDN_SCRIPT_OLD_PERSIAN = 59,
    UCDN_SCRIPT_KHAROSHTHI = 60,
    UCDN_SCRIPT_BALINESE = 61,
    UCDN_SCRIPT_CUNEIFORM = 62,
    UCDN_SCRIPT_PHOENICIAN = 63,
    UCDN_SCRIPT_PHAGS_PA = 64,
    UCDN_SCRIPT_NKO = 65,
    UCDN_SCRIPT_SUNDANESE = 66,
    UCDN_SCRIPT_LEPCHA = 67,
    UCDN_SCRIPT_OL_CHIKI = 68,
    UCDN_SCRIPT_VAI = 69,
    UCDN_SCRIPT_SAURASHTRA = 70,
    UCDN_SCRIPT_KAYAH_LI = 71,
    UCDN_SCRIPT_REJANG = 72,
    UCDN_SCRIPT_LYCIAN = 73,
    UCDN_SCRIPT_CARIAN = 74,
    UCDN_SCRIPT_LYDIAN = 75,
    UCDN_SCRIPT_CHAM = 76,
    UCDN_SCRIPT_TAI_THAM = 77,
    UCDN_SCRIPT_TAI_VIET = 78,
    UCDN_SCRIPT_AVESTAN = 79,
    UCDN_SCRIPT_EGYPTIAN_HIEROGLYPHS = 80,
    UCDN_SCRIPT_SAMARITAN = 81,
    UCDN_SCRIPT_LISU = 82,
    UCDN_SCRIPT_BAMUM = 83,
    UCDN_SCRIPT_JAVANESE = 84,
    UCDN_SCRIPT_MEETEI_MAYEK = 85,
    UCDN_SCRIPT_IMPERIAL_ARAMAIC = 86,
    UCDN_SCRIPT_OLD_SOUTH_ARABIAN = 87,
    UCDN_SCRIPT_INSCRIPTIONAL_PARTHIAN = 88,
    UCDN_SCRIPT_INSCRIPTIONAL_PAHLAVI = 89,
    UCDN_SCRIPT_OLD_TURKIC = 90,
    UCDN_SCRIPT_KAITHI = 91,
    UCDN_SCRIPT_BATAK = 92,
    UCDN_SCRIPT_BRAHMI = 93,
    UCDN_SCRIPT_MANDAIC = 94,
    UCDN_SCRIPT_CHAKMA = 95,
    UCDN_SCRIPT_MEROITIC_CURSIVE = 96,
    UCDN_SCRIPT_MEROITIC_HIEROGLYPHS = 97,
    UCDN_SCRIPT_MIAO = 98,
    UCDN_SCRIPT_SHARADA = 99,
    UCDN_SCRIPT_SORA_SOMPENG = 100,
    UCDN_SCRIPT_TAKRI = 101,
    UCDN_SCRIPT_UNKNOWN = 102,
    UCDN_SCRIPT_BASSA_VAH = 103,
    UCDN_SCRIPT_CAUCASIAN_ALBANIAN = 104,
    UCDN_SCRIPT_DUPLOYAN = 105,
    UCDN_SCRIPT_ELBASAN = 106,
    UCDN_SCRIPT_GRANTHA = 107,
    UCDN_SCRIPT_KHOJKI = 108,
    UCDN_SCRIPT_KHUDAWADI = 109,
    UCDN_SCRIPT_LINEAR_A = 110,
    UCDN_SCRIPT_MAHAJANI = 111,
    UCDN_SCRIPT_MANICHAEAN = 112,
    UCDN_SCRIPT_MENDE_KIKAKUI = 113,
    UCDN_SCRIPT_MODI = 114,
    UCDN_SCRIPT_MRO = 115,
    UCDN_SCRIPT_NABATAEAN = 116,
    UCDN_SCRIPT_OLD_NORTH_ARABIAN = 117,
    UCDN_SCRIPT_OLD_PERMIC = 118,
    UCDN_SCRIPT_PAHAWH_HMONG = 119,
    UCDN_SCRIPT_PALMYRENE = 120,
    UCDN_SCRIPT_PAU_CIN_HAU = 121,
    UCDN_SCRIPT_PSALTER_PAHLAVI = 122,
    UCDN_SCRIPT_SIDDHAM = 123,
    UCDN_SCRIPT_TIRHUTA = 124,
    UCDN_SCRIPT_WARANG_CITI = 125,
    UCDN_SCRIPT_AHOM = 126,
    UCDN_SCRIPT_ANATOLIAN_HIEROGLYPHS = 127,
    UCDN_SCRIPT_HATRAN = 128,
    UCDN_SCRIPT_MULTANI = 129,
    UCDN_SCRIPT_OLD_HUNGARIAN = 130,
    UCDN_SCRIPT_SIGNWRITING = 131,
    UCDN_SCRIPT_ADLAM = 132,
    UCDN_SCRIPT_BHAIKSUKI = 133,
    UCDN_SCRIPT_MARCHEN = 134,
    UCDN_SCRIPT_NEWA = 135,
    UCDN_SCRIPT_OSAGE = 136,
    UCDN_SCRIPT_TANGUT = 137,
    UCDN_SCRIPT_MASARAM_GONDI = 138,
    UCDN_SCRIPT_NUSHU = 139,
    UCDN_SCRIPT_SOYOMBO = 140,
    UCDN_SCRIPT_ZANABAZAR_SQUARE = 141,
    UCDN_SCRIPT_DOGRA = 142,
    UCDN_SCRIPT_GUNJALA_GONDI = 143,
    UCDN_SCRIPT_HANIFI_ROHINGYA = 144,
    UCDN_SCRIPT_MAKASAR = 145,
    UCDN_SCRIPT_MEDEFAIDRIN = 146,
    UCDN_SCRIPT_OLD_SOGDIAN = 147,
    UCDN_SCRIPT_SOGDIAN = 148,
    UCDN_SCRIPT_ELYMAIC = 149,
    UCDN_SCRIPT_NANDINAGARI = 150,
    UCDN_SCRIPT_NYIAKENG_PUACHUE_HMONG = 151,
    UCDN_SCRIPT_WANCHO = 152,
    UCDN_SCRIPT_CHORASMIAN = 153,
    UCDN_SCRIPT_DIVES_AKURU = 154,
    UCDN_SCRIPT_KHITAN_SMALL_SCRIPT = 155,
    UCDN_SCRIPT_YEZIDI = 156,
    UCDN_SCRIPT_VITHKUQI = 157,
    UCDN_SCRIPT_OLD_UYGHUR = 158,
    UCDN_SCRIPT_CYPRO_MINOAN = 159,
    UCDN_SCRIPT_TANGSA = 160,
    UCDN_SCRIPT_TOTO = 161,
    UCDN_SCRIPT_KAWI = 162,
    UCDN_SCRIPT_NAG_MUNDARI = 163,
    UCDN_LAST_SCRIPT = 163,
    UCDN_LINEBREAK_CLASS_OP = 0,
    UCDN_LINEBREAK_CLASS_CL = 1,
    UCDN_LINEBREAK_CLASS_CP = 2,
    UCDN_LINEBREAK_CLASS_QU = 3,
    UCDN_LINEBREAK_CLASS_GL = 4,
    UCDN_LINEBREAK_CLASS_NS = 5,
    UCDN_LINEBREAK_CLASS_EX = 6,
    UCDN_LINEBREAK_CLASS_SY = 7,
    UCDN_LINEBREAK_CLASS_IS = 8,
    UCDN_LINEBREAK_CLASS_PR = 9,
    UCDN_LINEBREAK_CLASS_PO = 10,
    UCDN_LINEBREAK_CLASS_NU = 11,
    UCDN_LINEBREAK_CLASS_AL = 12,
    UCDN_LINEBREAK_CLASS_HL = 13,
    UCDN_LINEBREAK_CLASS_ID = 14,
    UCDN_LINEBREAK_CLASS_IN = 15,
    UCDN_LINEBREAK_CLASS_HY = 16,
    UCDN_LINEBREAK_CLASS_BA = 17,
    UCDN_LINEBREAK_CLASS_BB = 18,
    UCDN_LINEBREAK_CLASS_B2 = 19,
    UCDN_LINEBREAK_CLASS_ZW = 20,
    UCDN_LINEBREAK_CLASS_CM = 21,
    UCDN_LINEBREAK_CLASS_WJ = 22,
    UCDN_LINEBREAK_CLASS_H2 = 23,
    UCDN_LINEBREAK_CLASS_H3 = 24,
    UCDN_LINEBREAK_CLASS_JL = 25,
    UCDN_LINEBREAK_CLASS_JV = 26,
    UCDN_LINEBREAK_CLASS_JT = 27,
    UCDN_LINEBREAK_CLASS_RI = 28,
    UCDN_LINEBREAK_CLASS_AI = 29,
    UCDN_LINEBREAK_CLASS_BK = 30,
    UCDN_LINEBREAK_CLASS_CB = 31,
    UCDN_LINEBREAK_CLASS_CJ = 32,
    UCDN_LINEBREAK_CLASS_CR = 33,
    UCDN_LINEBREAK_CLASS_LF = 34,
    UCDN_LINEBREAK_CLASS_NL = 35,
    UCDN_LINEBREAK_CLASS_SA = 36,
    UCDN_LINEBREAK_CLASS_SG = 37,
    UCDN_LINEBREAK_CLASS_SP = 38,
    UCDN_LINEBREAK_CLASS_XX = 39,
    UCDN_LINEBREAK_CLASS_ZWJ = 40,
    UCDN_LINEBREAK_CLASS_EB = 41,
    UCDN_LINEBREAK_CLASS_EM = 42,
    UCDN_GENERAL_CATEGORY_CC = 0,
    UCDN_GENERAL_CATEGORY_CF = 1,
    UCDN_GENERAL_CATEGORY_CN = 2,
    UCDN_GENERAL_CATEGORY_CO = 3,
    UCDN_GENERAL_CATEGORY_CS = 4,
    UCDN_GENERAL_CATEGORY_LL = 5,
    UCDN_GENERAL_CATEGORY_LM = 6,
    UCDN_GENERAL_CATEGORY_LO = 7,
    UCDN_GENERAL_CATEGORY_LT = 8,
    UCDN_GENERAL_CATEGORY_LU = 9,
    UCDN_GENERAL_CATEGORY_MC = 10,
    UCDN_GENERAL_CATEGORY_ME = 11,
    UCDN_GENERAL_CATEGORY_MN = 12,
    UCDN_GENERAL_CATEGORY_ND = 13,
    UCDN_GENERAL_CATEGORY_NL = 14,
    UCDN_GENERAL_CATEGORY_NO = 15,
    UCDN_GENERAL_CATEGORY_PC = 16,
    UCDN_GENERAL_CATEGORY_PD = 17,
    UCDN_GENERAL_CATEGORY_PE = 18,
    UCDN_GENERAL_CATEGORY_PF = 19,
    UCDN_GENERAL_CATEGORY_PI = 20,
    UCDN_GENERAL_CATEGORY_PO = 21,
    UCDN_GENERAL_CATEGORY_PS = 22,
    UCDN_GENERAL_CATEGORY_SC = 23,
    UCDN_GENERAL_CATEGORY_SK = 24,
    UCDN_GENERAL_CATEGORY_SM = 25,
    UCDN_GENERAL_CATEGORY_SO = 26,
    UCDN_GENERAL_CATEGORY_ZL = 27,
    UCDN_GENERAL_CATEGORY_ZP = 28,
    UCDN_GENERAL_CATEGORY_ZS = 29,
    UCDN_BIDI_CLASS_L = 0,
    UCDN_BIDI_CLASS_LRE = 1,
    UCDN_BIDI_CLASS_LRO = 2,
    UCDN_BIDI_CLASS_R = 3,
    UCDN_BIDI_CLASS_AL = 4,
    UCDN_BIDI_CLASS_RLE = 5,
    UCDN_BIDI_CLASS_RLO = 6,
    UCDN_BIDI_CLASS_PDF = 7,
    UCDN_BIDI_CLASS_EN = 8,
    UCDN_BIDI_CLASS_ES = 9,
    UCDN_BIDI_CLASS_ET = 10,
    UCDN_BIDI_CLASS_AN = 11,
    UCDN_BIDI_CLASS_CS = 12,
    UCDN_BIDI_CLASS_NSM = 13,
    UCDN_BIDI_CLASS_BN = 14,
    UCDN_BIDI_CLASS_B = 15,
    UCDN_BIDI_CLASS_S = 16,
    UCDN_BIDI_CLASS_WS = 17,
    UCDN_BIDI_CLASS_ON = 18,
    UCDN_BIDI_CLASS_LRI = 19,
    UCDN_BIDI_CLASS_RLI = 20,
    UCDN_BIDI_CLASS_FSI = 21,
    UCDN_BIDI_CLASS_PDI = 22,
    UCDN_BIDI_PAIRED_BRACKET_TYPE_OPEN = 0,
    UCDN_BIDI_PAIRED_BRACKET_TYPE_CLOSE = 1,
    UCDN_BIDI_PAIRED_BRACKET_TYPE_NONE = 2,
};


            %}

            %include exception.i
            %include std_string.i
            %include carrays.i
            %include cdata.i
            %include std_vector.i
            %include argcargv.i

            %array_class(unsigned char, uchar_array);

            %include <cstring.i>

            namespace std
            {
                %template(vectoruc) vector<unsigned char>;
                %template(vectori) vector<int>;
                %template(vectors) vector<std::string>;
                %template(vectorq) vector<mupdf::FzQuad>;
            };

            // Make sure that operator++() gets converted to __next__().
            //
            // Note that swig already seems to do:
            //
            //     operator* => __ref__
            //     operator== => __eq__
            //     operator!= => __ne__
            //     operator-> => __deref__
            //
            // Just need to add this method to containers that already have
            // begin() and end():
            //     def __iter__( self):
            //         return CppIterator( self)
            //

            %rename(__increment__) *::operator++;

            // Create fns that give access to arrays of some basic types, e.g. bytes_getitem().
            //
            %array_functions(unsigned char, bytes);

            // Useful for fz_stroke_state::dash_list[].
            %array_functions(float, floats);

%exception
{
    try
    {
        $action
    }
    catch( std::exception& e)
    {
        if (g_mupdf_trace_exceptions)
        {
            std::cerr
                    #ifndef _WIN32
                    << __PRETTY_FUNCTION__ << ": "
                    #endif
                    << "Converting C++ std::exception into Python exception: " << e.what()
                    << "\n";
        }
        SWIG_exception( SWIG_RuntimeError, e.what());
    }
    catch(...)
    {
        if (g_mupdf_trace_exceptions)
        {
            std::cerr
                    #ifndef _WIN32
                    << __PRETTY_FUNCTION__ << ": "
                    #endif
                    << "Converting unknown C++ exception into Python exception."
                    << "\n";
        }
        SWIG_exception( SWIG_RuntimeError, "Unknown exception");
    }
}

// Ensure SWIG handles OUTPUT params.
//
%include "cpointer.i"

// Tell swig about pdf_clean_file()'s (int,argv)-style args:
%apply (int ARGC, char **ARGV) { (int retainlen, char *retainlist[]) }

%include pybuffer.i

/* Convert Python buffer to (const unsigned char*, size_t) pair
for python_buffer_data(). */
%pybuffer_binary(
        const unsigned char* PYTHON_BUFFER_DATA,
        size_t PYTHON_BUFFER_SIZE
        );
/* Convert Python buffer to (unsigned char*, size_t) pair for
python_mutable_bytes_data(). */
%pybuffer_mutable_binary(
        unsigned char* PYTHON_BUFFER_MUTABLE_DATA,
        size_t PYTHON_BUFFER_MUTABLE_SIZE
        );

            #include <stdexcept>

            #include "mupdf/functions.h"
            #include "mupdf/classes.h"
            #include "mupdf/classes2.h"
            #include "mupdf/internal.h"
            #include "mupdf/exceptions.h"

            #ifdef NDEBUG
                static bool g_mupdf_trace_director = false;
                static bool g_mupdf_trace_exceptions = false;
            #else
                static bool g_mupdf_trace_director = mupdf::internal_env_flag("MUPDF_trace_director");
                static bool g_mupdf_trace_exceptions = mupdf::internal_env_flag("MUPDF_trace_exceptions");
            #endif

            

static std::string to_stdstring(PyObject* s)
{
    PyObject* repr_str = PyUnicode_AsEncodedString(s, "utf-8", "~E~");
    const char* repr_str_s = PyBytes_AS_STRING(repr_str);
    std::string ret = repr_str_s;
    Py_DECREF(repr_str);
    Py_DECREF(s);
    return ret;
}

static std::string py_repr(PyObject* x)
{
    if (!x) return "<C_nullptr>";
    PyObject* s = PyObject_Repr(x);
    return to_stdstring(s);
}

static std::string py_str(PyObject* x)
{
    if (!x) return "<C_nullptr>";
    PyObject* s = PyObject_Str(x);
    return to_stdstring(s);
}

/* Returns a Python `bytes` containing a copy of a `fz_buffer`'s
data. If <clear> is true we also clear and trim the buffer. */
PyObject* ll_fz_buffer_to_bytes_internal(fz_buffer* buffer, int clear)
{
    unsigned char* c = NULL;
    size_t len = mupdf::ll_fz_buffer_storage(buffer, &c);
    PyObject* ret = PyBytes_FromStringAndSize((const char*) c, (Py_ssize_t) len);
    if (clear)
    {
        /* We mimic the affects of fz_buffer_extract(), which
        leaves the buffer with zero capacity. */
        mupdf::ll_fz_clear_buffer(buffer);
        mupdf::ll_fz_trim_buffer(buffer);
    }
    return ret;
}

/* Returns a Python `memoryview` for specified memory. */
PyObject* python_memoryview_from_memory( void* data, size_t size, int writable)
{
    return PyMemoryView_FromMemory(
            (char*) data,
            (Py_ssize_t) size,
            writable ? PyBUF_WRITE : PyBUF_READ
            );
}

/* Returns a Python `memoryview` for a `fz_buffer`'s data. */
PyObject* ll_fz_buffer_storage_memoryview(fz_buffer* buffer, int writable)
{
    unsigned char* data = NULL;
    size_t len = mupdf::ll_fz_buffer_storage(buffer, &data);
    return python_memoryview_from_memory( data, len, writable);
}

/* Creates Python bytes from copy of raw data. */
PyObject* raw_to_python_bytes(const unsigned char* c, size_t len)
{
    return PyBytes_FromStringAndSize((const char*) c, (Py_ssize_t) len);
}

/* Creates Python bytes from copy of raw data. */
PyObject* raw_to_python_bytes(const void* c, size_t len)
{
    return PyBytes_FromStringAndSize((const char*) c, (Py_ssize_t) len);
}

/* The SWIG wrapper for this function returns a SWIG proxy for
a 'const unsigned char*' pointing to the raw data of a python
bytes. This proxy can then be passed from Python to functions
that take a 'const unsigned char*'.

For example to create a MuPDF fz_buffer* from a copy of a
Python bytes instance:
    bs = b'qwerty'
    buffer_ = mupdf.fz_new_buffer_from_copied_data(mupdf.python_buffer_data(bs), len(bs))
*/
const unsigned char* python_buffer_data(
        const unsigned char* PYTHON_BUFFER_DATA,
        size_t PYTHON_BUFFER_SIZE
        )
{
    return PYTHON_BUFFER_DATA;
}

unsigned char* python_mutable_buffer_data(
        unsigned char* PYTHON_BUFFER_MUTABLE_DATA,
        size_t PYTHON_BUFFER_MUTABLE_SIZE
        )
{
    return PYTHON_BUFFER_MUTABLE_DATA;
}

/* Casts an integer to a pdf_obj*. Used to convert SWIG's int
values for PDF_ENUM_NAME_* into PdfObj's. */
pdf_obj* obj_enum_to_obj(int n)
{
    return (pdf_obj*) (intptr_t) n;
}

/* SWIG-friendly alternative to ll_pdf_set_annot_color(). */
void ll_pdf_set_annot_color2(pdf_annot *annot, int n, float color0, float color1, float color2, float color3)
{
    float color[] = { color0, color1, color2, color3 };
    return mupdf::ll_pdf_set_annot_color(annot, n, color);
}


/* SWIG-friendly alternative to ll_pdf_set_annot_interior_color(). */
void ll_pdf_set_annot_interior_color2(pdf_annot *annot, int n, float color0, float color1, float color2, float color3)
{
    float color[] = { color0, color1, color2, color3 };
    return mupdf::ll_pdf_set_annot_interior_color(annot, n, color);
}

/* SWIG-friendly alternative to `fz_fill_text()`. */
void ll_fz_fill_text2(
        fz_device* dev,
        const fz_text* text,
        fz_matrix ctm,
        fz_colorspace* colorspace,
        float color0,
        float color1,
        float color2,
        float color3,
        float alpha,
        fz_color_params color_params
        )
{
    float color[] = {color0, color1, color2, color3};
    return mupdf::ll_fz_fill_text(dev, text, ctm, colorspace, color, alpha, color_params);
}

std::vector<unsigned char> fz_memrnd2(int length)
{
    std::vector<unsigned char>  ret(length);
    mupdf::fz_memrnd(&ret[0], length);
    return ret;
}


/* mupdfpy optimisation for copying pixmap. Copies first <n>
bytes of each pixel from <src> to <pm>. <pm> and <src> must
have same `.w` and `.h` */
void ll_fz_pixmap_copy( fz_pixmap* pm, const fz_pixmap* src, int n)
{
    assert( pm->w == src->w);
    assert( pm->h == src->h);
    assert( n <= pm->n);
    assert( n <= src->n);

    if (pm->n == src->n)
    {
        // identical samples
        assert( pm->stride == src->stride);
        memcpy( pm->samples, src->samples, pm->w * pm->h * pm->n);
    }
    else
    {
        for ( int y=0; y<pm->h; ++y)
        {
            for ( int x=0; x<pm->w; ++x)
            {
                memcpy(
                        pm->samples + pm->stride * y + pm->n * x,
                        src->samples + src->stride * y + src->n * x,
                        n
                        );
                if (pm->alpha)
                {
                    src->samples[ src->stride * y + src->n * x] = 255;
                }
            }
        }
    }
}

/* mupdfpy optimisation for copying raw data into pixmap. `samples` must
have enough data to fill the pixmap. */
void ll_fz_pixmap_copy_raw( fz_pixmap* pm, const void* samples)
{
    memcpy(pm->samples, samples, pm->stride * pm->h);
}

/* SWIG-friendly alternative to fz_runetochar(). */
std::vector<unsigned char> fz_runetochar2(int rune)
{
    std::vector<unsigned char>  buffer(10);
    int n = mupdf::ll_fz_runetochar((char*) &buffer[0], rune);
    assert(n < sizeof(buffer));
    buffer.resize(n);
    return buffer;
}

/* SWIG-friendly alternatives to fz_make_bookmark() and
fz_lookup_bookmark(), using long long instead of fz_bookmark
because SWIG appears to treat fz_bookmark as an int despite it
being a typedef for intptr_t, so ends up slicing. */
long long unsigned ll_fz_make_bookmark2(fz_document* doc, fz_location loc)
{
    fz_bookmark bm = mupdf::ll_fz_make_bookmark(doc, loc);
    return (long long unsigned) bm;
}

fz_location ll_fz_lookup_bookmark2(fz_document *doc, long long unsigned mark)
{
    return mupdf::ll_fz_lookup_bookmark(doc, (fz_bookmark) mark);
}
mupdf::FzLocation fz_lookup_bookmark2( mupdf::FzDocument doc, long long unsigned mark)
{
    return mupdf::FzLocation( ll_fz_lookup_bookmark2(doc.m_internal, mark));
}

struct fz_convert_color2_v
{
    float v0;
    float v1;
    float v2;
    float v3;
};

/* SWIG-friendly alternative for
ll_fz_convert_color(), taking `float* sv`. */
void ll_fz_convert_color2(
        fz_colorspace *ss,
        float* sv,
        fz_colorspace *ds,
        fz_convert_color2_v* dv,
        fz_colorspace *is,
        fz_color_params params
        )
{
    //float sv[] = { sv0, sv1, sv2, sv3};
    mupdf::ll_fz_convert_color(ss, sv, ds, &dv->v0, is, params);
}

/* SWIG-friendly alternative for
ll_fz_convert_color(), taking four explicit `float`
values for `sv`. */
void ll_fz_convert_color2(
        fz_colorspace *ss,
        float sv0,
        float sv1,
        float sv2,
        float sv3,
        fz_colorspace *ds,
        fz_convert_color2_v* dv,
        fz_colorspace *is,
        fz_color_params params
        )
{
    float sv[] = { sv0, sv1, sv2, sv3};
    mupdf::ll_fz_convert_color(ss, sv, ds, &dv->v0, is, params);
}

/* SWIG- Director class to allow fz_set_warning_callback() and
fz_set_error_callback() to be used with Python callbacks. Note that
we rename print() to _print() to match what SWIG does. */
struct DiagnosticCallback
{
    /* `description` must be "error" or "warning". */
    DiagnosticCallback(const char* description)
    :
    m_description(description)
    {
        #ifndef NDEBUG
        if (g_mupdf_trace_director)
        {
            std::cerr
                    << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
                    << " DiagnosticCallback[" << m_description << "]() constructor."
                    << "\n";
        }
        #endif
        if (m_description == "warning")
        {
            mupdf::ll_fz_set_warning_callback( s_print, this);
        }
        else if (m_description == "error")
        {
            mupdf::ll_fz_set_error_callback( s_print, this);
        }
        else
        {
            std::cerr
                    << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
                    << " DiagnosticCallback() constructor"
                    << " Unrecognised description: " << m_description
                    << "\n";
            assert(0);
        }
    }
    virtual void _print( const char* message)
    {
        #ifndef NDEBUG
        if (g_mupdf_trace_director)
        {
            std::cerr
                    << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
                    << " DiagnosticCallback[" << m_description << "]::_print()"
                    << " called (no derived class?)" << " message: " << message
                    << "\n";
        }
        #endif
    }
    virtual ~DiagnosticCallback()
    {
        #ifndef NDEBUG
        if (g_mupdf_trace_director)
        {
            std::cerr
                    << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
                    << " ~DiagnosticCallback[" << m_description << "]() destructor called"
                    << " this=" << this
                    << "\n";
        }
        #endif
    }
    static void s_print( void* self0, const char* message)
    {
        DiagnosticCallback* self = (DiagnosticCallback*) self0;
        try
        {
            return self->_print( message);
        }
        catch (std::exception& e)
        {
            /* It's important to swallow any exception from
            self->_print() because fz_set_warning_callback() and
            fz_set_error_callback() specifically require that
            the callback does not throw. But we always output a
            diagnostic. */
            std::cerr
                    << "DiagnosticCallback[" << self->m_description << "]::s_print()"
                    << " ignoring exception from _print(): "
                    << e.what()
                    << "\n";
        }
    }
    std::string m_description;
};

struct StoryPositionsCallback
{
    StoryPositionsCallback()
    {
        //printf( "StoryPositionsCallback() constructor\n");
    }

    virtual void call( const fz_story_element_position* position) = 0;

    static void s_call( fz_context* ctx, void* self0, const fz_story_element_position* position)
    {
        //printf( "StoryPositionsCallback::s_call()\n");
        (void) ctx;
        StoryPositionsCallback* self = (StoryPositionsCallback*) self0;
        self->call( position);
    }

    virtual ~StoryPositionsCallback()
    {
        //printf( "StoryPositionsCallback() destructor\n");
    }
};

void ll_fz_story_positions_director( fz_story *story, StoryPositionsCallback* cb)
{
    //printf( "ll_fz_story_positions_director()\n");
    mupdf::ll_fz_story_positions(
            story,
            StoryPositionsCallback::s_call,
            cb
            );
}

void Pixmap_set_alpha_helper(
    int balen,
    int n,
    int data_len,
    int zero_out,
    unsigned char* data,
    fz_pixmap* pix,
    int premultiply,
    int bground,
    const std::vector<int>& colors,
    const std::vector<int>& bgcolor
    )
{
    int i = 0;
    int j = 0;
    int k = 0;
    int data_fix = 255;
    while (i < balen) {
        unsigned char alpha = data[k];
        if (zero_out) {
            for (j = i; j < i+n; j++) {
                if (pix->samples[j] != (unsigned char) colors[j - i]) {
                    data_fix = 255;
                    break;
                } else {
                    data_fix = 0;
                }
            }
        }
        if (data_len) {
            if (data_fix == 0) {
                pix->samples[i+n] = 0;
            } else {
                pix->samples[i+n] = alpha;
            }
            if (premultiply && !bground) {
                for (j = i; j < i+n; j++) {
                    pix->samples[j] = fz_mul255(pix->samples[j], alpha);
                }
            } else if (bground) {
                for (j = i; j < i+n; j++) {
                    int m = (unsigned char) bgcolor[j - i];
                    pix->samples[j] = m + fz_mul255((pix->samples[j] - m), alpha);
                }
            }
        } else {
            pix->samples[i+n] = data_fix;
        }
        i += n+1;
        k += 1;
    }
}

void page_merge_helper(
        mupdf::PdfObj& old_annots,
        mupdf::PdfGraftMap& graft_map,
        mupdf::PdfDocument& doc_des,
        mupdf::PdfObj& new_annots,
        int n
        )
{
    for ( int i=0; i<n; ++i)
    {
        mupdf::PdfObj o = mupdf::pdf_array_get( old_annots, i);
        if (mupdf::pdf_dict_gets( o, "IRT").m_internal)
            continue;
        mupdf::PdfObj subtype = mupdf::pdf_dict_get( o, PDF_NAME(Subtype));
        if ( mupdf::pdf_name_eq( subtype, PDF_NAME(Link)))
            continue;
        if ( mupdf::pdf_name_eq( subtype, PDF_NAME(Popup)))
            continue;
        if ( mupdf::pdf_name_eq( subtype, PDF_NAME(Widget)))
        {
            /* fixme: C++ API doesn't yet wrap fz_warn() - it
            excludes all variadic fns. */
            //mupdf::fz_warn( "skipping widget annotation");
            continue;
        }
        mupdf::pdf_dict_del( o, PDF_NAME(Popup));
        mupdf::pdf_dict_del( o, PDF_NAME(P));
        mupdf::PdfObj copy_o = mupdf::pdf_graft_mapped_object( graft_map, o);
        mupdf::PdfObj annot = mupdf::pdf_new_indirect( doc_des, mupdf::pdf_to_num( copy_o), 0);
        mupdf::pdf_array_push( new_annots, annot);
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_bitmap_details(). */
    struct ll_fz_bitmap_details_outparams
    {
        int w;
        int h;
        int n;
        int stride;
    };

    /* Out-params function for fz_bitmap_details(). */
    void ll_fz_bitmap_details_outparams_fn(::fz_bitmap *bitmap, ll_fz_bitmap_details_outparams* outparams)
    {
        int w;
        int h;
        int n;
        int stride;
        ll_fz_bitmap_details(bitmap, &w, &h, &n, &stride);
        outparams->w = w;
        outparams->h = h;
        outparams->n = n;
        outparams->stride = stride;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_buffer_extract(). */
    struct ll_fz_buffer_extract_outparams
    {
        unsigned char *data;
    };

    /* Out-params function for fz_buffer_extract(). */
    size_t ll_fz_buffer_extract_outparams_fn(::fz_buffer *buf, ll_fz_buffer_extract_outparams* outparams)
    {
        outparams->data = NULL;
        unsigned char *data;
        size_t ret = ll_fz_buffer_extract(buf, &data);
        outparams->data = data;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_buffer_storage(). */
    struct ll_fz_buffer_storage_outparams
    {
        unsigned char *datap;
    };

    /* Out-params function for fz_buffer_storage(). */
    size_t ll_fz_buffer_storage_outparams_fn(::fz_buffer *buf, ll_fz_buffer_storage_outparams* outparams)
    {
        outparams->datap = NULL;
        unsigned char *datap;
        size_t ret = ll_fz_buffer_storage(buf, &datap);
        outparams->datap = datap;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_chartorune(). */
    struct ll_fz_chartorune_outparams
    {
        int rune;
    };

    /* Out-params function for fz_chartorune(). */
    int ll_fz_chartorune_outparams_fn(const char *str, ll_fz_chartorune_outparams* outparams)
    {
        int rune;
        int ret = ll_fz_chartorune(&rune, str);
        outparams->rune = rune;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_clamp_color(). */
    struct ll_fz_clamp_color_outparams
    {
        float out;
    };

    /* Out-params function for fz_clamp_color(). */
    void ll_fz_clamp_color_outparams_fn(::fz_colorspace *cs, const float *in, ll_fz_clamp_color_outparams* outparams)
    {
        float out;
        ll_fz_clamp_color(cs, in, &out);
        outparams->out = out;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_convert_color(). */
    struct ll_fz_convert_color_outparams
    {
        float dv;
    };

    /* Out-params function for fz_convert_color(). */
    void ll_fz_convert_color_outparams_fn(::fz_colorspace *ss, const float *sv, ::fz_colorspace *ds, ::fz_colorspace *is, ::fz_color_params params, ll_fz_convert_color_outparams* outparams)
    {
        float dv;
        ll_fz_convert_color(ss, sv, ds, &dv, is, params);
        outparams->dv = dv;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_convert_separation_colors(). */
    struct ll_fz_convert_separation_colors_outparams
    {
        float dst_color;
    };

    /* Out-params function for fz_convert_separation_colors(). */
    void ll_fz_convert_separation_colors_outparams_fn(::fz_colorspace *src_cs, const float *src_color, ::fz_separations *dst_seps, ::fz_colorspace *dst_cs, ::fz_color_params color_params, ll_fz_convert_separation_colors_outparams* outparams)
    {
        float dst_color;
        ll_fz_convert_separation_colors(src_cs, src_color, dst_seps, dst_cs, &dst_color, color_params);
        outparams->dst_color = dst_color;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_decomp_image_from_stream(). */
    struct ll_fz_decomp_image_from_stream_outparams
    {
        int l2extra;
    };

    /* Out-params function for fz_decomp_image_from_stream(). */
    ::fz_pixmap *ll_fz_decomp_image_from_stream_outparams_fn(::fz_stream *stm, ::fz_compressed_image *image, ::fz_irect *subarea, int indexed, int l2factor, ll_fz_decomp_image_from_stream_outparams* outparams)
    {
        int l2extra;
        ::fz_pixmap *ret = ll_fz_decomp_image_from_stream(stm, image, subarea, indexed, l2factor, &l2extra);
        outparams->l2extra = l2extra;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_deflate(). */
    struct ll_fz_deflate_outparams
    {
        size_t compressed_length;
    };

    /* Out-params function for fz_deflate(). */
    void ll_fz_deflate_outparams_fn(unsigned char *dest, const unsigned char *source, size_t source_length, ::fz_deflate_level level, ll_fz_deflate_outparams* outparams)
    {
        size_t compressed_length;
        ll_fz_deflate(dest, &compressed_length, source, source_length, level);
        outparams->compressed_length = compressed_length;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_dom_get_attribute(). */
    struct ll_fz_dom_get_attribute_outparams
    {
        const char *att;
    };

    /* Out-params function for fz_dom_get_attribute(). */
    const char *ll_fz_dom_get_attribute_outparams_fn(::fz_xml *elt, int i, ll_fz_dom_get_attribute_outparams* outparams)
    {
        outparams->att = NULL;
        const char *att;
        const char *ret = ll_fz_dom_get_attribute(elt, i, &att);
        outparams->att = att;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_drop_imp(). */
    struct ll_fz_drop_imp_outparams
    {
        int refs;
    };

    /* Out-params function for fz_drop_imp(). */
    int ll_fz_drop_imp_outparams_fn(void *p, ll_fz_drop_imp_outparams* outparams)
    {
        int refs;
        int ret = ll_fz_drop_imp(p, &refs);
        outparams->refs = refs;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_drop_imp16(). */
    struct ll_fz_drop_imp16_outparams
    {
        short refs;
    };

    /* Out-params function for fz_drop_imp16(). */
    int ll_fz_drop_imp16_outparams_fn(void *p, ll_fz_drop_imp16_outparams* outparams)
    {
        int16_t refs;
        int ret = ll_fz_drop_imp16(p, &refs);
        outparams->refs = refs;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_encode_character_with_fallback(). */
    struct ll_fz_encode_character_with_fallback_outparams
    {
        struct ::fz_font *out_font;
    };

    /* Out-params function for fz_encode_character_with_fallback(). */
    int ll_fz_encode_character_with_fallback_outparams_fn(::fz_font *font, int unicode, int script, int language, ll_fz_encode_character_with_fallback_outparams* outparams)
    {
        outparams->out_font = NULL;
        ::fz_font *out_font;
        int ret = ll_fz_encode_character_with_fallback(font, unicode, script, language, &out_font);
        outparams->out_font = out_font;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_error_callback(). */
    struct ll_fz_error_callback_outparams
    {
        void *user;
    };

    /* Out-params function for fz_error_callback(). */
    ::fz_error_cb *ll_fz_error_callback_outparams_fn(ll_fz_error_callback_outparams* outparams)
    {
        outparams->user = NULL;
        void *user;
        ::fz_error_cb *ret = ll_fz_error_callback(&user);
        outparams->user = user;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_fill_pixmap_with_color(). */
    struct ll_fz_fill_pixmap_with_color_outparams
    {
        float color;
    };

    /* Out-params function for fz_fill_pixmap_with_color(). */
    void ll_fz_fill_pixmap_with_color_outparams_fn(::fz_pixmap *pix, ::fz_colorspace *colorspace, ::fz_color_params color_params, ll_fz_fill_pixmap_with_color_outparams* outparams)
    {
        float color;
        ll_fz_fill_pixmap_with_color(pix, colorspace, &color, color_params);
        outparams->color = color;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_get_pixmap_from_image(). */
    struct ll_fz_get_pixmap_from_image_outparams
    {
        int w;
        int h;
    };

    /* Out-params function for fz_get_pixmap_from_image(). */
    ::fz_pixmap *ll_fz_get_pixmap_from_image_outparams_fn(::fz_image *image, const ::fz_irect *subarea, ::fz_matrix *ctm, ll_fz_get_pixmap_from_image_outparams* outparams)
    {
        int w;
        int h;
        ::fz_pixmap *ret = ll_fz_get_pixmap_from_image(image, subarea, ctm, &w, &h);
        outparams->w = w;
        outparams->h = h;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_getopt(). */
    struct ll_fz_getopt_outparams
    {
        char *nargv;
    };

    /* Out-params function for fz_getopt(). */
    int ll_fz_getopt_outparams_fn(int nargc, const char *ostr, ll_fz_getopt_outparams* outparams)
    {
        outparams->nargv = NULL;
        char *nargv;
        int ret = ll_fz_getopt(nargc, &nargv, ostr);
        outparams->nargv = nargv;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_grisu(). */
    struct ll_fz_grisu_outparams
    {
        int exp;
    };

    /* Out-params function for fz_grisu(). */
    int ll_fz_grisu_outparams_fn(float f, char *s, ll_fz_grisu_outparams* outparams)
    {
        int exp;
        int ret = ll_fz_grisu(f, s, &exp);
        outparams->exp = exp;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_has_option(). */
    struct ll_fz_has_option_outparams
    {
        const char *val;
    };

    /* Out-params function for fz_has_option(). */
    int ll_fz_has_option_outparams_fn(const char *opts, const char *key, ll_fz_has_option_outparams* outparams)
    {
        outparams->val = NULL;
        const char *val;
        int ret = ll_fz_has_option(opts, key, &val);
        outparams->val = val;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_image_resolution(). */
    struct ll_fz_image_resolution_outparams
    {
        int xres;
        int yres;
    };

    /* Out-params function for fz_image_resolution(). */
    void ll_fz_image_resolution_outparams_fn(::fz_image *image, ll_fz_image_resolution_outparams* outparams)
    {
        int xres;
        int yres;
        ll_fz_image_resolution(image, &xres, &yres);
        outparams->xres = xres;
        outparams->yres = yres;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_keep_imp(). */
    struct ll_fz_keep_imp_outparams
    {
        int refs;
    };

    /* Out-params function for fz_keep_imp(). */
    void *ll_fz_keep_imp_outparams_fn(void *p, ll_fz_keep_imp_outparams* outparams)
    {
        int refs;
        void *ret = ll_fz_keep_imp(p, &refs);
        outparams->refs = refs;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_keep_imp16(). */
    struct ll_fz_keep_imp16_outparams
    {
        short refs;
    };

    /* Out-params function for fz_keep_imp16(). */
    void *ll_fz_keep_imp16_outparams_fn(void *p, ll_fz_keep_imp16_outparams* outparams)
    {
        int16_t refs;
        void *ret = ll_fz_keep_imp16(p, &refs);
        outparams->refs = refs;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_keep_imp_locked(). */
    struct ll_fz_keep_imp_locked_outparams
    {
        int refs;
    };

    /* Out-params function for fz_keep_imp_locked(). */
    void *ll_fz_keep_imp_locked_outparams_fn(void *p, ll_fz_keep_imp_locked_outparams* outparams)
    {
        int refs;
        void *ret = ll_fz_keep_imp_locked(p, &refs);
        outparams->refs = refs;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_lookup_base14_font(). */
    struct ll_fz_lookup_base14_font_outparams
    {
        int len;
    };

    /* Out-params function for fz_lookup_base14_font(). */
    const unsigned char *ll_fz_lookup_base14_font_outparams_fn(const char *name, ll_fz_lookup_base14_font_outparams* outparams)
    {
        int len;
        const unsigned char *ret = ll_fz_lookup_base14_font(name, &len);
        outparams->len = len;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_lookup_builtin_font(). */
    struct ll_fz_lookup_builtin_font_outparams
    {
        int len;
    };

    /* Out-params function for fz_lookup_builtin_font(). */
    const unsigned char *ll_fz_lookup_builtin_font_outparams_fn(const char *name, int bold, int italic, ll_fz_lookup_builtin_font_outparams* outparams)
    {
        int len;
        const unsigned char *ret = ll_fz_lookup_builtin_font(name, bold, italic, &len);
        outparams->len = len;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_lookup_cjk_font(). */
    struct ll_fz_lookup_cjk_font_outparams
    {
        int len;
        int index;
    };

    /* Out-params function for fz_lookup_cjk_font(). */
    const unsigned char *ll_fz_lookup_cjk_font_outparams_fn(int ordering, ll_fz_lookup_cjk_font_outparams* outparams)
    {
        int len;
        int index;
        const unsigned char *ret = ll_fz_lookup_cjk_font(ordering, &len, &index);
        outparams->len = len;
        outparams->index = index;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_lookup_cjk_font_by_language(). */
    struct ll_fz_lookup_cjk_font_by_language_outparams
    {
        int len;
        int subfont;
    };

    /* Out-params function for fz_lookup_cjk_font_by_language(). */
    const unsigned char *ll_fz_lookup_cjk_font_by_language_outparams_fn(const char *lang, ll_fz_lookup_cjk_font_by_language_outparams* outparams)
    {
        int len;
        int subfont;
        const unsigned char *ret = ll_fz_lookup_cjk_font_by_language(lang, &len, &subfont);
        outparams->len = len;
        outparams->subfont = subfont;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_lookup_noto_emoji_font(). */
    struct ll_fz_lookup_noto_emoji_font_outparams
    {
        int len;
    };

    /* Out-params function for fz_lookup_noto_emoji_font(). */
    const unsigned char *ll_fz_lookup_noto_emoji_font_outparams_fn(ll_fz_lookup_noto_emoji_font_outparams* outparams)
    {
        int len;
        const unsigned char *ret = ll_fz_lookup_noto_emoji_font(&len);
        outparams->len = len;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_lookup_noto_font(). */
    struct ll_fz_lookup_noto_font_outparams
    {
        int len;
        int subfont;
    };

    /* Out-params function for fz_lookup_noto_font(). */
    const unsigned char *ll_fz_lookup_noto_font_outparams_fn(int script, int lang, ll_fz_lookup_noto_font_outparams* outparams)
    {
        int len;
        int subfont;
        const unsigned char *ret = ll_fz_lookup_noto_font(script, lang, &len, &subfont);
        outparams->len = len;
        outparams->subfont = subfont;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_lookup_noto_math_font(). */
    struct ll_fz_lookup_noto_math_font_outparams
    {
        int len;
    };

    /* Out-params function for fz_lookup_noto_math_font(). */
    const unsigned char *ll_fz_lookup_noto_math_font_outparams_fn(ll_fz_lookup_noto_math_font_outparams* outparams)
    {
        int len;
        const unsigned char *ret = ll_fz_lookup_noto_math_font(&len);
        outparams->len = len;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_lookup_noto_music_font(). */
    struct ll_fz_lookup_noto_music_font_outparams
    {
        int len;
    };

    /* Out-params function for fz_lookup_noto_music_font(). */
    const unsigned char *ll_fz_lookup_noto_music_font_outparams_fn(ll_fz_lookup_noto_music_font_outparams* outparams)
    {
        int len;
        const unsigned char *ret = ll_fz_lookup_noto_music_font(&len);
        outparams->len = len;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_lookup_noto_symbol1_font(). */
    struct ll_fz_lookup_noto_symbol1_font_outparams
    {
        int len;
    };

    /* Out-params function for fz_lookup_noto_symbol1_font(). */
    const unsigned char *ll_fz_lookup_noto_symbol1_font_outparams_fn(ll_fz_lookup_noto_symbol1_font_outparams* outparams)
    {
        int len;
        const unsigned char *ret = ll_fz_lookup_noto_symbol1_font(&len);
        outparams->len = len;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_lookup_noto_symbol2_font(). */
    struct ll_fz_lookup_noto_symbol2_font_outparams
    {
        int len;
    };

    /* Out-params function for fz_lookup_noto_symbol2_font(). */
    const unsigned char *ll_fz_lookup_noto_symbol2_font_outparams_fn(ll_fz_lookup_noto_symbol2_font_outparams* outparams)
    {
        int len;
        const unsigned char *ret = ll_fz_lookup_noto_symbol2_font(&len);
        outparams->len = len;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_new_deflated_data(). */
    struct ll_fz_new_deflated_data_outparams
    {
        size_t compressed_length;
    };

    /* Out-params function for fz_new_deflated_data(). */
    unsigned char *ll_fz_new_deflated_data_outparams_fn(const unsigned char *source, size_t source_length, ::fz_deflate_level level, ll_fz_new_deflated_data_outparams* outparams)
    {
        size_t compressed_length;
        unsigned char *ret = ll_fz_new_deflated_data(&compressed_length, source, source_length, level);
        outparams->compressed_length = compressed_length;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_new_deflated_data_from_buffer(). */
    struct ll_fz_new_deflated_data_from_buffer_outparams
    {
        size_t compressed_length;
    };

    /* Out-params function for fz_new_deflated_data_from_buffer(). */
    unsigned char *ll_fz_new_deflated_data_from_buffer_outparams_fn(::fz_buffer *buffer, ::fz_deflate_level level, ll_fz_new_deflated_data_from_buffer_outparams* outparams)
    {
        size_t compressed_length;
        unsigned char *ret = ll_fz_new_deflated_data_from_buffer(&compressed_length, buffer, level);
        outparams->compressed_length = compressed_length;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_new_display_list_from_svg(). */
    struct ll_fz_new_display_list_from_svg_outparams
    {
        float w;
        float h;
    };

    /* Out-params function for fz_new_display_list_from_svg(). */
    ::fz_display_list *ll_fz_new_display_list_from_svg_outparams_fn(::fz_buffer *buf, const char *base_uri, ::fz_archive *zip, ll_fz_new_display_list_from_svg_outparams* outparams)
    {
        float w;
        float h;
        ::fz_display_list *ret = ll_fz_new_display_list_from_svg(buf, base_uri, zip, &w, &h);
        outparams->w = w;
        outparams->h = h;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_new_display_list_from_svg_xml(). */
    struct ll_fz_new_display_list_from_svg_xml_outparams
    {
        float w;
        float h;
    };

    /* Out-params function for fz_new_display_list_from_svg_xml(). */
    ::fz_display_list *ll_fz_new_display_list_from_svg_xml_outparams_fn(::fz_xml_doc *xmldoc, ::fz_xml *xml, const char *base_uri, ::fz_archive *zip, ll_fz_new_display_list_from_svg_xml_outparams* outparams)
    {
        float w;
        float h;
        ::fz_display_list *ret = ll_fz_new_display_list_from_svg_xml(xmldoc, xml, base_uri, zip, &w, &h);
        outparams->w = w;
        outparams->h = h;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_new_draw_device_with_options(). */
    struct ll_fz_new_draw_device_with_options_outparams
    {
        struct ::fz_pixmap *pixmap;
    };

    /* Out-params function for fz_new_draw_device_with_options(). */
    ::fz_device *ll_fz_new_draw_device_with_options_outparams_fn(const ::fz_draw_options *options, ::fz_rect mediabox, ll_fz_new_draw_device_with_options_outparams* outparams)
    {
        outparams->pixmap = NULL;
        ::fz_pixmap *pixmap;
        ::fz_device *ret = ll_fz_new_draw_device_with_options(options, mediabox, &pixmap);
        outparams->pixmap = pixmap;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_new_image_from_compressed_buffer(). */
    struct ll_fz_new_image_from_compressed_buffer_outparams
    {
        float decode;
        int colorkey;
    };

    /* Out-params function for fz_new_image_from_compressed_buffer(). */
    ::fz_image *ll_fz_new_image_from_compressed_buffer_outparams_fn(int w, int h, int bpc, ::fz_colorspace *colorspace, int xres, int yres, int interpolate, int imagemask, ::fz_compressed_buffer *buffer, ::fz_image *mask, ll_fz_new_image_from_compressed_buffer_outparams* outparams)
    {
        float decode;
        int colorkey;
        ::fz_image *ret = ll_fz_new_image_from_compressed_buffer(w, h, bpc, colorspace, xres, yres, interpolate, imagemask, &decode, &colorkey, buffer, mask);
        outparams->decode = decode;
        outparams->colorkey = colorkey;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_new_image_of_size(). */
    struct ll_fz_new_image_of_size_outparams
    {
        float decode;
        int colorkey;
    };

    /* Out-params function for fz_new_image_of_size(). */
    ::fz_image *ll_fz_new_image_of_size_outparams_fn(int w, int h, int bpc, ::fz_colorspace *colorspace, int xres, int yres, int interpolate, int imagemask, ::fz_image *mask, size_t size, ::fz_image_get_pixmap_fn *get_pixmap, ::fz_image_get_size_fn *get_size, ::fz_drop_image_fn *drop, ll_fz_new_image_of_size_outparams* outparams)
    {
        float decode;
        int colorkey;
        ::fz_image *ret = ll_fz_new_image_of_size(w, h, bpc, colorspace, xres, yres, interpolate, imagemask, &decode, &colorkey, mask, size, get_pixmap, get_size, drop);
        outparams->decode = decode;
        outparams->colorkey = colorkey;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_new_svg_device_with_id(). */
    struct ll_fz_new_svg_device_with_id_outparams
    {
        int id;
    };

    /* Out-params function for fz_new_svg_device_with_id(). */
    ::fz_device *ll_fz_new_svg_device_with_id_outparams_fn(::fz_output *out, float page_width, float page_height, int text_format, int reuse_images, ll_fz_new_svg_device_with_id_outparams* outparams)
    {
        int id;
        ::fz_device *ret = ll_fz_new_svg_device_with_id(out, page_width, page_height, text_format, reuse_images, &id);
        outparams->id = id;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_new_test_device(). */
    struct ll_fz_new_test_device_outparams
    {
        int is_color;
    };

    /* Out-params function for fz_new_test_device(). */
    ::fz_device *ll_fz_new_test_device_outparams_fn(float threshold, int options, ::fz_device *passthrough, ll_fz_new_test_device_outparams* outparams)
    {
        int is_color;
        ::fz_device *ret = ll_fz_new_test_device(&is_color, threshold, options, passthrough);
        outparams->is_color = is_color;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_open_image_decomp_stream(). */
    struct ll_fz_open_image_decomp_stream_outparams
    {
        int l2factor;
    };

    /* Out-params function for fz_open_image_decomp_stream(). */
    ::fz_stream *ll_fz_open_image_decomp_stream_outparams_fn(::fz_stream *arg_0, ::fz_compression_params *arg_1, ll_fz_open_image_decomp_stream_outparams* outparams)
    {
        int l2factor;
        ::fz_stream *ret = ll_fz_open_image_decomp_stream(arg_0, arg_1, &l2factor);
        outparams->l2factor = l2factor;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_open_image_decomp_stream_from_buffer(). */
    struct ll_fz_open_image_decomp_stream_from_buffer_outparams
    {
        int l2factor;
    };

    /* Out-params function for fz_open_image_decomp_stream_from_buffer(). */
    ::fz_stream *ll_fz_open_image_decomp_stream_from_buffer_outparams_fn(::fz_compressed_buffer *arg_0, ll_fz_open_image_decomp_stream_from_buffer_outparams* outparams)
    {
        int l2factor;
        ::fz_stream *ret = ll_fz_open_image_decomp_stream_from_buffer(arg_0, &l2factor);
        outparams->l2factor = l2factor;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_page_presentation(). */
    struct ll_fz_page_presentation_outparams
    {
        float duration;
    };

    /* Out-params function for fz_page_presentation(). */
    ::fz_transition *ll_fz_page_presentation_outparams_fn(::fz_page *page, ::fz_transition *transition, ll_fz_page_presentation_outparams* outparams)
    {
        float duration;
        ::fz_transition *ret = ll_fz_page_presentation(page, transition, &duration);
        outparams->duration = duration;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_paint_shade(). */
    struct ll_fz_paint_shade_outparams
    {
        struct ::fz_shade_color_cache *cache;
    };

    /* Out-params function for fz_paint_shade(). */
    void ll_fz_paint_shade_outparams_fn(::fz_shade *shade, ::fz_colorspace *override_cs, ::fz_matrix ctm, ::fz_pixmap *dest, ::fz_color_params color_params, ::fz_irect bbox, const ::fz_overprint *eop, ll_fz_paint_shade_outparams* outparams)
    {
        outparams->cache = NULL;
        ::fz_shade_color_cache *cache;
        ll_fz_paint_shade(shade, override_cs, ctm, dest, color_params, bbox, eop, &cache);
        outparams->cache = cache;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_parse_page_range(). */
    struct ll_fz_parse_page_range_outparams
    {
        int a;
        int b;
    };

    /* Out-params function for fz_parse_page_range(). */
    const char *ll_fz_parse_page_range_outparams_fn(const char *s, int n, ll_fz_parse_page_range_outparams* outparams)
    {
        int a;
        int b;
        const char *ret = ll_fz_parse_page_range(s, &a, &b, n);
        outparams->a = a;
        outparams->b = b;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_read_best(). */
    struct ll_fz_read_best_outparams
    {
        int truncated;
    };

    /* Out-params function for fz_read_best(). */
    ::fz_buffer *ll_fz_read_best_outparams_fn(::fz_stream *stm, size_t initial, size_t worst_case, ll_fz_read_best_outparams* outparams)
    {
        int truncated;
        ::fz_buffer *ret = ll_fz_read_best(stm, initial, &truncated, worst_case);
        outparams->truncated = truncated;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_resolve_link(). */
    struct ll_fz_resolve_link_outparams
    {
        float xp;
        float yp;
    };

    /* Out-params function for fz_resolve_link(). */
    ::fz_location ll_fz_resolve_link_outparams_fn(::fz_document *doc, const char *uri, ll_fz_resolve_link_outparams* outparams)
    {
        float xp;
        float yp;
        ::fz_location ret = ll_fz_resolve_link(doc, uri, &xp, &yp);
        outparams->xp = xp;
        outparams->yp = yp;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_search_chapter_page_number(). */
    struct ll_fz_search_chapter_page_number_outparams
    {
        int hit_mark;
    };

    /* Out-params function for fz_search_chapter_page_number(). */
    int ll_fz_search_chapter_page_number_outparams_fn(::fz_document *doc, int chapter, int page, const char *needle, ::fz_quad *hit_bbox, int hit_max, ll_fz_search_chapter_page_number_outparams* outparams)
    {
        int hit_mark;
        int ret = ll_fz_search_chapter_page_number(doc, chapter, page, needle, &hit_mark, hit_bbox, hit_max);
        outparams->hit_mark = hit_mark;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_search_display_list(). */
    struct ll_fz_search_display_list_outparams
    {
        int hit_mark;
    };

    /* Out-params function for fz_search_display_list(). */
    int ll_fz_search_display_list_outparams_fn(::fz_display_list *list, const char *needle, ::fz_quad *hit_bbox, int hit_max, ll_fz_search_display_list_outparams* outparams)
    {
        int hit_mark;
        int ret = ll_fz_search_display_list(list, needle, &hit_mark, hit_bbox, hit_max);
        outparams->hit_mark = hit_mark;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_search_page(). */
    struct ll_fz_search_page_outparams
    {
        int hit_mark;
    };

    /* Out-params function for fz_search_page(). */
    int ll_fz_search_page_outparams_fn(::fz_page *page, const char *needle, ::fz_quad *hit_bbox, int hit_max, ll_fz_search_page_outparams* outparams)
    {
        int hit_mark;
        int ret = ll_fz_search_page(page, needle, &hit_mark, hit_bbox, hit_max);
        outparams->hit_mark = hit_mark;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_search_page_number(). */
    struct ll_fz_search_page_number_outparams
    {
        int hit_mark;
    };

    /* Out-params function for fz_search_page_number(). */
    int ll_fz_search_page_number_outparams_fn(::fz_document *doc, int number, const char *needle, ::fz_quad *hit_bbox, int hit_max, ll_fz_search_page_number_outparams* outparams)
    {
        int hit_mark;
        int ret = ll_fz_search_page_number(doc, number, needle, &hit_mark, hit_bbox, hit_max);
        outparams->hit_mark = hit_mark;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_search_stext_page(). */
    struct ll_fz_search_stext_page_outparams
    {
        int hit_mark;
    };

    /* Out-params function for fz_search_stext_page(). */
    int ll_fz_search_stext_page_outparams_fn(::fz_stext_page *text, const char *needle, ::fz_quad *hit_bbox, int hit_max, ll_fz_search_stext_page_outparams* outparams)
    {
        int hit_mark;
        int ret = ll_fz_search_stext_page(text, needle, &hit_mark, hit_bbox, hit_max);
        outparams->hit_mark = hit_mark;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_separation_equivalent(). */
    struct ll_fz_separation_equivalent_outparams
    {
        float dst_color;
    };

    /* Out-params function for fz_separation_equivalent(). */
    void ll_fz_separation_equivalent_outparams_fn(const ::fz_separations *seps, int idx, ::fz_colorspace *dst_cs, ::fz_colorspace *prf, ::fz_color_params color_params, ll_fz_separation_equivalent_outparams* outparams)
    {
        float dst_color;
        ll_fz_separation_equivalent(seps, idx, dst_cs, &dst_color, prf, color_params);
        outparams->dst_color = dst_color;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_store_scavenge(). */
    struct ll_fz_store_scavenge_outparams
    {
        int phase;
    };

    /* Out-params function for fz_store_scavenge(). */
    int ll_fz_store_scavenge_outparams_fn(size_t size, ll_fz_store_scavenge_outparams* outparams)
    {
        int phase;
        int ret = ll_fz_store_scavenge(size, &phase);
        outparams->phase = phase;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_store_scavenge_external(). */
    struct ll_fz_store_scavenge_external_outparams
    {
        int phase;
    };

    /* Out-params function for fz_store_scavenge_external(). */
    int ll_fz_store_scavenge_external_outparams_fn(size_t size, ll_fz_store_scavenge_external_outparams* outparams)
    {
        int phase;
        int ret = ll_fz_store_scavenge_external(size, &phase);
        outparams->phase = phase;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_strsep(). */
    struct ll_fz_strsep_outparams
    {
        char *stringp;
    };

    /* Out-params function for fz_strsep(). */
    char *ll_fz_strsep_outparams_fn(const char *delim, ll_fz_strsep_outparams* outparams)
    {
        outparams->stringp = NULL;
        char *stringp;
        char *ret = ll_fz_strsep(&stringp, delim);
        outparams->stringp = stringp;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_strtof(). */
    struct ll_fz_strtof_outparams
    {
        char *es;
    };

    /* Out-params function for fz_strtof(). */
    float ll_fz_strtof_outparams_fn(const char *s, ll_fz_strtof_outparams* outparams)
    {
        outparams->es = NULL;
        char *es;
        float ret = ll_fz_strtof(s, &es);
        outparams->es = es;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for fz_warning_callback(). */
    struct ll_fz_warning_callback_outparams
    {
        void *user;
    };

    /* Out-params function for fz_warning_callback(). */
    ::fz_warning_cb *ll_fz_warning_callback_outparams_fn(ll_fz_warning_callback_outparams* outparams)
    {
        outparams->user = NULL;
        void *user;
        ::fz_warning_cb *ret = ll_fz_warning_callback(&user);
        outparams->user = user;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_annot_MK_BC(). */
    struct ll_pdf_annot_MK_BC_outparams
    {
        int n;
    };

    /* Out-params function for pdf_annot_MK_BC(). */
    void ll_pdf_annot_MK_BC_outparams_fn(::pdf_annot *annot, float color[4], ll_pdf_annot_MK_BC_outparams* outparams)
    {
        int n;
        ll_pdf_annot_MK_BC(annot, &n, color);
        outparams->n = n;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_annot_MK_BG(). */
    struct ll_pdf_annot_MK_BG_outparams
    {
        int n;
    };

    /* Out-params function for pdf_annot_MK_BG(). */
    void ll_pdf_annot_MK_BG_outparams_fn(::pdf_annot *annot, float color[4], ll_pdf_annot_MK_BG_outparams* outparams)
    {
        int n;
        ll_pdf_annot_MK_BG(annot, &n, color);
        outparams->n = n;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_annot_color(). */
    struct ll_pdf_annot_color_outparams
    {
        int n;
    };

    /* Out-params function for pdf_annot_color(). */
    void ll_pdf_annot_color_outparams_fn(::pdf_annot *annot, float color[4], ll_pdf_annot_color_outparams* outparams)
    {
        int n;
        ll_pdf_annot_color(annot, &n, color);
        outparams->n = n;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_annot_default_appearance(). */
    struct ll_pdf_annot_default_appearance_outparams
    {
        const char *font;
        float size;
        int n;
    };

    /* Out-params function for pdf_annot_default_appearance(). */
    void ll_pdf_annot_default_appearance_outparams_fn(::pdf_annot *annot, float color[4], ll_pdf_annot_default_appearance_outparams* outparams)
    {
        outparams->font = NULL;
        const char *font;
        float size;
        int n;
        ll_pdf_annot_default_appearance(annot, &font, &size, &n, color);
        outparams->font = font;
        outparams->size = size;
        outparams->n = n;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_annot_interior_color(). */
    struct ll_pdf_annot_interior_color_outparams
    {
        int n;
    };

    /* Out-params function for pdf_annot_interior_color(). */
    void ll_pdf_annot_interior_color_outparams_fn(::pdf_annot *annot, float color[4], ll_pdf_annot_interior_color_outparams* outparams)
    {
        int n;
        ll_pdf_annot_interior_color(annot, &n, color);
        outparams->n = n;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_annot_line_ending_styles(). */
    struct ll_pdf_annot_line_ending_styles_outparams
    {
        enum pdf_line_ending start_style;
        enum pdf_line_ending end_style;
    };

    /* Out-params function for pdf_annot_line_ending_styles(). */
    void ll_pdf_annot_line_ending_styles_outparams_fn(::pdf_annot *annot, ll_pdf_annot_line_ending_styles_outparams* outparams)
    {
        enum pdf_line_ending start_style;
        enum pdf_line_ending end_style;
        ll_pdf_annot_line_ending_styles(annot, &start_style, &end_style);
        outparams->start_style = start_style;
        outparams->end_style = end_style;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_array_get_string(). */
    struct ll_pdf_array_get_string_outparams
    {
        size_t sizep;
    };

    /* Out-params function for pdf_array_get_string(). */
    const char *ll_pdf_array_get_string_outparams_fn(::pdf_obj *array, int index, ll_pdf_array_get_string_outparams* outparams)
    {
        size_t sizep;
        const char *ret = ll_pdf_array_get_string(array, index, &sizep);
        outparams->sizep = sizep;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_decode_cmap(). */
    struct ll_pdf_decode_cmap_outparams
    {
        unsigned int cpt;
    };

    /* Out-params function for pdf_decode_cmap(). */
    int ll_pdf_decode_cmap_outparams_fn(::pdf_cmap *cmap, unsigned char *s, unsigned char *e, ll_pdf_decode_cmap_outparams* outparams)
    {
        unsigned int cpt;
        int ret = ll_pdf_decode_cmap(cmap, s, e, &cpt);
        outparams->cpt = cpt;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_dict_get_put_drop(). */
    struct ll_pdf_dict_get_put_drop_outparams
    {
        struct ::pdf_obj *old_val;
    };

    /* Out-params function for pdf_dict_get_put_drop(). */
    void ll_pdf_dict_get_put_drop_outparams_fn(::pdf_obj *dict, ::pdf_obj *key, ::pdf_obj *val, ll_pdf_dict_get_put_drop_outparams* outparams)
    {
        outparams->old_val = NULL;
        ::pdf_obj *old_val;
        ll_pdf_dict_get_put_drop(dict, key, val, &old_val);
        outparams->old_val = old_val;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_dict_get_string(). */
    struct ll_pdf_dict_get_string_outparams
    {
        size_t sizep;
    };

    /* Out-params function for pdf_dict_get_string(). */
    const char *ll_pdf_dict_get_string_outparams_fn(::pdf_obj *dict, ::pdf_obj *key, ll_pdf_dict_get_string_outparams* outparams)
    {
        size_t sizep;
        const char *ret = ll_pdf_dict_get_string(dict, key, &sizep);
        outparams->sizep = sizep;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_edit_text_field_value(). */
    struct ll_pdf_edit_text_field_value_outparams
    {
        int selStart;
        int selEnd;
        char *newvalue;
    };

    /* Out-params function for pdf_edit_text_field_value(). */
    int ll_pdf_edit_text_field_value_outparams_fn(::pdf_annot *widget, const char *value, const char *change, ll_pdf_edit_text_field_value_outparams* outparams)
    {
        outparams->newvalue = NULL;
        int selStart;
        int selEnd;
        char *newvalue;
        int ret = ll_pdf_edit_text_field_value(widget, value, change, &selStart, &selEnd, &newvalue);
        outparams->selStart = selStart;
        outparams->selEnd = selEnd;
        outparams->newvalue = newvalue;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_eval_function(). */
    struct ll_pdf_eval_function_outparams
    {
        float out;
    };

    /* Out-params function for pdf_eval_function(). */
    void ll_pdf_eval_function_outparams_fn(::pdf_function *func, const float *in, int inlen, int outlen, ll_pdf_eval_function_outparams* outparams)
    {
        float out;
        ll_pdf_eval_function(func, in, inlen, &out, outlen);
        outparams->out = out;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_field_event_validate(). */
    struct ll_pdf_field_event_validate_outparams
    {
        char *newvalue;
    };

    /* Out-params function for pdf_field_event_validate(). */
    int ll_pdf_field_event_validate_outparams_fn(::pdf_document *doc, ::pdf_obj *field, const char *value, ll_pdf_field_event_validate_outparams* outparams)
    {
        outparams->newvalue = NULL;
        char *newvalue;
        int ret = ll_pdf_field_event_validate(doc, field, value, &newvalue);
        outparams->newvalue = newvalue;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_js_event_result_validate(). */
    struct ll_pdf_js_event_result_validate_outparams
    {
        char *newvalue;
    };

    /* Out-params function for pdf_js_event_result_validate(). */
    int ll_pdf_js_event_result_validate_outparams_fn(::pdf_js *js, ll_pdf_js_event_result_validate_outparams* outparams)
    {
        outparams->newvalue = NULL;
        char *newvalue;
        int ret = ll_pdf_js_event_result_validate(js, &newvalue);
        outparams->newvalue = newvalue;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_js_execute(). */
    struct ll_pdf_js_execute_outparams
    {
        char *result;
    };

    /* Out-params function for pdf_js_execute(). */
    void ll_pdf_js_execute_outparams_fn(::pdf_js *js, const char *name, const char *code, ll_pdf_js_execute_outparams* outparams)
    {
        outparams->result = NULL;
        char *result;
        ll_pdf_js_execute(js, name, code, &result);
        outparams->result = result;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_load_encoding(). */
    struct ll_pdf_load_encoding_outparams
    {
        const char *estrings;
    };

    /* Out-params function for pdf_load_encoding(). */
    void ll_pdf_load_encoding_outparams_fn(const char *encoding, ll_pdf_load_encoding_outparams* outparams)
    {
        outparams->estrings = NULL;
        const char *estrings;
        ll_pdf_load_encoding(&estrings, encoding);
        outparams->estrings = estrings;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_load_to_unicode(). */
    struct ll_pdf_load_to_unicode_outparams
    {
        const char *strings;
    };

    /* Out-params function for pdf_load_to_unicode(). */
    void ll_pdf_load_to_unicode_outparams_fn(::pdf_document *doc, ::pdf_font_desc *font, char *collection, ::pdf_obj *cmapstm, ll_pdf_load_to_unicode_outparams* outparams)
    {
        outparams->strings = NULL;
        const char *strings;
        ll_pdf_load_to_unicode(doc, font, &strings, collection, cmapstm);
        outparams->strings = strings;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_lookup_anchor(). */
    struct ll_pdf_lookup_anchor_outparams
    {
        float xp;
        float yp;
    };

    /* Out-params function for pdf_lookup_anchor(). */
    int ll_pdf_lookup_anchor_outparams_fn(::pdf_document *doc, const char *name, ll_pdf_lookup_anchor_outparams* outparams)
    {
        float xp;
        float yp;
        int ret = ll_pdf_lookup_anchor(doc, name, &xp, &yp);
        outparams->xp = xp;
        outparams->yp = yp;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_lookup_cmap_full(). */
    struct ll_pdf_lookup_cmap_full_outparams
    {
        int out;
    };

    /* Out-params function for pdf_lookup_cmap_full(). */
    int ll_pdf_lookup_cmap_full_outparams_fn(::pdf_cmap *cmap, unsigned int cpt, ll_pdf_lookup_cmap_full_outparams* outparams)
    {
        int out;
        int ret = ll_pdf_lookup_cmap_full(cmap, cpt, &out);
        outparams->out = out;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_lookup_substitute_font(). */
    struct ll_pdf_lookup_substitute_font_outparams
    {
        int len;
    };

    /* Out-params function for pdf_lookup_substitute_font(). */
    const unsigned char *ll_pdf_lookup_substitute_font_outparams_fn(int mono, int serif, int bold, int italic, ll_pdf_lookup_substitute_font_outparams* outparams)
    {
        int len;
        const unsigned char *ret = ll_pdf_lookup_substitute_font(mono, serif, bold, italic, &len);
        outparams->len = len;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_map_one_to_many(). */
    struct ll_pdf_map_one_to_many_outparams
    {
        int many;
    };

    /* Out-params function for pdf_map_one_to_many(). */
    void ll_pdf_map_one_to_many_outparams_fn(::pdf_cmap *cmap, unsigned int one, size_t len, ll_pdf_map_one_to_many_outparams* outparams)
    {
        int many;
        ll_pdf_map_one_to_many(cmap, one, &many, len);
        outparams->many = many;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_obj_memo(). */
    struct ll_pdf_obj_memo_outparams
    {
        int memo;
    };

    /* Out-params function for pdf_obj_memo(). */
    int ll_pdf_obj_memo_outparams_fn(::pdf_obj *obj, int bit, ll_pdf_obj_memo_outparams* outparams)
    {
        int memo;
        int ret = ll_pdf_obj_memo(obj, bit, &memo);
        outparams->memo = memo;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_page_presentation(). */
    struct ll_pdf_page_presentation_outparams
    {
        float duration;
    };

    /* Out-params function for pdf_page_presentation(). */
    ::fz_transition *ll_pdf_page_presentation_outparams_fn(::pdf_page *page, ::fz_transition *transition, ll_pdf_page_presentation_outparams* outparams)
    {
        float duration;
        ::fz_transition *ret = ll_pdf_page_presentation(page, transition, &duration);
        outparams->duration = duration;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_page_write(). */
    struct ll_pdf_page_write_outparams
    {
        struct ::pdf_obj *presources;
        ::fz_buffer *pcontents;
    };

    /* Out-params function for pdf_page_write(). */
    ::fz_device *ll_pdf_page_write_outparams_fn(::pdf_document *doc, ::fz_rect mediabox, ll_pdf_page_write_outparams* outparams)
    {
        outparams->presources = NULL;
        outparams->pcontents = NULL;
        ::pdf_obj *presources;
        ::fz_buffer *pcontents;
        ::fz_device *ret = ll_pdf_page_write(doc, mediabox, &presources, &pcontents);
        outparams->presources = presources;
        outparams->pcontents = pcontents;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_parse_default_appearance(). */
    struct ll_pdf_parse_default_appearance_outparams
    {
        const char *font;
        float size;
        int n;
    };

    /* Out-params function for pdf_parse_default_appearance(). */
    void ll_pdf_parse_default_appearance_outparams_fn(const char *da, float color[4], ll_pdf_parse_default_appearance_outparams* outparams)
    {
        outparams->font = NULL;
        const char *font;
        float size;
        int n;
        ll_pdf_parse_default_appearance(da, &font, &size, &n, color);
        outparams->font = font;
        outparams->size = size;
        outparams->n = n;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_parse_ind_obj(). */
    struct ll_pdf_parse_ind_obj_outparams
    {
        int num;
        int gen;
        long long stm_ofs;
        int try_repair;
    };

    /* Out-params function for pdf_parse_ind_obj(). */
    ::pdf_obj *ll_pdf_parse_ind_obj_outparams_fn(::pdf_document *doc, ::fz_stream *f, ll_pdf_parse_ind_obj_outparams* outparams)
    {
        int num;
        int gen;
        int64_t stm_ofs;
        int try_repair;
        ::pdf_obj *ret = ll_pdf_parse_ind_obj(doc, f, &num, &gen, &stm_ofs, &try_repair);
        outparams->num = num;
        outparams->gen = gen;
        outparams->stm_ofs = stm_ofs;
        outparams->try_repair = try_repair;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_parse_journal_obj(). */
    struct ll_pdf_parse_journal_obj_outparams
    {
        int onum;
        ::fz_buffer *ostm;
        int newobj;
    };

    /* Out-params function for pdf_parse_journal_obj(). */
    ::pdf_obj *ll_pdf_parse_journal_obj_outparams_fn(::pdf_document *doc, ::fz_stream *stm, ll_pdf_parse_journal_obj_outparams* outparams)
    {
        outparams->ostm = NULL;
        int onum;
        ::fz_buffer *ostm;
        int newobj;
        ::pdf_obj *ret = ll_pdf_parse_journal_obj(doc, stm, &onum, &ostm, &newobj);
        outparams->onum = onum;
        outparams->ostm = ostm;
        outparams->newobj = newobj;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_process_contents(). */
    struct ll_pdf_process_contents_outparams
    {
        struct ::pdf_obj *out_res;
    };

    /* Out-params function for pdf_process_contents(). */
    void ll_pdf_process_contents_outparams_fn(::pdf_processor *proc, ::pdf_document *doc, ::pdf_obj *obj, ::pdf_obj *res, ::fz_cookie *cookie, ll_pdf_process_contents_outparams* outparams)
    {
        outparams->out_res = NULL;
        ::pdf_obj *out_res;
        ll_pdf_process_contents(proc, doc, obj, res, cookie, &out_res);
        outparams->out_res = out_res;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_repair_obj(). */
    struct ll_pdf_repair_obj_outparams
    {
        long long stmofsp;
        long long stmlenp;
        struct ::pdf_obj *encrypt;
        struct ::pdf_obj *id;
        struct ::pdf_obj *page;
        long long tmpofs;
        struct ::pdf_obj *root;
    };

    /* Out-params function for pdf_repair_obj(). */
    int ll_pdf_repair_obj_outparams_fn(::pdf_document *doc, ::pdf_lexbuf *buf, ll_pdf_repair_obj_outparams* outparams)
    {
        outparams->encrypt = NULL;
        outparams->id = NULL;
        outparams->page = NULL;
        outparams->root = NULL;
        int64_t stmofsp;
        int64_t stmlenp;
        ::pdf_obj *encrypt;
        ::pdf_obj *id;
        ::pdf_obj *page;
        int64_t tmpofs;
        ::pdf_obj *root;
        int ret = ll_pdf_repair_obj(doc, buf, &stmofsp, &stmlenp, &encrypt, &id, &page, &tmpofs, &root);
        outparams->stmofsp = stmofsp;
        outparams->stmlenp = stmlenp;
        outparams->encrypt = encrypt;
        outparams->id = id;
        outparams->page = page;
        outparams->tmpofs = tmpofs;
        outparams->root = root;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_resolve_link(). */
    struct ll_pdf_resolve_link_outparams
    {
        float xp;
        float yp;
    };

    /* Out-params function for pdf_resolve_link(). */
    int ll_pdf_resolve_link_outparams_fn(::pdf_document *doc, const char *uri, ll_pdf_resolve_link_outparams* outparams)
    {
        float xp;
        float yp;
        int ret = ll_pdf_resolve_link(doc, uri, &xp, &yp);
        outparams->xp = xp;
        outparams->yp = yp;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_sample_shade_function(). */
    struct ll_pdf_sample_shade_function_outparams
    {
        struct ::pdf_function *func;
    };

    /* Out-params function for pdf_sample_shade_function(). */
    void ll_pdf_sample_shade_function_outparams_fn(float shade[256][33], int n, int funcs, float t0, float t1, ll_pdf_sample_shade_function_outparams* outparams)
    {
        outparams->func = NULL;
        ::pdf_function *func;
        ll_pdf_sample_shade_function(shade, n, funcs, &func, t0, t1);
        outparams->func = func;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_signature_contents(). */
    struct ll_pdf_signature_contents_outparams
    {
        char *contents;
    };

    /* Out-params function for pdf_signature_contents(). */
    size_t ll_pdf_signature_contents_outparams_fn(::pdf_document *doc, ::pdf_obj *signature, ll_pdf_signature_contents_outparams* outparams)
    {
        outparams->contents = NULL;
        char *contents;
        size_t ret = ll_pdf_signature_contents(doc, signature, &contents);
        outparams->contents = contents;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_sprint_obj(). */
    struct ll_pdf_sprint_obj_outparams
    {
        size_t len;
    };

    /* Out-params function for pdf_sprint_obj(). */
    char *ll_pdf_sprint_obj_outparams_fn(char *buf, size_t cap, ::pdf_obj *obj, int tight, int ascii, ll_pdf_sprint_obj_outparams* outparams)
    {
        size_t len;
        char *ret = ll_pdf_sprint_obj(buf, cap, &len, obj, tight, ascii);
        outparams->len = len;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_to_string(). */
    struct ll_pdf_to_string_outparams
    {
        size_t sizep;
    };

    /* Out-params function for pdf_to_string(). */
    const char *ll_pdf_to_string_outparams_fn(::pdf_obj *obj, ll_pdf_to_string_outparams* outparams)
    {
        size_t sizep;
        const char *ret = ll_pdf_to_string(obj, &sizep);
        outparams->sizep = sizep;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_undoredo_state(). */
    struct ll_pdf_undoredo_state_outparams
    {
        int steps;
    };

    /* Out-params function for pdf_undoredo_state(). */
    int ll_pdf_undoredo_state_outparams_fn(::pdf_document *doc, ll_pdf_undoredo_state_outparams* outparams)
    {
        int steps;
        int ret = ll_pdf_undoredo_state(doc, &steps);
        outparams->steps = steps;
        return ret;
    }
}

namespace mupdf
{
    /* Out-params helper class for pdf_walk_tree(). */
    struct ll_pdf_walk_tree_outparams
    {
        struct ::pdf_obj *names;
        struct ::pdf_obj *values;
    };

    /* Out-params function for pdf_walk_tree(). */
    void ll_pdf_walk_tree_outparams_fn(::pdf_obj *tree, ::pdf_obj *kid_name, void (*arrive)(::fz_context *, ::pdf_obj *, void *, ::pdf_obj **), void (*leave)(::fz_context *, ::pdf_obj *, void *), void *arg, ll_pdf_walk_tree_outparams* outparams)
    {
        outparams->names = NULL;
        outparams->values = NULL;
        ::pdf_obj *names;
        ::pdf_obj *values;
        ll_pdf_walk_tree(tree, kid_name, arrive, leave, arg, &names, &values);
        outparams->names = names;
        outparams->values = values;
    }
}


enum
{
    UCDN_EAST_ASIAN_F = 0,
    UCDN_EAST_ASIAN_H = 1,
    UCDN_EAST_ASIAN_W = 2,
    UCDN_EAST_ASIAN_NA = 3,
    UCDN_EAST_ASIAN_A = 4,
    UCDN_EAST_ASIAN_N = 5,
    UCDN_SCRIPT_COMMON = 0,
    UCDN_SCRIPT_LATIN = 1,
    UCDN_SCRIPT_GREEK = 2,
    UCDN_SCRIPT_CYRILLIC = 3,
    UCDN_SCRIPT_ARMENIAN = 4,
    UCDN_SCRIPT_HEBREW = 5,
    UCDN_SCRIPT_ARABIC = 6,
    UCDN_SCRIPT_SYRIAC = 7,
    UCDN_SCRIPT_THAANA = 8,
    UCDN_SCRIPT_DEVANAGARI = 9,
    UCDN_SCRIPT_BENGALI = 10,
    UCDN_SCRIPT_GURMUKHI = 11,
    UCDN_SCRIPT_GUJARATI = 12,
    UCDN_SCRIPT_ORIYA = 13,
    UCDN_SCRIPT_TAMIL = 14,
    UCDN_SCRIPT_TELUGU = 15,
    UCDN_SCRIPT_KANNADA = 16,
    UCDN_SCRIPT_MALAYALAM = 17,
    UCDN_SCRIPT_SINHALA = 18,
    UCDN_SCRIPT_THAI = 19,
    UCDN_SCRIPT_LAO = 20,
    UCDN_SCRIPT_TIBETAN = 21,
    UCDN_SCRIPT_MYANMAR = 22,
    UCDN_SCRIPT_GEORGIAN = 23,
    UCDN_SCRIPT_HANGUL = 24,
    UCDN_SCRIPT_ETHIOPIC = 25,
    UCDN_SCRIPT_CHEROKEE = 26,
    UCDN_SCRIPT_CANADIAN_ABORIGINAL = 27,
    UCDN_SCRIPT_OGHAM = 28,
    UCDN_SCRIPT_RUNIC = 29,
    UCDN_SCRIPT_KHMER = 30,
    UCDN_SCRIPT_MONGOLIAN = 31,
    UCDN_SCRIPT_HIRAGANA = 32,
    UCDN_SCRIPT_KATAKANA = 33,
    UCDN_SCRIPT_BOPOMOFO = 34,
    UCDN_SCRIPT_HAN = 35,
    UCDN_SCRIPT_YI = 36,
    UCDN_SCRIPT_OLD_ITALIC = 37,
    UCDN_SCRIPT_GOTHIC = 38,
    UCDN_SCRIPT_DESERET = 39,
    UCDN_SCRIPT_INHERITED = 40,
    UCDN_SCRIPT_TAGALOG = 41,
    UCDN_SCRIPT_HANUNOO = 42,
    UCDN_SCRIPT_BUHID = 43,
    UCDN_SCRIPT_TAGBANWA = 44,
    UCDN_SCRIPT_LIMBU = 45,
    UCDN_SCRIPT_TAI_LE = 46,
    UCDN_SCRIPT_LINEAR_B = 47,
    UCDN_SCRIPT_UGARITIC = 48,
    UCDN_SCRIPT_SHAVIAN = 49,
    UCDN_SCRIPT_OSMANYA = 50,
    UCDN_SCRIPT_CYPRIOT = 51,
    UCDN_SCRIPT_BRAILLE = 52,
    UCDN_SCRIPT_BUGINESE = 53,
    UCDN_SCRIPT_COPTIC = 54,
    UCDN_SCRIPT_NEW_TAI_LUE = 55,
    UCDN_SCRIPT_GLAGOLITIC = 56,
    UCDN_SCRIPT_TIFINAGH = 57,
    UCDN_SCRIPT_SYLOTI_NAGRI = 58,
    UCDN_SCRIPT_OLD_PERSIAN = 59,
    UCDN_SCRIPT_KHAROSHTHI = 60,
    UCDN_SCRIPT_BALINESE = 61,
    UCDN_SCRIPT_CUNEIFORM = 62,
    UCDN_SCRIPT_PHOENICIAN = 63,
    UCDN_SCRIPT_PHAGS_PA = 64,
    UCDN_SCRIPT_NKO = 65,
    UCDN_SCRIPT_SUNDANESE = 66,
    UCDN_SCRIPT_LEPCHA = 67,
    UCDN_SCRIPT_OL_CHIKI = 68,
    UCDN_SCRIPT_VAI = 69,
    UCDN_SCRIPT_SAURASHTRA = 70,
    UCDN_SCRIPT_KAYAH_LI = 71,
    UCDN_SCRIPT_REJANG = 72,
    UCDN_SCRIPT_LYCIAN = 73,
    UCDN_SCRIPT_CARIAN = 74,
    UCDN_SCRIPT_LYDIAN = 75,
    UCDN_SCRIPT_CHAM = 76,
    UCDN_SCRIPT_TAI_THAM = 77,
    UCDN_SCRIPT_TAI_VIET = 78,
    UCDN_SCRIPT_AVESTAN = 79,
    UCDN_SCRIPT_EGYPTIAN_HIEROGLYPHS = 80,
    UCDN_SCRIPT_SAMARITAN = 81,
    UCDN_SCRIPT_LISU = 82,
    UCDN_SCRIPT_BAMUM = 83,
    UCDN_SCRIPT_JAVANESE = 84,
    UCDN_SCRIPT_MEETEI_MAYEK = 85,
    UCDN_SCRIPT_IMPERIAL_ARAMAIC = 86,
    UCDN_SCRIPT_OLD_SOUTH_ARABIAN = 87,
    UCDN_SCRIPT_INSCRIPTIONAL_PARTHIAN = 88,
    UCDN_SCRIPT_INSCRIPTIONAL_PAHLAVI = 89,
    UCDN_SCRIPT_OLD_TURKIC = 90,
    UCDN_SCRIPT_KAITHI = 91,
    UCDN_SCRIPT_BATAK = 92,
    UCDN_SCRIPT_BRAHMI = 93,
    UCDN_SCRIPT_MANDAIC = 94,
    UCDN_SCRIPT_CHAKMA = 95,
    UCDN_SCRIPT_MEROITIC_CURSIVE = 96,
    UCDN_SCRIPT_MEROITIC_HIEROGLYPHS = 97,
    UCDN_SCRIPT_MIAO = 98,
    UCDN_SCRIPT_SHARADA = 99,
    UCDN_SCRIPT_SORA_SOMPENG = 100,
    UCDN_SCRIPT_TAKRI = 101,
    UCDN_SCRIPT_UNKNOWN = 102,
    UCDN_SCRIPT_BASSA_VAH = 103,
    UCDN_SCRIPT_CAUCASIAN_ALBANIAN = 104,
    UCDN_SCRIPT_DUPLOYAN = 105,
    UCDN_SCRIPT_ELBASAN = 106,
    UCDN_SCRIPT_GRANTHA = 107,
    UCDN_SCRIPT_KHOJKI = 108,
    UCDN_SCRIPT_KHUDAWADI = 109,
    UCDN_SCRIPT_LINEAR_A = 110,
    UCDN_SCRIPT_MAHAJANI = 111,
    UCDN_SCRIPT_MANICHAEAN = 112,
    UCDN_SCRIPT_MENDE_KIKAKUI = 113,
    UCDN_SCRIPT_MODI = 114,
    UCDN_SCRIPT_MRO = 115,
    UCDN_SCRIPT_NABATAEAN = 116,
    UCDN_SCRIPT_OLD_NORTH_ARABIAN = 117,
    UCDN_SCRIPT_OLD_PERMIC = 118,
    UCDN_SCRIPT_PAHAWH_HMONG = 119,
    UCDN_SCRIPT_PALMYRENE = 120,
    UCDN_SCRIPT_PAU_CIN_HAU = 121,
    UCDN_SCRIPT_PSALTER_PAHLAVI = 122,
    UCDN_SCRIPT_SIDDHAM = 123,
    UCDN_SCRIPT_TIRHUTA = 124,
    UCDN_SCRIPT_WARANG_CITI = 125,
    UCDN_SCRIPT_AHOM = 126,
    UCDN_SCRIPT_ANATOLIAN_HIEROGLYPHS = 127,
    UCDN_SCRIPT_HATRAN = 128,
    UCDN_SCRIPT_MULTANI = 129,
    UCDN_SCRIPT_OLD_HUNGARIAN = 130,
    UCDN_SCRIPT_SIGNWRITING = 131,
    UCDN_SCRIPT_ADLAM = 132,
    UCDN_SCRIPT_BHAIKSUKI = 133,
    UCDN_SCRIPT_MARCHEN = 134,
    UCDN_SCRIPT_NEWA = 135,
    UCDN_SCRIPT_OSAGE = 136,
    UCDN_SCRIPT_TANGUT = 137,
    UCDN_SCRIPT_MASARAM_GONDI = 138,
    UCDN_SCRIPT_NUSHU = 139,
    UCDN_SCRIPT_SOYOMBO = 140,
    UCDN_SCRIPT_ZANABAZAR_SQUARE = 141,
    UCDN_SCRIPT_DOGRA = 142,
    UCDN_SCRIPT_GUNJALA_GONDI = 143,
    UCDN_SCRIPT_HANIFI_ROHINGYA = 144,
    UCDN_SCRIPT_MAKASAR = 145,
    UCDN_SCRIPT_MEDEFAIDRIN = 146,
    UCDN_SCRIPT_OLD_SOGDIAN = 147,
    UCDN_SCRIPT_SOGDIAN = 148,
    UCDN_SCRIPT_ELYMAIC = 149,
    UCDN_SCRIPT_NANDINAGARI = 150,
    UCDN_SCRIPT_NYIAKENG_PUACHUE_HMONG = 151,
    UCDN_SCRIPT_WANCHO = 152,
    UCDN_SCRIPT_CHORASMIAN = 153,
    UCDN_SCRIPT_DIVES_AKURU = 154,
    UCDN_SCRIPT_KHITAN_SMALL_SCRIPT = 155,
    UCDN_SCRIPT_YEZIDI = 156,
    UCDN_SCRIPT_VITHKUQI = 157,
    UCDN_SCRIPT_OLD_UYGHUR = 158,
    UCDN_SCRIPT_CYPRO_MINOAN = 159,
    UCDN_SCRIPT_TANGSA = 160,
    UCDN_SCRIPT_TOTO = 161,
    UCDN_SCRIPT_KAWI = 162,
    UCDN_SCRIPT_NAG_MUNDARI = 163,
    UCDN_LAST_SCRIPT = 163,
    UCDN_LINEBREAK_CLASS_OP = 0,
    UCDN_LINEBREAK_CLASS_CL = 1,
    UCDN_LINEBREAK_CLASS_CP = 2,
    UCDN_LINEBREAK_CLASS_QU = 3,
    UCDN_LINEBREAK_CLASS_GL = 4,
    UCDN_LINEBREAK_CLASS_NS = 5,
    UCDN_LINEBREAK_CLASS_EX = 6,
    UCDN_LINEBREAK_CLASS_SY = 7,
    UCDN_LINEBREAK_CLASS_IS = 8,
    UCDN_LINEBREAK_CLASS_PR = 9,
    UCDN_LINEBREAK_CLASS_PO = 10,
    UCDN_LINEBREAK_CLASS_NU = 11,
    UCDN_LINEBREAK_CLASS_AL = 12,
    UCDN_LINEBREAK_CLASS_HL = 13,
    UCDN_LINEBREAK_CLASS_ID = 14,
    UCDN_LINEBREAK_CLASS_IN = 15,
    UCDN_LINEBREAK_CLASS_HY = 16,
    UCDN_LINEBREAK_CLASS_BA = 17,
    UCDN_LINEBREAK_CLASS_BB = 18,
    UCDN_LINEBREAK_CLASS_B2 = 19,
    UCDN_LINEBREAK_CLASS_ZW = 20,
    UCDN_LINEBREAK_CLASS_CM = 21,
    UCDN_LINEBREAK_CLASS_WJ = 22,
    UCDN_LINEBREAK_CLASS_H2 = 23,
    UCDN_LINEBREAK_CLASS_H3 = 24,
    UCDN_LINEBREAK_CLASS_JL = 25,
    UCDN_LINEBREAK_CLASS_JV = 26,
    UCDN_LINEBREAK_CLASS_JT = 27,
    UCDN_LINEBREAK_CLASS_RI = 28,
    UCDN_LINEBREAK_CLASS_AI = 29,
    UCDN_LINEBREAK_CLASS_BK = 30,
    UCDN_LINEBREAK_CLASS_CB = 31,
    UCDN_LINEBREAK_CLASS_CJ = 32,
    UCDN_LINEBREAK_CLASS_CR = 33,
    UCDN_LINEBREAK_CLASS_LF = 34,
    UCDN_LINEBREAK_CLASS_NL = 35,
    UCDN_LINEBREAK_CLASS_SA = 36,
    UCDN_LINEBREAK_CLASS_SG = 37,
    UCDN_LINEBREAK_CLASS_SP = 38,
    UCDN_LINEBREAK_CLASS_XX = 39,
    UCDN_LINEBREAK_CLASS_ZWJ = 40,
    UCDN_LINEBREAK_CLASS_EB = 41,
    UCDN_LINEBREAK_CLASS_EM = 42,
    UCDN_GENERAL_CATEGORY_CC = 0,
    UCDN_GENERAL_CATEGORY_CF = 1,
    UCDN_GENERAL_CATEGORY_CN = 2,
    UCDN_GENERAL_CATEGORY_CO = 3,
    UCDN_GENERAL_CATEGORY_CS = 4,
    UCDN_GENERAL_CATEGORY_LL = 5,
    UCDN_GENERAL_CATEGORY_LM = 6,
    UCDN_GENERAL_CATEGORY_LO = 7,
    UCDN_GENERAL_CATEGORY_LT = 8,
    UCDN_GENERAL_CATEGORY_LU = 9,
    UCDN_GENERAL_CATEGORY_MC = 10,
    UCDN_GENERAL_CATEGORY_ME = 11,
    UCDN_GENERAL_CATEGORY_MN = 12,
    UCDN_GENERAL_CATEGORY_ND = 13,
    UCDN_GENERAL_CATEGORY_NL = 14,
    UCDN_GENERAL_CATEGORY_NO = 15,
    UCDN_GENERAL_CATEGORY_PC = 16,
    UCDN_GENERAL_CATEGORY_PD = 17,
    UCDN_GENERAL_CATEGORY_PE = 18,
    UCDN_GENERAL_CATEGORY_PF = 19,
    UCDN_GENERAL_CATEGORY_PI = 20,
    UCDN_GENERAL_CATEGORY_PO = 21,
    UCDN_GENERAL_CATEGORY_PS = 22,
    UCDN_GENERAL_CATEGORY_SC = 23,
    UCDN_GENERAL_CATEGORY_SK = 24,
    UCDN_GENERAL_CATEGORY_SM = 25,
    UCDN_GENERAL_CATEGORY_SO = 26,
    UCDN_GENERAL_CATEGORY_ZL = 27,
    UCDN_GENERAL_CATEGORY_ZP = 28,
    UCDN_GENERAL_CATEGORY_ZS = 29,
    UCDN_BIDI_CLASS_L = 0,
    UCDN_BIDI_CLASS_LRE = 1,
    UCDN_BIDI_CLASS_LRO = 2,
    UCDN_BIDI_CLASS_R = 3,
    UCDN_BIDI_CLASS_AL = 4,
    UCDN_BIDI_CLASS_RLE = 5,
    UCDN_BIDI_CLASS_RLO = 6,
    UCDN_BIDI_CLASS_PDF = 7,
    UCDN_BIDI_CLASS_EN = 8,
    UCDN_BIDI_CLASS_ES = 9,
    UCDN_BIDI_CLASS_ET = 10,
    UCDN_BIDI_CLASS_AN = 11,
    UCDN_BIDI_CLASS_CS = 12,
    UCDN_BIDI_CLASS_NSM = 13,
    UCDN_BIDI_CLASS_BN = 14,
    UCDN_BIDI_CLASS_B = 15,
    UCDN_BIDI_CLASS_S = 16,
    UCDN_BIDI_CLASS_WS = 17,
    UCDN_BIDI_CLASS_ON = 18,
    UCDN_BIDI_CLASS_LRI = 19,
    UCDN_BIDI_CLASS_RLI = 20,
    UCDN_BIDI_CLASS_FSI = 21,
    UCDN_BIDI_CLASS_PDI = 22,
    UCDN_BIDI_PAIRED_BRACKET_TYPE_OPEN = 0,
    UCDN_BIDI_PAIRED_BRACKET_TYPE_CLOSE = 1,
    UCDN_BIDI_PAIRED_BRACKET_TYPE_NONE = 2,
};


%pointer_functions(int, pint);

%pythoncode %{

import inspect
import os
import re
import sys
import traceback

def log( text):
    print( text, file=sys.stderr)

g_mupdf_trace_director = (os.environ.get('MUPDF_trace_director') == '1')

def fz_lookup_metadata_extra(self, key):
    """
    Python implementation override of FzDocument.lookup_metadata().

    Returns string or None if not found.
    """
    e = new_pint()
    ret = ll_fz_lookup_metadata(self.m_internal, key, e)
    e = pint_value(e)
    if e < 0:
        return None
    return ret

FzDocument.fz_lookup_metadata = fz_lookup_metadata_extra
fz_lookup_metadata = fz_lookup_metadata_extra

def pdf_lookup_metadata_extra(self, key):
    """
    Python implementation override of PdfDocument.pdf_lookup_metadata().

    Returns string or None if not found.
    """
    e = new_pint()
    ret = ll_pdf_lookup_metadata(self.m_internal, key, e)
    e = pint_value(e)
    if e < 0:
        return None
    return ret

PdfDocument.pdf_lookup_metadata = pdf_lookup_metadata_extra
pdf_lookup_metadata = pdf_lookup_metadata_extra
import inspect
import io
import os
import sys
import traceback
import types
def exception_info(
        exception_or_traceback=None,
        limit=None,
        file=None,
        chain=True,
        outer=True,
        show_exception_type=True,
        _filelinefn=True,
        ):
    '''
    Shows an exception and/or backtrace.

    Alternative to `traceback.*` functions that print/return information about
    exceptions and backtraces, such as:

        * `traceback.format_exc()`
        * `traceback.format_exception()`
        * `traceback.print_exc()`
        * `traceback.print_exception()`

    Install as system default with:

        `sys.excepthook = lambda type_, exception, traceback: jlib.exception_info( exception)`

    Returns `None`, or the generated text if `file` is 'return'.

    Args:
        exception_or_traceback:
            `None`, a `BaseException`, a `types.TracebackType` (typically from
            an exception's `.__traceback__` member) or an `inspect.FrameInfo`.

            If `None` we use current exception from `sys.exc_info()` if set,
            otherwise the current backtrace from `inspect.stack()`.
        limit:
            As in `traceback.*` functions: `None` to show all frames, positive
            to show last `limit` frames, negative to exclude outermost `-limit`
            frames. Zero to not show any backtraces.
        file:
            As in `traceback.*` functions: file-like object to which we write
            output, or `sys.stderr` if `None`. Special value 'return' makes us
            return our output as a string.
        chain:
            As in `traceback.*` functions: if true (the default) we show
            chained exceptions as described in PEP-3134. Special value
            'because' reverses the usual ordering, showing higher-level
            exceptions first and joining with 'Because:' text.
        outer:
            If true (the default) we also show an exception's outer frames
            above the `catch` block (see next section for details). We
            use `outer=false` internally for chained exceptions to avoid
            duplication.
        show_exception_type:
            Controls whether exception text is prefixed by
            `f'{type(exception)}: '`. If callable we only include this prefix
            if `show_exception_type(exception)` is true. Otherwise if true (the
            default) we include the prefix for all exceptions (this mimcs the
            behaviour of `traceback.*` functions). Otherwise we exclude the
            prefix for all exceptions.
        _filelinefn:
            Internal only; makes us omit file:line: information to allow simple
            doctest comparison with expected output.

    Differences from `traceback.*` functions:

        Frames are displayed as one line in the form::

            <file>:<line>:<function>: <text>

        Filenames are displayed as relative to the current directory if
        applicable.

        Inclusion of outer frames:
            Unlike `traceback.*` functions, stack traces for exceptions include
            outer stack frames above the point at which an exception was caught
            - i.e. frames from the top-level <module> or thread creation to the
            catch block. [Search for 'sys.exc_info backtrace incomplete' for
            more details.]

            We separate the two parts of the backtrace using a marker line
            '^except raise:' where '^except' points upwards to the frame that
            caught the exception and 'raise:' refers downwards to the frame
            that raised the exception.

            So the backtrace for an exception looks like this::

                <file>:<line>:<fn>: <text>  [in root module.]
                ...                         [... other frames]
                <file>:<line>:<fn>: <text>  [in except: block where exception was caught.]
                ^except raise:              [marker line]
                <file>:<line>:<fn>: <text>  [in try: block.]
                ...                         [... other frames]
                <file>:<line>:<fn>: <text>  [where the exception was raised.]

    Examples:

        In these examples we use `file=sys.stdout` so we can check the output
        with `doctest`, and set `_filelinefn=0` so that the output can be
        matched easily. We also use `+ELLIPSIS` and `...` to match arbitrary
        outer frames from the doctest code itself.

        Basic handling of an exception:

            >>> def c():
            ...     raise Exception( 'c() failed')
            >>> def b():
            ...     try:
            ...         c()
            ...     except Exception as e:
            ...         exception_info( e, file=sys.stdout, _filelinefn=0)
            >>> def a():
            ...     b()

            >>> a() # doctest: +REPORT_UDIFF +ELLIPSIS
            Traceback (most recent call last):
                ...
                a(): b()
                b(): exception_info( e, file=sys.stdout, _filelinefn=0)
                ^except raise:
                b(): c()
                c(): raise Exception( 'c() failed')
            Exception: c() failed

        Handling of chained exceptions:

            >>> def e():
            ...     raise Exception( 'e(): deliberate error')
            >>> def d():
            ...     e()
            >>> def c():
            ...     try:
            ...         d()
            ...     except Exception as e:
            ...         raise Exception( 'c: d() failed') from e
            >>> def b():
            ...     try:
            ...         c()
            ...     except Exception as e:
            ...         exception_info( file=sys.stdout, chain=g_chain, _filelinefn=0)
            >>> def a():
            ...     b()

            With `chain=True` (the default), we output low-level exceptions
            first, matching the behaviour of `traceback.*` functions:

                >>> g_chain = True
                >>> a() # doctest: +REPORT_UDIFF +ELLIPSIS
                Traceback (most recent call last):
                    c(): d()
                    d(): e()
                    e(): raise Exception( 'e(): deliberate error')
                Exception: e(): deliberate error
                <BLANKLINE>
                The above exception was the direct cause of the following exception:
                Traceback (most recent call last):
                    ...
                    <module>(): a() # doctest: +REPORT_UDIFF +ELLIPSIS
                    a(): b()
                    b(): exception_info( file=sys.stdout, chain=g_chain, _filelinefn=0)
                    ^except raise:
                    b(): c()
                    c(): raise Exception( 'c: d() failed') from e
                Exception: c: d() failed

            With `chain='because'`, we output high-level exceptions first:
                >>> g_chain = 'because'
                >>> a() # doctest: +REPORT_UDIFF +ELLIPSIS
                Traceback (most recent call last):
                    ...
                    <module>(): a() # doctest: +REPORT_UDIFF +ELLIPSIS
                    a(): b()
                    b(): exception_info( file=sys.stdout, chain=g_chain, _filelinefn=0)
                    ^except raise:
                    b(): c()
                    c(): raise Exception( 'c: d() failed') from e
                Exception: c: d() failed
                <BLANKLINE>
                Because:
                Traceback (most recent call last):
                    c(): d()
                    d(): e()
                    e(): raise Exception( 'e(): deliberate error')
                Exception: e(): deliberate error

        Show current backtrace by passing `exception_or_traceback=None`:
            >>> def c():
            ...     exception_info( None, file=sys.stdout, _filelinefn=0)
            >>> def b():
            ...     return c()
            >>> def a():
            ...     return b()

            >>> a() # doctest: +REPORT_UDIFF +ELLIPSIS
            Traceback (most recent call last):
                ...
                <module>(): a() # doctest: +REPORT_UDIFF +ELLIPSIS
                a(): return b()
                b(): return c()
                c(): exception_info( None, file=sys.stdout, _filelinefn=0)

        Show an exception's `.__traceback__` backtrace:
            >>> def c():
            ...     raise Exception( 'foo') # raise
            >>> def b():
            ...     return c()  # call c
            >>> def a():
            ...     try:
            ...         b() # call b
            ...     except Exception as e:
            ...         exception_info( e.__traceback__, file=sys.stdout, _filelinefn=0)

            >>> a() # doctest: +REPORT_UDIFF +ELLIPSIS
            Traceback (most recent call last):
                ...
                a(): b() # call b
                b(): return c()  # call c
                c(): raise Exception( 'foo') # raise
    '''
    # Set exactly one of <exception> and <tb>.
    #
    if isinstance( exception_or_traceback, (types.TracebackType, inspect.FrameInfo)):
        # Simple backtrace, no Exception information.
        exception = None
        tb = exception_or_traceback
    elif isinstance( exception_or_traceback, BaseException):
        exception = exception_or_traceback
        tb = None
    elif exception_or_traceback is None:
        # Show exception if available, else backtrace.
        _, exception, tb = sys.exc_info()
        tb = None if exception else inspect.stack()[1:]
    else:
        assert 0, f'Unrecognised exception_or_traceback type: {type(exception_or_traceback)}'

    if file == 'return':
        out = io.StringIO()
    else:
        out = file if file else sys.stderr

    def do_chain( exception):
        exception_info(
                exception,
                limit,
                out,
                chain,
                outer=False,
                show_exception_type=show_exception_type,
                _filelinefn=_filelinefn,
                )

    if exception and chain and chain != 'because' and chain != 'because-compact':
        # Output current exception first.
        if exception.__cause__:
            do_chain( exception.__cause__)
            out.write( '\nThe above exception was the direct cause of the following exception:\n')
        elif exception.__context__:
            do_chain( exception.__context__)
            out.write( '\nDuring handling of the above exception, another exception occurred:\n')

    cwd = os.getcwd() + os.sep

    def output_frames( frames, reverse, limit):
        if limit == 0:
            return
        if reverse:
            assert isinstance( frames, list)
            frames = reversed( frames)
        if limit is not None:
            frames = list( frames)
            frames = frames[ -limit:]
        for frame in frames:
            f, filename, line, fnname, text, index = frame
            text = text[0].strip() if text else ''
            if filename.startswith( cwd):
                filename = filename[ len(cwd):]
            if filename.startswith( f'.{os.sep}'):
                filename = filename[ 2:]
            if _filelinefn:
                out.write( f'    {filename}:{line}:{fnname}(): {text}\n')
            else:
                out.write( f'    {fnname}(): {text}\n')

    if limit != 0:
        out.write( 'Traceback (most recent call last):\n')
        if exception:
            tb = exception.__traceback__
            assert tb
            if outer:
                output_frames( inspect.getouterframes( tb.tb_frame), reverse=True, limit=limit)
                out.write( '    ^except raise:\n')
            limit2 = 0 if limit == 0 else None
            output_frames( inspect.getinnerframes( tb), reverse=False, limit=limit2)
        else:
            if not isinstance( tb, list):
                inner = inspect.getinnerframes(tb)
                outer = inspect.getouterframes(tb.tb_frame)
                tb = outer + inner
                tb.reverse()
            output_frames( tb, reverse=True, limit=limit)

    if exception:
        if callable(show_exception_type):
            show_exception_type2 = show_exception_type( exception)
        else:
            show_exception_type2 = show_exception_type
        if show_exception_type2:
            lines = traceback.format_exception_only( type(exception), exception)
            for line in lines:
                out.write( line)
        else:
            out.write( str( exception) + '\n')

    if exception and (chain == 'because' or chain == 'because-compact'):
        # Output current exception afterwards.
        pre, post = ('\n', '\n') if chain == 'because' else ('', ' ')
        if exception.__cause__:
            out.write( f'{pre}Because:{post}')
            do_chain( exception.__cause__)
        elif exception.__context__:
            out.write( f'{pre}Because: error occurred handling this exception:{post}')
            do_chain( exception.__context__)

    if file == 'return':
        return out.getvalue()
def ll_fz_bitmap_details(bitmap):
    """
    Wrapper for out-params of fz_bitmap_details().
    Returns: int w, int h, int n, int stride
    """
    outparams = ll_fz_bitmap_details_outparams()
    ret = ll_fz_bitmap_details_outparams_fn(bitmap, outparams)
    return outparams.w, outparams.h, outparams.n, outparams.stride

def fz_bitmap_details_outparams_fn(bitmap):
    """
    Class-aware helper for out-params of fz_bitmap_details() [fz_bitmap_details()].
    """
    w, h, n, stride = ll_fz_bitmap_details(bitmap.m_internal)
    return w, h, n, stride

fz_bitmap_details = fz_bitmap_details_outparams_fn


def ll_fz_buffer_extract(buf):
    """
    Wrapper for out-params of fz_buffer_extract().
    Returns: size_t, unsigned char *data
    """
    outparams = ll_fz_buffer_extract_outparams()
    ret = ll_fz_buffer_extract_outparams_fn(buf, outparams)
    return ret, outparams.data

def fz_buffer_extract_outparams_fn(buf):
    """
    Class-aware helper for out-params of fz_buffer_extract() [fz_buffer_extract()].
    """
    ret, data = ll_fz_buffer_extract(buf.m_internal)
    return ret, data

fz_buffer_extract = fz_buffer_extract_outparams_fn


def ll_fz_buffer_storage(buf):
    """
    Wrapper for out-params of fz_buffer_storage().
    Returns: size_t, unsigned char *datap
    """
    outparams = ll_fz_buffer_storage_outparams()
    ret = ll_fz_buffer_storage_outparams_fn(buf, outparams)
    return ret, outparams.datap

def fz_buffer_storage_outparams_fn(buf):
    """
    Class-aware helper for out-params of fz_buffer_storage() [fz_buffer_storage()].
    """
    ret, datap = ll_fz_buffer_storage(buf.m_internal)
    return ret, datap

fz_buffer_storage = fz_buffer_storage_outparams_fn


def ll_fz_chartorune(str):
    """
    Wrapper for out-params of fz_chartorune().
    Returns: int, int rune
    """
    outparams = ll_fz_chartorune_outparams()
    ret = ll_fz_chartorune_outparams_fn(str, outparams)
    return ret, outparams.rune

def fz_chartorune_outparams_fn(str):
    """
    Class-aware helper for out-params of fz_chartorune() [fz_chartorune()].
    """
    ret, rune = ll_fz_chartorune(str)
    return ret, rune

fz_chartorune = fz_chartorune_outparams_fn


def ll_fz_clamp_color(cs, in_):
    """
    Wrapper for out-params of fz_clamp_color().
    Returns: float out
    """
    outparams = ll_fz_clamp_color_outparams()
    ret = ll_fz_clamp_color_outparams_fn(cs, in_, outparams)
    return outparams.out

def fz_clamp_color_outparams_fn(cs, in_):
    """
    Class-aware helper for out-params of fz_clamp_color() [fz_clamp_color()].
    """
    out = ll_fz_clamp_color(cs.m_internal, in_)
    return out

fz_clamp_color = fz_clamp_color_outparams_fn


def ll_fz_convert_color(ss, sv, ds, is_, params):
    """
    Wrapper for out-params of fz_convert_color().
    Returns: float dv
    """
    outparams = ll_fz_convert_color_outparams()
    ret = ll_fz_convert_color_outparams_fn(ss, sv, ds, is_, params, outparams)
    return outparams.dv

def fz_convert_color_outparams_fn(ss, sv, ds, is_, params):
    """
    Class-aware helper for out-params of fz_convert_color() [fz_convert_color()].
    """
    dv = ll_fz_convert_color(ss.m_internal, sv, ds.m_internal, is_.m_internal, params.internal())
    return dv

fz_convert_color = fz_convert_color_outparams_fn


def ll_fz_convert_separation_colors(src_cs, src_color, dst_seps, dst_cs, color_params):
    """
    Wrapper for out-params of fz_convert_separation_colors().
    Returns: float dst_color
    """
    outparams = ll_fz_convert_separation_colors_outparams()
    ret = ll_fz_convert_separation_colors_outparams_fn(src_cs, src_color, dst_seps, dst_cs, color_params, outparams)
    return outparams.dst_color

def fz_convert_separation_colors_outparams_fn(src_cs, src_color, dst_seps, dst_cs, color_params):
    """
    Class-aware helper for out-params of fz_convert_separation_colors() [fz_convert_separation_colors()].
    """
    dst_color = ll_fz_convert_separation_colors(src_cs.m_internal, src_color, dst_seps.m_internal, dst_cs.m_internal, color_params.internal())
    return dst_color

fz_convert_separation_colors = fz_convert_separation_colors_outparams_fn


def ll_fz_decomp_image_from_stream(stm, image, subarea, indexed, l2factor):
    """
    Wrapper for out-params of fz_decomp_image_from_stream().
    Returns: fz_pixmap *, int l2extra
    """
    outparams = ll_fz_decomp_image_from_stream_outparams()
    ret = ll_fz_decomp_image_from_stream_outparams_fn(stm, image, subarea, indexed, l2factor, outparams)
    return ret, outparams.l2extra

def fz_decomp_image_from_stream_outparams_fn(stm, image, subarea, indexed, l2factor):
    """
    Class-aware helper for out-params of fz_decomp_image_from_stream() [fz_decomp_image_from_stream()].
    """
    ret, l2extra = ll_fz_decomp_image_from_stream(stm.m_internal, image.m_internal, subarea.internal(), indexed, l2factor)
    return FzPixmap(ret), l2extra

fz_decomp_image_from_stream = fz_decomp_image_from_stream_outparams_fn


def ll_fz_deflate(dest, source, source_length, level):
    """
    Wrapper for out-params of fz_deflate().
    Returns: size_t compressed_length
    """
    outparams = ll_fz_deflate_outparams()
    ret = ll_fz_deflate_outparams_fn(dest, source, source_length, level, outparams)
    return outparams.compressed_length

def fz_deflate_outparams_fn(dest, source, source_length, level):
    """
    Class-aware helper for out-params of fz_deflate() [fz_deflate()].
    """
    compressed_length = ll_fz_deflate(dest, source, source_length, level)
    return compressed_length

fz_deflate = fz_deflate_outparams_fn


def ll_fz_dom_get_attribute(elt, i):
    """
    Wrapper for out-params of fz_dom_get_attribute().
    Returns: const char *, const char *att
    """
    outparams = ll_fz_dom_get_attribute_outparams()
    ret = ll_fz_dom_get_attribute_outparams_fn(elt, i, outparams)
    return ret, outparams.att

def fz_dom_get_attribute_outparams_fn(elt, i):
    """
    Class-aware helper for out-params of fz_dom_get_attribute() [fz_dom_get_attribute()].
    """
    ret, att = ll_fz_dom_get_attribute(elt.m_internal, i)
    return ret, att

fz_dom_get_attribute = fz_dom_get_attribute_outparams_fn


def ll_fz_drop_imp(p):
    """
    Wrapper for out-params of fz_drop_imp().
    Returns: int, int refs
    """
    outparams = ll_fz_drop_imp_outparams()
    ret = ll_fz_drop_imp_outparams_fn(p, outparams)
    return ret, outparams.refs

def ll_fz_drop_imp16(p):
    """
    Wrapper for out-params of fz_drop_imp16().
    Returns: int, int16_t refs
    """
    outparams = ll_fz_drop_imp16_outparams()
    ret = ll_fz_drop_imp16_outparams_fn(p, outparams)
    return ret, outparams.refs

def ll_fz_encode_character_with_fallback(font, unicode, script, language):
    """
    Wrapper for out-params of fz_encode_character_with_fallback().
    Returns: int, ::fz_font *out_font
    """
    outparams = ll_fz_encode_character_with_fallback_outparams()
    ret = ll_fz_encode_character_with_fallback_outparams_fn(font, unicode, script, language, outparams)
    return ret, outparams.out_font

def fz_encode_character_with_fallback_outparams_fn(font, unicode, script, language):
    """
    Class-aware helper for out-params of fz_encode_character_with_fallback() [fz_encode_character_with_fallback()].
    """
    ret, out_font = ll_fz_encode_character_with_fallback(font.m_internal, unicode, script, language)
    return ret, FzFont(ll_fz_keep_font( out_font))

fz_encode_character_with_fallback = fz_encode_character_with_fallback_outparams_fn


def ll_fz_error_callback():
    """
    Wrapper for out-params of fz_error_callback().
    Returns: fz_error_cb *, void *user
    """
    outparams = ll_fz_error_callback_outparams()
    ret = ll_fz_error_callback_outparams_fn(outparams)
    return ret, outparams.user

def fz_error_callback_outparams_fn():
    """
    Class-aware helper for out-params of fz_error_callback() [fz_error_callback()].
    """
    ret, user = ll_fz_error_callback()
    return ret, user

fz_error_callback = fz_error_callback_outparams_fn


def ll_fz_fill_pixmap_with_color(pix, colorspace, color_params):
    """
    Wrapper for out-params of fz_fill_pixmap_with_color().
    Returns: float color
    """
    outparams = ll_fz_fill_pixmap_with_color_outparams()
    ret = ll_fz_fill_pixmap_with_color_outparams_fn(pix, colorspace, color_params, outparams)
    return outparams.color

def fz_fill_pixmap_with_color_outparams_fn(pix, colorspace, color_params):
    """
    Class-aware helper for out-params of fz_fill_pixmap_with_color() [fz_fill_pixmap_with_color()].
    """
    color = ll_fz_fill_pixmap_with_color(pix.m_internal, colorspace.m_internal, color_params.internal())
    return color

fz_fill_pixmap_with_color = fz_fill_pixmap_with_color_outparams_fn


def ll_fz_get_pixmap_from_image(image, subarea, ctm):
    """
    Wrapper for out-params of fz_get_pixmap_from_image().
    Returns: fz_pixmap *, int w, int h
    """
    outparams = ll_fz_get_pixmap_from_image_outparams()
    ret = ll_fz_get_pixmap_from_image_outparams_fn(image, subarea, ctm, outparams)
    return ret, outparams.w, outparams.h

def fz_get_pixmap_from_image_outparams_fn(image, subarea, ctm):
    """
    Class-aware helper for out-params of fz_get_pixmap_from_image() [fz_get_pixmap_from_image()].
    """
    ret, w, h = ll_fz_get_pixmap_from_image(image.m_internal, subarea.internal(), ctm.internal())
    return FzPixmap(ret), w, h

fz_get_pixmap_from_image = fz_get_pixmap_from_image_outparams_fn


def ll_fz_getopt(nargc, ostr):
    """
    Wrapper for out-params of fz_getopt().
    Returns: int, char *nargv
    """
    outparams = ll_fz_getopt_outparams()
    ret = ll_fz_getopt_outparams_fn(nargc, ostr, outparams)
    return ret, outparams.nargv

def fz_getopt_outparams_fn(nargc, ostr):
    """
    Class-aware helper for out-params of fz_getopt() [fz_getopt()].
    """
    ret, nargv = ll_fz_getopt(nargc, ostr)
    return ret, nargv

fz_getopt = fz_getopt_outparams_fn


def ll_fz_grisu(f, s):
    """
    Wrapper for out-params of fz_grisu().
    Returns: int, int exp
    """
    outparams = ll_fz_grisu_outparams()
    ret = ll_fz_grisu_outparams_fn(f, s, outparams)
    return ret, outparams.exp

def fz_grisu_outparams_fn(f, s):
    """
    Class-aware helper for out-params of fz_grisu() [fz_grisu()].
    """
    ret, exp = ll_fz_grisu(f, s)
    return ret, exp

fz_grisu = fz_grisu_outparams_fn


def ll_fz_has_option(opts, key):
    """
    Wrapper for out-params of fz_has_option().
    Returns: int, const char *val
    """
    outparams = ll_fz_has_option_outparams()
    ret = ll_fz_has_option_outparams_fn(opts, key, outparams)
    return ret, outparams.val

def fz_has_option_outparams_fn(opts, key):
    """
    Class-aware helper for out-params of fz_has_option() [fz_has_option()].
    """
    ret, val = ll_fz_has_option(opts, key)
    return ret, val

fz_has_option = fz_has_option_outparams_fn


def ll_fz_image_resolution(image):
    """
    Wrapper for out-params of fz_image_resolution().
    Returns: int xres, int yres
    """
    outparams = ll_fz_image_resolution_outparams()
    ret = ll_fz_image_resolution_outparams_fn(image, outparams)
    return outparams.xres, outparams.yres

def fz_image_resolution_outparams_fn(image):
    """
    Class-aware helper for out-params of fz_image_resolution() [fz_image_resolution()].
    """
    xres, yres = ll_fz_image_resolution(image.m_internal)
    return xres, yres

fz_image_resolution = fz_image_resolution_outparams_fn


def ll_fz_keep_imp(p):
    """
    Wrapper for out-params of fz_keep_imp().
    Returns: void *, int refs
    """
    outparams = ll_fz_keep_imp_outparams()
    ret = ll_fz_keep_imp_outparams_fn(p, outparams)
    return ret, outparams.refs

def ll_fz_keep_imp16(p):
    """
    Wrapper for out-params of fz_keep_imp16().
    Returns: void *, int16_t refs
    """
    outparams = ll_fz_keep_imp16_outparams()
    ret = ll_fz_keep_imp16_outparams_fn(p, outparams)
    return ret, outparams.refs

def ll_fz_keep_imp_locked(p):
    """
    Wrapper for out-params of fz_keep_imp_locked().
    Returns: void *, int refs
    """
    outparams = ll_fz_keep_imp_locked_outparams()
    ret = ll_fz_keep_imp_locked_outparams_fn(p, outparams)
    return ret, outparams.refs

def ll_fz_lookup_base14_font(name):
    """
    Wrapper for out-params of fz_lookup_base14_font().
    Returns: const unsigned char *, int len
    """
    outparams = ll_fz_lookup_base14_font_outparams()
    ret = ll_fz_lookup_base14_font_outparams_fn(name, outparams)
    return ret, outparams.len

def fz_lookup_base14_font_outparams_fn(name):
    """
    Class-aware helper for out-params of fz_lookup_base14_font() [fz_lookup_base14_font()].
    """
    ret, len = ll_fz_lookup_base14_font(name)
    return ret, len

fz_lookup_base14_font = fz_lookup_base14_font_outparams_fn


def ll_fz_lookup_builtin_font(name, bold, italic):
    """
    Wrapper for out-params of fz_lookup_builtin_font().
    Returns: const unsigned char *, int len
    """
    outparams = ll_fz_lookup_builtin_font_outparams()
    ret = ll_fz_lookup_builtin_font_outparams_fn(name, bold, italic, outparams)
    return ret, outparams.len

def fz_lookup_builtin_font_outparams_fn(name, bold, italic):
    """
    Class-aware helper for out-params of fz_lookup_builtin_font() [fz_lookup_builtin_font()].
    """
    ret, len = ll_fz_lookup_builtin_font(name, bold, italic)
    return ret, len

fz_lookup_builtin_font = fz_lookup_builtin_font_outparams_fn


def ll_fz_lookup_cjk_font(ordering):
    """
    Wrapper for out-params of fz_lookup_cjk_font().
    Returns: const unsigned char *, int len, int index
    """
    outparams = ll_fz_lookup_cjk_font_outparams()
    ret = ll_fz_lookup_cjk_font_outparams_fn(ordering, outparams)
    return ret, outparams.len, outparams.index

def fz_lookup_cjk_font_outparams_fn(ordering):
    """
    Class-aware helper for out-params of fz_lookup_cjk_font() [fz_lookup_cjk_font()].
    """
    ret, len, index = ll_fz_lookup_cjk_font(ordering)
    return ret, len, index

fz_lookup_cjk_font = fz_lookup_cjk_font_outparams_fn


def ll_fz_lookup_cjk_font_by_language(lang):
    """
    Wrapper for out-params of fz_lookup_cjk_font_by_language().
    Returns: const unsigned char *, int len, int subfont
    """
    outparams = ll_fz_lookup_cjk_font_by_language_outparams()
    ret = ll_fz_lookup_cjk_font_by_language_outparams_fn(lang, outparams)
    return ret, outparams.len, outparams.subfont

def fz_lookup_cjk_font_by_language_outparams_fn(lang):
    """
    Class-aware helper for out-params of fz_lookup_cjk_font_by_language() [fz_lookup_cjk_font_by_language()].
    """
    ret, len, subfont = ll_fz_lookup_cjk_font_by_language(lang)
    return ret, len, subfont

fz_lookup_cjk_font_by_language = fz_lookup_cjk_font_by_language_outparams_fn


def ll_fz_lookup_noto_emoji_font():
    """
    Wrapper for out-params of fz_lookup_noto_emoji_font().
    Returns: const unsigned char *, int len
    """
    outparams = ll_fz_lookup_noto_emoji_font_outparams()
    ret = ll_fz_lookup_noto_emoji_font_outparams_fn(outparams)
    return ret, outparams.len

def fz_lookup_noto_emoji_font_outparams_fn():
    """
    Class-aware helper for out-params of fz_lookup_noto_emoji_font() [fz_lookup_noto_emoji_font()].
    """
    ret, len = ll_fz_lookup_noto_emoji_font()
    return ret, len

fz_lookup_noto_emoji_font = fz_lookup_noto_emoji_font_outparams_fn


def ll_fz_lookup_noto_font(script, lang):
    """
    Wrapper for out-params of fz_lookup_noto_font().
    Returns: const unsigned char *, int len, int subfont
    """
    outparams = ll_fz_lookup_noto_font_outparams()
    ret = ll_fz_lookup_noto_font_outparams_fn(script, lang, outparams)
    return ret, outparams.len, outparams.subfont

def fz_lookup_noto_font_outparams_fn(script, lang):
    """
    Class-aware helper for out-params of fz_lookup_noto_font() [fz_lookup_noto_font()].
    """
    ret, len, subfont = ll_fz_lookup_noto_font(script, lang)
    return ret, len, subfont

fz_lookup_noto_font = fz_lookup_noto_font_outparams_fn


def ll_fz_lookup_noto_math_font():
    """
    Wrapper for out-params of fz_lookup_noto_math_font().
    Returns: const unsigned char *, int len
    """
    outparams = ll_fz_lookup_noto_math_font_outparams()
    ret = ll_fz_lookup_noto_math_font_outparams_fn(outparams)
    return ret, outparams.len

def fz_lookup_noto_math_font_outparams_fn():
    """
    Class-aware helper for out-params of fz_lookup_noto_math_font() [fz_lookup_noto_math_font()].
    """
    ret, len = ll_fz_lookup_noto_math_font()
    return ret, len

fz_lookup_noto_math_font = fz_lookup_noto_math_font_outparams_fn


def ll_fz_lookup_noto_music_font():
    """
    Wrapper for out-params of fz_lookup_noto_music_font().
    Returns: const unsigned char *, int len
    """
    outparams = ll_fz_lookup_noto_music_font_outparams()
    ret = ll_fz_lookup_noto_music_font_outparams_fn(outparams)
    return ret, outparams.len

def fz_lookup_noto_music_font_outparams_fn():
    """
    Class-aware helper for out-params of fz_lookup_noto_music_font() [fz_lookup_noto_music_font()].
    """
    ret, len = ll_fz_lookup_noto_music_font()
    return ret, len

fz_lookup_noto_music_font = fz_lookup_noto_music_font_outparams_fn


def ll_fz_lookup_noto_symbol1_font():
    """
    Wrapper for out-params of fz_lookup_noto_symbol1_font().
    Returns: const unsigned char *, int len
    """
    outparams = ll_fz_lookup_noto_symbol1_font_outparams()
    ret = ll_fz_lookup_noto_symbol1_font_outparams_fn(outparams)
    return ret, outparams.len

def fz_lookup_noto_symbol1_font_outparams_fn():
    """
    Class-aware helper for out-params of fz_lookup_noto_symbol1_font() [fz_lookup_noto_symbol1_font()].
    """
    ret, len = ll_fz_lookup_noto_symbol1_font()
    return ret, len

fz_lookup_noto_symbol1_font = fz_lookup_noto_symbol1_font_outparams_fn


def ll_fz_lookup_noto_symbol2_font():
    """
    Wrapper for out-params of fz_lookup_noto_symbol2_font().
    Returns: const unsigned char *, int len
    """
    outparams = ll_fz_lookup_noto_symbol2_font_outparams()
    ret = ll_fz_lookup_noto_symbol2_font_outparams_fn(outparams)
    return ret, outparams.len

def fz_lookup_noto_symbol2_font_outparams_fn():
    """
    Class-aware helper for out-params of fz_lookup_noto_symbol2_font() [fz_lookup_noto_symbol2_font()].
    """
    ret, len = ll_fz_lookup_noto_symbol2_font()
    return ret, len

fz_lookup_noto_symbol2_font = fz_lookup_noto_symbol2_font_outparams_fn


def ll_fz_new_deflated_data(source, source_length, level):
    """
    Wrapper for out-params of fz_new_deflated_data().
    Returns: unsigned char *, size_t compressed_length
    """
    outparams = ll_fz_new_deflated_data_outparams()
    ret = ll_fz_new_deflated_data_outparams_fn(source, source_length, level, outparams)
    return ret, outparams.compressed_length

def fz_new_deflated_data_outparams_fn(source, source_length, level):
    """
    Class-aware helper for out-params of fz_new_deflated_data() [fz_new_deflated_data()].
    """
    ret, compressed_length = ll_fz_new_deflated_data(source, source_length, level)
    return ret, compressed_length

fz_new_deflated_data = fz_new_deflated_data_outparams_fn


def ll_fz_new_deflated_data_from_buffer(buffer, level):
    """
    Wrapper for out-params of fz_new_deflated_data_from_buffer().
    Returns: unsigned char *, size_t compressed_length
    """
    outparams = ll_fz_new_deflated_data_from_buffer_outparams()
    ret = ll_fz_new_deflated_data_from_buffer_outparams_fn(buffer, level, outparams)
    return ret, outparams.compressed_length

def fz_new_deflated_data_from_buffer_outparams_fn(buffer, level):
    """
    Class-aware helper for out-params of fz_new_deflated_data_from_buffer() [fz_new_deflated_data_from_buffer()].
    """
    ret, compressed_length = ll_fz_new_deflated_data_from_buffer(buffer.m_internal, level)
    return ret, compressed_length

fz_new_deflated_data_from_buffer = fz_new_deflated_data_from_buffer_outparams_fn


def ll_fz_new_display_list_from_svg(buf, base_uri, zip):
    """
    Wrapper for out-params of fz_new_display_list_from_svg().
    Returns: fz_display_list *, float w, float h
    """
    outparams = ll_fz_new_display_list_from_svg_outparams()
    ret = ll_fz_new_display_list_from_svg_outparams_fn(buf, base_uri, zip, outparams)
    return ret, outparams.w, outparams.h

def fz_new_display_list_from_svg_outparams_fn(buf, base_uri, zip):
    """
    Class-aware helper for out-params of fz_new_display_list_from_svg() [fz_new_display_list_from_svg()].
    """
    ret, w, h = ll_fz_new_display_list_from_svg(buf.m_internal, base_uri, zip.m_internal)
    return FzDisplayList(ret), w, h

fz_new_display_list_from_svg = fz_new_display_list_from_svg_outparams_fn


def ll_fz_new_display_list_from_svg_xml(xmldoc, xml, base_uri, zip):
    """
    Wrapper for out-params of fz_new_display_list_from_svg_xml().
    Returns: fz_display_list *, float w, float h
    """
    outparams = ll_fz_new_display_list_from_svg_xml_outparams()
    ret = ll_fz_new_display_list_from_svg_xml_outparams_fn(xmldoc, xml, base_uri, zip, outparams)
    return ret, outparams.w, outparams.h

def fz_new_display_list_from_svg_xml_outparams_fn(xmldoc, xml, base_uri, zip):
    """
    Class-aware helper for out-params of fz_new_display_list_from_svg_xml() [fz_new_display_list_from_svg_xml()].
    """
    ret, w, h = ll_fz_new_display_list_from_svg_xml(xmldoc.m_internal, xml.m_internal, base_uri, zip.m_internal)
    return FzDisplayList(ret), w, h

fz_new_display_list_from_svg_xml = fz_new_display_list_from_svg_xml_outparams_fn


def ll_fz_new_draw_device_with_options(options, mediabox):
    """
    Wrapper for out-params of fz_new_draw_device_with_options().
    Returns: fz_device *, ::fz_pixmap *pixmap
    """
    outparams = ll_fz_new_draw_device_with_options_outparams()
    ret = ll_fz_new_draw_device_with_options_outparams_fn(options, mediabox, outparams)
    return ret, outparams.pixmap

def fz_new_draw_device_with_options_outparams_fn(options, mediabox):
    """
    Class-aware helper for out-params of fz_new_draw_device_with_options() [fz_new_draw_device_with_options()].
    """
    ret, pixmap = ll_fz_new_draw_device_with_options(options.internal(), mediabox.internal())
    return FzDevice(ret), FzPixmap( pixmap)

fz_new_draw_device_with_options = fz_new_draw_device_with_options_outparams_fn


def ll_fz_new_image_from_compressed_buffer(w, h, bpc, colorspace, xres, yres, interpolate, imagemask, buffer, mask):
    """
    Wrapper for out-params of fz_new_image_from_compressed_buffer().
    Returns: fz_image *, float decode, int colorkey
    """
    outparams = ll_fz_new_image_from_compressed_buffer_outparams()
    ret = ll_fz_new_image_from_compressed_buffer_outparams_fn(w, h, bpc, colorspace, xres, yres, interpolate, imagemask, buffer, mask, outparams)
    return ret, outparams.decode, outparams.colorkey

def fz_new_image_from_compressed_buffer_outparams_fn(w, h, bpc, colorspace, xres, yres, interpolate, imagemask, buffer, mask):
    """
    Class-aware helper for out-params of fz_new_image_from_compressed_buffer() [fz_new_image_from_compressed_buffer()].
    """
    ret, decode, colorkey = ll_fz_new_image_from_compressed_buffer(w, h, bpc, colorspace.m_internal, xres, yres, interpolate, imagemask, buffer.m_internal, mask.m_internal)
    return FzImage(ret), decode, colorkey

fz_new_image_from_compressed_buffer = fz_new_image_from_compressed_buffer_outparams_fn


def ll_fz_new_image_of_size(w, h, bpc, colorspace, xres, yres, interpolate, imagemask, mask, size, get_pixmap, get_size, drop):
    """
    Wrapper for out-params of fz_new_image_of_size().
    Returns: fz_image *, float decode, int colorkey
    """
    outparams = ll_fz_new_image_of_size_outparams()
    ret = ll_fz_new_image_of_size_outparams_fn(w, h, bpc, colorspace, xres, yres, interpolate, imagemask, mask, size, get_pixmap, get_size, drop, outparams)
    return ret, outparams.decode, outparams.colorkey

def fz_new_image_of_size_outparams_fn(w, h, bpc, colorspace, xres, yres, interpolate, imagemask, mask, size, get_pixmap, get_size, drop):
    """
    Class-aware helper for out-params of fz_new_image_of_size() [fz_new_image_of_size()].
    """
    ret, decode, colorkey = ll_fz_new_image_of_size(w, h, bpc, colorspace.m_internal, xres, yres, interpolate, imagemask, mask.m_internal, size, get_pixmap, get_size, drop)
    return FzImage(ret), decode, colorkey

fz_new_image_of_size = fz_new_image_of_size_outparams_fn


def ll_fz_new_svg_device_with_id(out, page_width, page_height, text_format, reuse_images):
    """
    Wrapper for out-params of fz_new_svg_device_with_id().
    Returns: fz_device *, int id
    """
    outparams = ll_fz_new_svg_device_with_id_outparams()
    ret = ll_fz_new_svg_device_with_id_outparams_fn(out, page_width, page_height, text_format, reuse_images, outparams)
    return ret, outparams.id

def fz_new_svg_device_with_id_outparams_fn(out, page_width, page_height, text_format, reuse_images):
    """
    Class-aware helper for out-params of fz_new_svg_device_with_id() [fz_new_svg_device_with_id()].
    """
    ret, id = ll_fz_new_svg_device_with_id(out.m_internal, page_width, page_height, text_format, reuse_images)
    return FzDevice(ret), id

fz_new_svg_device_with_id = fz_new_svg_device_with_id_outparams_fn


def ll_fz_new_test_device(threshold, options, passthrough):
    """
    Wrapper for out-params of fz_new_test_device().
    Returns: fz_device *, int is_color
    """
    outparams = ll_fz_new_test_device_outparams()
    ret = ll_fz_new_test_device_outparams_fn(threshold, options, passthrough, outparams)
    return ret, outparams.is_color

def fz_new_test_device_outparams_fn(threshold, options, passthrough):
    """
    Class-aware helper for out-params of fz_new_test_device() [fz_new_test_device()].
    """
    ret, is_color = ll_fz_new_test_device(threshold, options, passthrough.m_internal)
    return FzDevice(ret), is_color

fz_new_test_device = fz_new_test_device_outparams_fn


def ll_fz_open_image_decomp_stream(arg_0, arg_1):
    """
    Wrapper for out-params of fz_open_image_decomp_stream().
    Returns: fz_stream *, int l2factor
    """
    outparams = ll_fz_open_image_decomp_stream_outparams()
    ret = ll_fz_open_image_decomp_stream_outparams_fn(arg_0, arg_1, outparams)
    return ret, outparams.l2factor

def fz_open_image_decomp_stream_outparams_fn(arg_0, arg_1):
    """
    Class-aware helper for out-params of fz_open_image_decomp_stream() [fz_open_image_decomp_stream()].
    """
    ret, l2factor = ll_fz_open_image_decomp_stream(arg_0.m_internal, arg_1.m_internal)
    return FzStream(ret), l2factor

fz_open_image_decomp_stream = fz_open_image_decomp_stream_outparams_fn


def ll_fz_open_image_decomp_stream_from_buffer(arg_0):
    """
    Wrapper for out-params of fz_open_image_decomp_stream_from_buffer().
    Returns: fz_stream *, int l2factor
    """
    outparams = ll_fz_open_image_decomp_stream_from_buffer_outparams()
    ret = ll_fz_open_image_decomp_stream_from_buffer_outparams_fn(arg_0, outparams)
    return ret, outparams.l2factor

def fz_open_image_decomp_stream_from_buffer_outparams_fn(arg_0):
    """
    Class-aware helper for out-params of fz_open_image_decomp_stream_from_buffer() [fz_open_image_decomp_stream_from_buffer()].
    """
    ret, l2factor = ll_fz_open_image_decomp_stream_from_buffer(arg_0.m_internal)
    return FzStream(ret), l2factor

fz_open_image_decomp_stream_from_buffer = fz_open_image_decomp_stream_from_buffer_outparams_fn


def ll_fz_page_presentation(page, transition):
    """
    Wrapper for out-params of fz_page_presentation().
    Returns: fz_transition *, float duration
    """
    outparams = ll_fz_page_presentation_outparams()
    ret = ll_fz_page_presentation_outparams_fn(page, transition, outparams)
    return ret, outparams.duration

def fz_page_presentation_outparams_fn(page, transition):
    """
    Class-aware helper for out-params of fz_page_presentation() [fz_page_presentation()].
    """
    ret, duration = ll_fz_page_presentation(page.m_internal, transition.internal())
    return FzTransition(ret), duration

fz_page_presentation = fz_page_presentation_outparams_fn


def ll_fz_paint_shade(shade, override_cs, ctm, dest, color_params, bbox, eop):
    """
    Wrapper for out-params of fz_paint_shade().
    Returns: ::fz_shade_color_cache *cache
    """
    outparams = ll_fz_paint_shade_outparams()
    ret = ll_fz_paint_shade_outparams_fn(shade, override_cs, ctm, dest, color_params, bbox, eop, outparams)
    return outparams.cache

def fz_paint_shade_outparams_fn(shade, override_cs, ctm, dest, color_params, bbox, eop):
    """
    Class-aware helper for out-params of fz_paint_shade() [fz_paint_shade()].
    """
    cache = ll_fz_paint_shade(shade.m_internal, override_cs.m_internal, ctm.internal(), dest.m_internal, color_params.internal(), bbox.internal(), eop.m_internal)
    return FzShadeColorCache(ll_fz_keep_shade_color_cache( cache))

fz_paint_shade = fz_paint_shade_outparams_fn


def ll_fz_parse_page_range(s, n):
    """
    Wrapper for out-params of fz_parse_page_range().
    Returns: const char *, int a, int b
    """
    outparams = ll_fz_parse_page_range_outparams()
    ret = ll_fz_parse_page_range_outparams_fn(s, n, outparams)
    return ret, outparams.a, outparams.b

def fz_parse_page_range_outparams_fn(s, n):
    """
    Class-aware helper for out-params of fz_parse_page_range() [fz_parse_page_range()].
    """
    ret, a, b = ll_fz_parse_page_range(s, n)
    return ret, a, b

fz_parse_page_range = fz_parse_page_range_outparams_fn


def ll_fz_read_best(stm, initial, worst_case):
    """
    Wrapper for out-params of fz_read_best().
    Returns: fz_buffer *, int truncated
    """
    outparams = ll_fz_read_best_outparams()
    ret = ll_fz_read_best_outparams_fn(stm, initial, worst_case, outparams)
    return ret, outparams.truncated

def fz_read_best_outparams_fn(stm, initial, worst_case):
    """
    Class-aware helper for out-params of fz_read_best() [fz_read_best()].
    """
    ret, truncated = ll_fz_read_best(stm.m_internal, initial, worst_case)
    return FzBuffer(ret), truncated

fz_read_best = fz_read_best_outparams_fn


def ll_fz_resolve_link(doc, uri):
    """
    Wrapper for out-params of fz_resolve_link().
    Returns: fz_location, float xp, float yp
    """
    outparams = ll_fz_resolve_link_outparams()
    ret = ll_fz_resolve_link_outparams_fn(doc, uri, outparams)
    return ret, outparams.xp, outparams.yp

def fz_resolve_link_outparams_fn(doc, uri):
    """
    Class-aware helper for out-params of fz_resolve_link() [fz_resolve_link()].
    """
    ret, xp, yp = ll_fz_resolve_link(doc.m_internal, uri)
    return FzLocation(ret), xp, yp

fz_resolve_link = fz_resolve_link_outparams_fn


def ll_fz_search_chapter_page_number(doc, chapter, page, needle, hit_bbox, hit_max):
    """
    Wrapper for out-params of fz_search_chapter_page_number().
    Returns: int, int hit_mark
    """
    outparams = ll_fz_search_chapter_page_number_outparams()
    ret = ll_fz_search_chapter_page_number_outparams_fn(doc, chapter, page, needle, hit_bbox, hit_max, outparams)
    return ret, outparams.hit_mark

def fz_search_chapter_page_number_outparams_fn(doc, chapter, page, needle, hit_bbox, hit_max):
    """
    Class-aware helper for out-params of fz_search_chapter_page_number() [fz_search_chapter_page_number()].
    """
    ret, hit_mark = ll_fz_search_chapter_page_number(doc.m_internal, chapter, page, needle, hit_bbox.internal(), hit_max)
    return ret, hit_mark

fz_search_chapter_page_number = fz_search_chapter_page_number_outparams_fn


def ll_fz_search_display_list(list, needle, hit_bbox, hit_max):
    """
    Wrapper for out-params of fz_search_display_list().
    Returns: int, int hit_mark
    """
    outparams = ll_fz_search_display_list_outparams()
    ret = ll_fz_search_display_list_outparams_fn(list, needle, hit_bbox, hit_max, outparams)
    return ret, outparams.hit_mark

def fz_search_display_list_outparams_fn(list, needle, hit_bbox, hit_max):
    """
    Class-aware helper for out-params of fz_search_display_list() [fz_search_display_list()].
    """
    ret, hit_mark = ll_fz_search_display_list(list.m_internal, needle, hit_bbox.internal(), hit_max)
    return ret, hit_mark

fz_search_display_list = fz_search_display_list_outparams_fn


def ll_fz_search_page(page, needle, hit_bbox, hit_max):
    """
    Wrapper for out-params of fz_search_page().
    Returns: int, int hit_mark
    """
    outparams = ll_fz_search_page_outparams()
    ret = ll_fz_search_page_outparams_fn(page, needle, hit_bbox, hit_max, outparams)
    return ret, outparams.hit_mark

def fz_search_page_outparams_fn(page, needle, hit_bbox, hit_max):
    """
    Class-aware helper for out-params of fz_search_page() [fz_search_page()].
    """
    ret, hit_mark = ll_fz_search_page(page.m_internal, needle, hit_bbox.internal(), hit_max)
    return ret, hit_mark

fz_search_page = fz_search_page_outparams_fn


def ll_fz_search_page_number(doc, number, needle, hit_bbox, hit_max):
    """
    Wrapper for out-params of fz_search_page_number().
    Returns: int, int hit_mark
    """
    outparams = ll_fz_search_page_number_outparams()
    ret = ll_fz_search_page_number_outparams_fn(doc, number, needle, hit_bbox, hit_max, outparams)
    return ret, outparams.hit_mark

def fz_search_page_number_outparams_fn(doc, number, needle, hit_bbox, hit_max):
    """
    Class-aware helper for out-params of fz_search_page_number() [fz_search_page_number()].
    """
    ret, hit_mark = ll_fz_search_page_number(doc.m_internal, number, needle, hit_bbox.internal(), hit_max)
    return ret, hit_mark

fz_search_page_number = fz_search_page_number_outparams_fn


def ll_fz_search_stext_page(text, needle, hit_bbox, hit_max):
    """
    Wrapper for out-params of fz_search_stext_page().
    Returns: int, int hit_mark
    """
    outparams = ll_fz_search_stext_page_outparams()
    ret = ll_fz_search_stext_page_outparams_fn(text, needle, hit_bbox, hit_max, outparams)
    return ret, outparams.hit_mark

def fz_search_stext_page_outparams_fn(text, needle, hit_bbox, hit_max):
    """
    Class-aware helper for out-params of fz_search_stext_page() [fz_search_stext_page()].
    """
    ret, hit_mark = ll_fz_search_stext_page(text.m_internal, needle, hit_bbox.internal(), hit_max)
    return ret, hit_mark

fz_search_stext_page = fz_search_stext_page_outparams_fn


def ll_fz_separation_equivalent(seps, idx, dst_cs, prf, color_params):
    """
    Wrapper for out-params of fz_separation_equivalent().
    Returns: float dst_color
    """
    outparams = ll_fz_separation_equivalent_outparams()
    ret = ll_fz_separation_equivalent_outparams_fn(seps, idx, dst_cs, prf, color_params, outparams)
    return outparams.dst_color

def fz_separation_equivalent_outparams_fn(seps, idx, dst_cs, prf, color_params):
    """
    Class-aware helper for out-params of fz_separation_equivalent() [fz_separation_equivalent()].
    """
    dst_color = ll_fz_separation_equivalent(seps.m_internal, idx, dst_cs.m_internal, prf.m_internal, color_params.internal())
    return dst_color

fz_separation_equivalent = fz_separation_equivalent_outparams_fn


def ll_fz_store_scavenge(size):
    """
    Wrapper for out-params of fz_store_scavenge().
    Returns: int, int phase
    """
    outparams = ll_fz_store_scavenge_outparams()
    ret = ll_fz_store_scavenge_outparams_fn(size, outparams)
    return ret, outparams.phase

def fz_store_scavenge_outparams_fn(size):
    """
    Class-aware helper for out-params of fz_store_scavenge() [fz_store_scavenge()].
    """
    ret, phase = ll_fz_store_scavenge(size)
    return ret, phase

fz_store_scavenge = fz_store_scavenge_outparams_fn


def ll_fz_store_scavenge_external(size):
    """
    Wrapper for out-params of fz_store_scavenge_external().
    Returns: int, int phase
    """
    outparams = ll_fz_store_scavenge_external_outparams()
    ret = ll_fz_store_scavenge_external_outparams_fn(size, outparams)
    return ret, outparams.phase

def fz_store_scavenge_external_outparams_fn(size):
    """
    Class-aware helper for out-params of fz_store_scavenge_external() [fz_store_scavenge_external()].
    """
    ret, phase = ll_fz_store_scavenge_external(size)
    return ret, phase

fz_store_scavenge_external = fz_store_scavenge_external_outparams_fn


def ll_fz_strsep(delim):
    """
    Wrapper for out-params of fz_strsep().
    Returns: char *, char *stringp
    """
    outparams = ll_fz_strsep_outparams()
    ret = ll_fz_strsep_outparams_fn(delim, outparams)
    return ret, outparams.stringp

def fz_strsep_outparams_fn(delim):
    """
    Class-aware helper for out-params of fz_strsep() [fz_strsep()].
    """
    ret, stringp = ll_fz_strsep(delim)
    return ret, stringp

fz_strsep = fz_strsep_outparams_fn


def ll_fz_strtof(s):
    """
    Wrapper for out-params of fz_strtof().
    Returns: float, char *es
    """
    outparams = ll_fz_strtof_outparams()
    ret = ll_fz_strtof_outparams_fn(s, outparams)
    return ret, outparams.es

def fz_strtof_outparams_fn(s):
    """
    Class-aware helper for out-params of fz_strtof() [fz_strtof()].
    """
    ret, es = ll_fz_strtof(s)
    return ret, es

fz_strtof = fz_strtof_outparams_fn


def ll_fz_warning_callback():
    """
    Wrapper for out-params of fz_warning_callback().
    Returns: fz_warning_cb *, void *user
    """
    outparams = ll_fz_warning_callback_outparams()
    ret = ll_fz_warning_callback_outparams_fn(outparams)
    return ret, outparams.user

def fz_warning_callback_outparams_fn():
    """
    Class-aware helper for out-params of fz_warning_callback() [fz_warning_callback()].
    """
    ret, user = ll_fz_warning_callback()
    return ret, user

fz_warning_callback = fz_warning_callback_outparams_fn


def ll_pdf_annot_MK_BC(annot, color):
    """
    Wrapper for out-params of pdf_annot_MK_BC().
    Returns: int n
    """
    outparams = ll_pdf_annot_MK_BC_outparams()
    ret = ll_pdf_annot_MK_BC_outparams_fn(annot, color, outparams)
    return outparams.n

def pdf_annot_MK_BC_outparams_fn(annot, color):
    """
    Class-aware helper for out-params of pdf_annot_MK_BC() [pdf_annot_MK_BC()].
    """
    n = ll_pdf_annot_MK_BC(annot.m_internal, color)
    return n

pdf_annot_MK_BC = pdf_annot_MK_BC_outparams_fn


def ll_pdf_annot_MK_BG(annot, color):
    """
    Wrapper for out-params of pdf_annot_MK_BG().
    Returns: int n
    """
    outparams = ll_pdf_annot_MK_BG_outparams()
    ret = ll_pdf_annot_MK_BG_outparams_fn(annot, color, outparams)
    return outparams.n

def pdf_annot_MK_BG_outparams_fn(annot, color):
    """
    Class-aware helper for out-params of pdf_annot_MK_BG() [pdf_annot_MK_BG()].
    """
    n = ll_pdf_annot_MK_BG(annot.m_internal, color)
    return n

pdf_annot_MK_BG = pdf_annot_MK_BG_outparams_fn


def ll_pdf_annot_color(annot, color):
    """
    Wrapper for out-params of pdf_annot_color().
    Returns: int n
    """
    outparams = ll_pdf_annot_color_outparams()
    ret = ll_pdf_annot_color_outparams_fn(annot, color, outparams)
    return outparams.n

def pdf_annot_color_outparams_fn(annot, color):
    """
    Class-aware helper for out-params of pdf_annot_color() [pdf_annot_color()].
    """
    n = ll_pdf_annot_color(annot.m_internal, color)
    return n

pdf_annot_color = pdf_annot_color_outparams_fn


def ll_pdf_annot_default_appearance(annot, color):
    """
    Wrapper for out-params of pdf_annot_default_appearance().
    Returns: const char *font, float size, int n
    """
    outparams = ll_pdf_annot_default_appearance_outparams()
    ret = ll_pdf_annot_default_appearance_outparams_fn(annot, color, outparams)
    return outparams.font, outparams.size, outparams.n

def pdf_annot_default_appearance_outparams_fn(annot, color):
    """
    Class-aware helper for out-params of pdf_annot_default_appearance() [pdf_annot_default_appearance()].
    """
    font, size, n = ll_pdf_annot_default_appearance(annot.m_internal, color)
    return font, size, n

pdf_annot_default_appearance = pdf_annot_default_appearance_outparams_fn


def ll_pdf_annot_interior_color(annot, color):
    """
    Wrapper for out-params of pdf_annot_interior_color().
    Returns: int n
    """
    outparams = ll_pdf_annot_interior_color_outparams()
    ret = ll_pdf_annot_interior_color_outparams_fn(annot, color, outparams)
    return outparams.n

def pdf_annot_interior_color_outparams_fn(annot, color):
    """
    Class-aware helper for out-params of pdf_annot_interior_color() [pdf_annot_interior_color()].
    """
    n = ll_pdf_annot_interior_color(annot.m_internal, color)
    return n

pdf_annot_interior_color = pdf_annot_interior_color_outparams_fn


def ll_pdf_annot_line_ending_styles(annot):
    """
    Wrapper for out-params of pdf_annot_line_ending_styles().
    Returns: enum pdf_line_ending start_style, enum pdf_line_ending end_style
    """
    outparams = ll_pdf_annot_line_ending_styles_outparams()
    ret = ll_pdf_annot_line_ending_styles_outparams_fn(annot, outparams)
    return outparams.start_style, outparams.end_style

def pdf_annot_line_ending_styles_outparams_fn(annot):
    """
    Class-aware helper for out-params of pdf_annot_line_ending_styles() [pdf_annot_line_ending_styles()].
    """
    start_style, end_style = ll_pdf_annot_line_ending_styles(annot.m_internal)
    return start_style, end_style

pdf_annot_line_ending_styles = pdf_annot_line_ending_styles_outparams_fn


def ll_pdf_array_get_string(array, index):
    """
    Wrapper for out-params of pdf_array_get_string().
    Returns: const char *, size_t sizep
    """
    outparams = ll_pdf_array_get_string_outparams()
    ret = ll_pdf_array_get_string_outparams_fn(array, index, outparams)
    return ret, outparams.sizep

def pdf_array_get_string_outparams_fn(array, index):
    """
    Class-aware helper for out-params of pdf_array_get_string() [pdf_array_get_string()].
    """
    ret, sizep = ll_pdf_array_get_string(array.m_internal, index)
    return ret, sizep

pdf_array_get_string = pdf_array_get_string_outparams_fn


def ll_pdf_decode_cmap(cmap, s, e):
    """
    Wrapper for out-params of pdf_decode_cmap().
    Returns: int, unsigned int cpt
    """
    outparams = ll_pdf_decode_cmap_outparams()
    ret = ll_pdf_decode_cmap_outparams_fn(cmap, s, e, outparams)
    return ret, outparams.cpt

def pdf_decode_cmap_outparams_fn(cmap, s, e):
    """
    Class-aware helper for out-params of pdf_decode_cmap() [pdf_decode_cmap()].
    """
    ret, cpt = ll_pdf_decode_cmap(cmap.m_internal, s, e)
    return ret, cpt

pdf_decode_cmap = pdf_decode_cmap_outparams_fn


def ll_pdf_dict_get_put_drop(dict, key, val):
    """
    Wrapper for out-params of pdf_dict_get_put_drop().
    Returns: ::pdf_obj *old_val
    """
    outparams = ll_pdf_dict_get_put_drop_outparams()
    ret = ll_pdf_dict_get_put_drop_outparams_fn(dict, key, val, outparams)
    return outparams.old_val

def ll_pdf_dict_get_string(dict, key):
    """
    Wrapper for out-params of pdf_dict_get_string().
    Returns: const char *, size_t sizep
    """
    outparams = ll_pdf_dict_get_string_outparams()
    ret = ll_pdf_dict_get_string_outparams_fn(dict, key, outparams)
    return ret, outparams.sizep

def pdf_dict_get_string_outparams_fn(dict, key):
    """
    Class-aware helper for out-params of pdf_dict_get_string() [pdf_dict_get_string()].
    """
    ret, sizep = ll_pdf_dict_get_string(dict.m_internal, key.m_internal)
    return ret, sizep

pdf_dict_get_string = pdf_dict_get_string_outparams_fn


def ll_pdf_edit_text_field_value(widget, value, change):
    """
    Wrapper for out-params of pdf_edit_text_field_value().
    Returns: int, int selStart, int selEnd, char *newvalue
    """
    outparams = ll_pdf_edit_text_field_value_outparams()
    ret = ll_pdf_edit_text_field_value_outparams_fn(widget, value, change, outparams)
    return ret, outparams.selStart, outparams.selEnd, outparams.newvalue

def pdf_edit_text_field_value_outparams_fn(widget, value, change):
    """
    Class-aware helper for out-params of pdf_edit_text_field_value() [pdf_edit_text_field_value()].
    """
    ret, selStart, selEnd, newvalue = ll_pdf_edit_text_field_value(widget.m_internal, value, change)
    return ret, selStart, selEnd, newvalue

pdf_edit_text_field_value = pdf_edit_text_field_value_outparams_fn


def ll_pdf_eval_function(func, in_, inlen, outlen):
    """
    Wrapper for out-params of pdf_eval_function().
    Returns: float out
    """
    outparams = ll_pdf_eval_function_outparams()
    ret = ll_pdf_eval_function_outparams_fn(func, in_, inlen, outlen, outparams)
    return outparams.out

def pdf_eval_function_outparams_fn(func, in_, inlen, outlen):
    """
    Class-aware helper for out-params of pdf_eval_function() [pdf_eval_function()].
    """
    out = ll_pdf_eval_function(func.m_internal, in_, inlen, outlen)
    return out

pdf_eval_function = pdf_eval_function_outparams_fn


def ll_pdf_field_event_validate(doc, field, value):
    """
    Wrapper for out-params of pdf_field_event_validate().
    Returns: int, char *newvalue
    """
    outparams = ll_pdf_field_event_validate_outparams()
    ret = ll_pdf_field_event_validate_outparams_fn(doc, field, value, outparams)
    return ret, outparams.newvalue

def pdf_field_event_validate_outparams_fn(doc, field, value):
    """
    Class-aware helper for out-params of pdf_field_event_validate() [pdf_field_event_validate()].
    """
    ret, newvalue = ll_pdf_field_event_validate(doc.m_internal, field.m_internal, value)
    return ret, newvalue

pdf_field_event_validate = pdf_field_event_validate_outparams_fn


def ll_pdf_js_event_result_validate(js):
    """
    Wrapper for out-params of pdf_js_event_result_validate().
    Returns: int, char *newvalue
    """
    outparams = ll_pdf_js_event_result_validate_outparams()
    ret = ll_pdf_js_event_result_validate_outparams_fn(js, outparams)
    return ret, outparams.newvalue

def pdf_js_event_result_validate_outparams_fn(js):
    """
    Class-aware helper for out-params of pdf_js_event_result_validate() [pdf_js_event_result_validate()].
    """
    ret, newvalue = ll_pdf_js_event_result_validate(js.m_internal)
    return ret, newvalue

pdf_js_event_result_validate = pdf_js_event_result_validate_outparams_fn


def ll_pdf_js_execute(js, name, code):
    """
    Wrapper for out-params of pdf_js_execute().
    Returns: char *result
    """
    outparams = ll_pdf_js_execute_outparams()
    ret = ll_pdf_js_execute_outparams_fn(js, name, code, outparams)
    return outparams.result

def pdf_js_execute_outparams_fn(js, name, code):
    """
    Class-aware helper for out-params of pdf_js_execute() [pdf_js_execute()].
    """
    result = ll_pdf_js_execute(js.m_internal, name, code)
    return result

pdf_js_execute = pdf_js_execute_outparams_fn


def ll_pdf_load_encoding(encoding):
    """
    Wrapper for out-params of pdf_load_encoding().
    Returns: const char *estrings
    """
    outparams = ll_pdf_load_encoding_outparams()
    ret = ll_pdf_load_encoding_outparams_fn(encoding, outparams)
    return outparams.estrings

def pdf_load_encoding_outparams_fn(encoding):
    """
    Class-aware helper for out-params of pdf_load_encoding() [pdf_load_encoding()].
    """
    estrings = ll_pdf_load_encoding(encoding)
    return estrings

pdf_load_encoding = pdf_load_encoding_outparams_fn


def ll_pdf_load_to_unicode(doc, font, collection, cmapstm):
    """
    Wrapper for out-params of pdf_load_to_unicode().
    Returns: const char *strings
    """
    outparams = ll_pdf_load_to_unicode_outparams()
    ret = ll_pdf_load_to_unicode_outparams_fn(doc, font, collection, cmapstm, outparams)
    return outparams.strings

def pdf_load_to_unicode_outparams_fn(doc, font, collection, cmapstm):
    """
    Class-aware helper for out-params of pdf_load_to_unicode() [pdf_load_to_unicode()].
    """
    strings = ll_pdf_load_to_unicode(doc.m_internal, font.m_internal, collection, cmapstm.m_internal)
    return strings

pdf_load_to_unicode = pdf_load_to_unicode_outparams_fn


def ll_pdf_lookup_anchor(doc, name):
    """
    Wrapper for out-params of pdf_lookup_anchor().
    Returns: int, float xp, float yp
    """
    outparams = ll_pdf_lookup_anchor_outparams()
    ret = ll_pdf_lookup_anchor_outparams_fn(doc, name, outparams)
    return ret, outparams.xp, outparams.yp

def pdf_lookup_anchor_outparams_fn(doc, name):
    """
    Class-aware helper for out-params of pdf_lookup_anchor() [pdf_lookup_anchor()].
    """
    ret, xp, yp = ll_pdf_lookup_anchor(doc.m_internal, name)
    return ret, xp, yp

pdf_lookup_anchor = pdf_lookup_anchor_outparams_fn


def ll_pdf_lookup_cmap_full(cmap, cpt):
    """
    Wrapper for out-params of pdf_lookup_cmap_full().
    Returns: int, int out
    """
    outparams = ll_pdf_lookup_cmap_full_outparams()
    ret = ll_pdf_lookup_cmap_full_outparams_fn(cmap, cpt, outparams)
    return ret, outparams.out

def pdf_lookup_cmap_full_outparams_fn(cmap, cpt):
    """
    Class-aware helper for out-params of pdf_lookup_cmap_full() [pdf_lookup_cmap_full()].
    """
    ret, out = ll_pdf_lookup_cmap_full(cmap.m_internal, cpt)
    return ret, out

pdf_lookup_cmap_full = pdf_lookup_cmap_full_outparams_fn


def ll_pdf_lookup_substitute_font(mono, serif, bold, italic):
    """
    Wrapper for out-params of pdf_lookup_substitute_font().
    Returns: const unsigned char *, int len
    """
    outparams = ll_pdf_lookup_substitute_font_outparams()
    ret = ll_pdf_lookup_substitute_font_outparams_fn(mono, serif, bold, italic, outparams)
    return ret, outparams.len

def pdf_lookup_substitute_font_outparams_fn(mono, serif, bold, italic):
    """
    Class-aware helper for out-params of pdf_lookup_substitute_font() [pdf_lookup_substitute_font()].
    """
    ret, len = ll_pdf_lookup_substitute_font(mono, serif, bold, italic)
    return ret, len

pdf_lookup_substitute_font = pdf_lookup_substitute_font_outparams_fn


def ll_pdf_map_one_to_many(cmap, one, len):
    """
    Wrapper for out-params of pdf_map_one_to_many().
    Returns: int many
    """
    outparams = ll_pdf_map_one_to_many_outparams()
    ret = ll_pdf_map_one_to_many_outparams_fn(cmap, one, len, outparams)
    return outparams.many

def pdf_map_one_to_many_outparams_fn(cmap, one, len):
    """
    Class-aware helper for out-params of pdf_map_one_to_many() [pdf_map_one_to_many()].
    """
    many = ll_pdf_map_one_to_many(cmap.m_internal, one, len)
    return many

pdf_map_one_to_many = pdf_map_one_to_many_outparams_fn


def ll_pdf_obj_memo(obj, bit):
    """
    Wrapper for out-params of pdf_obj_memo().
    Returns: int, int memo
    """
    outparams = ll_pdf_obj_memo_outparams()
    ret = ll_pdf_obj_memo_outparams_fn(obj, bit, outparams)
    return ret, outparams.memo

def pdf_obj_memo_outparams_fn(obj, bit):
    """
    Class-aware helper for out-params of pdf_obj_memo() [pdf_obj_memo()].
    """
    ret, memo = ll_pdf_obj_memo(obj.m_internal, bit)
    return ret, memo

pdf_obj_memo = pdf_obj_memo_outparams_fn


def ll_pdf_page_presentation(page, transition):
    """
    Wrapper for out-params of pdf_page_presentation().
    Returns: fz_transition *, float duration
    """
    outparams = ll_pdf_page_presentation_outparams()
    ret = ll_pdf_page_presentation_outparams_fn(page, transition, outparams)
    return ret, outparams.duration

def pdf_page_presentation_outparams_fn(page, transition):
    """
    Class-aware helper for out-params of pdf_page_presentation() [pdf_page_presentation()].
    """
    ret, duration = ll_pdf_page_presentation(page.m_internal, transition.internal())
    return FzTransition(ret), duration

pdf_page_presentation = pdf_page_presentation_outparams_fn


def ll_pdf_page_write(doc, mediabox):
    """
    Wrapper for out-params of pdf_page_write().
    Returns: fz_device *, ::pdf_obj *presources, ::fz_buffer *pcontents
    """
    outparams = ll_pdf_page_write_outparams()
    ret = ll_pdf_page_write_outparams_fn(doc, mediabox, outparams)
    return ret, outparams.presources, outparams.pcontents

def pdf_page_write_outparams_fn(doc, mediabox):
    """
    Class-aware helper for out-params of pdf_page_write() [pdf_page_write()].
    """
    ret, presources, pcontents = ll_pdf_page_write(doc.m_internal, mediabox.internal())
    return FzDevice(ret), PdfObj( presources), FzBuffer( pcontents)

pdf_page_write = pdf_page_write_outparams_fn


def ll_pdf_parse_default_appearance(da, color):
    """
    Wrapper for out-params of pdf_parse_default_appearance().
    Returns: const char *font, float size, int n
    """
    outparams = ll_pdf_parse_default_appearance_outparams()
    ret = ll_pdf_parse_default_appearance_outparams_fn(da, color, outparams)
    return outparams.font, outparams.size, outparams.n

def pdf_parse_default_appearance_outparams_fn(da, color):
    """
    Class-aware helper for out-params of pdf_parse_default_appearance() [pdf_parse_default_appearance()].
    """
    font, size, n = ll_pdf_parse_default_appearance(da, color)
    return font, size, n

pdf_parse_default_appearance = pdf_parse_default_appearance_outparams_fn


def ll_pdf_parse_ind_obj(doc, f):
    """
    Wrapper for out-params of pdf_parse_ind_obj().
    Returns: pdf_obj *, int num, int gen, int64_t stm_ofs, int try_repair
    """
    outparams = ll_pdf_parse_ind_obj_outparams()
    ret = ll_pdf_parse_ind_obj_outparams_fn(doc, f, outparams)
    return ret, outparams.num, outparams.gen, outparams.stm_ofs, outparams.try_repair

def pdf_parse_ind_obj_outparams_fn(doc, f):
    """
    Class-aware helper for out-params of pdf_parse_ind_obj() [pdf_parse_ind_obj()].
    """
    ret, num, gen, stm_ofs, try_repair = ll_pdf_parse_ind_obj(doc.m_internal, f.m_internal)
    return PdfObj(ret), num, gen, stm_ofs, try_repair

pdf_parse_ind_obj = pdf_parse_ind_obj_outparams_fn


def ll_pdf_parse_journal_obj(doc, stm):
    """
    Wrapper for out-params of pdf_parse_journal_obj().
    Returns: pdf_obj *, int onum, ::fz_buffer *ostm, int newobj
    """
    outparams = ll_pdf_parse_journal_obj_outparams()
    ret = ll_pdf_parse_journal_obj_outparams_fn(doc, stm, outparams)
    return ret, outparams.onum, outparams.ostm, outparams.newobj

def pdf_parse_journal_obj_outparams_fn(doc, stm):
    """
    Class-aware helper for out-params of pdf_parse_journal_obj() [pdf_parse_journal_obj()].
    """
    ret, onum, ostm, newobj = ll_pdf_parse_journal_obj(doc.m_internal, stm.m_internal)
    return PdfObj(ret), onum, FzBuffer( ostm), newobj

pdf_parse_journal_obj = pdf_parse_journal_obj_outparams_fn


def ll_pdf_process_contents(proc, doc, obj, res, cookie):
    """
    Wrapper for out-params of pdf_process_contents().
    Returns: ::pdf_obj *out_res
    """
    outparams = ll_pdf_process_contents_outparams()
    ret = ll_pdf_process_contents_outparams_fn(proc, doc, obj, res, cookie, outparams)
    return outparams.out_res

def pdf_process_contents_outparams_fn(proc, doc, obj, res, cookie):
    """
    Class-aware helper for out-params of pdf_process_contents() [pdf_process_contents()].
    """
    out_res = ll_pdf_process_contents(proc.m_internal, doc.m_internal, obj.m_internal, res.m_internal, cookie.m_internal)
    return PdfObj(ll_pdf_keep_obj( out_res))

pdf_process_contents = pdf_process_contents_outparams_fn


def ll_pdf_repair_obj(doc, buf):
    """
    Wrapper for out-params of pdf_repair_obj().
    Returns: int, int64_t stmofsp, int64_t stmlenp, ::pdf_obj *encrypt, ::pdf_obj *id, ::pdf_obj *page, int64_t tmpofs, ::pdf_obj *root
    """
    outparams = ll_pdf_repair_obj_outparams()
    ret = ll_pdf_repair_obj_outparams_fn(doc, buf, outparams)
    return ret, outparams.stmofsp, outparams.stmlenp, outparams.encrypt, outparams.id, outparams.page, outparams.tmpofs, outparams.root

def pdf_repair_obj_outparams_fn(doc, buf):
    """
    Class-aware helper for out-params of pdf_repair_obj() [pdf_repair_obj()].
    """
    ret, stmofsp, stmlenp, encrypt, id, page, tmpofs, root = ll_pdf_repair_obj(doc.m_internal, buf.m_internal)
    return ret, stmofsp, stmlenp, PdfObj(ll_pdf_keep_obj( encrypt)), PdfObj(ll_pdf_keep_obj( id)), PdfObj(ll_pdf_keep_obj( page)), tmpofs, PdfObj(ll_pdf_keep_obj( root))

pdf_repair_obj = pdf_repair_obj_outparams_fn


def ll_pdf_resolve_link(doc, uri):
    """
    Wrapper for out-params of pdf_resolve_link().
    Returns: int, float xp, float yp
    """
    outparams = ll_pdf_resolve_link_outparams()
    ret = ll_pdf_resolve_link_outparams_fn(doc, uri, outparams)
    return ret, outparams.xp, outparams.yp

def pdf_resolve_link_outparams_fn(doc, uri):
    """
    Class-aware helper for out-params of pdf_resolve_link() [pdf_resolve_link()].
    """
    ret, xp, yp = ll_pdf_resolve_link(doc.m_internal, uri)
    return ret, xp, yp

pdf_resolve_link = pdf_resolve_link_outparams_fn


def ll_pdf_sample_shade_function(shade, n, funcs, t0, t1):
    """
    Wrapper for out-params of pdf_sample_shade_function().
    Returns: ::pdf_function *func
    """
    outparams = ll_pdf_sample_shade_function_outparams()
    ret = ll_pdf_sample_shade_function_outparams_fn(shade, n, funcs, t0, t1, outparams)
    return outparams.func

def pdf_sample_shade_function_outparams_fn(shade, n, funcs, t0, t1):
    """
    Class-aware helper for out-params of pdf_sample_shade_function() [pdf_sample_shade_function()].
    """
    func = ll_pdf_sample_shade_function(shade, n, funcs, t0, t1)
    return PdfFunction(ll_pdf_keep_function( func))

pdf_sample_shade_function = pdf_sample_shade_function_outparams_fn


def ll_pdf_signature_contents(doc, signature):
    """
    Wrapper for out-params of pdf_signature_contents().
    Returns: size_t, char *contents
    """
    outparams = ll_pdf_signature_contents_outparams()
    ret = ll_pdf_signature_contents_outparams_fn(doc, signature, outparams)
    return ret, outparams.contents

def pdf_signature_contents_outparams_fn(doc, signature):
    """
    Class-aware helper for out-params of pdf_signature_contents() [pdf_signature_contents()].
    """
    ret, contents = ll_pdf_signature_contents(doc.m_internal, signature.m_internal)
    return ret, contents

pdf_signature_contents = pdf_signature_contents_outparams_fn


def ll_pdf_sprint_obj(buf, cap, obj, tight, ascii):
    """
    Wrapper for out-params of pdf_sprint_obj().
    Returns: char *, size_t len
    """
    outparams = ll_pdf_sprint_obj_outparams()
    ret = ll_pdf_sprint_obj_outparams_fn(buf, cap, obj, tight, ascii, outparams)
    return ret, outparams.len

def pdf_sprint_obj_outparams_fn(buf, cap, obj, tight, ascii):
    """
    Class-aware helper for out-params of pdf_sprint_obj() [pdf_sprint_obj()].
    """
    ret, len = ll_pdf_sprint_obj(buf, cap, obj.m_internal, tight, ascii)
    return ret, len

pdf_sprint_obj = pdf_sprint_obj_outparams_fn


def ll_pdf_to_string(obj):
    """
    Wrapper for out-params of pdf_to_string().
    Returns: const char *, size_t sizep
    """
    outparams = ll_pdf_to_string_outparams()
    ret = ll_pdf_to_string_outparams_fn(obj, outparams)
    return ret, outparams.sizep

def pdf_to_string_outparams_fn(obj):
    """
    Class-aware helper for out-params of pdf_to_string() [pdf_to_string()].
    """
    ret, sizep = ll_pdf_to_string(obj.m_internal)
    return ret, sizep

pdf_to_string = pdf_to_string_outparams_fn


def ll_pdf_undoredo_state(doc):
    """
    Wrapper for out-params of pdf_undoredo_state().
    Returns: int, int steps
    """
    outparams = ll_pdf_undoredo_state_outparams()
    ret = ll_pdf_undoredo_state_outparams_fn(doc, outparams)
    return ret, outparams.steps

def pdf_undoredo_state_outparams_fn(doc):
    """
    Class-aware helper for out-params of pdf_undoredo_state() [pdf_undoredo_state()].
    """
    ret, steps = ll_pdf_undoredo_state(doc.m_internal)
    return ret, steps

pdf_undoredo_state = pdf_undoredo_state_outparams_fn


def ll_pdf_walk_tree(tree, kid_name, arrive, leave, arg):
    """
    Wrapper for out-params of pdf_walk_tree().
    Returns: ::pdf_obj *names, ::pdf_obj *values
    """
    outparams = ll_pdf_walk_tree_outparams()
    ret = ll_pdf_walk_tree_outparams_fn(tree, kid_name, arrive, leave, arg, outparams)
    return outparams.names, outparams.values

def pdf_walk_tree_outparams_fn(tree, kid_name, arrive, leave, arg):
    """
    Class-aware helper for out-params of pdf_walk_tree() [pdf_walk_tree()].
    """
    names, values = ll_pdf_walk_tree(tree.m_internal, kid_name.m_internal, arrive, leave, arg)
    return PdfObj(ll_pdf_keep_obj( names)), PdfObj(ll_pdf_keep_obj( values))

pdf_walk_tree = pdf_walk_tree_outparams_fn


def FzBitmap_fz_bitmap_details_outparams_fn( self):
    """
    Helper for out-params of class method fz_bitmap::ll_fz_bitmap_details() [fz_bitmap_details()].
    """
    w, h, n, stride = ll_fz_bitmap_details( self.m_internal)
    return w, h, n, stride

FzBitmap.fz_bitmap_details = FzBitmap_fz_bitmap_details_outparams_fn


def FzBuffer_fz_buffer_extract_outparams_fn( self):
    """
    Helper for out-params of class method fz_buffer::ll_fz_buffer_extract() [fz_buffer_extract()].
    """
    ret, data = ll_fz_buffer_extract( self.m_internal)
    return ret, data

FzBuffer.fz_buffer_extract = FzBuffer_fz_buffer_extract_outparams_fn


def FzBuffer_fz_buffer_storage_outparams_fn( self):
    """
    Helper for out-params of class method fz_buffer::ll_fz_buffer_storage() [fz_buffer_storage()].
    """
    ret, datap = ll_fz_buffer_storage( self.m_internal)
    return ret, datap

FzBuffer.fz_buffer_storage = FzBuffer_fz_buffer_storage_outparams_fn


def FzBuffer_fz_new_display_list_from_svg_outparams_fn( self, base_uri, zip):
    """
    Helper for out-params of class method fz_buffer::ll_fz_new_display_list_from_svg() [fz_new_display_list_from_svg()].
    """
    ret, w, h = ll_fz_new_display_list_from_svg( self.m_internal, base_uri, zip.m_internal)
    return FzDisplayList(ret), w, h

FzBuffer.fz_new_display_list_from_svg = FzBuffer_fz_new_display_list_from_svg_outparams_fn


def FzColorspace_fz_clamp_color_outparams_fn( self, in_):
    """
    Helper for out-params of class method fz_colorspace::ll_fz_clamp_color() [fz_clamp_color()].
    """
    out = ll_fz_clamp_color( self.m_internal, in_)
    return out

FzColorspace.fz_clamp_color = FzColorspace_fz_clamp_color_outparams_fn


def FzColorspace_fz_convert_color_outparams_fn( self, sv, params):
    """
    Helper for out-params of class method fz_colorspace::ll_fz_convert_color() [fz_convert_color()].
    """
    dv = ll_fz_convert_color( self.m_internal, sv, params.internal())
    return dv

FzColorspace.fz_convert_color = FzColorspace_fz_convert_color_outparams_fn


def FzColorspace_fz_convert_separation_colors_outparams_fn( self, src_color, dst_seps, color_params):
    """
    Helper for out-params of class method fz_colorspace::ll_fz_convert_separation_colors() [fz_convert_separation_colors()].
    """
    dst_color = ll_fz_convert_separation_colors( self.m_internal, src_color, dst_seps.m_internal, color_params.internal())
    return dst_color

FzColorspace.fz_convert_separation_colors = FzColorspace_fz_convert_separation_colors_outparams_fn


def FzCompressedBuffer_fz_open_image_decomp_stream_from_buffer_outparams_fn( self):
    """
    Helper for out-params of class method fz_compressed_buffer::ll_fz_open_image_decomp_stream_from_buffer() [fz_open_image_decomp_stream_from_buffer()].
    """
    ret, l2factor = ll_fz_open_image_decomp_stream_from_buffer( self.m_internal)
    return FzStream(ret), l2factor

FzCompressedBuffer.fz_open_image_decomp_stream_from_buffer = FzCompressedBuffer_fz_open_image_decomp_stream_from_buffer_outparams_fn


def FzDisplayList_fz_search_display_list_outparams_fn( self, needle, hit_bbox, hit_max):
    """
    Helper for out-params of class method fz_display_list::ll_fz_search_display_list() [fz_search_display_list()].
    """
    ret, hit_mark = ll_fz_search_display_list( self.m_internal, needle, hit_bbox.internal(), hit_max)
    return ret, hit_mark

FzDisplayList.fz_search_display_list = FzDisplayList_fz_search_display_list_outparams_fn


def FzDocument_fz_resolve_link_outparams_fn( self, uri):
    """
    Helper for out-params of class method fz_document::ll_fz_resolve_link() [fz_resolve_link()].
    """
    ret, xp, yp = ll_fz_resolve_link( self.m_internal, uri)
    return FzLocation(ret), xp, yp

FzDocument.fz_resolve_link = FzDocument_fz_resolve_link_outparams_fn


def FzDocument_fz_search_chapter_page_number_outparams_fn( self, chapter, page, needle, hit_bbox, hit_max):
    """
    Helper for out-params of class method fz_document::ll_fz_search_chapter_page_number() [fz_search_chapter_page_number()].
    """
    ret, hit_mark = ll_fz_search_chapter_page_number( self.m_internal, chapter, page, needle, hit_bbox.internal(), hit_max)
    return ret, hit_mark

FzDocument.fz_search_chapter_page_number = FzDocument_fz_search_chapter_page_number_outparams_fn


def FzDocument_fz_search_page_number_outparams_fn( self, number, needle, hit_bbox, hit_max):
    """
    Helper for out-params of class method fz_document::ll_fz_search_page_number() [fz_search_page_number()].
    """
    ret, hit_mark = ll_fz_search_page_number( self.m_internal, number, needle, hit_bbox.internal(), hit_max)
    return ret, hit_mark

FzDocument.fz_search_page_number = FzDocument_fz_search_page_number_outparams_fn


def FzFont_fz_encode_character_with_fallback_outparams_fn( self, unicode, script, language):
    """
    Helper for out-params of class method fz_font::ll_fz_encode_character_with_fallback() [fz_encode_character_with_fallback()].
    """
    ret, out_font = ll_fz_encode_character_with_fallback( self.m_internal, unicode, script, language)
    return ret, FzFont(ll_fz_keep_font( out_font))

FzFont.fz_encode_character_with_fallback = FzFont_fz_encode_character_with_fallback_outparams_fn


def FzImage_fz_get_pixmap_from_image_outparams_fn( self, subarea, ctm):
    """
    Helper for out-params of class method fz_image::ll_fz_get_pixmap_from_image() [fz_get_pixmap_from_image()].
    """
    ret, w, h = ll_fz_get_pixmap_from_image( self.m_internal, subarea.internal(), ctm.internal())
    return FzPixmap(ret), w, h

FzImage.fz_get_pixmap_from_image = FzImage_fz_get_pixmap_from_image_outparams_fn


def FzImage_fz_image_resolution_outparams_fn( self):
    """
    Helper for out-params of class method fz_image::ll_fz_image_resolution() [fz_image_resolution()].
    """
    xres, yres = ll_fz_image_resolution( self.m_internal)
    return xres, yres

FzImage.fz_image_resolution = FzImage_fz_image_resolution_outparams_fn


def FzOutput_fz_new_svg_device_with_id_outparams_fn( self, page_width, page_height, text_format, reuse_images):
    """
    Helper for out-params of class method fz_output::ll_fz_new_svg_device_with_id() [fz_new_svg_device_with_id()].
    """
    ret, id = ll_fz_new_svg_device_with_id( self.m_internal, page_width, page_height, text_format, reuse_images)
    return FzDevice(ret), id

FzOutput.fz_new_svg_device_with_id = FzOutput_fz_new_svg_device_with_id_outparams_fn


def FzPage_fz_page_presentation_outparams_fn( self, transition):
    """
    Helper for out-params of class method fz_page::ll_fz_page_presentation() [fz_page_presentation()].
    """
    ret, duration = ll_fz_page_presentation( self.m_internal, transition.internal())
    return FzTransition(ret), duration

FzPage.fz_page_presentation = FzPage_fz_page_presentation_outparams_fn


def FzPixmap_fz_fill_pixmap_with_color_outparams_fn( self, colorspace, color_params):
    """
    Helper for out-params of class method fz_pixmap::ll_fz_fill_pixmap_with_color() [fz_fill_pixmap_with_color()].
    """
    color = ll_fz_fill_pixmap_with_color( self.m_internal, colorspace.m_internal, color_params.internal())
    return color

FzPixmap.fz_fill_pixmap_with_color = FzPixmap_fz_fill_pixmap_with_color_outparams_fn


def FzSeparations_fz_separation_equivalent_outparams_fn( self, idx, dst_cs, prf, color_params):
    """
    Helper for out-params of class method fz_separations::ll_fz_separation_equivalent() [fz_separation_equivalent()].
    """
    dst_color = ll_fz_separation_equivalent( self.m_internal, idx, dst_cs.m_internal, prf.m_internal, color_params.internal())
    return dst_color

FzSeparations.fz_separation_equivalent = FzSeparations_fz_separation_equivalent_outparams_fn


def FzShade_fz_paint_shade_outparams_fn( self, override_cs, ctm, dest, color_params, bbox, eop):
    """
    Helper for out-params of class method fz_shade::ll_fz_paint_shade() [fz_paint_shade()].
    """
    cache = ll_fz_paint_shade( self.m_internal, override_cs.m_internal, ctm.internal(), dest.m_internal, color_params.internal(), bbox.internal(), eop.m_internal)
    return FzShadeColorCache(ll_fz_keep_shade_color_cache( cache))

FzShade.fz_paint_shade = FzShade_fz_paint_shade_outparams_fn


def FzStextPage_fz_search_stext_page_outparams_fn( self, needle, hit_bbox, hit_max):
    """
    Helper for out-params of class method fz_stext_page::ll_fz_search_stext_page() [fz_search_stext_page()].
    """
    ret, hit_mark = ll_fz_search_stext_page( self.m_internal, needle, hit_bbox.internal(), hit_max)
    return ret, hit_mark

FzStextPage.fz_search_stext_page = FzStextPage_fz_search_stext_page_outparams_fn


def FzStream_fz_decomp_image_from_stream_outparams_fn( self, image, subarea, indexed, l2factor):
    """
    Helper for out-params of class method fz_stream::ll_fz_decomp_image_from_stream() [fz_decomp_image_from_stream()].
    """
    ret, l2extra = ll_fz_decomp_image_from_stream( self.m_internal, image.m_internal, subarea.internal(), indexed, l2factor)
    return FzPixmap(ret), l2extra

FzStream.fz_decomp_image_from_stream = FzStream_fz_decomp_image_from_stream_outparams_fn


def FzStream_fz_open_image_decomp_stream_outparams_fn( self, arg_1):
    """
    Helper for out-params of class method fz_stream::ll_fz_open_image_decomp_stream() [fz_open_image_decomp_stream()].
    """
    ret, l2factor = ll_fz_open_image_decomp_stream( self.m_internal, arg_1.m_internal)
    return FzStream(ret), l2factor

FzStream.fz_open_image_decomp_stream = FzStream_fz_open_image_decomp_stream_outparams_fn


def FzStream_fz_read_best_outparams_fn( self, initial, worst_case):
    """
    Helper for out-params of class method fz_stream::ll_fz_read_best() [fz_read_best()].
    """
    ret, truncated = ll_fz_read_best( self.m_internal, initial, worst_case)
    return FzBuffer(ret), truncated

FzStream.fz_read_best = FzStream_fz_read_best_outparams_fn


def FzXml_fz_dom_get_attribute_outparams_fn( self, i):
    """
    Helper for out-params of class method fz_xml::ll_fz_dom_get_attribute() [fz_dom_get_attribute()].
    """
    ret, att = ll_fz_dom_get_attribute( self.m_internal, i)
    return ret, att

FzXml.fz_dom_get_attribute = FzXml_fz_dom_get_attribute_outparams_fn


def FzXml_fz_new_display_list_from_svg_xml_outparams_fn( self, xmldoc, base_uri, zip):
    """
    Helper for out-params of class method fz_xml::ll_fz_new_display_list_from_svg_xml() [fz_new_display_list_from_svg_xml()].
    """
    ret, w, h = ll_fz_new_display_list_from_svg_xml( self.m_internal, xmldoc.m_internal, base_uri, zip.m_internal)
    return FzDisplayList(ret), w, h

FzXml.fz_new_display_list_from_svg_xml = FzXml_fz_new_display_list_from_svg_xml_outparams_fn


def PdfAnnot_pdf_annot_MK_BC_outparams_fn( self, color):
    """
    Helper for out-params of class method pdf_annot::ll_pdf_annot_MK_BC() [pdf_annot_MK_BC()].
    """
    n = ll_pdf_annot_MK_BC( self.m_internal, color)
    return n

PdfAnnot.pdf_annot_MK_BC = PdfAnnot_pdf_annot_MK_BC_outparams_fn


def PdfAnnot_pdf_annot_MK_BG_outparams_fn( self, color):
    """
    Helper for out-params of class method pdf_annot::ll_pdf_annot_MK_BG() [pdf_annot_MK_BG()].
    """
    n = ll_pdf_annot_MK_BG( self.m_internal, color)
    return n

PdfAnnot.pdf_annot_MK_BG = PdfAnnot_pdf_annot_MK_BG_outparams_fn


def PdfAnnot_pdf_annot_color_outparams_fn( self, color):
    """
    Helper for out-params of class method pdf_annot::ll_pdf_annot_color() [pdf_annot_color()].
    """
    n = ll_pdf_annot_color( self.m_internal, color)
    return n

PdfAnnot.pdf_annot_color = PdfAnnot_pdf_annot_color_outparams_fn


def PdfAnnot_pdf_annot_default_appearance_outparams_fn( self, color):
    """
    Helper for out-params of class method pdf_annot::ll_pdf_annot_default_appearance() [pdf_annot_default_appearance()].
    """
    font, size, n = ll_pdf_annot_default_appearance( self.m_internal, color)
    return font, size, n

PdfAnnot.pdf_annot_default_appearance = PdfAnnot_pdf_annot_default_appearance_outparams_fn


def PdfAnnot_pdf_annot_interior_color_outparams_fn( self, color):
    """
    Helper for out-params of class method pdf_annot::ll_pdf_annot_interior_color() [pdf_annot_interior_color()].
    """
    n = ll_pdf_annot_interior_color( self.m_internal, color)
    return n

PdfAnnot.pdf_annot_interior_color = PdfAnnot_pdf_annot_interior_color_outparams_fn


def PdfAnnot_pdf_annot_line_ending_styles_outparams_fn( self):
    """
    Helper for out-params of class method pdf_annot::ll_pdf_annot_line_ending_styles() [pdf_annot_line_ending_styles()].
    """
    start_style, end_style = ll_pdf_annot_line_ending_styles( self.m_internal)
    return start_style, end_style

PdfAnnot.pdf_annot_line_ending_styles = PdfAnnot_pdf_annot_line_ending_styles_outparams_fn


def PdfAnnot_pdf_edit_text_field_value_outparams_fn( self, value, change):
    """
    Helper for out-params of class method pdf_annot::ll_pdf_edit_text_field_value() [pdf_edit_text_field_value()].
    """
    ret, selStart, selEnd, newvalue = ll_pdf_edit_text_field_value( self.m_internal, value, change)
    return ret, selStart, selEnd, newvalue

PdfAnnot.pdf_edit_text_field_value = PdfAnnot_pdf_edit_text_field_value_outparams_fn


def PdfCmap_pdf_decode_cmap_outparams_fn( self, s, e):
    """
    Helper for out-params of class method pdf_cmap::ll_pdf_decode_cmap() [pdf_decode_cmap()].
    """
    ret, cpt = ll_pdf_decode_cmap( self.m_internal, s, e)
    return ret, cpt

PdfCmap.pdf_decode_cmap = PdfCmap_pdf_decode_cmap_outparams_fn


def PdfCmap_pdf_lookup_cmap_full_outparams_fn( self, cpt):
    """
    Helper for out-params of class method pdf_cmap::ll_pdf_lookup_cmap_full() [pdf_lookup_cmap_full()].
    """
    ret, out = ll_pdf_lookup_cmap_full( self.m_internal, cpt)
    return ret, out

PdfCmap.pdf_lookup_cmap_full = PdfCmap_pdf_lookup_cmap_full_outparams_fn


def PdfCmap_pdf_map_one_to_many_outparams_fn( self, one, len):
    """
    Helper for out-params of class method pdf_cmap::ll_pdf_map_one_to_many() [pdf_map_one_to_many()].
    """
    many = ll_pdf_map_one_to_many( self.m_internal, one, len)
    return many

PdfCmap.pdf_map_one_to_many = PdfCmap_pdf_map_one_to_many_outparams_fn


def PdfDocument_pdf_field_event_validate_outparams_fn( self, field, value):
    """
    Helper for out-params of class method pdf_document::ll_pdf_field_event_validate() [pdf_field_event_validate()].
    """
    ret, newvalue = ll_pdf_field_event_validate( self.m_internal, field.m_internal, value)
    return ret, newvalue

PdfDocument.pdf_field_event_validate = PdfDocument_pdf_field_event_validate_outparams_fn


def PdfDocument_pdf_load_to_unicode_outparams_fn( self, font, collection, cmapstm):
    """
    Helper for out-params of class method pdf_document::ll_pdf_load_to_unicode() [pdf_load_to_unicode()].
    """
    strings = ll_pdf_load_to_unicode( self.m_internal, font.m_internal, collection, cmapstm.m_internal)
    return strings

PdfDocument.pdf_load_to_unicode = PdfDocument_pdf_load_to_unicode_outparams_fn


def PdfDocument_pdf_lookup_anchor_outparams_fn( self, name):
    """
    Helper for out-params of class method pdf_document::ll_pdf_lookup_anchor() [pdf_lookup_anchor()].
    """
    ret, xp, yp = ll_pdf_lookup_anchor( self.m_internal, name)
    return ret, xp, yp

PdfDocument.pdf_lookup_anchor = PdfDocument_pdf_lookup_anchor_outparams_fn


def PdfDocument_pdf_page_write_outparams_fn( self, mediabox):
    """
    Helper for out-params of class method pdf_document::ll_pdf_page_write() [pdf_page_write()].
    """
    ret, presources, pcontents = ll_pdf_page_write( self.m_internal, mediabox.internal())
    return FzDevice(ret), PdfObj( presources), FzBuffer( pcontents)

PdfDocument.pdf_page_write = PdfDocument_pdf_page_write_outparams_fn


def PdfDocument_pdf_parse_ind_obj_outparams_fn( self, f):
    """
    Helper for out-params of class method pdf_document::ll_pdf_parse_ind_obj() [pdf_parse_ind_obj()].
    """
    ret, num, gen, stm_ofs, try_repair = ll_pdf_parse_ind_obj( self.m_internal, f.m_internal)
    return PdfObj(ret), num, gen, stm_ofs, try_repair

PdfDocument.pdf_parse_ind_obj = PdfDocument_pdf_parse_ind_obj_outparams_fn


def PdfDocument_pdf_parse_journal_obj_outparams_fn( self, stm):
    """
    Helper for out-params of class method pdf_document::ll_pdf_parse_journal_obj() [pdf_parse_journal_obj()].
    """
    ret, onum, ostm, newobj = ll_pdf_parse_journal_obj( self.m_internal, stm.m_internal)
    return PdfObj(ret), onum, FzBuffer( ostm), newobj

PdfDocument.pdf_parse_journal_obj = PdfDocument_pdf_parse_journal_obj_outparams_fn


def PdfDocument_pdf_repair_obj_outparams_fn( self, buf):
    """
    Helper for out-params of class method pdf_document::ll_pdf_repair_obj() [pdf_repair_obj()].
    """
    ret, stmofsp, stmlenp, encrypt, id, page, tmpofs, root = ll_pdf_repair_obj( self.m_internal, buf.m_internal)
    return ret, stmofsp, stmlenp, PdfObj(ll_pdf_keep_obj( encrypt)), PdfObj(ll_pdf_keep_obj( id)), PdfObj(ll_pdf_keep_obj( page)), tmpofs, PdfObj(ll_pdf_keep_obj( root))

PdfDocument.pdf_repair_obj = PdfDocument_pdf_repair_obj_outparams_fn


def PdfDocument_pdf_resolve_link_outparams_fn( self, uri):
    """
    Helper for out-params of class method pdf_document::ll_pdf_resolve_link() [pdf_resolve_link()].
    """
    ret, xp, yp = ll_pdf_resolve_link( self.m_internal, uri)
    return ret, xp, yp

PdfDocument.pdf_resolve_link = PdfDocument_pdf_resolve_link_outparams_fn


def PdfDocument_pdf_signature_contents_outparams_fn( self, signature):
    """
    Helper for out-params of class method pdf_document::ll_pdf_signature_contents() [pdf_signature_contents()].
    """
    ret, contents = ll_pdf_signature_contents( self.m_internal, signature.m_internal)
    return ret, contents

PdfDocument.pdf_signature_contents = PdfDocument_pdf_signature_contents_outparams_fn


def PdfDocument_pdf_undoredo_state_outparams_fn( self):
    """
    Helper for out-params of class method pdf_document::ll_pdf_undoredo_state() [pdf_undoredo_state()].
    """
    ret, steps = ll_pdf_undoredo_state( self.m_internal)
    return ret, steps

PdfDocument.pdf_undoredo_state = PdfDocument_pdf_undoredo_state_outparams_fn


def PdfFunction_pdf_eval_function_outparams_fn( self, in_, inlen, outlen):
    """
    Helper for out-params of class method pdf_function::ll_pdf_eval_function() [pdf_eval_function()].
    """
    out = ll_pdf_eval_function( self.m_internal, in_, inlen, outlen)
    return out

PdfFunction.pdf_eval_function = PdfFunction_pdf_eval_function_outparams_fn


def PdfJs_pdf_js_event_result_validate_outparams_fn( self):
    """
    Helper for out-params of class method pdf_js::ll_pdf_js_event_result_validate() [pdf_js_event_result_validate()].
    """
    ret, newvalue = ll_pdf_js_event_result_validate( self.m_internal)
    return ret, newvalue

PdfJs.pdf_js_event_result_validate = PdfJs_pdf_js_event_result_validate_outparams_fn


def PdfJs_pdf_js_execute_outparams_fn( self, name, code):
    """
    Helper for out-params of class method pdf_js::ll_pdf_js_execute() [pdf_js_execute()].
    """
    result = ll_pdf_js_execute( self.m_internal, name, code)
    return result

PdfJs.pdf_js_execute = PdfJs_pdf_js_execute_outparams_fn


def PdfObj_pdf_array_get_string_outparams_fn( self, index):
    """
    Helper for out-params of class method pdf_obj::ll_pdf_array_get_string() [pdf_array_get_string()].
    """
    ret, sizep = ll_pdf_array_get_string( self.m_internal, index)
    return ret, sizep

PdfObj.pdf_array_get_string = PdfObj_pdf_array_get_string_outparams_fn


def PdfObj_pdf_dict_get_string_outparams_fn( self):
    """
    Helper for out-params of class method pdf_obj::ll_pdf_dict_get_string() [pdf_dict_get_string()].
    """
    ret, sizep = ll_pdf_dict_get_string( self.m_internal)
    return ret, sizep

PdfObj.pdf_dict_get_string = PdfObj_pdf_dict_get_string_outparams_fn


def PdfObj_pdf_obj_memo_outparams_fn( self, bit):
    """
    Helper for out-params of class method pdf_obj::ll_pdf_obj_memo() [pdf_obj_memo()].
    """
    ret, memo = ll_pdf_obj_memo( self.m_internal, bit)
    return ret, memo

PdfObj.pdf_obj_memo = PdfObj_pdf_obj_memo_outparams_fn


def PdfObj_pdf_to_string_outparams_fn( self):
    """
    Helper for out-params of class method pdf_obj::ll_pdf_to_string() [pdf_to_string()].
    """
    ret, sizep = ll_pdf_to_string( self.m_internal)
    return ret, sizep

PdfObj.pdf_to_string = PdfObj_pdf_to_string_outparams_fn


def PdfObj_pdf_walk_tree_outparams_fn( self, arrive, leave, arg):
    """
    Helper for out-params of class method pdf_obj::ll_pdf_walk_tree() [pdf_walk_tree()].
    """
    names, values = ll_pdf_walk_tree( self.m_internal, arrive, leave, arg)
    return PdfObj(ll_pdf_keep_obj( names)), PdfObj(ll_pdf_keep_obj( values))

PdfObj.pdf_walk_tree = PdfObj_pdf_walk_tree_outparams_fn


def PdfPage_pdf_page_presentation_outparams_fn( self, transition):
    """
    Helper for out-params of class method pdf_page::ll_pdf_page_presentation() [pdf_page_presentation()].
    """
    ret, duration = ll_pdf_page_presentation( self.m_internal, transition.internal())
    return FzTransition(ret), duration

PdfPage.pdf_page_presentation = PdfPage_pdf_page_presentation_outparams_fn


def PdfProcessor_pdf_process_contents_outparams_fn( self, doc, obj, res, cookie):
    """
    Helper for out-params of class method pdf_processor::ll_pdf_process_contents() [pdf_process_contents()].
    """
    out_res = ll_pdf_process_contents( self.m_internal, doc.m_internal, obj.m_internal, res.m_internal, cookie.m_internal)
    return PdfObj(ll_pdf_keep_obj( out_res))

PdfProcessor.pdf_process_contents = PdfProcessor_pdf_process_contents_outparams_fn




# Wrap fz_parse_page_range() to fix SWIG bug where a NULL return
# value seems to mess up the returned list - we end up with ret
# containing two elements rather than three, e.g. [0, 2]. This
# occurs with SWIG-3.0; maybe fixed in SWIG-4?
#
ll_fz_parse_page_range_orig = ll_fz_parse_page_range
def ll_fz_parse_page_range(s, n):
    ret = ll_fz_parse_page_range_orig(s, n)
    if len(ret) == 2:
        return None, 0, 0
    else:
        return ret[0], ret[1], ret[2]
fz_parse_page_range = ll_fz_parse_page_range

# Provide native python implementation of format_output_path() (->
# fz_format_output_path).
#
def ll_fz_format_output_path( format, page):
    m = re.search( '(%[0-9]*d)', format)
    if m:
        ret = format[ :m.start(1)] + str(page) + format[ m.end(1):]
    else:
        dot = format.rfind( '.')
        if dot < 0:
            dot = len( format)
        ret = format[:dot] + str(page) + format[dot:]
    return ret
fz_format_output_path = ll_fz_format_output_path

class IteratorWrap:
    """
    This is a Python iterator for containers that have C++-style
    begin() and end() methods that return iterators.

    Iterators must have the following methods:

        __increment__(): move to next item in the container.
        __ref__(): return reference to item in the container.

    Must also be able to compare two iterators for equality.

    """
    def __init__( self, container):
        self.container = container
        self.pos = None
        self.end = container.end()
    def __iter__( self):
        return self
    def __next__( self):    # for python2.
        if self.pos is None:
            self.pos = self.container.begin()
        else:
            self.pos.__increment__()
        if self.pos == self.end:
            raise StopIteration()
        return self.pos.__ref__()
    def next( self):    # for python3.
        return self.__next__()

# The auto-generated Python class method
# FzBuffer.fz_buffer_extract() returns (size, data).
#
# But these raw values aren't particularly useful to
# Python code so we change the method to return a Python
# bytes instance instead, using the special C function
# buffer_extract_bytes() defined above.
#
# The raw values for a buffer are available via
# fz_buffer_storage().

def ll_fz_buffer_extract(buffer):
    """
    Returns buffer data as a Python bytes instance, leaving the
    buffer empty.
    """
    assert isinstance( buffer, fz_buffer)
    return ll_fz_buffer_to_bytes_internal(buffer, clear=1)
def fz_buffer_extract(buffer):
    """
    Returns buffer data as a Python bytes instance, leaving the
    buffer empty.
    """
    assert isinstance( buffer, FzBuffer)
    return ll_fz_buffer_extract(buffer.m_internal)
FzBuffer.fz_buffer_extract = fz_buffer_extract

def ll_fz_buffer_extract_copy( buffer):
    """
    Returns buffer data as a Python bytes instance, leaving the
    buffer unchanged.
    """
    assert isinstance( buffer, fz_buffer)
    return ll_fz_buffer_to_bytes_internal(buffer, clear=0)
def fz_buffer_extract_copy( buffer):
    """
    Returns buffer data as a Python bytes instance, leaving the
    buffer unchanged.
    """
    assert isinstance( buffer, FzBuffer)
    return ll_fz_buffer_extract_copy(buffer.m_internal)
FzBuffer.fz_buffer_extract_copy = fz_buffer_extract_copy

# [ll_fz_buffer_storage_memoryview() is implemented in C.]
def fz_buffer_storage_memoryview( buffer, writable=False):
    """
    Returns a read-only or writable Python `memoryview` onto
    `fz_buffer` data. This relies on `buffer` existing and
    not changing size while the `memoryview` is used.
    """
    assert isinstance( buffer, FzBuffer)
    return ll_fz_buffer_storage_memoryview( buffer.m_internal, writable)
FzBuffer.fz_buffer_storage_memoryview = fz_buffer_storage_memoryview

# Overwrite wrappers for fz_new_buffer_from_copied_data() to
# take Python buffer.
#
ll_fz_new_buffer_from_copied_data_orig = ll_fz_new_buffer_from_copied_data
def ll_fz_new_buffer_from_copied_data(data):
    """
    Returns fz_buffer containing copy of `data`, which should
    be a `bytes` or similar Python buffer instance.
    """
    buffer_ = ll_fz_new_buffer_from_copied_data_orig(python_buffer_data(data), len(data))
    return buffer_
def fz_new_buffer_from_copied_data(data):
    """
    Returns FzBuffer containing copy of `data`, which should be
    a `bytes` or similar Python buffer instance.
    """
    return FzBuffer( ll_fz_new_buffer_from_copied_data( data))
FzBuffer.fz_new_buffer_from_copied_data = fz_new_buffer_from_copied_data

def ll_pdf_dict_getl(obj, *tail):
    """
    Python implementation of ll_pdf_dict_getl(), because SWIG
    doesn't handle variadic args. Each item in `tail` should be
    `mupdf.pdf_obj`.
    """
    for key in tail:
        if not obj:
            break
        obj = ll_pdf_dict_get(obj, key)
    assert isinstance(obj, pdf_obj)
    return obj
def pdf_dict_getl(obj, *tail):
    """
    Python implementation of pdf_dict_getl(), because SWIG
    doesn't handle variadic args. Each item in `tail` should be
    a `mupdf.PdfObj`.
    """
    for key in tail:
        if not obj.m_internal:
            break
        obj = pdf_dict_get(obj, key)
    assert isinstance(obj, PdfObj)
    return obj
PdfObj.pdf_dict_getl = pdf_dict_getl

def ll_pdf_dict_putl(obj, val, *tail):
    """
    Python implementation of ll_pdf_dict_putl() because SWIG
    doesn't handle variadic args. Each item in `tail` should
    be a SWIG wrapper for a `pdf_obj`.
    """
    if ll_pdf_is_indirect( obj):
        obj = ll_pdf_resolve_indirect_chain( obj)
    if not pdf_is_dict( obj):
        raise Exception(f'not a dict: {obj}')
    if not tail:
        return
    doc = ll_pdf_get_bound_document( obj)
    for i, key in enumerate( tail[:-1]):
        assert isinstance( key, PdfObj), f'Item {i} in `tail` should be a pdf_obj but is a {type(key)}.'
        next_obj = ll_pdf_dict_get( obj, key)
        if not next_obj:
            # We have to create entries
            next_obj = ll_pdf_new_dict( doc, 1)
            ll_pdf_dict_put( obj, key, next_obj)
        obj = next_obj
    key = tail[-1]
    ll_pdf_dict_put( obj, key, val)
def pdf_dict_putl(obj, val, *tail):
    """
    Python implementation of pdf_dict_putl(fz_context *ctx,
    pdf_obj *obj, pdf_obj *val, ...) because SWIG doesn't
    handle variadic args. Each item in `tail` should
    be a SWIG wrapper for a `PdfObj`.
    """
    if pdf_is_indirect( obj):
        obj = pdf_resolve_indirect_chain( obj)
    if not pdf_is_dict( obj):
        raise Exception(f'not a dict: {obj}')
    if not tail:
        return
    doc = pdf_get_bound_document( obj)
    for i, key in enumerate( tail[:-1]):
        assert isinstance( key, PdfObj), f'item {i} in `tail` should be a PdfObj but is a {type(key)}.'
        next_obj = pdf_dict_get( obj, key)
        if not next_obj.m_internal:
            # We have to create entries
            next_obj = pdf_new_dict( doc, 1)
            pdf_dict_put( obj, key, next_obj)
        obj = next_obj
    key = tail[-1]
    pdf_dict_put( obj, key, val)
PdfObj.pdf_dict_putl = pdf_dict_putl

def pdf_dict_putl_drop(obj, *tail):
    raise Exception('mupdf.pdf_dict_putl_drop() is unsupported and unnecessary in Python because reference counting is automatic. Instead use mupdf.pdf_dict_putl().')
PdfObj.pdf_dict_putl_drop = pdf_dict_putl_drop

def ll_pdf_set_annot_color(annot, color):
    """
    Low-level Python implementation of pdf_set_annot_color()
    using ll_pdf_set_annot_color2().
    """
    if isinstance(color, float):
        ll_pdf_set_annot_color2(annot, 1, color, 0, 0, 0)
    elif len(color) == 1:
        ll_pdf_set_annot_color2(annot, 1, color[0], 0, 0, 0)
    elif len(color) == 2:
        ll_pdf_set_annot_color2(annot, 2, color[0], color[1], 0, 0)
    elif len(color) == 3:
        ll_pdf_set_annot_color2(annot, 3, color[0], color[1], color[2], 0)
    elif len(color) == 4:
        ll_pdf_set_annot_color2(annot, 4, color[0], color[1], color[2], color[3])
    else:
        raise Exception( f'Unexpected color should be float or list of 1-4 floats: {color}')
def pdf_set_annot_color(self, color):
    return ll_pdf_set_annot_color(self.m_internal, color)
PdfAnnot.pdf_set_annot_color = pdf_set_annot_color

def ll_pdf_set_annot_interior_color(annot, color):
    """
    Low-level Python version of pdf_set_annot_color() using
    pdf_set_annot_color2().
    """
    if isinstance(color, float):
        ll_pdf_set_annot_interior_color2(annot, 1, color, 0, 0, 0)
    elif len(color) == 1:
        ll_pdf_set_annot_interior_color2(annot, 1, color[0], 0, 0, 0)
    elif len(color) == 2:
        ll_pdf_set_annot_interior_color2(annot, 2, color[0], color[1], 0, 0)
    elif len(color) == 3:
        ll_pdf_set_annot_interior_color2(annot, 3, color[0], color[1], color[2], 0)
    elif len(color) == 4:
        ll_pdf_set_annot_interior_color2(annot, 4, color[0], color[1], color[2], color[3])
    else:
        raise Exception( f'Unexpected color should be float or list of 1-4 floats: {color}')
def pdf_set_annot_interior_color(self, color):
    """
    Python version of pdf_set_annot_color() using
    pdf_set_annot_color2().
    """
    return ll_pdf_set_annot_interior_color(self.m_internal, color)
PdfAnnot.pdf_set_annot_interior_color = pdf_set_annot_interior_color

def ll_fz_fill_text( dev, text, ctm, colorspace, color, alpha, color_params):
    """
    Low-level Python version of fz_fill_text() taking list/tuple for `color`.
    """
    color = tuple(color) + (0,) * (4-len(color))
    assert len(color) == 4, f'color not len 4: len={len(color)}: {color}'
    return ll_fz_fill_text2(dev, text, ctm, colorspace, *color, alpha, color_params)
def fz_fill_text(dev, text, ctm, colorspace, color, alpha, color_params):
    """
    Python version of fz_fill_text() taking list/tuple for `color`.
    """
    return ll_fz_fill_text(
            dev.m_internal,
            text.m_internal,
            ctm.internal(),
            colorspace.m_internal,
            color,
            alpha,
            color_params.internal(),
            )
FzDevice.fz_fill_text = fz_fill_text

# Override mupdf_convert_color() to return (rgb0, rgb1, rgb2, rgb3).
def ll_fz_convert_color( ss, sv, ds, is_, params):
    """
    Low-level Python version of fz_convert_color().

    `sv` should be a float or list of 1-4 floats or a SWIG
    representation of a float*.

    Returns (dv0, dv1, dv2, dv3).
    """
    dv = fz_convert_color2_v()
    if isinstance( sv, float):
       ll_fz_convert_color2( ss, sv, 0.0, 0.0, 0.0, ds, dv, is_, params)
    elif isinstance( sv, (tuple, list)):
        sv2 = tuple(sv) + (0,) * (4-len(sv))
        ll_fz_convert_color2( ss, *sv2, ds, dv, is_, params)
    else:
        # Assume `sv` is SWIG representation of a `float*`.
        ll_fz_convert_color2( ss, sv, ds, dv, is_, params)
    return dv.v0, dv.v1, dv.v2, dv.v3
def fz_convert_color( ss, sv, ds, is_, params):
    """
    Python version of fz_convert_color().

    `sv` should be a float or list of 1-4 floats or a SWIG
    representation of a float*.

    Returns (dv0, dv1, dv2, dv3).
    """
    return ll_fz_convert_color( ss.m_internal, sv, ds.m_internal, is_.m_internal, params.internal())
FzColorspace.fz_convert_color = fz_convert_color

# Override fz_set_warning_callback() and
# fz_set_error_callback() to use Python classes derived from
# our SWIG Director class DiagnosticCallback (defined in C), so
# that fnptrs can call Python code.
#

# We store DiagnosticCallbackPython instances in these
# globals to ensure they continue to exist after
# set_diagnostic_callback() returns.
#
set_warning_callback_s = None
set_error_callback_s = None

# Override set_error_callback().
class DiagnosticCallbackPython( DiagnosticCallback):
    """
    Overrides Director class DiagnosticCallback's virtual
    `_print()` method in Python.
    """
    def __init__( self, description, printfn):
        super().__init__( description)
        self.printfn = printfn
        if g_mupdf_trace_director:
            log( f'DiagnosticCallbackPython[{self.m_description}].__init__() self={self!r} printfn={printfn!r}')
    def __del__( self):
        if g_mupdf_trace_director:
            log( f'DiagnosticCallbackPython[{self.m_description}].__del__() destructor called.')
    def _print( self, message):
        if g_mupdf_trace_director:
            log( f'DiagnosticCallbackPython[{self.m_description}]._print(): Calling self.printfn={self.printfn!r} with message={message!r}')
        try:
            self.printfn( message)
        except Exception as e:
            # This shouldn't happen, so always output a diagnostic.
            log( f'DiagnosticCallbackPython[{self.m_description}]._print(): Warning: exception from self.printfn={self.printfn!r}: e={e!r}')
            # Calling `raise` here serves to test
            # `DiagnosticCallback()`'s swallowing of what will
            # be a C++ exception. But we could swallow the
            # exception here instead.
            raise

def set_diagnostic_callback( description, printfn):
    if g_mupdf_trace_director:
        log( f'set_diagnostic_callback() description={description!r} printfn={printfn!r}')
    if printfn:
        ret = DiagnosticCallbackPython( description, printfn)
        return ret
    else:
        if g_mupdf_trace_director:
            log( f'Calling ll_fz_set_{description}_callback() with (None, None)')
        if description == 'error':
            ll_fz_set_error_callback( None, None)
        elif description == 'warning':
            ll_fz_set_warning_callback( None, None)
        else:
            assert 0, f'Unrecognised description={description!r}'
        return None

def fz_set_error_callback( printfn):
    global set_error_callback_s
    set_error_callback_s = set_diagnostic_callback( 'error', printfn)

def fz_set_warning_callback( printfn):
    global set_warning_callback_s
    set_warning_callback_s = set_diagnostic_callback( 'warning', printfn)

# Direct access to fz_pixmap samples.
def ll_fz_pixmap_samples_memoryview( pixmap):
    """
    Returns a writable Python `memoryview` for a `fz_pixmap`.
    """
    assert isinstance( pixmap, fz_pixmap)
    ret = python_memoryview_from_memory(
            ll_fz_pixmap_samples( pixmap),
            ll_fz_pixmap_stride( pixmap) * ll_fz_pixmap_height( pixmap),
            1, # writable
            )
    return ret
def fz_pixmap_samples_memoryview( pixmap):
    """
    Returns a writable Python `memoryview` for a `FzPixmap`.
    """
    return ll_fz_pixmap_samples_memoryview( pixmap.m_internal)
FzPixmap.fz_pixmap_samples_memoryview = fz_pixmap_samples_memoryview

# Avoid potential unsafe use of variadic args by forcing a
# single arg and escaping all '%' characters. (Passing ('%s',
# text) does not work - results in "(null)" being output.)
#
ll_fz_warn_original = ll_fz_warn
def ll_fz_warn( text):
    assert isinstance( text, str), f'text={text!r} str={str!r}'
    text = text.replace( '%', '%%')
    return ll_fz_warn_original( text)
fz_warn = ll_fz_warn

# Force use of pdf_load_field_name2() instead of
# pdf_load_field_name() because the latter returns a char*
# buffer that must be freed by the caller.
ll_pdf_load_field_name = ll_pdf_load_field_name2
pdf_load_field_name = pdf_load_field_name2
PdfObj.pdf_load_field_name = pdf_load_field_name

# It's important that when we create class derived
# from StoryPositionsCallback, we ensure that
# StoryPositionsCallback's constructor is called. Otherwise
# the new instance doesn't seem to be an instance of
# StoryPositionsCallback.
#
class StoryPositionsCallback_python( StoryPositionsCallback):
    def __init__( self, python_callback):
        super().__init__()
        self.python_callback = python_callback
    def call( self, position):
        self.python_callback( position)

ll_fz_story_positions_orig = ll_fz_story_positions
def ll_fz_story_positions( story, python_callback):
    """
    Custom replacement for `ll_fz_story_positions()` that takes
    a Python callable `python_callback`.
    """
    #log( f'll_fz_story_positions() type(story)={type(story)!r} type(python_callback)={type(python_callback)!r}')
    python_callback_instance = StoryPositionsCallback_python( python_callback)
    ll_fz_story_positions_director( story, python_callback_instance)
def fz_story_positions( story, python_callback):
    #log( f'fz_story_positions() type(story)={type(story)!r} type(python_callback)={type(python_callback)!r}')
    assert isinstance( story, FzStory)
    assert callable( python_callback)
    def python_callback2( position):
        position2 = FzStoryElementPosition( position)
        python_callback( position2)
    ll_fz_story_positions( story.m_internal, python_callback2)
FzStory.fz_story_positions = fz_story_positions

# Monkey-patch `FzDocumentWriter.__init__()` to set `self._out`
# to any `FzOutput2` arg. This ensures that the Python part of
# the derived `FzOutput2` instance is kept alive for use by the
# `FzDocumentWriter`, otherwise Python can delete it, then get
# a SEGV if C++ tries to call the derived Python methods.
#
# [We don't patch equivalent class-aware functions such
# as `fz_new_pdf_writer_with_output()` because they are
# not available to C++/Python, because FzDocumentWriter is
# non-copyable.]
#
FzDocumentWriter__init__0 = FzDocumentWriter.__init__
def FzDocumentWriter__init__1(self, *args):
    out = None
    for arg in args:
        if isinstance( arg, FzOutput2):
            assert not out, "More than one FzOutput2 passed to FzDocumentWriter.__init__()"
            out = arg
    if out:
        self._out = out
    return FzDocumentWriter__init__0(self, *args)
FzDocumentWriter.__init__ = FzDocumentWriter__init__1
FzLink.__iter__ = lambda self: IteratorWrap( self)
FzStextBlock.__iter__ = lambda self: IteratorWrap( self)
FzStextLine.__iter__ = lambda self: IteratorWrap( self)
FzStextPage.__iter__ = lambda self: IteratorWrap( self)
fz_aa_context.__str__ = lambda s: to_string_fz_aa_context(s)
fz_aa_context.__repr__ = lambda s: to_string_fz_aa_context(s)
fz_color_params.__str__ = lambda s: to_string_fz_color_params(s)
fz_color_params.__repr__ = lambda s: to_string_fz_color_params(s)
fz_cookie.__str__ = lambda s: to_string_fz_cookie(s)
fz_cookie.__repr__ = lambda s: to_string_fz_cookie(s)
fz_draw_options.__str__ = lambda s: to_string_fz_draw_options(s)
fz_draw_options.__repr__ = lambda s: to_string_fz_draw_options(s)
fz_irect.__str__ = lambda s: to_string_fz_irect(s)
fz_irect.__repr__ = lambda s: to_string_fz_irect(s)
fz_location.__str__ = lambda s: to_string_fz_location(s)
fz_location.__repr__ = lambda s: to_string_fz_location(s)
fz_matrix.__str__ = lambda s: to_string_fz_matrix(s)
fz_matrix.__repr__ = lambda s: to_string_fz_matrix(s)
fz_md5.__str__ = lambda s: to_string_fz_md5(s)
fz_md5.__repr__ = lambda s: to_string_fz_md5(s)
fz_pdfocr_options.__str__ = lambda s: to_string_fz_pdfocr_options(s)
fz_pdfocr_options.__repr__ = lambda s: to_string_fz_pdfocr_options(s)
fz_point.__str__ = lambda s: to_string_fz_point(s)
fz_point.__repr__ = lambda s: to_string_fz_point(s)
fz_pwg_options.__str__ = lambda s: to_string_fz_pwg_options(s)
fz_pwg_options.__repr__ = lambda s: to_string_fz_pwg_options(s)
fz_quad.__str__ = lambda s: to_string_fz_quad(s)
fz_quad.__repr__ = lambda s: to_string_fz_quad(s)
fz_rect.__str__ = lambda s: to_string_fz_rect(s)
fz_rect.__repr__ = lambda s: to_string_fz_rect(s)
fz_stext_options.__str__ = lambda s: to_string_fz_stext_options(s)
fz_stext_options.__repr__ = lambda s: to_string_fz_stext_options(s)
fz_story_element_position.__str__ = lambda s: to_string_fz_story_element_position(s)
fz_story_element_position.__repr__ = lambda s: to_string_fz_story_element_position(s)
fz_transition.__str__ = lambda s: to_string_fz_transition(s)
fz_transition.__repr__ = lambda s: to_string_fz_transition(s)
pdf_filter_factory.__str__ = lambda s: to_string_pdf_filter_factory(s)
pdf_filter_factory.__repr__ = lambda s: to_string_pdf_filter_factory(s)
pdf_filter_options.__str__ = lambda s: to_string_pdf_filter_options(s)
pdf_filter_options.__repr__ = lambda s: to_string_pdf_filter_options(s)
pdf_layer_config.__str__ = lambda s: to_string_pdf_layer_config(s)
pdf_layer_config.__repr__ = lambda s: to_string_pdf_layer_config(s)
pdf_layer_config_ui.__str__ = lambda s: to_string_pdf_layer_config_ui(s)
pdf_layer_config_ui.__repr__ = lambda s: to_string_pdf_layer_config_ui(s)
pdf_redact_options.__str__ = lambda s: to_string_pdf_redact_options(s)
pdf_redact_options.__repr__ = lambda s: to_string_pdf_redact_options(s)
pdf_sanitize_filter_options.__str__ = lambda s: to_string_pdf_sanitize_filter_options(s)
pdf_sanitize_filter_options.__repr__ = lambda s: to_string_pdf_sanitize_filter_options(s)
pdf_write_options.__str__ = lambda s: to_string_pdf_write_options(s)
pdf_write_options.__repr__ = lambda s: to_string_pdf_write_options(s)
FzAaContext.__str__ = lambda self: self.to_string()
FzAaContext.__repr__ = lambda self: self.to_string()
FzColorParams.__str__ = lambda self: self.to_string()
FzColorParams.__repr__ = lambda self: self.to_string()
FzCookie.__str__ = lambda self: self.to_string()
FzCookie.__repr__ = lambda self: self.to_string()
FzDrawOptions.__str__ = lambda self: self.to_string()
FzDrawOptions.__repr__ = lambda self: self.to_string()
FzIrect.__str__ = lambda self: self.to_string()
FzIrect.__repr__ = lambda self: self.to_string()
FzLocation.__str__ = lambda self: self.to_string()
FzLocation.__repr__ = lambda self: self.to_string()
FzMatrix.__str__ = lambda self: self.to_string()
FzMatrix.__repr__ = lambda self: self.to_string()
FzMd5.__str__ = lambda self: self.to_string()
FzMd5.__repr__ = lambda self: self.to_string()
FzPdfocrOptions.__str__ = lambda self: self.to_string()
FzPdfocrOptions.__repr__ = lambda self: self.to_string()
FzPoint.__str__ = lambda self: self.to_string()
FzPoint.__repr__ = lambda self: self.to_string()
FzPwgOptions.__str__ = lambda self: self.to_string()
FzPwgOptions.__repr__ = lambda self: self.to_string()
FzQuad.__str__ = lambda self: self.to_string()
FzQuad.__repr__ = lambda self: self.to_string()
FzRect.__str__ = lambda self: self.to_string()
FzRect.__repr__ = lambda self: self.to_string()
FzStextOptions.__str__ = lambda self: self.to_string()
FzStextOptions.__repr__ = lambda self: self.to_string()
FzStoryElementPosition.__str__ = lambda self: self.to_string()
FzStoryElementPosition.__repr__ = lambda self: self.to_string()
FzTransition.__str__ = lambda self: self.to_string()
FzTransition.__repr__ = lambda self: self.to_string()
PdfFilterFactory.__str__ = lambda self: self.to_string()
PdfFilterFactory.__repr__ = lambda self: self.to_string()
PdfFilterOptions.__str__ = lambda self: self.to_string()
PdfFilterOptions.__repr__ = lambda self: self.to_string()
PdfLayerConfig.__str__ = lambda self: self.to_string()
PdfLayerConfig.__repr__ = lambda self: self.to_string()
PdfLayerConfigUi.__str__ = lambda self: self.to_string()
PdfLayerConfigUi.__repr__ = lambda self: self.to_string()
PdfRedactOptions.__str__ = lambda self: self.to_string()
PdfRedactOptions.__repr__ = lambda self: self.to_string()
PdfSanitizeFilterOptions.__str__ = lambda self: self.to_string()
PdfSanitizeFilterOptions.__repr__ = lambda self: self.to_string()
PdfWriteOptions.__str__ = lambda self: self.to_string()
PdfWriteOptions.__repr__ = lambda self: self.to_string()
%}
