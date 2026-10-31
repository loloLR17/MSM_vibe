-- Read only. Keep analysis identity explicit when comparing cohorts.
SELECT analysis_id,campaign_sha,code_sha,result_sha FROM wm_runs;
SELECT json_extract(definition_json,'$.metric') AS metric,
 json_extract(definition_json,'$.clock') AS clock,method_id FROM wm_methods;
SELECT analysis_id,method_id,width_s,count(*) AS windows FROM wm_metrics
GROUP BY analysis_id,method_id,width_s;
SELECT m.analysis_id,d.method_id,m.width_s,m.start_mcu_s,
 json_extract(d.definition_json,'$.variant') AS variant,
 json_extract(d.definition_json,'$.clock') AS clock,
 json_extract(m.result_json,'$.quality') AS quality,
 json_extract(m.result_json,'$.rms_x') AS x_mm_s2,
 json_extract(m.result_json,'$.rms_y') AS y_mm_s2,
 json_extract(m.result_json,'$.rms_z') AS z_mm_s2,
 json_extract(m.result_json,'$.TR2_WM_A_MAX') AS principal_mm_s2,
 json_extract(m.result_json,'$.winning_axis') AS axis,
 json_extract(m.result_json,'$.equivalent_velocity_mm_s') AS dependent_equivalent_mm_s
FROM wm_metrics m JOIN wm_methods d USING(method_id)
WHERE m.analysis_id=(SELECT analysis_id FROM wm_runs ORDER BY analysis_id LIMIT 1)
ORDER BY d.method_id,m.width_s,m.start_mcu_s;
SELECT (SELECT count(*) FROM window_metrics) AS old_RMS,
 (SELECT count(*) FROM pk_components) AS old_PK,
 (SELECT count(*) FROM prequal_metrics) AS old_tests,
 (SELECT count(*) FROM wm_metrics) AS new_Wm;
