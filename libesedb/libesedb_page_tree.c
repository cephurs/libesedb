/*
 * Page tree functions
 *
 * Copyright (c) 2009, Joachim Metz <forensics@hoffmannbv.nl>,
 * Hoffmann Investigations. All rights reserved.
 *
 * Refer to AUTHORS for acknowledgements.
 *
 * This software is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this software.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <common.h>
#include <memory.h>
#include <types.h>

#include <liberror.h>
#include <libnotify.h>

#include "libesedb_catalog_definition.h"
#include "libesedb_data_definition.h"
#include "libesedb_debug.h"
#include "libesedb_definitions.h"
#include "libesedb_debug.h"
#include "libesedb_libuna.h"
#include "libesedb_list_type.h"
#include "libesedb_page.h"
#include "libesedb_page_tree.h"
#include "libesedb_string.h"
#include "libesedb_table_definition.h"

#include "esedb_page_values.h"

/* Creates a page tree
 * Returns 1 if successful or -1 on error
 */
int libesedb_page_tree_initialize(
     libesedb_page_tree_t **page_tree,
     libesedb_table_definition_t *table_definition,
     liberror_error_t **error )
{
	static char *function = "libesedb_page_tree_initialize";

	if( page_tree == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid page tree.",
		 function );

		return( -1 );
	}
	if( *page_tree == NULL )
	{
		*page_tree = (libesedb_page_tree_t *) memory_allocate(
		                                       sizeof( libesedb_page_tree_t ) );

		if( *page_tree == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_MEMORY,
			 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
			 "%s: unable to create page tree.",
			 function );

			return( -1 );
		}
		if( memory_set(
		     *page_tree,
		     0,
		     sizeof( libesedb_page_tree_t ) ) == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_MEMORY,
			 LIBERROR_MEMORY_ERROR_SET_FAILED,
			 "%s: unable to clear page tree.",
			 function );

			memory_free(
			 *page_tree );

			*page_tree = NULL;

			return( -1 );
		}
		if( libesedb_list_initialize(
		     &( ( *page_tree )->table_definition_list ),
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create table definition list.",
			 function );

			memory_free(
			 *page_tree );

			*page_tree = NULL;

			return( -1 );
		}
		if( libesedb_list_initialize(
		     &( ( *page_tree )->value_definition_list ),
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create value definition list.",
			 function );

			libesedb_list_free(
			 &( ( *page_tree )->table_definition_list ),
			 NULL,
			 NULL );
			memory_free(
			 *page_tree );

			*page_tree = NULL;

			return( -1 );
		}
		( *page_tree )->table_definition = table_definition;
	}
	return( 1 );
}

/* Frees page tree
 * Returns 1 if successful or -1 on error
 */
int libesedb_page_tree_free(
     libesedb_page_tree_t **page_tree,
     liberror_error_t **error )
{
	static char *function = "libesedb_page_tree_free";
	int result            = 1;

	if( page_tree == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid page tree.",
		 function );

		return( -1 );
	}
	if( *page_tree != NULL )
	{
		if( libesedb_list_free(
		     &( ( *page_tree )->table_definition_list ),
		     &libesedb_table_definition_free,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free table definition list.",
			 function );

			result = -1;
		}
		if( libesedb_list_free(
		     &( ( *page_tree )->value_definition_list ),
		     &libesedb_data_definition_free,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free value definition list.",
			 function );

			result = -1;
		}
		memory_free(
		 *page_tree );

		*page_tree = NULL;
	}
	return( result );
}

/* Retrieves the table definition with the specified identifier
 * Returns 1 if successful, 0 if no corresponding table definition was found or -1 on error
 */
int libesedb_page_tree_get_table_definition_by_identifier(
     libesedb_page_tree_t *page_tree,
     uint32_t identifier,
     libesedb_table_definition_t **table_definition,
     liberror_error_t **error )
{
	libesedb_list_element_t *list_element = NULL;
	static char *function                 = "libesedb_page_tree_get_table_definition_by_identifier";
	int list_element_iterator             = 0;

	if( page_tree == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid page tree.",
		 function );

		return( -1 );
	}
	if( page_tree->table_definition_list == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid page tree - missing table definition list.",
		 function );

		return( -1 );
	}
	if( table_definition == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid table definition.",
		 function );

		return( -1 );
	}
	list_element = page_tree->table_definition_list->first;

	for( list_element_iterator = 0;
	     list_element_iterator < page_tree->table_definition_list->amount_of_elements;
	     list_element_iterator++ )
	{
		if( list_element == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: corruption detected for element: %d.",
			 function,
			 list_element_iterator + 1 );

			return( -1 );
		}
		if( list_element->value == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: missing table definition for list element: %d.",
			 function,
			 list_element_iterator + 1 );

			return( -1 );
		}
		*table_definition = (libesedb_table_definition_t *) list_element->value;

		if( ( *table_definition )->table_catalog_definition == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: missing table catalog definition for list element: %d.",
			 function,
			 list_element_iterator + 1 );

			return( -1 );
		}
		if( ( *table_definition )->table_catalog_definition->identifier == identifier )
		{
			return( 1 );
		}
		list_element = list_element->next;
	}
	*table_definition = NULL;

	return( 0 );
}

/* Reads a page tree and its values
 * Returns 1 if successful or -1 on error
 */
int libesedb_page_tree_read(
     libesedb_page_tree_t *page_tree,
     libesedb_io_handle_t *io_handle,
     uint32_t father_data_page_number,
     uint8_t flags,
     liberror_error_t **error )
{
	libesedb_page_t *page = NULL;
	static char *function = "libesedb_page_tree_read";

	if( page_tree == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid page tree.",
		 function );

		return( -1 );
	}
	if( io_handle == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid io handle.",
		 function );

		return( -1 );
	}
#if defined( HAVE_DEBUG_OUTPUT )
	libnotify_verbose_printf(
	 "%s: reading page tree with FDP number\t\t: %" PRIu32 "\n",
	 function,
	 father_data_page_number );
