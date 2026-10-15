#include "live_fifo_acquisition.h"
char metrology_bytes[sizeof(LiveMetrology)];
char acquisition_bytes[sizeof(LiveFifoAcquisition)];
char engine_bytes[sizeof(LiveCommandEngine)];
char statistics_bytes[sizeof(AcquisitionWindow)];
