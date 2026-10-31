-- Read-only queries for the derived v4 catalogue: always select a specific analysis.
SELECT analysis_id,campaign_sha,code_sha,result_sha FROM prequal_runs;
SELECT name,definition_json FROM (SELECT json_extract(definition_json,'$.name') AS name,definition_json FROM prequal_methods);
SELECT analysis_id,method_id,width_s,count(*) AS rows FROM prequal_metrics GROUP BY analysis_id,method_id,width_s;
-- Reconstruct all trends without mixing versions/cohorts.
SELECT p.analysis_id,p.method_id,json_extract(d.definition_json,'$.name') AS method,
 json_extract(d.definition_json,'$.clock') AS clock,p.width_s,p.start_mcu_s,
 json_extract(p.result_json,'$.quality') AS quality,
 json_extract(p.result_json,'$.direction_max') AS directional_rms_mg,
 json_extract(p.result_json,'$.rms_x') AS x,json_extract(p.result_json,'$.rms_y') AS y,
 json_extract(p.result_json,'$.rms_z') AS z
FROM prequal_metrics p JOIN prequal_methods d USING(method_id)
WHERE p.analysis_id=(SELECT analysis_id FROM prequal_runs ORDER BY analysis_id LIMIT 1)
ORDER BY p.method_id,p.width_s,p.start_mcu_s;
SELECT (SELECT count(*) FROM window_metrics) AS old_RMS,
 (SELECT count(*) FROM pk_components) AS old_PK,
 (SELECT count(*) FROM prequal_metrics) AS new_global;
