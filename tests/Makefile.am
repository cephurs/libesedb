AM_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/common \
	@LIBCERROR_CPPFLAGS@ \
	@LIBCDATA_CPPFLAGS@ \
	@LIBCLOCALE_CPPFLAGS@ \
	@LIBCNOTIFY_CPPFLAGS@ \
	@LIBCSPLIT_CPPFLAGS@ \
	@LIBUNA_CPPFLAGS@ \
	@LIBCFILE_CPPFLAGS@ \
	@LIBCPATH_CPPFLAGS@ \
	@LIBBFIO_CPPFLAGS@ \
	@LIBFCACHE_CPPFLAGS@ \
	@LIBFDATA_CPPFLAGS@ \
	@LIBFDATETIME_CPPFLAGS@ \
	@LIBFGUID_CPPFLAGS@ \
	@LIBFVALUE_CPPFLAGS@ \
	@LIBFWNT_CPPFLAGS@ \
	@LIBCSYSTEM_CPPFLAGS@ \
	@PTHREAD_CPPFLAGS@ \
	@LIBESEDB_DLL_IMPORT@

if HAVE_PYTHON_TESTS
TESTS_PYESEDB = \
	test_python_functions.sh
endif

TESTS = \
	test_api_functions.sh \
	test_api_types.sh \
	test_esedbinfo.sh \
	test_esedbexport.sh \
	$(TESTS_PYESEDB)

check_SCRIPTS = \
	pyesedb_test_get_version.py \
	pyesedb_test_open_close.py \
	test_api_functions.sh \
	test_api_types.sh \
	test_esedbexport.sh \
	test_esedbinfo.sh \
	test_python_functions.sh \
	test_runner.sh

EXTRA_DIST = \
	$(check_SCRIPTS)

check_PROGRAMS = \
	esedb_test_catalog \
	esedb_test_catalog_definition \
	esedb_test_column \
	esedb_test_column_type \
	esedb_test_data_definition \
	esedb_test_data_segment \
	esedb_test_database \
	esedb_test_error \
	esedb_test_file \
	esedb_test_index \
	esedb_test_io_handle \
	esedb_test_key \
	esedb_test_long_value \
	esedb_test_multi_value \
	esedb_test_notify \
	esedb_test_page \
	esedb_test_page_tree \
	esedb_test_page_value \
	esedb_test_record \
	esedb_test_support \
	esedb_test_table \
	esedb_test_table_definition

esedb_test_catalog_SOURCES = \
	esedb_test_catalog.c \
	esedb_test_libcerror.h \
	esedb_test_libesedb.h \
	esedb_test_macros.h \
	esedb_test_memory.c esedb_test_memory.h \
	esedb_test_unused.h

esedb_test_catalog_LDADD = \
	../libesedb/libesedb.la \
	@LIBCERROR_LIBADD@

esedb_test_catalog_definition_SOURCES = \
	esedb_test_catalog_definition.c \
	esedb_test_libcerror.h \
	esedb_test_libesedb.h \
	esedb_test_macros.h \
	esedb_test_memory.c esedb_test_memory.h \
	esedb_test_unused.h

esedb_test_catalog_definition_LDADD = \
	../libesedb/libesedb.la \
	@LIBCERROR_LIBADD@

esedb_test_column_SOURCES = \
	esedb_test_column.c \
	esedb_test_libcerror.h \
	esedb_test_libesedb.h \
	esedb_test_macros.h \
	esedb_test_memory.c esedb_test_memory.h \
	esedb_test_unused.h

esedb_test_column_LDADD = \
	../libesedb/libesedb.la \
	@LIBCERROR_LIBADD@

esedb_test_column_type_SOURCES = \
	esedb_test_column_type.c \
	esedb_test_libcerror.h \
	esedb_test_libesedb.h \
	esedb_test_macros.h \
	esedb_test_memory.c esedb_test_memory.h \
	esedb_test_unused.h

