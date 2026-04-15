/*----------------------------------------------------------------------------*/
/**
* @pkg lpcon
*/
/**
* Connection for file /dev/usb/lp0.
*
* (C) T&T, Kiev, Ukraine 2026.<br>
* started 15.04.2026 15:50:25<br>
* @pkgdoc lpcon
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef LPCON_H_1776257425
#define LPCON_H_1776257425
/*----------------------------------------------------------------------------*/
#include "connection.h"

class LpConnection : public Connection {
protected:
  int fd = 0;
  void lpClose();
public:
  bool open(const std::string& lpFilePath = std::string());
  virtual ~LpConnection() {lpClose();}
  void close() override {lpClose();};

  int read(void *buffer, size_t buffer_size, uint32_t timeout_ms) override;
  int write(const void *buffer, size_t size) override;

  bool isOpened() const override {return fd != 0;}
};

/*----------------------------------------------------------------------------*/
#endif /*LPCON_H_1776257425*/

