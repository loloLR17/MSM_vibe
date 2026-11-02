#include "budget_diagnostic.h"
#include <assert.h>
#include <stdio.h>
int main(void)
{
 /* Exact boundary is accepted; one additional cycle identifies each limit.
  * Simultaneous violations must remain visible, rather than first-only. */
 assert(r3_budget_failure_bits(400000,160000,800000,160000)==0);
 assert(r3_budget_failure_bits(400001,0,0,0)==1);
 assert(r3_budget_failure_bits(0,160001,0,0)==2);
 assert(r3_budget_failure_bits(0,0,800001,0)==4);
 assert(r3_budget_failure_bits(0,0,0,160001)==8);
 assert(r3_budget_failure_bits(400001,160001,800001,160001)==15);
 assert(r3_budget_failure_bits(400001,0,800001,0)==5);
 assert(r3_budget_failure_bits(0,0,0,83847)==0); /* observed A1H SD max */
 puts("PASS exact boundaries, one-cycle violations, combined violations and observed non-SD A1H case");
}