esedb_test_column_type_LDADD = \
	../libesedb/libesedb.la \
	@LIBCERROR_LIBADD@

esedb_test_data_definition_SOURCES = \
	esedb_test_data_definition.c \
	esedb_test_libcerror.h \
	esedb_test_libesedb.h \
	esedb_test_macros.h \
	esedb_test_memory.c esedb_test_memory.h \
	esedb_test_unused.h

esedb_test_data_definition_LDADD = \
	../libesedb/libesedb.la \
	@LIBCERROR_LIBADD@

esedb_test_data_segment_SOURCES = \
	esedb_test_data_segment.c \
	esedb_test_libcerror.h \
	esedb_test_libesedb.h \
	esedb_test_macros.h \
	esedb_test_memory.c esedb_test_memory.h \
	esedb_test_unused.h

esedb_test_data_segment_LDADD = \
	../libesedb/libesedb.la \
	@LIBCERROR_LIBADD@

esedb_test_database_SOURCES = \
	esedb_test_database.c \
	esedb_test_libcerror.h \
	esedb_test_libesedb.h \
	esedb_test_macros.h \
	esedb_test_memory.c esedb_test_memory.h \
	esedb_test_unused.h

esedb_test_database_LDADD = \
	../libesedb/libesedb.la \
	@LIBCERROR_LIBADD@

esedb_test_error_SOURCES = \
	esedb_test_error.c \
	esedb_test_libesedb.h \
	esedb_test_macros.h \
	esedb_test_unused.h

esedb_test_error_LDADD = \
	../libesedb/libesedb.la

esedb_test_file_SOURCES = \
	esedb_test_file.c \
	esedb_test_libcerror.h \
	esedb_test_libclocale.h \
	esedb_test_libcsystem.h \
	esedb_test_libesedb.h \
	esedb_test_libuna.h \
	esedb_test_macros.h \
	esedb_test_memory.c esedb_test_memory.h

esedb_test_file_LDADD = \
	@LIBCSYSTEM_LIBADD@ \
	@LIBUNA_LIBADD@ \
	@LIBCNOTIFY_LIBADD@ \
	@LIBCLOCALE_LIBADD@ \
	@LIBCDATA_LIBADD@ \
	../libesedb/libesedb.la \
	@LIBCTHREADS_LIBADD@ \
	@LIBCERROR_LIBADD@ \
	@PTHREAD_LIBADD@

esedb_test_index_SOURCES = \
	esedb_test_index.c \
	esedb_test_libcerror.h \
	esedb_test_libesedb.h \
	esedb_test_macros.h \
	esedb_test_memory.c esedb_test_memory.h \
	esedb_test_unused.h

esedb_test_index_LDADD = \
	../libesedb/libesedb.la \
	@LIBCERROR_LIBADD@

esedb_test_io_handle_SOURCES = \
	esedb_test_io_handle.c \
	esedb_test_libcerror.h \
	esedb_test_libesedb.h \
	esedb_test_macros.h \
	esedb_test_memory.c esedb_test_memory.h \
	esedb_test_unused.h

esedb_test_io_handle_LDADD = \
	../libesedb/libesedb.la \
	@LIBCERROR_LIBADD@

esedb_test_key_SOURCES = \
	esedb_test_key.c \
	esedb_test_libcerror.h \
	esedb_test_libesedb.h \
	esedb_test_libfdata.h \
	esedb_test_macros.h \
	esedb_test_memory.c esedb_test_memory.h \
	esedb_test_unused.h

esedb_test_key_LDADD = \
	../libesedb/libesedb.la \
	@LIBCERROR_LIBADD@

esedb_test_long_value_SOURCES = \
	esedb_test_libcerror.h \
	esedb_test_libesedb.h \
	esedb_test_long_value.c \
	esedb_test_macros.h \
	esedb_test_memory.c esedb_test_memory.h \
	esedb_test_unused.h

esedb_test_long_value_LDADD = \
	../libesedb/libesedb.la \
	@LIBCERROR_LIBADD@

