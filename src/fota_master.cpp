#include "fota_master_app.hh"

int main() {
  auto fota = fotaMasterApp();
  fota.start();
  return 0;
}