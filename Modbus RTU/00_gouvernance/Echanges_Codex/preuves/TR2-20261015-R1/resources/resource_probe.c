#include "tr2/application/live_configuration.h"
char r1_service_size[sizeof(LiveConfiguration)];
char r1_media_size[sizeof(TransactionalImageMedia)];
char r1_candidate_size[TR2_LIVE_LOGICAL_SIZE];
char r1_snapshot_size[sizeof(ActiveConfigurationSnapshot)];