esedb_test_multi_value_SOURCES = \
	esedb_test_libcerror.h \
	esedb_test_libesedb.h \
	esedb_test_macros.h \
	esedb_test_memory.c esedb_test_memory.h \
	esedb_test_multi_value.c \
	esedb_test_unused.h

esedb_test_multi_value_LDADD = \
	../libesedb/libesedb.la \
	@LIBCERROR_LIBADD@

esedb_test_notify_SOURCES = \
	esedb_test_libesedb.h \
	esedb_test_macros.h \
	esedb_test_notify.c \
	esedb_test_unused.h

esedb_test_notify_LDADD = \
	../libesedb/libesedb.la

esedb_test_page_SOURCES = \
	esedb_test_libcerror.h \
	esedb_test_libesedb.h \
	esedb_test_macros.h \
	esedb_test_memory.c esedb_test_memory.h \
	esedb_test_page.c \
	esedb_test_unused.h

esedb_test_page_LDADD = \
	../libesedb/libesedb.la \
	@LIBCERROR_LIBADD@

esedb_test_page_tree_SOURCES = \
	esedb_test_libcerror.h \
	esedb_test_libesedb.h \
	esedb_test_macros.h \
	esedb_test_memory.c esedb_test_memory.h \
	esedb_test_page_tree.c \
	esedb_test_unused.h

esedb_test_page_tree_LDADD = \
	../libesedb/libesedb.la \
	@LIBCERROR_LIBADD@

esedb_test_page_value_SOURCES = \
	esedb_test_libcerror.h \
	esedb_test_libesedb.h \
	esedb_test_macros.h \
	esedb_test_memory.c esedb_test_memory.h \
	esedb_test_page_value.c \
	esedb_test_unused.h

esedb_test_page_value_LDADD = \
	../libesedb/libesedb.la \
	@LIBCERROR_LIBADD@

esedb_test_record_SOURCES = \
	esedb_test_libcerror.h \
	esedb_test_libesedb.h \
	esedb_test_macros.h \
	esedb_test_memory.c esedb_test_memory.h \
	esedb_test_record.c \
	esedb_test_unused.h

esedb_test_record_LDADD = \
	../libesedb/libesedb.la \
	@LIBCERROR_LIBADD@

esedb_test_support_SOURCES = \
	esedb_test_libbfio.h \
	esedb_test_libcerror.h \
	esedb_test_libclocale.h \
	esedb_test_libcsystem.h \
	esedb_test_libesedb.h \
	esedb_test_libuna.h \
	esedb_test_macros.h \
	esedb_test_support.c

esedb_test_support_LDADD = \
	@LIBCSYSTEM_LIBADD@ \
	@LIBBFIO_LIBADD@ \
	@LIBCPATH_LIBADD@ \
	@LIBCFILE_LIBADD@ \
	@LIBUNA_LIBADD@ \
	@LIBCNOTIFY_LIBADD@ \
	@LIBCLOCALE_LIBADD@ \
	@LIBCDATA_LIBADD@ \
	../libesedb/libesedb.la \
	@LIBCERROR_LIBADD@

esedb_test_table_SOURCES = \
	esedb_test_libcerror.h \
	esedb_test_libesedb.h \
	esedb_test_macros.h \
	esedb_test_memory.c esedb_test_memory.h \
	esedb_test_table.c \
	esedb_test_unused.h

esedb_test_table_LDADD = \
	../libesedb/libesedb.la \
	@LIBCERROR_LIBADD@

esedb_test_table_definition_SOURCES = \
	esedb_test_libcerror.h \
	esedb_test_libesedb.h \
	esedb_test_macros.h \
	esedb_test_memory.c esedb_test_memory.h \
	esedb_test_table_definition.c \
	esedb_test_unused.h

esedb_test_table_definition_LDADD = \
	../libesedb/libesedb.la \
	@LIBCERROR_LIBADD@

MAINTAINERCLEANFILES = \
	Makefile.in

distclean: clean
	/bin/rm -f Makefile

