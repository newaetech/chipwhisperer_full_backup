##
## Match Unit Configuration to Match Output false path
##

set_false_path -from [get_pins -filter {REF_PIN_NAME=~CLK*} -of_objects [get_cells -hierarchical -filter {NAME =~ "*allx_typeA_match_detection.ltlib_v1_0_0_allx_typeA_inst/DUT/I_WHOLE_SLICE.G_SLICE_IDX[*].U_ALL_SRL_SLICE/u_srl*/S*"}]] -to [get_pins -filter {REF_PIN_NAME=~D*} -of_objects [get_cells -hierarchical -filter {NAME =~ "*allx_typeA_match_detection.ltlib_v1_0_0_allx_typeA_inst/DUT/I_WHOLE_SLICE.G_SLICE_IDX[*].U_ALL_SRL_SLICE/I_IS_TERMINATION_SLICE_W_OUTPUT_REG.DOUT_O_reg*"}]]

##
## ILA Sample Counter Match Condition out False Paths
##
set_false_path -from [get_pins -filter {REF_PIN_NAME=~CLK*} -of_objects [get_cells -hierarchical -filter { NAME =~  "*ila_core_inst/u_ila_cap_ctrl/u_cap_addrgen/u_cap_sample_counter/u_scnt_cmp/allx_typeA_match_detection.ltlib_v1_0_0_allx_typeA_inst/DUT/I_WHOLE_SLICE.G_SLICE_IDX[*].U_ALL_SRL_SLICE/u_srl*/S*"}]] -to [get_cells -hierarchical -filter { NAME =~  "*ila_core_inst/u_ila_cap_ctrl/u_cap_addrgen/u_cap_sample_counter/u_scnt_cmp_q*" && IS_SEQUENTIAL } ]

##
## ILA Window Counter Match Condition out False Paths
##
set_false_path -from [get_pins -filter {REF_PIN_NAME=~CLK*} -of_objects [get_cells -hierarchical -filter { NAME =~  "*ila_core_inst/u_ila_cap_ctrl/u_cap_addrgen/u_cap_window_counter/u_wcnt_lcmp/allx_typeA_match_detection.ltlib_v1_0_0_allx_typeA_inst/DUT/I_WHOLE_SLICE.G_SLICE_IDX[*].U_ALL_SRL_SLICE/u_srl*/S*"}]] -to [get_cells -hierarchical -filter { NAME =~  "*ila_core_inst/u_ila_cap_ctrl/u_cap_addrgen/u_cap_window_counter/u_wcnt_lcmp_q*" && IS_SEQUENTIAL } ]
set_false_path -from [get_pins -filter {REF_PIN_NAME=~CLK*} -of_objects [get_cells -hierarchical -filter { NAME =~  "*ila_core_inst/u_ila_cap_ctrl/u_cap_addrgen/u_cap_window_counter/u_wcnt_hcmp/allx_typeA_match_detection.ltlib_v1_0_0_allx_typeA_inst/DUT/I_WHOLE_SLICE.G_SLICE_IDX[*].U_ALL_SRL_SLICE/u_srl*/S*"}]] -to [get_cells -hierarchical -filter { NAME =~  "*ila_core_inst/u_ila_cap_ctrl/u_cap_addrgen/u_cap_window_counter/u_wcnt_hcmp_q*" && IS_SEQUENTIAL } ]

##
## ILA ADVANCED Trigger False Paths
##
set_false_path -from [get_cells -hierarchical -filter { NAME =~ "*ADV_TRIG.u_adv_trig/fsm_mem_data*" && IS_SEQUENTIAL }] -to [get_cells -hierarchical -filter { NAME =~ "*ila_core_inst/ADV_TRIG.u_adv_trig/current_state_reg*" && IS_SEQUENTIAL} ] 
set_false_path -from [get_cells -hierarchical -filter { NAME =~ "*ADV_TRIG.u_adv_trig/fsm_mem_data*" && IS_SEQUENTIAL }] -to [get_cells -hierarchical -filter { NAME =~ "*ila_core_inst/ADV_TRIG.u_adv_trig/cntcmpsel_reg*" && IS_SEQUENTIAL} ] 
set_false_path -from [get_cells -hierarchical -filter { NAME =~ "*ADV_TRIG.u_adv_trig/fsm_mem_data*" && IS_SEQUENTIAL }] -to [get_cells -hierarchical -filter { NAME =~ "*ila_core_inst/ADV_TRIG.u_adv_trig/FLAG*" && IS_SEQUENTIAL} ]
set_false_path -from [get_cells -hierarchical -filter { NAME =~ "*ADV_TRIG.u_adv_trig/fsm_mem_data*" && IS_SEQUENTIAL }] -to [get_cells -hierarchical -filter { NAME =~ "*ila_core_inst/COUNTER.u_count/G_COUNTER[*].U_COUNTER/counter_reg*" && IS_SEQUENTIAL}]
set_false_path -from [get_cells -hierarchical -filter { NAME =~ "*ADV_TRIG.u_adv_trig/fsm_mem_data*" && IS_SEQUENTIAL }] -to [get_cells -hierarchical -filter { NAME =~ "*ADV_TRIG.u_adv_trig/trigger_reg*" && IS_SEQUENTIAL }]
set_false_path -from [get_cells -hierarchical -filter { NAME =~ "*ADV_TRIG.u_adv_trig/fsm_mem_data*" && IS_SEQUENTIAL }] -to [get_cells -hierarchical -filter { NAME =~ "*ADV_TRIG.u_adv_trig/CAPTURE_O_reg*" && IS_SEQUENTIAL }]
