/*----------------------------------------------------------------------------*/
/**
* @pkg usb_ids
*/
/**
* USB VID/PID hardcoded database.
*
* (C) T&T team, Kiev, Ukraine 2025.<br>
* started 25.10.2025 10:28:50<br>
* @pkgdoc usb_ids
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef USB_IDS_H_1761377330
#define USB_IDS_H_1761377330
/*----------------------------------------------------------------------------*/
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

const char* usb_get_vendor_name(uint16_t vid);
const char* usb_get_product_name(uint16_t vid, uint16_t pid);

#ifdef __cplusplus
} //extern "C"
#endif /*__cplusplus*/
/*----------------------------------------------------------------------------*/
#endif /*USB_IDS_H_1761377330*/

