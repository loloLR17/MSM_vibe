-- Isolated R5 analysis catalogue, NEVER a migration of TR2 supervision schema7.
PRAGMA application_id = 1380270913; -- 0x52455341: RESA, reserved catalogue marker
PRAGMA user_version = 1;
CREATE TABLE campaigns (
 campaign_id TEXT PRIMARY KEY CHECK(length(campaign_id)=64), source_path TEXT NOT NULL,
 uid_json TEXT NOT NULL, session TEXT NOT NULL, format_version INTEGER NOT NULL,
 firmware_sha TEXT NOT NULL, site TEXT NOT NULL, utc_status TEXT NOT NULL,
 duration_mcu_s REAL NOT NULL, rate_est_hz REAL NOT NULL, sensitivity_mg_per_count REAL NOT NULL,
 qualification TEXT NOT NULL, manifest_json TEXT NOT NULL
);
CREATE TABLE analysis_runs (
 analysis_id TEXT PRIMARY KEY, campaign_id TEXT NOT NULL REFERENCES campaigns(campaign_id),
 algorithm TEXT NOT NULL, version TEXT NOT NULL, parameters_json TEXT NOT NULL,
 results_digest TEXT NOT NULL, status TEXT NOT NULL CHECK(status='COMPLETE')
);
CREATE INDEX analysis_by_campaign ON analysis_runs(campaign_id, analysis_id);
CREATE TABLE window_metrics (
 analysis_id TEXT NOT NULL REFERENCES analysis_runs(analysis_id),
 window_s INTEGER NOT NULL CHECK(window_s IN (1,10,60)), start_mcu_s REAL NOT NULL,
 end_mcu_s REAL NOT NULL, indicator TEXT NOT NULL CHECK(indicator IN ('raw_DC','band_2_25')),
 unit TEXT NOT NULL CHECK(unit='mg'), band_low_hz REAL, band_high_hz REAL,
 valid_samples INTEGER NOT NULL CHECK(valid_samples>=0), coverage_est REAL NOT NULL,
 quality TEXT NOT NULL, rms_x REAL, rms_y REAL, rms_z REAL, rms_global REAL,
 peak_x REAL, peak_y REAL, peak_z REAL, peak_vector REAL,
 PRIMARY KEY(analysis_id, window_s, indicator, start_mcu_s)
);
CREATE INDEX metrics_by_series ON window_metrics(analysis_id, indicator, window_s, start_mcu_s);
CREATE TABLE artifact_refs (
 analysis_id TEXT NOT NULL REFERENCES analysis_runs(analysis_id), kind TEXT NOT NULL,
 path TEXT NOT NULL, sha256 TEXT NOT NULL, bytes INTEGER NOT NULL,
 PRIMARY KEY(analysis_id,kind)
);