#endif

	if( libesedb_page_initialize(
	     &page,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create page.",
		 function );

		return( -1 );
	}
	if( libesedb_page_read(
	     page,
	     io_handle,
	     father_data_page_number,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_IO,
		 LIBERROR_IO_ERROR_READ_FAILED,
		 "%s: unable to read page: %" PRIu32 ".",
		 function,
		 father_data_page_number );

		libesedb_page_free(
		 &page,
		 NULL );

		return( -1 );
	}
	if( ( page->flags & LIBESEDB_PAGE_FLAG_IS_LEAF ) == LIBESEDB_PAGE_FLAG_IS_LEAF )
	{
		if( libesedb_page_tree_read_leaf_page_values(
		     page_tree,
		     page,
		     io_handle,
		     flags,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_IO,
			 LIBERROR_IO_ERROR_READ_FAILED,
			 "%s: unable to read leaf page values.",
			 function );

			return( -1 );
		}
	}
	else if( ( page->flags & LIBESEDB_PAGE_FLAG_IS_ROOT ) == LIBESEDB_PAGE_FLAG_IS_ROOT )
	{
		if( libesedb_page_tree_read_father_data_page_values(
		     page_tree,
		     page,
		     io_handle,
		     flags,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_IO,
			 LIBERROR_IO_ERROR_READ_FAILED,
			 "%s: unable to read father data page values.",
			 function );

			return( -1 );
		}
	}
	if( libesedb_page_free(
	     &page,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_FINALIZE_FAILED,
		 "%s: unable to free page.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Reads the father data page values from the page
 * Returns 1 if successful or -1 on error
 */
int libesedb_page_tree_read_father_data_page_values(
     libesedb_page_tree_t *page_tree,
     libesedb_page_t *page,
     libesedb_io_handle_t *io_handle,
     uint8_t flags,
     liberror_error_t **error )
{
	libesedb_page_t *space_tree_page  = NULL;
	libesedb_page_value_t *page_value = NULL;
	static char *function             = "libesedb_page_tree_read_father_data_page_values";
	uint32_t required_flags           = 0;
	uint32_t space_tree_page_number   = 0;
	uint32_t supported_flags          = 0;

#if defined( HAVE_DEBUG_OUTPUT )
	uint32_t extent_space             = 0;
	uint32_t initial_amount_of_pages  = 0;
	uint32_t test                     = 0;
#endif

	if( page_tree == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid page tree.",
		 function );

		return( -1 );
	}
	if( page == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid page.",
		 function );

		return( -1 );
	}
	if( io_handle == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid io handle.",
		 function );

		return( -1 );
	}
	required_flags = LIBESEDB_PAGE_FLAG_IS_ROOT;

	if( ( page->flags & required_flags ) != required_flags )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing required page flags: 0x%08" PRIx32 ".",
		 function,
		 page->flags );

		return( -1 );
	}
	supported_flags = required_flags
	                | LIBESEDB_PAGE_FLAG_IS_PARENT
	                | LIBESEDB_PAGE_FLAG_IS_INDEX
	                | LIBESEDB_PAGE_FLAG_IS_LONG_VALUE
	                | LIBESEDB_PAGE_FLAG_IS_PRIMARY
	                | LIBESEDB_PAGE_FLAG_IS_NEW_RECORD_FORMAT;

	if( ( page->flags & ~supported_flags ) != 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported page flags: 0x%08" PRIx32 ".",
		 function,
		 page->flags );

		return( -1 );
	}
	if( page->previous_page_number != 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported previous page number: %" PRIu32 ".",
		 function,
		 page->previous_page_number );

		return( -1 );
	}
	if( page->next_page_number != 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported next page number: %" PRIu32 ".",
		 function,
		 page->next_page_number );

		return( -1 );
	}
	if( libesedb_page_get_value(
	     page,
	     0,
	     &page_value,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve page value: 0.",
		 function );

		return( -1 );
	}
	if( page_value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid page value.",
		 function );

		return( -1 );
	}
	endian_little_convert_32bit(
	 space_tree_page_number,
	 ( (esedb_father_data_page_header_t *) page_value->data )->space_tree_page_number );

	/* TODO handle the root page header */

#if defined( HAVE_DEBUG_OUTPUT )
	endian_little_convert_32bit(
	 initial_amount_of_pages,
	 ( (esedb_father_data_page_header_t *) page_value->data )->initial_amount_of_pages );
	libnotify_verbose_printf(
	 "%s: header initial amount of pages\t: %" PRIu32 "\n",
	 function,
	 initial_amount_of_pages );

	endian_little_convert_32bit(
	 test,
	 ( (esedb_father_data_page_header_t *) page_value->data )->parent_father_data_page_number );
	libnotify_verbose_printf(
	 "%s: header parent FDP number\t: %" PRIu32 "\n",
	 function,
	 test );

	endian_little_convert_32bit(
	 extent_space,
	 ( (esedb_father_data_page_header_t *) page_value->data )->extent_space );
	libnotify_verbose_printf(
	 "%s: header extent space\t\t: %" PRIu32 "\n",
	 function,
	 extent_space );

	libnotify_verbose_printf(
	 "%s: header space tree page number\t: %" PRIu32 " (0x%08" PRIx32 ")\n",
	 function,
	 space_tree_page_number,
	 space_tree_page_number );

	libnotify_verbose_printf(
	 "%s: header primary extent\t\t: %" PRIu32 "-%c\n",
	 function,
	 initial_amount_of_pages,
	 ( extent_space == 0 ? 's' : 'm' ) );

	libnotify_verbose_printf(
	 "\n" );
