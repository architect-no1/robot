#include "Config.h"

#include "../config4cpp/Configuration.h"

namespace yolo {

#define BASESPEED_DEFAULT 6
int BASESPEED = BASESPEED_DEFAULT;

static Configuration *cfg = NULL;

void readBaseConfig(const char *filename) {
  cfg = Configuration::create();
  try {
    cfg->parse(filename);
    BASESPEED = cfg->lookupInt("", "BASESPEED", BASESPEED_DEFAULT);
  } catch (ConfigurationException e) {
  }
}

} // namespace yolo
