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
#include "lpcon.h"
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <errno.h>
/*----------------------------------------------------------------------------*/
bool LpConnection :: open(const std::string& lpFilePath)
{
  lpClose();
  std::string name = lpFilePath;
  if(name.empty()) {
    name = "/dev/usb/lp0";
  }
  int f = ::open(name.c_str(), O_RDWR | O_NOCTTY);
  if(f < 0) {
    m_error = errno;
    m_message = strerror(errno);
    return false;
  }
  fd = f;
  m_message.clear();
  m_error = 0;
  return true;
}
/*----------------------------------------------------------------------------*/
void LpConnection :: lpClose()
{
  if(fd) {
    ::close(fd);
    fd = 0;
  }
}
/*----------------------------------------------------------------------------*/
int LpConnection :: read(void *buffer, size_t buffer_size, uint32_t timeout)
{
  if(!isOpened()) {
    m_error = -1;
    m_message = "Not opened";
    return -1;
  }

  int received = 0, s;
  struct timeval tv;
  fd_set rfds;

  /* Wait up timeout mseconds. */
  if(timeout > 2147) {
    s = timeout / 1000;
    timeout %= 1000;
  } else {
    s = 0;
  }


  tv.tv_sec = s;
  tv.tv_usec = timeout * 1000;

  /* Watch stdin (fd 0) to see when it has input. */
  FD_ZERO(&rfds);
  FD_SET(fd, &rfds);

  if((!tv.tv_sec && !tv.tv_usec) || select(fd + 1, &rfds, NULL, NULL, &tv) > 0) {
    received = ::read(fd, buffer, buffer_size);
  }

  if(received < 0) {
    m_error = errno;
    m_message = strerror(errno);
    fd = 0;
  }

  return received;
}
/*----------------------------------------------------------------------------*/
int LpConnection :: write(const void *buffer, size_t size)
{
  if(!isOpened()) {
    m_error = -1;
    m_message = "Not opened";
    return -1;
  }
  int r = ::write(fd, buffer, size);
  if(r < 0) {
    m_error = errno;
    m_message = strerror(errno);
    fd = 0;
  }
  return r;
}
/*----------------------------------------------------------------------------*/

