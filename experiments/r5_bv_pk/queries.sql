-- Read-only, bind :analysis_id (a single cohort), :clock, :band.
SELECT analysis_id,method_id,campaign_sha FROM pk_analysis_runs;
-- Complete saved rows, including PSD/amplitude/quality: enough to reconstruct all plots.
SELECT p.row_json FROM pk_components p
WHERE p.analysis_id=:analysis_id ORDER BY p.start_mcu_s,p.window_mcu_s,p.axis;
-- Main plotted 60s candidate and qualified short-window peak, separate frequencies.
SELECT m.band,m.unit,m.clock,p.axis,p.start_mcu_s,
 json_extract(p.row_json,'$.frequency_Hz') AS spectral60_frequency_Hz,
 p.frequency_Hz AS reported_peak_frequency_Hz,p.component_pk,p.reported_pk,
 p.status,p.comparison_eligible,p.comparison_zone
FROM pk_components p JOIN pk_analysis_runs a USING(analysis_id)
JOIN pk_methods m USING(method_id)
WHERE p.analysis_id=:analysis_id AND p.window_mcu_s=60 ORDER BY p.start_mcu_s,p.axis;
SELECT definition_json FROM pk_reference;
-- Whole-minute comparisons ONLY if all three axes qualify, never silently discard an axis.
SELECT a.analysis_id,m.band,m.clock,p.start_mcu_s,max(p.reported_pk) AS maximum_component_peak
FROM pk_components p JOIN pk_analysis_runs a USING(analysis_id) JOIN pk_methods m USING(method_id)
WHERE p.window_mcu_s=60 GROUP BY a.analysis_id,m.band,m.clock,p.start_mcu_s
HAVING count(*)=3 AND sum(p.comparison_eligible)=3;
-- Prior RMS data preserved in the migrated copy.
SELECT count(*) AS previous_RMS_rows FROM window_metrics;
