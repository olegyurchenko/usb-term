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
#include <string>
#include <vector>
#include <stdint.h>
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
class UsbConnection {
protected:
  UsbConnectionPrivate *con = nullptr;
  std::string m_message;
  int m_error = 0;
  bool reopen();
public:
  bool open(uint16_t vendor_id, uint16_t product_id);
  void close();
  ~UsbConnection() {close();}

  int read(void *buffer, size_t buffer_size, uint32_t timeout_ms);
  int write(const void *buffer, size_t size);

  bool isOpened() const {return con != nullptr;}
  bool isError() const {return m_error != 0;}
  const std::string& message() const {return m_message;}
  int error() const {return m_error;}
};
/*----------------------------------------------------------------------------*/
#endif /*USBCON_H_1761289625*/

