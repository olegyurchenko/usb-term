/*----------------------------------------------------------------------------*/
/**
* @pkg connection
*/
/**
* Virtual base for connection.
*
* (C) T&T, Kiev, Ukraine 2026.<br>
* started 15.04.2026 14:23:46<br>
* @pkgdoc connection
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef CONNECTION_H_1776252226
#define CONNECTION_H_1776252226
#include <string>
#include <stdint.h>
/*----------------------------------------------------------------------------*/
class Connection {
protected:
  std::string m_message;
  int m_error = 0;
  Connection() {}
public:
  virtual ~Connection() {}
  virtual void close() = 0;
  virtual int read(void *buffer, size_t buffer_size, uint32_t timeout_ms) = 0;
  virtual int write(const void *buffer, size_t size) = 0;

  virtual bool isOpened() const = 0;
  bool isError() const {return m_error != 0;}
  const std::string& message() const {return m_message;}
  int error() const {return m_error;}

};

/*----------------------------------------------------------------------------*/
#endif /*CONNECTION_H_1776252226*/