#endif

	/* Read the space tree pages
	 */
	if( extent_space > 0 )
	{
		if( ( space_tree_page_number == 0 )
		 && ( space_tree_page_number >= 0xff000000UL ) )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
			 "%s: unsupported space tree page number.",
			 function,
			 space_tree_page_number );

			libesedb_page_free(
			 &space_tree_page,
			 NULL );

			return( -1 );
		}
		/* Read the owned pages space tree page
		 */
		if( libesedb_page_initialize(
		     &space_tree_page,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create space tree page.",
			 function );

			return( -1 );
		}
		if( libesedb_page_read(
		     space_tree_page,
		     io_handle,
		     space_tree_page_number,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_IO,
			 LIBERROR_IO_ERROR_READ_FAILED,
			 "%s: unable to read space tree page: %" PRIu32 ".",
			 function,
			 space_tree_page_number );

			libesedb_page_free(
			 &space_tree_page,
			 NULL );

			return( -1 );
		}
		if( page->father_data_page_object_identifier != space_tree_page->father_data_page_object_identifier )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
			 "%s: mismatch in father data page object identifier (%" PRIu32 " != %" PRIu32 ").",
			 function,
			 page->father_data_page_object_identifier,
			 space_tree_page->father_data_page_object_identifier );

			libesedb_page_free(
			 &space_tree_page,
			 NULL );

			return( -1 );
		}
		if( libesedb_page_tree_read_space_tree_page_values(
		     page_tree,
		     space_tree_page,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_IO,
			 LIBERROR_IO_ERROR_READ_FAILED,
			 "%s: unable to read space tree page values.",
			 function );

			libesedb_page_free(
			 &space_tree_page,
			 NULL );

			return( -1 );
		}
		if( libesedb_page_free(
		     &space_tree_page,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free space tree page.",
			 function );

			return( -1 );
		}

		/* Read the available pages space tree page
		 */
		space_tree_page_number += 1;

		if( libesedb_page_initialize(
		     &space_tree_page,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create space tree page.",
			 function );

			return( -1 );
		}
		if( libesedb_page_read(
		     space_tree_page,
		     io_handle,
		     space_tree_page_number,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_IO,
			 LIBERROR_IO_ERROR_READ_FAILED,
			 "%s: unable to read space tree page: %" PRIu32 ".",
			 function,
			 space_tree_page_number );

			libesedb_page_free(
			 &space_tree_page,
			 NULL );

			return( -1 );
		}
		if( page->father_data_page_object_identifier != space_tree_page->father_data_page_object_identifier )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
			 "%s: mismatch in father data page object identifier (%" PRIu32 " != %" PRIu32 ").",
			 function,
			 page->father_data_page_object_identifier,
			 space_tree_page->father_data_page_object_identifier );

			libesedb_page_free(
			 &space_tree_page,
			 NULL );

			return( -1 );
		}
		if( libesedb_page_tree_read_space_tree_page_values(
		     page_tree,
		     space_tree_page,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_IO,
			 LIBERROR_IO_ERROR_READ_FAILED,
			 "%s: unable to read space tree page values.",
			 function );

			libesedb_page_free(
			 &space_tree_page,
			 NULL );

			return( -1 );
		}
		if( libesedb_page_free(
		     &space_tree_page,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free space tree page.",
			 function );

			return( -1 );
		}
	}
	/* Read the page values
	 */
	if( libesedb_page_tree_read_child_pages(
	     page_tree,
	     page,
	     io_handle,
	     flags,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_IO,
		 LIBERROR_IO_ERROR_READ_FAILED,
		 "%s: unable to read leaf page values.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Reads the child page values from a parent page
 * Returns 1 if successful or -1 on error
 */
int libesedb_page_tree_read_parent_page_values(
     libesedb_page_tree_t *page_tree,
     libesedb_page_t *page,
     libesedb_io_handle_t *io_handle,
     uint8_t flags,
     liberror_error_t **error )
{
	libesedb_page_value_t *page_value = NULL;
	static char *function             = "libesedb_page_tree_read_parent_page_values";
	uint32_t required_flags           = 0;
	uint32_t supported_flags          = 0;

	if( page_tree == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid page tree.",
		 function );

		return( -1 );
	}
	if( page == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid page.",
		 function );

		return( -1 );
	}
	if( io_handle == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid io handle.",
		 function );

		return( -1 );
	}
	required_flags = LIBESEDB_PAGE_FLAG_IS_PARENT;

	if( ( page->flags & required_flags ) != required_flags )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing required page flags: 0x%08" PRIx32 ".",
		 function,
		 page->flags );

		return( -1 );
	}
	supported_flags = required_flags
	                | LIBESEDB_PAGE_FLAG_IS_ROOT
	                | LIBESEDB_PAGE_FLAG_IS_INDEX
	                | LIBESEDB_PAGE_FLAG_IS_LONG_VALUE
	                | LIBESEDB_PAGE_FLAG_IS_PRIMARY
	                | LIBESEDB_PAGE_FLAG_IS_NEW_RECORD_FORMAT;

	if( ( page->flags & ~supported_flags ) != 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported page flags: 0x%08" PRIx32 ".",
		 function,
		 page->flags );

		return( -1 );
	}
	if( page->previous_page_number != 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported previous page number: %" PRIu32 ".",
		 function,
		 page->previous_page_number );

		return( -1 );
	}
	if( page->next_page_number != 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported next page number: %" PRIu32 ".",
		 function,
		 page->next_page_number );

		return( -1 );
	}
	if( libesedb_page_get_value(
	     page,
	     0,
	     &page_value,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve page value: 0.",
		 function );

		return( -1 );
	}
	if( page_value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid page value.",
		 function );

		return( -1 );
	}
#if defined( HAVE_DEBUG_OUTPUT )
	libnotify_verbose_printf(
	 "%s: header:\n",
	 function );
	libnotify_verbose_print_data(
	 page_value->data,
	 page_value->size );
