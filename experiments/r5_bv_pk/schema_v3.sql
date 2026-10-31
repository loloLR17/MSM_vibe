-- Apply ONLY to a NEW SQLite backup of the isolated schema2 catalogue.
-- Existing campaigns/metric_definitions/analysis_runs/window_metrics preserved.
CREATE TABLE pk_reference(reference_id TEXT PRIMARY KEY,definition_json TEXT NOT NULL);
CREATE TABLE pk_methods(method_id TEXT PRIMARY KEY,band TEXT NOT NULL,unit TEXT NOT NULL,clock TEXT NOT NULL,
 edition TEXT NOT NULL,derived_version TEXT NOT NULL,reference_id TEXT NOT NULL REFERENCES pk_reference(reference_id),definition_json TEXT NOT NULL);
CREATE TABLE pk_analysis_runs(analysis_id TEXT PRIMARY KEY,campaign_sha TEXT NOT NULL REFERENCES campaigns(campaign_id),
 method_id TEXT NOT NULL REFERENCES pk_methods(method_id),runner_sha256 TEXT NOT NULL,results_sha256 TEXT NOT NULL,manifest_json TEXT NOT NULL,
 UNIQUE(campaign_sha,method_id,runner_sha256));
CREATE TABLE pk_components(analysis_id TEXT NOT NULL REFERENCES pk_analysis_runs(analysis_id),start_mcu_s REAL NOT NULL,
 end_mcu_s REAL NOT NULL,window_mcu_s INTEGER NOT NULL CHECK(window_mcu_s IN (10,60)),axis TEXT NOT NULL CHECK(axis IN ('X','Y','Z')),
 sample_n INTEGER NOT NULL,valid_input_n INTEGER NOT NULL,frame_status TEXT NOT NULL,status TEXT NOT NULL,frequency_Hz REAL,
 component_pk REAL,component_rms REAL,reported_pk REAL,contrast_dB REAL,shape_error REAL,comparison_eligible INTEGER NOT NULL CHECK(comparison_eligible IN (0,1)),
 comparison_zone TEXT NOT NULL,row_json TEXT NOT NULL,PRIMARY KEY(analysis_id,start_mcu_s,window_mcu_s,axis));
CREATE INDEX pk_time_lookup ON pk_components(analysis_id,window_mcu_s,start_mcu_s);
CREATE TABLE pk_artifacts(analysis_id TEXT NOT NULL REFERENCES pk_analysis_runs(analysis_id),kind TEXT NOT NULL,path TEXT NOT NULL,
 sha256 TEXT NOT NULL,PRIMARY KEY(analysis_id,kind));
CREATE TABLE pk_controls(control_id TEXT PRIMARY KEY,definition_json TEXT NOT NULL);
CREATE TABLE pk_migration_audit(singleton INTEGER PRIMARY KEY CHECK(singleton=1),source_path TEXT NOT NULL,source_sha256 TEXT NOT NULL,
 source_snapshot_json TEXT NOT NULL);
PRAGMA application_id=1380270915;
PRAGMA user_version=3;
