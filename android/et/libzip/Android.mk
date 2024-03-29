LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libzip

LOCAL_SRC_FILES := mkstemp.c zip_add_dir.c zip_add.c zip_close.c zip_delete.c zip_dirent.c \
	zip_entry_free.c zip_entry_new.c zip_err_str.c zip_error_clear.c zip_error_get_sys_type.c \
	zip_error_get.c zip_error_strerror.c zip_error_to_str.c zip_error.c zip_fclose.c \
	zip_file_error_clear.c zip_file_error_get.c zip_file_get_offset.c zip_file_strerror.c \
	zip_filerange_crc.c zip_fopen_index.c zip_fopen.c zip_fread.c zip_free.c zip_get_archive_comment.c \
	zip_get_archive_flag.c zip_get_file_comment.c zip_get_name.c zip_get_num_files.c zip_memdup.c \
	zip_name_locate.c zip_new.c zip_open.c zip_rename.c zip_replace.c zip_set_archive_comment.c \
	zip_set_archive_flag.c zip_set_file_comment.c zip_set_name.c zip_source_buffer.c zip_source_file.c \
	zip_source_filep.c zip_source_free.c zip_source_function.c zip_source_zip.c zip_stat_index.c \
	zip_stat_init.c zip_stat.c zip_strerror.c zip_unchange_all.c zip_unchange_archive.c zip_unchange_data.c \
	zip_unchange.c
	
include $(BUILD_STATIC_LIBRARY)