#endif

	/* Read the page values
	 */
	if( libesedb_page_tree_read_child_pages(
	     page_tree,
	     page,
	     io_handle,
	     flags,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_IO,
		 LIBERROR_IO_ERROR_READ_FAILED,
		 "%s: unable to read leaf page values.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Reads the child pages values from a parent page
 * Returns 1 if successful or -1 on error
 */
int libesedb_page_tree_read_child_pages(
     libesedb_page_tree_t *page_tree,
     libesedb_page_t *page,
     libesedb_io_handle_t *io_handle,
     uint8_t flags,
     liberror_error_t **error )
{
	libesedb_page_t *child_page              = NULL;
	libesedb_page_value_t *page_value        = NULL;
	uint8_t *page_value_data                 = NULL;
	static char *function                    = "libesedb_page_tree_read_child_pages";
	uint32_t child_page_number               = 0;
	uint32_t previous_child_page_number      = 0;
	uint32_t previous_next_child_page_number = 0;
	uint16_t amount_of_page_values           = 0;
	uint16_t page_value_iterator             = 0;
	uint16_t page_value_size                 = 0;

#if defined( HAVE_DEBUG_OUTPUT )
	uint16_t page_key_size                   = 0;
	uint16_t page_key_type                   = 0;
#endif

	if( page_tree == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid page tree.",
		 function );

		return( -1 );
	}
	if( page == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid page.",
		 function );

		return( -1 );
	}
	if( io_handle == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid io handle.",
		 function );

		return( -1 );
	}
	if( libesedb_page_get_amount_of_values(
	     page,
	     &amount_of_page_values,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve amount of page values.",
		 function );

		return( -1 );
	}
	/* Read the page values
	 */
	for( page_value_iterator = 1;
	     page_value_iterator < amount_of_page_values;
	     page_value_iterator++ )
	{
		if( libesedb_page_get_value(
		     page,
		     page_value_iterator,
		     &page_value,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve page value: %" PRIu16 ".",
			 function,
			 page_value_iterator );

			return( -1 );
		}
		/* TODO handle leaf page values */

		page_value_data = page_value->data;
		page_value_size = page_value->size;

#if defined( HAVE_DEBUG_OUTPUT )
		if( ( page_value->flags & 0x04 ) == 0x04 )
		{
			endian_little_convert_16bit(
			 page_key_type,
			 page_value_data );

			page_value_data += 2;
			page_value_size -= 2;

#if defined( HAVE_DEBUG_OUTPUT )
			libnotify_verbose_printf(
			 "%s: value: %03d key type\t\t: 0x%04" PRIx32 " (%" PRIu32 ")\n",
			 function,
			 page_value_iterator,
			 page_key_type,
			 page_key_type );
#endif
		}
		else if( ( page_value->flags & 0x7 ) != 0 )
		{
			libnotify_verbose_printf(
			 "MARKER: unsupported page value flags: 0x%02" PRIx8 "\n",
			 page_value->flags );

			libnotify_verbose_print_data(
			 page_value_data,
			 page_value_size );
		}
		endian_little_convert_16bit(
		 page_key_size,
		 page_value_data );

		page_value_data += 2;
		page_value_size -= 2;

		libnotify_verbose_printf(
		 "%s: value: %03d highest key size\t: %" PRIu16 "\n",
		 function,
		 page_value_iterator,
		 page_key_size );

		if( page_key_size > page_value_size )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_RANGE,
			 "%s: page key size exceeds page value size.",
			 function );

			return( -1 );
		}
		libnotify_verbose_printf(
		 "%s: value: %03d highest key value\t: ",
		 function,
		 page_value_iterator );

		while( page_key_size > 0 )
		{
			libnotify_verbose_printf(
			 "%02" PRIx8 " ",
			 *page_value_data );

			page_value_data++;
			page_value_size--;
			page_key_size--;
		}
		libnotify_verbose_printf(
		 "\n" );

		endian_little_convert_32bit(
		 child_page_number,
		 page_value_data );

		libnotify_verbose_printf(
		 "%s: value: %03d child page number\t: %" PRIu32 "\n",
		 function,
		 page_value_iterator,
		 child_page_number );
		libnotify_verbose_printf(
		 "\n" );
#endif

		/* TODO can an upper bound be determined ?
		 */
		if( child_page_number >= 0x117f02 )
		{
#if defined( HAVE_DEBUG_OUTPUT )
			libnotify_verbose_printf(
			 "%s: value: %03d ignoring child page\n",
			 page_value_iterator );
#endif

			continue;
		}
		if( libesedb_page_initialize(
		     &child_page,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create child page.",
			 function );

			return( -1 );
		}
		if( libesedb_page_read(
		     child_page,
		     io_handle,
		     child_page_number,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_IO,
			 LIBERROR_IO_ERROR_READ_FAILED,
			 "%s: unable to read child page: %" PRIu32 ".",
			 function,
			 child_page_number );

			libesedb_page_free(
			 &child_page,
			 NULL );

			return( -1 );
		}
		if( page->father_data_page_object_identifier != child_page->father_data_page_object_identifier )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
			 "%s: mismatch in father data page object identifier (%" PRIu32 " != %" PRIu32 ").",
			 function,
			 page->father_data_page_object_identifier,
			 child_page->father_data_page_object_identifier );

			libesedb_page_free(
			 &child_page,
			 NULL );

			return( -1 );
		}
		if( ( child_page->flags & LIBESEDB_PAGE_FLAG_IS_LEAF ) == LIBESEDB_PAGE_FLAG_IS_LEAF )
		{
			if( page_value_iterator > 1 )
			{
				if( child_page->page_number != previous_next_child_page_number )
				{
					libnotify_verbose_printf(
					 "%s: mismatch in child page number (%" PRIu32 " != %" PRIu32 ").\n",
					 function,
					 previous_next_child_page_number,
					 child_page->page_number );
				}
				if( child_page->previous_page_number != previous_child_page_number )
				{
					libnotify_verbose_printf(
					 "%s: mismatch in previous child page number (%" PRIu32 " != %" PRIu32 ").\n",
					 function,
					 previous_child_page_number,
					 child_page->previous_page_number );
				}
			}
/* TODO need the actual values for the following checks
			if( page_value_iterator == 1 )
			{
				if( child_page->previous_page_number != 0 )
				{
					libnotify_verbose_printf(
					 "%s: mismatch in previous child page number (%" PRIu32 " != %" PRIu32 ").",
					 function,
					 0,
					 child_page->previous_page_number );
				}
			}
			if( page_value_iterator == ( amount_of_page_values - 1 ) )
			{
				if( child_page->next_page_number != 0 )
				{
					libnotify_verbose_printf(
					 "%s: mismatch in next child page number (%" PRIu32 " != %" PRIu32 ").",
					 function,
					 0,
					 child_page->previous_page_number );
				}
			}
*/
			previous_child_page_number      = child_page->page_number;
			previous_next_child_page_number = child_page->next_page_number;

			if( libesedb_page_tree_read_leaf_page_values(
			     page_tree,
			     child_page,
			     io_handle,
			     flags,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_IO,
				 LIBERROR_IO_ERROR_READ_FAILED,
				 "%s: unable to read leaf page values.",
				 function );

				libesedb_page_free(
				 &child_page,
				 NULL );

				return( -1 );
			}
		}
		else if( ( child_page->flags & LIBESEDB_PAGE_FLAG_IS_PARENT ) == LIBESEDB_PAGE_FLAG_IS_PARENT )
		{
			if( libesedb_page_tree_read_parent_page_values(
			     page_tree,
			     child_page,
			     io_handle,
			     flags,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_IO,
				 LIBERROR_IO_ERROR_READ_FAILED,
				 "%s: unable to read parent page values.",
				 function );

				libesedb_page_free(
				 &child_page,
				 NULL );

				return( -1 );
			}
		}
		if( libesedb_page_free(
		     &child_page,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free child page.",
			 function );

			return( -1 );
		}
	}
