#
# Clock constraints
#
set_false_path -from [get_cells U0/*/U_STAT/U_DIRTY_LDC] -to [get_cells -of_objects [filter [all_fanout -flat -endpoints_only -from [get_nets CONTROL[0]]] IS_CLOCK]]
set_false_path -from [get_cells -of_objects [filter [all_fanout -flat -endpoints_only -from [get_nets CONTROL[0]]] IS_CLOCK]] -to [get_cells U0/*/U_STAT/U_DIRTY_LDC]
set_false_path -from [get_cells U0/*/U_RST/U_ARM_XFER/U_GEN_DELAY[3].U_FD] -to [get_cells U0/*/U_STAT/U_DIRTY_LDC]
