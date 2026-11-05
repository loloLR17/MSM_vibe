from pathlib import Path
import json,math,hashlib
P=Path(__file__).resolve().parents[1]
# Drawing dimensions are separate from unverified assembly/measurement bounds.
a={'H_socket':(8.51,.13,'Samtec series print .335, .XXX ±.005in≈.13mm'),'H_J5':(8.51,.13,'same Samtec print'),'b_male':(2.5,.38,'Connfly double-row body2.5, general .X±.38'),'p_male':(6.10,.25,'Connfly DS1021 type1 D6.10, .XX±.25'),'tip_CN12':(8.22,.10,'user value, ±.10 ENGINEERING ASSUMPTION not measured uncertainty'),'body_CN12':(2.4,.10,'user value, ±.10 ENGINEERING ASSUMPTION'),'G':(12.25,.10,'proposed assembly tolerance including seating/parallelism, NOT guaranteed by a selected spacer')}
# reserve .30mm between plastics, manufacturer insertion3.68..6.35.
lo=lambda k:a[k][0]-a[k][1];hi=lambda k:a[k][0]+a[k][1]
L={'J5_no_bottoming':hi('tip_CN12')+hi('H_J5')+.1-6.35,'J5_body_gap_0p30':hi('H_J5')+hi('body_CN12')+.1+.3,'Zio_no_bottoming':hi('H_socket')+hi('b_male')+hi('p_male')+.1-6.35,'Zio_body_gap_0p30':hi('H_socket')+hi('b_male')+.1+.3}
U={'J5_min_engagement':lo('tip_CN12')+lo('H_J5')-.1-3.68,'Zio_min_engagement':lo('H_socket')+lo('b_male')+lo('p_male')-.1-3.68}
low=max(L.values());high=min(U.values());assert low<12.25<high
cases={}
for G in [11.,12.,12.25]:
 cases[str(G)]={'J5_insertion_nominal':8.22+8.51-G,'J5_gap_nominal':G-8.51-2.4,'Zio_insertion_nominal':8.51+2.5+6.1-G,'Zio_gap_nominal':G-8.51-2.5,'J5_insertion_worst_range':[lo('tip_CN12')+lo('H_J5')-(G+.1),hi('tip_CN12')+hi('H_J5')-(G-.1)],'J5_min_gap':G-.1-hi('H_J5')-hi('body_CN12'),'Zio_insertion_worst_range':[lo('H_socket')+lo('b_male')+lo('p_male')-(G+.1),hi('H_socket')+hi('b_male')+hi('p_male')-(G-.1)],'Zio_min_gap':G-.1-hi('H_socket')-hi('b_male')}
# Real ST end of socket vs spacer axis; tolerances here are purchasing/assembly acceptance budgets.
sp=[]
for OD,dtol in [(4.,.1),(3.5,.05)]:
 nominal=97.39-95.105-OD/2
 worst=nominal-.125-.15-dtol/2
 sp.append({'OD':OD,'OD_tolerance_assumed':dtol,'nominal_edge_gap':nominal,'worst_gap_assuming_half_length_p125_and_relative_position_p150':worst,'guard_mm':.2,'guard_pass_under_bounds_excluding_flash':worst>=.2,'worst_gap_with_catalogue_flash_0p38':worst-.38,'guaranteed_all_catalogue_cases':False})
r={'status':'FEASIBLE_INTERVAL_UNDER_EXPLICIT_BOUNDS_NOT_HARDWARE_QUALIFIED','inputs_nominal_pm_and_provenance':a,'insertion_limits':[3.68,6.35],'body_gap_guard':.3,'lower_constraints':L,'upper_constraints':U,'G_nominal_candidate_interval':[low,high],'recommended_nominal':12.25,'scenarios':cases,'spacers':sp,'unbounded_terms':['actual CN12 height uncertainties','actual selected spacer length/OD tolerance and nut/head stack','seating offsets, board bow and parallelism','socket sway2deg and local flash0.38mm max from Samtec print require local fit verification','contact plating interoperability and retention vibration','exact current module/Click revision and connector tails'],'all_rows_manufacturer_guaranteed':False,'PCB_thickness':'1.6mm declared, ±10% budget; G defined between PCB surfaces, so thickness is not added to J5/Zio insertion; used for tail clearance','tail_scenario':{'module_tail_max_assumed':3.5,'shield_min_thickness':1.44,'G_min_at12p25':12.15,'tail_lowest_z':12.15+1.44-3.5,'max_ST_socket_height_budget':8.64,'nominal_budget_gap':12.15+1.44-3.5-8.64},'fabrication':'NO_GO_NO_REAL_MATING_OBSERVED'}
(P/'preuves/tolerances.json').write_text(json.dumps(r,ensure_ascii=False,indent=2)+'\n');print('conditional G interval',low,high,'scenarios',cases['12.25'],'spacer',sp)
