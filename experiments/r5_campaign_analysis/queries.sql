-- Read-only queries against the isolated catalogue, bind :analysis_id.
SELECT campaign_id, source_path, site, utc_status, duration_mcu_s FROM campaigns;
SELECT start_mcu_s, rms_global, coverage_est, quality
FROM window_metrics WHERE analysis_id=:analysis_id AND indicator='band_2_25'
AND window_s=60 ORDER BY start_mcu_s;
SELECT start_mcu_s, rms_global, peak_vector, quality
FROM window_metrics WHERE analysis_id=:analysis_id AND indicator='band_2_25'
AND window_s=10 AND quality='VALID_ESTIMATED'
ORDER BY rms_global DESC,start_mcu_s LIMIT 1;
SELECT start_mcu_s, rms_x, rms_y, rms_z, quality
FROM window_metrics WHERE analysis_id=:analysis_id AND indicator='band_2_25'
AND window_s=1 AND start_mcu_s>=600 AND start_mcu_s<660 ORDER BY start_mcu_s;
