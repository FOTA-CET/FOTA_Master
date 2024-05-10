#include "fota_master_app.hh"

int main() {
  auto fota = fotaMasterApp();
  fota.configure();
  fota.start();
  return 0;
}