#if defined( HAVE_DEBUG_OUTPUT )
	libnotify_verbose_printf(
	 "\n" );
#endif

	return( 1 );
}

/* Reads the space tree page values from the page
 * Returns 1 if successful or -1 on error
 */
int libesedb_page_tree_read_space_tree_page_values(
     libesedb_page_tree_t *page_tree,
     libesedb_page_t *page,
     liberror_error_t **error )
{
	libesedb_page_t *space_tree_page  = NULL;
	libesedb_page_value_t *page_value = NULL;
	static char *function             = "libesedb_page_tree_read_space_tree_page_values";
	uint32_t required_flags           = 0;
	uint32_t supported_flags          = 0;
	uint16_t amount_of_page_values    = 0;
	uint16_t page_key_size            = 0;
	uint16_t page_value_iterator      = 0;

#if defined( HAVE_DEBUG_OUTPUT )
	uint32_t test                     = 0;
	uint32_t total_amount_of_pages    = 0;
#endif

	if( page_tree == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid page tree.",
		 function );

		return( -1 );
	}
	required_flags = LIBESEDB_PAGE_FLAG_IS_ROOT
	               | LIBESEDB_PAGE_FLAG_IS_SPACE_TREE;

	if( ( page->flags & required_flags ) != required_flags )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing required page flags: 0x%08" PRIx32 ".",
		 function,
		 page->flags );

		return( -1 );
	}
	supported_flags = required_flags
	                | LIBESEDB_PAGE_FLAG_IS_LEAF
	                | LIBESEDB_PAGE_FLAG_IS_PARENT
	                | LIBESEDB_PAGE_FLAG_IS_INDEX
	                | LIBESEDB_PAGE_FLAG_IS_LONG_VALUE
	                | LIBESEDB_PAGE_FLAG_IS_PRIMARY
	                | LIBESEDB_PAGE_FLAG_IS_NEW_RECORD_FORMAT;

	if( ( page->flags & ~supported_flags ) != 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported page flags: 0x%08" PRIx32 ".",
		 function,
		 page->flags );

		return( -1 );
	}
	if( page->previous_page_number != 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported previous page number: %" PRIu32 ".",
		 function,
		 page->previous_page_number );

		return( -1 );
	}
	if( page->next_page_number != 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported next page number: %" PRIu32 ".",
		 function,
		 page->next_page_number );

		return( -1 );
	}
	if( libesedb_page_get_amount_of_values(
	     page,
	     &amount_of_page_values,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve amount of page values.",
		 function );

		return( -1 );
	}
	if( libesedb_page_get_value(
	     page,
	     page_value_iterator,
	     &page_value,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve page value: %" PRIu16 ".",
		 function,
		 page_value_iterator );

		return( -1 );
	}
	if( page_value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid page value.",
		 function );

		return( -1 );
	}
#if defined( HAVE_DEBUG_OUTPUT )
	libnotify_verbose_printf(
	 "%s: header:\n",
	 function );
	libnotify_verbose_print_data(
	 page_value->data,
	 page_value->size );
#endif

	if( ( page->flags & LIBESEDB_PAGE_FLAG_IS_LEAF ) == LIBESEDB_PAGE_FLAG_IS_LEAF )
	{
		if( page_value->size == 16 )
		{
			if( ( page_value->data[  0 ] != 0 )
			 || ( page_value->data[  1 ] != 0 )
			 || ( page_value->data[  2 ] != 0 )
			 || ( page_value->data[  3 ] != 0 )
			 || ( page_value->data[  4 ] != 0 )
			 || ( page_value->data[  5 ] != 0 )
			 || ( page_value->data[  6 ] != 0 )
			 || ( page_value->data[  7 ] != 0 )
			 || ( page_value->data[  8 ] != 0 )
			 || ( page_value->data[  9 ] != 0 )
			 || ( page_value->data[ 10 ] != 0 )
			 || ( page_value->data[ 11 ] != 0 )
			 || ( page_value->data[ 12 ] != 0 )
			 || ( page_value->data[ 13 ] != 0 )
			 || ( page_value->data[ 14 ] != 0 )
			 || ( page_value->data[ 15 ] != 0 ) )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
				 "%s: unsupported header.",
				 function );

				return( -1 );
			}
		}
		else if( page_value->size != 0 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
			 "%s: unsupported header size.",
			 function );

			return( -1 );
		}
	}
	/* TODO handle the space tree page header */

	for( page_value_iterator = 1;
	     page_value_iterator < amount_of_page_values;
	     page_value_iterator++ )
	{
		if( libesedb_page_get_value(
		     page,
		     page_value_iterator,
		     &page_value,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve page value: %" PRIu16 ".",
			 function,
			 page_value_iterator );

			return( -1 );
		}
		if( ( page->flags & LIBESEDB_PAGE_FLAG_IS_LEAF ) == LIBESEDB_PAGE_FLAG_IS_LEAF )
		{
			if( ( page_value->flags & 0x05 ) != 0 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
				 "%s: unsupported page value flags: 0x%02" PRIx8 ".",
				 function,
				 page_value->flags );

				return( -1 );
			}
			if( page_value->size != sizeof( esedb_space_tree_page_entry_t ) )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
				 "%s: unsupported page value size: %" PRIzd ".",
				 function,
				 page_value->size );

				return( -1 );
			}
			endian_little_convert_16bit(
			 page_key_size,
			 ( (esedb_space_tree_page_entry_t *) page_value->data )->key_size );

			if( page_key_size != 4 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
				 "%s: unsupported page key size: %" PRIu16 ".",
				 function,
				 page_key_size);

				return( -1 );
			}
			/* TODO handle the space tree page values */

