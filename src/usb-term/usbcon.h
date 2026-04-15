/*----------------------------------------------------------------------------*/
/**
* @pkg usbcon
*/
/**
* USB connection class.
*
* (C) T&T, Kiev, Ukraine 2025.<br>
* started 24.10.2025 10:07:05<br>
* @pkgdoc usbcon
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef USBCON_H_1761289625
#define USBCON_H_1761289625
/*----------------------------------------------------------------------------*/
#include <vector>
#include "connection.h"
/*----------------------------------------------------------------------------*/
struct UsbDeviceInfo {
  uint16_t idVendor = 0;
  uint16_t idProduct = 0;
  int busNumber = 0;
  int deviceAddress = 0;
  std::string vendor;
  std::string product;
  std::string serial;
};
std::vector<UsbDeviceInfo> usbDeviceList();

class UsbConnectionPrivate;
class UsbConnection : public Connection{
protected:
  UsbConnectionPrivate *con = nullptr;
  std::string m_message;
  int m_error = 0;
  bool reopen();
  void usbClose();
public:
  bool open(uint16_t vendor_id, uint16_t product_id);
  void close() override {usbClose();};
  ~UsbConnection() {usbClose();}

  int read(void *buffer, size_t buffer_size, uint32_t timeout_ms) override;
  int write(const void *buffer, size_t size) override;

  bool isOpened() const override {return con != nullptr;}
};
/*----------------------------------------------------------------------------*/
#endif /*USBCON_H_1761289625*/

