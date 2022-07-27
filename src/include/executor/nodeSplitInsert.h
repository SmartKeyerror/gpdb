/*-------------------------------------------------------------------------
 *
 * nodeSplitInsert.h
 *        Prototypes for nodeSplitInsert.
 *
 * Portions Copyright (c) 2012, EMC Corp.
 * Portions Copyright (c) 2012-Present VMware, Inc. or its affiliates.
 *
 *
 * IDENTIFICATION
 *	    src/include/executor/nodeSplitInsert.h
 *
 *-------------------------------------------------------------------------
 */

#ifndef NODESplitInsert_H
#define NODESplitInsert_H

extern SplitInsertState* ExecInitSplitInsert(SplitInsert *node, EState *estate, int eflags);
extern void ExecEndSplitInsert(SplitInsertState *node);

#endif   /* NODESplitInsert_H */