#if defined( HAVE_DEBUG_OUTPUT )
			libnotify_verbose_printf(
			 "%s: value: %03d key size\t\t: %" PRIu16 "\n",
			 function,
			 page_value_iterator,
			 page_key_size );

			endian_little_convert_32bit(
			 test,
			 ( (esedb_space_tree_page_entry_t *) page_value->data )->last_page_number );
			libnotify_verbose_printf(
			 "%s: value: %03d key value\t\t: %" PRIu32 " (0x%08" PRIx32 ")\n",
			 function,
			 page_value_iterator,
			 test,
			 test );

			endian_little_convert_32bit(
			 test,
			 ( (esedb_space_tree_page_entry_t *) page_value->data )->amount_of_pages );
			libnotify_verbose_printf(
			 "%s: value: %03d amount of pages\t: %" PRIu32 "\n",
			 function,
			 page_value_iterator,
			 test );

			libnotify_verbose_printf(
			 "\n" );

			if( ( page_value->flags & 0x02 ) == 0 )
			{
				total_amount_of_pages += test;
			}
#endif
		}
		else if( ( page->flags & LIBESEDB_PAGE_FLAG_IS_PARENT ) == LIBESEDB_PAGE_FLAG_IS_PARENT )
		{
			libnotify_verbose_printf(
			 "%s: data:\n",
			 function );
			libnotify_verbose_print_data(
			 page_value->data,
			 page_value->size );
#ifdef X
			if( libesedb_page_initialize(
			     &space_tree_page,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
				 "%s: unable to create space tree page.",
				 function );

				return( -1 );
			}
			if( libesedb_page_read(
			     space_tree_page,
			     io_handle,
			     space_tree_page_number,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_IO,
				 LIBERROR_IO_ERROR_READ_FAILED,
				 "%s: unable to read space tree page: %" PRIu32 ".",
				 function,
				 space_tree_page_number );

				libesedb_page_free(
				 &space_tree_page,
				 NULL );

				return( -1 );
			}
			if( page->father_data_page_object_identifier != space_tree_page->father_data_page_object_identifier )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
				 "%s: mismatch in father data page object identifier (%" PRIu32 " != %" PRIu32 ").",
				 function,
				 page->father_data_page_object_identifier,
				 space_tree_page->father_data_page_object_identifier );

				libesedb_page_free(
				 &space_tree_page,
				 NULL );

				return( -1 );
			}
			if( libesedb_page_tree_read_space_tree_page_values(
			     page_tree,
			     space_tree_page,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_IO,
				 LIBERROR_IO_ERROR_READ_FAILED,
				 "%s: unable to read space tree page values.",
				 function );

				libesedb_page_free(
				 &space_tree_page,
				 NULL );

				return( -1 );
			}
			if( libesedb_page_free(
			     &space_tree_page,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_FINALIZE_FAILED,
				 "%s: unable to free space tree page.",
				 function );

				return( -1 );
			}
#endif
		}
	}

#if defined( HAVE_DEBUG_OUTPUT )
	libnotify_verbose_printf(
	 "%s: total amount of pages\t\t: %" PRIu32 "\n",
	 function,
	 total_amount_of_pages );

	libnotify_verbose_printf(
	 "\n" );
#endif

	return( 1 );
}

/* Reads the leaf page values from the page
 * Returns 1 if successful or -1 on error
 */
int libesedb_page_tree_read_leaf_page_values(
     libesedb_page_tree_t *page_tree,
     libesedb_page_t *page,
     libesedb_io_handle_t *io_handle,
     uint8_t flags,
     liberror_error_t **error )
{
	libesedb_catalog_definition_t *catalog_definition = NULL;
	libesedb_data_definition_t *data_definition       = NULL;
	libesedb_page_value_t *page_value                 = NULL;
	libesedb_table_definition_t *table_definition     = NULL;
	uint8_t *page_value_data                          = NULL;
	static char *function                             = "libesedb_page_tree_read_leaf_page_values";
	uint32_t required_flags                           = 0;
	uint32_t supported_flags                          = 0;
	uint16_t amount_of_page_values                    = 0;
	uint16_t page_value_iterator                      = 0;
	uint16_t page_key_size                            = 0;
	uint16_t page_key_type                            = 0;
	uint16_t page_value_size                          = 0;
	int result                                        = 0;

	if( page_tree == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid page tree.",
		 function );

		return( -1 );
	}
	if( page == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid page.",
		 function );

		return( -1 );
	}
	required_flags = LIBESEDB_PAGE_FLAG_IS_LEAF;

	if( ( page->flags & required_flags ) != required_flags )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing required page flags: 0x%08" PRIx32 ".",
		 function,
		 page->flags );

		return( -1 );
	}
	supported_flags = required_flags
	                | LIBESEDB_PAGE_FLAG_IS_ROOT
	                | LIBESEDB_PAGE_FLAG_IS_INDEX
	                | LIBESEDB_PAGE_FLAG_IS_LONG_VALUE
	                | LIBESEDB_PAGE_FLAG_IS_PRIMARY
	                | LIBESEDB_PAGE_FLAG_IS_NEW_RECORD_FORMAT;

	if( ( page->flags & ~supported_flags ) != 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported page flags: 0x%08" PRIx32 ".",
		 function,
		 page->flags );

		return( -1 );
	}
	if( libesedb_page_get_amount_of_values(
	     page,
	     &amount_of_page_values,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve amount of page values.",
		 function );

		return( -1 );
	}
	if( libesedb_page_get_value(
	     page,
	     page_value_iterator,
	     &page_value,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve page value: %" PRIu16 ".",
		 function,
		 page_value_iterator );

		return( -1 );
	}
	if( page_value == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid page value.",
		 function );

		return( -1 );
	}
