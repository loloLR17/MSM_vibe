-- Secondary isolated catalogue. NEVER run against existing R5 schema1/service DB.
PRAGMA application_id=1380270914;
PRAGMA user_version=2;
CREATE TABLE campaigns(campaign_id TEXT PRIMARY KEY, source_path TEXT NOT NULL, manifest_json TEXT NOT NULL);
CREATE TABLE metric_definitions(
 metric_definition_id TEXT PRIMARY KEY, version TEXT NOT NULL, normative_edition TEXT NOT NULL,
 unit TEXT NOT NULL CHECK(unit IN ('mg','mm/s')), band_low_hz REAL NOT NULL,band_high_hz REAL NOT NULL,
 axes_status TEXT NOT NULL,definition_json TEXT NOT NULL
);
CREATE TABLE analysis_runs(
 analysis_id TEXT PRIMARY KEY,campaign_id TEXT NOT NULL REFERENCES campaigns(campaign_id),
 metric_definition_id TEXT NOT NULL REFERENCES metric_definitions(metric_definition_id),
 code_sha256 TEXT NOT NULL,results_digest TEXT NOT NULL, status TEXT NOT NULL,
 UNIQUE(campaign_id,metric_definition_id,code_sha256)
);
CREATE INDEX run_by_campaign ON analysis_runs(campaign_id,metric_definition_id);
CREATE TABLE window_metrics(
 analysis_id TEXT NOT NULL REFERENCES analysis_runs(analysis_id),window_mcu_s INTEGER NOT NULL CHECK(window_mcu_s IN (60,120)),
 start_mcu_s REAL NOT NULL,end_mcu_s REAL NOT NULL, observed_n INTEGER NOT NULL,
 valid_n INTEGER NOT NULL,coverage_observed REAL NOT NULL,quality TEXT NOT NULL,
 rms_x REAL,rms_y REAL,rms_z REAL,direction_max REAL,vector_technical REAL,peak_vector REAL,
 PRIMARY KEY(analysis_id,window_mcu_s,start_mcu_s)
);
CREATE TABLE artifact_refs(
 analysis_id TEXT NOT NULL REFERENCES analysis_runs(analysis_id),kind TEXT NOT NULL,
 path TEXT NOT NULL,sha256 TEXT NOT NULL,PRIMARY KEY(analysis_id,kind)
);
