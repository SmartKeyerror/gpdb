#include "postgres.h"
#include "miscadmin.h"

#include "cdb/cdbhash.h"
#include "cdb/cdbutil.h"
#include "commands/tablecmds.h"
#include "executor/instrument.h"
#include "executor/nodeSplitUpdate.h"

#include "utils/memutils.h"


static TupleTableSlot *
ExecSplitInsert(PlanState *pstate)
{
	SplitInsertState *node = castNode(SplitInsertState, pstate);
	PlanState *outerNode = outerPlanState(node);
	SplitInsert *plannode = (SplitInsert *) node->ps.plan;
	int          tl_len = list_length(plannode->plan->targetList);
	int          num_relations = list_length(plannode->insertTargetRelid) + 1;

	TupleTableSlot *slot = NULL;

	if (node->current_offset == 0)
	{
		slot = ExecProcNode(outerNode);

		if (TupIsNull(slot))
			return NULL;

		slot_getallattrs(slot);
		node->current_offset++;
		ExecStoreAllNullTuple(node->resultTuple);
		for (int i = 0; i < tl_len - 1; i++)
		{
			node->resultTuple->tts_values[i] = slot->tts_values[i];
			node->resultTuple->tts_isnull[i] = slot->tts_isnull[i];
		}
		node->resultTuple->tts_isnull[tl_len-1] = false;
	}

	node->resultTuple->tts_values[tl_len-1] = Int32GetDatum(node->current_offset);

	if (node->current_offset == num_relations)
		node->current_offset = 0;
	else
		node->current_offset++;

	return node->resultTuple;
}

SplitInsertState *
ExecInitSplitInsert(SplitInsert *node, Estate *estate, int eflags)
{
	SplitInsertState *splitinsertstate;

	/* Check for unsupported flags */
	Assert(!(eflags & (EXEC_FLAG_BACKWARD | EXEC_FLAG_MARK | EXEC_FLAG_REWIND)));

	splitinsertstate = makeNode(SplitInsertState);
	splitinsertstate->ps.plan = (Plan *) node;
	splitinsertstate->ps.state = estate;
	splitinsertstate->ps.ExecProcNode = ExecSplitInsert;
	splitinsertstate->current_offset = 0;

	/*
	 * then initialize outer plan
	 */
	Plan *outerPlan = outerPlan(node);
	outerPlanState(splitinsertstate) = ExecInitNode(outerPlan, estate, eflags);

	ExecAssignExprContext(estate, &splitinsertstate->ps);

	TupleDesc tupDesc = ExecTypeFromTL(node->plan.targetlist);
	splitinsertstate->resultTuple = ExecInitExtraTupleSlot(estate, tupDesc, &TTSOpsVirtual);

	ExecInitResultTupleSlotTL(&splitinsertstate->ps, &TTSOpsVirtual);
	splitinsertstate->ps.ps_ProjInfo = NULL;

	if (estate->es_instrument && (estate->es_instrument & INSTRUMENT_CDB))
	{
		splitinsertstate->ps.cdbexplainbuf = makeStringInfo();
	}

	return splitinsertstate;
}

/* Release Resources Requested by SplitInsert node. */
void
ExecEndSplitInsert(SplitInsertState *node)
{
	ExecFreeExprContext(&node->ps);
	ExecClearTuple(node->ps.ps_ResultTupleSlot);
	ExecClearTuple(node->resultTuple);
	ExecEndNode(outerPlanState(node));
}
