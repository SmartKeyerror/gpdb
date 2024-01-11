/*-------------------------------------------------------------------------
 *
 * cdbsrlz.h
 *	  definitions for plan serialization utilities
 *
 * Portions Copyright (c) 2004-2008, Greenplum inc
 * Portions Copyright (c) 2012-Present VMware, Inc. or its affiliates.
 *
 *
 * IDENTIFICATION
 *	    src/include/cdb/cdbsrlz.h
 *
 * NOTES
 *
 *-------------------------------------------------------------------------
 */

#ifndef CDBSRLZ_H
#define CDBSRLZ_H

#include "nodes/nodes.h"

extern char *serializeNode(Node *node, int *size, int *uncompressed_size);
extern Node *deserializeNode(const char *strNode, int size);

extern char *compress_string(const char *src, int uncompressed_size, int *size);
extern char *uncompress_string(const char *src, int size, int *uncompressed_size_p);

#endif   /* CDBSRLZ_H */