#if defined( HAVE_DEBUG_OUTPUT )
	page_value_data = page_value->data;
	page_value_size = page_value->size;

	libnotify_verbose_printf(
	 "%s: value: %03d value:\n",
	 function,
	 page_value_iterator );
	libnotify_verbose_print_data(
	 page_value_data,
	 page_value->size );

	libnotify_verbose_printf(
	 "%s: header (record key)\t\t\t\t: ",
	 function,
	 page_value_iterator );

	while( page_value_size > 0 )
	{
		libnotify_verbose_printf(
		 "%02" PRIx8 " ",
		 *page_value_data );

		page_value_data++;
		page_value_size--;
	}
	libnotify_verbose_printf(
	 "\n" );
	libnotify_verbose_printf(
	 "\n" );
#endif

	/* TODO handle the leaf page header */

	for( page_value_iterator = 1;
	     page_value_iterator < amount_of_page_values;
	     page_value_iterator++ )
	{
		if( libesedb_page_get_value(
		     page,
		     page_value_iterator,
		     &page_value,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve page value: %" PRIu16 ".",
			 function,
			 page_value_iterator );

			return( -1 );
		}
		/* TODO handle the leaf page keys */

		page_value_data = page_value->data;
		page_value_size = page_value->size;

#if defined( HAVE_DEBUG_OUTPUT )
		libnotify_verbose_printf(
		 "%s: value: %03d value:\n",
		 function,
		 page_value_iterator );
		libnotify_verbose_print_data(
		 page_value_data,
		 page_value->size );

		libnotify_verbose_printf(
		 "%s: value: %03d page tag flags\t\t\t: ",
		 function,
		 page_value_iterator );
		libesedb_debug_print_page_tag_flags(
		 page_value->flags );
		libnotify_verbose_printf(
		 "\n" );
#endif

		if( ( page_value->flags & 0x04 ) == 0x04 )
		{
			endian_little_convert_16bit(
			 page_key_type,
			 page_value_data );

			page_value_data += 2;
			page_value_size -= 2;

#if defined( HAVE_DEBUG_OUTPUT )
			libnotify_verbose_printf(
			 "%s: value: %03d key type\t\t\t\t: 0x%04" PRIx32 " (%" PRIu32 ")\n",
			 function,
			 page_value_iterator,
			 page_key_type,
			 page_key_type );
#endif
		}
		endian_little_convert_16bit(
		 page_key_size,
		 page_value_data );

		page_value_data += 2;
		page_value_size -= 2;

#if defined( HAVE_DEBUG_OUTPUT )
		libnotify_verbose_printf(
		 "%s: value: %03d key size\t\t\t\t: %" PRIu16 "\n",
		 function,
		 page_value_iterator,
		 page_key_size );
#endif

		if( page_key_size > page_value_size )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_RANGE,
			 "%s: page key size exceeds page value size.",
			 function );

			return( -1 );
		}
#if defined( HAVE_DEBUG_OUTPUT )
		libnotify_verbose_printf(
		 "%s: value: %03d key value\t\t\t\t: ",
		 function,
		 page_value_iterator );

		while( page_key_size > 0 )
		{
			libnotify_verbose_printf(
			 "%02" PRIx8 " ",
			 *page_value_data );

			page_value_data++;
			page_value_size--;
			page_key_size--;
		}
		libnotify_verbose_printf(
		 "\n" );
