#include "nusmv/core/bmc/sbmc/sbmcGen.h"
#include "nusmv/core/dd/ddAc.h"

void print_accepting_states(NuSMVEnv_ptr env,
		  Prop_ptr prop,
		  DDMgr_ptr dd,
		  BddEnc_ptr enc,
		  const OptsHandler_ptr opts,
		  const StreamMgr_ptr streams,
		  bdd_ptr init, 
		  bdd_ptr accepted)
{
  
  const MasterPrinter_ptr wffprint =
    MASTER_PRINTER(NuSMVEnv_get_value(env, ENV_WFF_PRINTER));
  OStream_ptr stream = StreamMgr_get_output_ostream(streams);
  FILE * out = StreamMgr_get_output_stream(streams);
  node_ptr p = Prop_get_expr(prop);
  const char* ctlspec = (char*) sprint_node(wffprint, p);
  const char * file_name = get_print_accepting(opts);
  const char** inames;
  int lev;
  int index_of_spec = Prop_get_index(prop);
  double init_size, acc_size, inacc_size;
  const int dd_size = dd_get_size(dd);
  
  bdd_ptr init_and_accepted = bdd_dup(init);
  bdd_and_accumulate(dd, &init_and_accepted, accepted);
  
  init_size = BddEnc_count_states_of_bdd(enc, init);
  acc_size = BddEnc_count_states_of_bdd(enc, accepted);
  inacc_size = BddEnc_count_states_of_bdd(enc, init_and_accepted);
  
  /* get input names */
  inames = ALLOC( const char*, dd_size);
  nusmv_assert((const char**) NULL != inames);
  
  for(lev = 0; lev < dd_size; ++lev) {
    int index = dd_get_index_at_level(dd, lev);
    if(BddEnc_has_var_at_index(enc, index)) {
      inames[lev] = (const char*) sprint_node(wffprint,
                      BddEnc_get_var_name_from_index(enc, index));
    }
    else {
      inames[lev] = (const char*) NULL;
    }
  }
  
  if(strcmp(file_name, "print") == 0) {
    
    OStream_printf(stream, "\ninitial states: ");
    Cudd_DumpFormula_modified(dd, 1, &init, inames, out);
    OStream_printf(stream, "\nnumber of initial states: %g", init_size);
    OStream_printf(stream, "\n");

    OStream_printf(stream, "accepting states: ");
    Cudd_DumpFormula_modified(dd, 1, &accepted, inames, out);
    OStream_printf(stream, "\nnumber of accepting states: %g", acc_size);
    OStream_printf(stream, "\n");
    
    OStream_printf(stream, "initial and accepting States: ");
    Cudd_DumpFormula_modified(dd, 1, &init_and_accepted, inames, out);
    OStream_printf(stream, "\nnumber of initial and accepting states: %g", inacc_size);
    OStream_printf(stream, "\n\n");
  }
  
  else {
    if (index_of_spec != 0){
      out = fopen(file_name, "a");
    }
    else{
      out = fopen(file_name, "w");
    }

    fprintf(out, "CTLSPEC:              ");
    fprintf(out, "%s", ctlspec);
    fprintf(out, "\n");
    
    fprintf(out, "INIT:                 ");
    Cudd_DumpFormula_modified(dd, 1, &init, inames, out);
    fprintf(out, "\nINIT_SIZE:            %g", init_size);
    fprintf(out, "\n");
    
    fprintf(out, "ACCEPTING:            ");
    Cudd_DumpFormula_modified(dd, 1, &accepted, inames, out);
    fprintf(out, "\nACCEPTING_SIZE:       %g", acc_size);
    fprintf(out, "\n");
    
    fprintf(out, "INITACCEPTING:        ");
    Cudd_DumpFormula_modified(dd, 1, &init_and_accepted, inames, out);
    fprintf(out, "\nINITACCEPTING_SIZE:   %g", inacc_size);
    fprintf(out, "\n");
    
    if (Prop_get_status(prop) == Prop_True) {
      fprintf(out, "ANSWER:               TRUE\n\n");
    }
    else{
      fprintf(out, "ANSWER:               FALSE\n\n");
    }
    fclose(out);
    
  }
    
  /* aufraeumen */
  bdd_free(dd,init_and_accepted);
  
  {
    int i;
    /* free inames */
    for( i=0; i < dd_size; ++i) {
      if( (const char*) NULL != inames[i]) { FREE( inames[i]); }
    }
    FREE(inames);
  }
  
}