#endif

		if( ( page->flags & LIBESEDB_PAGE_FLAG_IS_INDEX ) == LIBESEDB_PAGE_FLAG_IS_INDEX )
		{
#if defined( HAVE_DEBUG_OUTPUT )
			libnotify_verbose_printf(
			 "%s: value: %03d index value\t\t\t: ",
			 function,
			 page_value_iterator );

			while( page_value_size > 0 )
			{
				libnotify_verbose_printf(
				 "%02" PRIx8 " ",
				 *page_value_data );

				page_value_data++;
				page_value_size--;
			}
			libnotify_verbose_printf(
			 "\n" );
			libnotify_verbose_printf(
			 "\n" );
#endif
		}
		else if( ( page->flags & LIBESEDB_PAGE_FLAG_IS_LONG_VALUE ) == LIBESEDB_PAGE_FLAG_IS_LONG_VALUE )
		{
#if defined( HAVE_DEBUG_OUTPUT )
			libnotify_verbose_printf(
			 "%s: value: %03d long value:\n",
			 function,
			 page_value_iterator );
			libnotify_verbose_print_data(
			 page_value_data,
			 page_value_size );
#endif
		}
		else
		{
			/* The catalog is read using build-in catalog definition types
			 */
			if( ( flags & LIBESEDB_PAGE_TREE_FLAG_READ_CATALOG_DEFINITION ) == LIBESEDB_PAGE_TREE_FLAG_READ_CATALOG_DEFINITION )
			{
				if( libesedb_catalog_definition_initialize(
				     &catalog_definition,
				     error ) != 1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
					 "%s: unable to create catalog definition.",
					 function );

					return( -1 );
				}
				if( libesedb_catalog_definition_read(
				     catalog_definition,
				     page_value_data,
				     page_value_size,
				     error ) != 1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_IO,
					 LIBERROR_IO_ERROR_READ_FAILED,
					 "%s: unable to read catalog page value: %d catalog definition.",
					 function,
					 page_value_iterator );

					libesedb_catalog_definition_free(
					 (intptr_t *) catalog_definition,
					 NULL );

					catalog_definition = NULL;

					return( -1 );
				}
				if( ( catalog_definition->type != LIBESEDB_CATALOG_DEFINITION_TYPE_TABLE )
				 && ( ( table_definition == NULL )
				  || ( table_definition->table_catalog_definition == NULL )
				  || ( table_definition->table_catalog_definition->father_data_page_object_identifier != catalog_definition->father_data_page_object_identifier ) ) )
				{
						result = libesedb_page_tree_get_table_definition_by_identifier(
						          page_tree,
						          catalog_definition->father_data_page_object_identifier,
						          &table_definition,
						          error );

						if( result == -1 )
						{
							liberror_error_set(
							 error,
							 LIBERROR_ERROR_DOMAIN_RUNTIME,
							 LIBERROR_RUNTIME_ERROR_GET_FAILED,
							 "%s: unable to retrieve table definition: %" PRIu32 ".",
							 function,
							 catalog_definition->father_data_page_object_identifier );
						}
						else if( result == 0 )
						{
							liberror_error_set(
							 error,
							 LIBERROR_ERROR_DOMAIN_RUNTIME,
							 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
							 "%s: missing table definition: %" PRIu32 ".",
							 function,
							 catalog_definition->father_data_page_object_identifier );
						}
						if( result != 1 )
						{
							libesedb_catalog_definition_free(
							 (intptr_t *) catalog_definition,
							 NULL );

							catalog_definition = NULL;

							return( -1 );
						}
				}
				switch( catalog_definition->type )
				{
					case LIBESEDB_CATALOG_DEFINITION_TYPE_TABLE:
						table_definition = NULL;

						if( libesedb_table_definition_initialize(
						     &table_definition,
						     catalog_definition,
						     error ) != 1 )
						{
							liberror_error_set(
							 error,
							 LIBERROR_ERROR_DOMAIN_RUNTIME,
							 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
							 "%s: unable to create table definition.",
							 function );

							libesedb_table_definition_free(
							 (intptr_t *) table_definition,
							 NULL );
							libesedb_catalog_definition_free(
							 (intptr_t *) catalog_definition,
							 NULL );

							table_definition   = NULL;
							catalog_definition = NULL;

							return( -1 );
						}
						catalog_definition = NULL;

						if( libesedb_list_append_value(
						     page_tree->table_definition_list,
						     (intptr_t *) table_definition,
						     error ) != 1 )
						{
							liberror_error_set(
							 error,
							 LIBERROR_ERROR_DOMAIN_RUNTIME,
							 LIBERROR_RUNTIME_ERROR_APPEND_FAILED,
							 "%s: unable to append table definition to table definition list.",
							 function );

							libesedb_table_definition_free(
							 (intptr_t *) table_definition,
							 NULL );

							table_definition = NULL;

							return( -1 );
						}
						break;

					case LIBESEDB_CATALOG_DEFINITION_TYPE_COLUMN:
						if( libesedb_table_definition_append_column_catalog_definition(
						     table_definition,
						     catalog_definition,
						     error ) != 1 )
						{
							liberror_error_set(
							 error,
							 LIBERROR_ERROR_DOMAIN_RUNTIME,
							 LIBERROR_RUNTIME_ERROR_APPEND_FAILED,
							 "%s: unable to append column catalog definition to table definition.",
							 function );

							libesedb_catalog_definition_free(
							 (intptr_t *) catalog_definition,
							 NULL );

							catalog_definition = NULL;

							return( -1 );
						}
						catalog_definition = NULL;

						break;

					case LIBESEDB_CATALOG_DEFINITION_TYPE_INDEX:
						if( libesedb_table_definition_append_index_catalog_definition(
						     table_definition,
						     catalog_definition,
						     error ) != 1 )
						{
							liberror_error_set(
							 error,
							 LIBERROR_ERROR_DOMAIN_RUNTIME,
							 LIBERROR_RUNTIME_ERROR_APPEND_FAILED,
							 "%s: unable to append index catalog definition to table definition.",
							 function );

							libesedb_catalog_definition_free(
							 (intptr_t *) catalog_definition,
							 NULL );

							catalog_definition = NULL;

							return( -1 );
						}
						catalog_definition = NULL;

						break;

					case LIBESEDB_CATALOG_DEFINITION_TYPE_LONG_VALUE:
						if( libesedb_table_definition_set_long_value_catalog_definition(
						     table_definition,
						     catalog_definition,
						     error ) != 1 )
						{
							liberror_error_set(
							 error,
							 LIBERROR_ERROR_DOMAIN_RUNTIME,
							 LIBERROR_RUNTIME_ERROR_SET_FAILED,
							 "%s: unable to set long value catalog definition in table definition.",
							 function );

							libesedb_catalog_definition_free(
							 (intptr_t *) catalog_definition,
							 NULL );

							catalog_definition = NULL;

							return( -1 );
						}
						catalog_definition = NULL;

						break;

					default:
						liberror_error_set(
						 error,
						 LIBERROR_ERROR_DOMAIN_RUNTIME,
						 LIBERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
						 "%s: unsupported catalog definition type: %" PRIu16 ".",
						 function,
						 catalog_definition->type );

						libesedb_catalog_definition_free(
						 (intptr_t *) catalog_definition,
						 NULL );

						catalog_definition = NULL;

						return( -1 );
				}
			}
			else
			{
				if( page_tree->table_definition == NULL )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
					 "%s: invalid page tree - missing table definition.",
					 function );

					return( -1 );
				}
				if( libesedb_data_definition_initialize(
				     &data_definition,
				     error ) != 1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
					 "%s: unable to create data definition.",
					 function );

					return( -1 );
				}
				if( libesedb_data_definition_read(
				     data_definition,
				     page_tree->table_definition->column_catalog_definition_list,
				     io_handle,
				     page_value_data,
				     page_value_size,
				     error ) != 1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_IO,
					 LIBERROR_IO_ERROR_READ_FAILED,
					 "%s: unable to read page value: %d data definition.",
					 function,
					 page_value_iterator );

					libesedb_data_definition_free(
					 (intptr_t *) data_definition,
					 NULL );

					data_definition = NULL;

					return( -1 );
				}
				if( libesedb_list_append_value(
				     page_tree->value_definition_list,
				     (intptr_t *) data_definition,
				     error ) != 1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_APPEND_FAILED,
					 "%s: unable to append value data definition to list.",
					 function );

					libesedb_data_definition_free(
					 (intptr_t *) data_definition,
					 NULL );

					data_definition = NULL;

					return( -1 );
				}
				data_definition = NULL;
			}
		}
	}
	return( 1 );
}

