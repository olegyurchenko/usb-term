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
#include "usbcon.h"
#include <stdarg.h>
#include <stdio.h>
#include <libusb-1.0/libusb.h>
#include <memory>
#include <string>
//#include <stdexcept>
#include "usb_ids.h"
/*----------------------------------------------------------------------------*/
static void trace(const char *file, int line, const char *format, ...) {
  va_list ap;
  va_start(ap, format);
  fprintf(stderr, "%s:%d:", file, line);
  vfprintf(stderr, format, ap);
  va_end(ap);
}
/*----------------------------------------------------------------------------*/
std::string string_format( const char *format, ... )
{
  va_list ap;
  va_start(ap, format);
  int size_s = vsnprintf( nullptr, 0, format, ap) + 1; // Extra space for '\0'
  va_end(ap);
  if( size_s <= 0 ){
    //throw std::runtime_error( "Error during formatting." );
    trace(__FILE__, __LINE__, "Error format string: `%s`\n", format);
    return std::string();
  }
  va_start(ap, format);
  auto size = static_cast<size_t>( size_s );
  std::unique_ptr<char[]> buf( new char[ size ] );
  vsnprintf( buf.get(), size, format, ap);
  va_end(ap);
  return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
}
/*----------------------------------------------------------------------------*/
class UsbConnectionPrivate {
public:
  uint16_t vendor_id = 0;
  uint16_t product_id = 0;
  libusb_context *ctx = nullptr;
  libusb_device_handle *dev_handle = nullptr;
  libusb_hotplug_callback_handle callback_handle = 0;
  bool kernel_driver_active = false;
  bool must_reopen = false;
  int config_number = 0;
  int interface_number = 0;
  int altsettings_num = 0;
  uint8_t read_ep = 0;
  uint8_t write_ep = 0;
  std::string message;
  //--------------------------------------
  ~UsbConnectionPrivate() { close();}
  //--------------------------------------
  void close() {
    if(ctx) {
      if(dev_handle) {
        libusb_release_interface(dev_handle, interface_number);
        if(kernel_driver_active) {
          libusb_attach_kernel_driver(dev_handle, interface_number); // Reattach if we detached
        }
        libusb_close(dev_handle);
        dev_handle = nullptr;
      }

      libusb_hotplug_deregister_callback(ctx, callback_handle);
      libusb_exit(ctx);
      ctx = nullptr;
    }
  }
  //--------------------------------------
  bool findEndpoints() {
    struct libusb_config_descriptor *config;
    int config_index = 0, interface_index, altsettings_index;

    read_ep = 0;
    write_ep = 0;
    libusb_get_config_descriptor(libusb_get_device(dev_handle), config_index, &config);
    if (!config) {
      trace(__FILE__, __LINE__, "Could not get config #%d descriptor.\n", config_number);
      message = string_format("Could not get config #%d descriptor.\n", config_number);
      return false;
    }

    config_number = config->bConfigurationValue;
    // Iterate through interfaces and their altsettings to find endpoints
    for (interface_index = 0; interface_index < config->bNumInterfaces; interface_index ++) {
      const struct libusb_interface *inter = &config->interface[interface_index];
      for (altsettings_index = 0; altsettings_index < inter->num_altsetting; altsettings_index ++) {
        const struct libusb_interface_descriptor *inter_desc = &inter->altsetting[altsettings_index];
        interface_number = inter_desc->bInterfaceNumber;
        altsettings_num = inter_desc->bAlternateSetting;

        // Check if this is the Printer class interface you care about (class 7)
        // Or if it's Vendor Specific (class 255)
        if (
            ((inter_desc->bInterfaceClass == LIBUSB_CLASS_PRINTER && (inter_desc->bInterfaceProtocol == 1 || inter_desc->bInterfaceProtocol == 2)) || // Unidirectional or Bidirectional Printer
             (inter_desc->bInterfaceClass == LIBUSB_CLASS_VENDOR_SPEC && inter_desc->bAlternateSetting == 2))) { // Vendor Specific for AltSetting 2

          unsigned char endpoint_in = 0, endpoint_out = 0;
          //out_endpoint_address = find_endpoint(dev_handle, LIBUSB_ENDPOINT_OUT);
          //in_endpoint_address = find_endpoint(dev_handle, LIBUSB_ENDPOINT_IN);

          for (int k = 0; k < inter_desc->bNumEndpoints; k++) {
            const struct libusb_endpoint_descriptor *ep_desc = &inter_desc->endpoint[k];

            if ((ep_desc->bEndpointAddress & LIBUSB_ENDPOINT_DIR_MASK) == LIBUSB_ENDPOINT_IN
                && (ep_desc->bmAttributes & LIBUSB_TRANSFER_TYPE_MASK) == LIBUSB_TRANSFER_TYPE_BULK) { // Look for Bulk endpoints
              endpoint_in = ep_desc->bEndpointAddress;
            }

            if ((ep_desc->bEndpointAddress & LIBUSB_ENDPOINT_DIR_MASK) == LIBUSB_ENDPOINT_OUT
                && (ep_desc->bmAttributes & LIBUSB_TRANSFER_TYPE_MASK) == LIBUSB_TRANSFER_TYPE_BULK) { // Look for Bulk endpoints
              endpoint_out = ep_desc->bEndpointAddress;
            }
            if(endpoint_in != 0 && endpoint_out != 0) {
              read_ep = endpoint_in;
              write_ep = endpoint_out;
              break;
            }
          }
        }
        if(read_ep && write_ep) {
          break;
        }
      }
      if(read_ep && write_ep) {
        break;
      }
    }

    libusb_free_config_descriptor(config);
    return read_ep != 0 && write_ep != 0;
  }
  //--------------------------------------
  static int LIBUSB_CALL hotplugCallback(libusb_context *ctx, libusb_device *dev,libusb_hotplug_event event, void *user_data) {
    struct libusb_device_descriptor desc;
    UsbConnectionPrivate *data;

    (void) ctx;
    data = (UsbConnectionPrivate *) user_data;
    if(data == NULL) {
      trace(__FILE__, __LINE__, "Invalid usb driver data\n");
    }

    int rc = libusb_get_device_descriptor(dev, &desc);
    if (rc < 0) {
      trace(__FILE__, __LINE__, "Error getting device descriptor\n");
      return 0;
    }

    if (event == LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED) {
      trace(__FILE__, __LINE__, "Device attached: %04x:%04x\n", desc.idVendor, desc.idProduct);
      // Тут потрібно спробувати знову відкрити пристрій та налаштувати його
      if(data != NULL) {
        data->must_reopen = true;
      }
    } else if (event == LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT) {
      trace(__FILE__, __LINE__,  "Device detached: %04x:%04x\n", desc.idVendor, desc.idProduct);
      // Тут потрібно очистити всі існуючі хендли та позначити пристрій як недоступний
      if(data != NULL) {
        data->must_reopen = true;
      }
    }
    return 0;
  }

  static int open(uint16_t vendor_id, uint16_t product_id, UsbConnectionPrivate *dst)
  {
    int r = 0;

    dst->vendor_id = vendor_id;
    dst->product_id = product_id;

    r = libusb_init(&dst->ctx);
    if (r < 0) {
      trace(__FILE__, __LINE__, "Failed to initialize libusb: %s\n", libusb_error_name(r));
      return r;
    }
    // Set verbose debugging output
    libusb_set_option(dst->ctx, LIBUSB_OPTION_LOG_LEVEL, /*__debug ? LIBUSB_LOG_LEVEL_DEBUG :*/ LIBUSB_LOG_LEVEL_INFO); // Keep DEBUG level

    // Find the device
    dst->dev_handle = libusb_open_device_with_vid_pid(dst->ctx, vendor_id, product_id);
    if (!dst->dev_handle) {
      trace(__FILE__, __LINE__, "Open device error: VID=0x%04X, PID=0x%04X\n", vendor_id, product_id);
      dst->message = string_format("Open device error: VID=0x%04X, PID=0x%04X", vendor_id, product_id);
      return -1;
    }

    do {
      r = libusb_hotplug_register_callback(dst->ctx,
                                           (libusb_hotplug_event) (LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED | LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT),
                                           LIBUSB_HOTPLUG_ENUMERATE, // Також викликати для вже підключених пристроїв
                                           vendor_id, product_id, LIBUSB_HOTPLUG_MATCH_ANY,
                                           &UsbConnectionPrivate::hotplugCallback, dst, &dst->callback_handle);
      if (r < 0) {
        trace(__FILE__, __LINE__, "Hotplug registration failed: %s\n", libusb_error_name(r));
        dst->message = string_format("Hotplug registration failed: %s", libusb_error_name(r));
        break;
      }
      dst->must_reopen = false;

      // Check if a kernel driver is active and detach it
      dst->kernel_driver_active = libusb_kernel_driver_active(dst->dev_handle, dst->interface_number); // Interface 0
      if (dst->kernel_driver_active) {
        trace(__FILE__, __LINE__, "Kernel driver active, detaching...\n");
        r = libusb_detach_kernel_driver(dst->dev_handle, 0); // Detach interface 0
        if (r < 0) {
          trace(__FILE__, __LINE__, "Failed to detach kernel driver: %s\n", libusb_error_name(r));
          dst->message = string_format("Failed to detach kernel driver: %s", libusb_error_name(r));
          break;
        }
      }

      r = dst->findEndpoints() ? 0 : -1;
      if(r != 0) {
        trace(__FILE__, __LINE__, "Failed to find endpoints\n");
        if(dst->message.empty()) {
          dst->message = "Failed to find endpoints";
        } else {
          dst->message = std::string("Failed to find endpoints:") + dst->message;
        }
        break;
      }

      trace(__FILE__, __LINE__, "Opening: configuration:%d interface:%d alterSettings:%d\n", dst->config_number, dst->interface_number, dst->altsettings_num);
      // Set configuration
      r = libusb_set_configuration(dst->dev_handle, dst->config_number); // Configuration 1 (usually the first one)
      if (r < 0) {
        trace(__FILE__, __LINE__, "Failed to set configuration #%d: %s\n", dst->config_number, libusb_error_name(r));
        dst->message = string_format("Failed to set configuration #%d: %s", dst->config_number, libusb_error_name(r));
        break;
      }

      // Claim interface
      r = libusb_claim_interface(dst->dev_handle, dst->interface_number); // Claim interface 0
      if (r < 0) {
        trace(__FILE__, __LINE__, "Failed to claim interface #%d: %s\n", dst->interface_number, libusb_error_name(r));
        dst->message = string_format("Failed to claim interface #%d: %s", dst->interface_number, libusb_error_name(r));
        break;
      }

      //Try setting Alternate Setting
      r = libusb_set_interface_alt_setting(dst->dev_handle, dst->interface_number, dst->altsettings_num);
      if (r != 0) {
        trace(__FILE__, __LINE__, "Failed to set interface alternate setting to %d: %s\n", dst->altsettings_num, libusb_error_name(r));
        dst->message = string_format("Failed to set interface alternate setting to %d: %s", dst->altsettings_num, libusb_error_name(r));
      }
    } while(0);
    if(r != 0) {
      dst->close();
    }
    return r;
  }

};
/*----------------------------------------------------------------------------*/
bool UsbConnection :: open(uint16_t vendor_id, uint16_t product_id) {
  close();
  con = new UsbConnectionPrivate;
  m_message.clear();
  m_error = UsbConnectionPrivate :: open(vendor_id, product_id, con);
  if(isError()) {
    m_message = con->message;
    delete con;
    con = nullptr;
  }
  return isOpened();
}
/*----------------------------------------------------------------------------*/
bool UsbConnection :: reopen()
{
  if(con) {
    if(!con->must_reopen) {
      return true;
    }
    con->close();
    return open(con->vendor_id, con->product_id);
  }
  m_error = -1;
  m_message = "Not opened";
  return false;
}
/*----------------------------------------------------------------------------*/
void UsbConnection :: close() {
  if(con) {
    delete con;
    con = nullptr;
  }
}
/*----------------------------------------------------------------------------*/
int UsbConnection :: read(void *buffer, size_t size, uint32_t ms)
{
  if(!reopen()) {
    return -1;
  }

  m_message.clear();
  m_error = 0;
  int actual_length = 0;
  int r = libusb_bulk_transfer(con->dev_handle, con->read_ep, static_cast<unsigned char *>(buffer), size, &actual_length, ms);
  if (r < 0) {
    if(r == LIBUSB_ERROR_TIMEOUT) {
      r = 0;
    } else {
      if (r == LIBUSB_ERROR_NO_DEVICE) {
        trace(__FILE__, __LINE__, "Printer disconnected or reset! Re-establishing connection...\n");
        con->must_reopen = 1;
      } else {
        trace(__FILE__, __LINE__, "Failed to read data: %s, timeout:%u\n", libusb_error_name(r), ms);
        m_error = r;
        m_message = libusb_error_name(r);
      }
    }
  } else {
    r = actual_length;
  }
  return r;
}
/*----------------------------------------------------------------------------*/
int UsbConnection :: write(const void *buffer, size_t size)
{
  if(!reopen()) {
    return -1;
  }

  m_message.clear();
  m_error = 0;
  int r = libusb_bulk_transfer(con->dev_handle, con->write_ep, static_cast<unsigned char*>(const_cast<void*>(buffer)), size, nullptr, 1000); // 5000ms timeout for reading
  if (r < 0) {
    if (r == LIBUSB_ERROR_NO_DEVICE) {
      trace(__FILE__, __LINE__, "Printer disconnected or reset! Re-establishing connection...\n");
      con->must_reopen = true;
    } else {
      trace(__FILE__, __LINE__, "Failed to write data: %s, size:%lu\n", libusb_error_name(r), size);
      con->must_reopen = true; //!!!!!!!!!!
    }
    m_error = r;
    m_message = libusb_error_name(r);
  } else {
    r = size;
    //trace_hex(__FILE__, __LINE__, ">>", buffer, size);
  }
  return r;
}
/*----------------------------------------------------------------------------*/
static std::string device_string_descriptor(libusb_device_handle *handle, uint8_t desc_index, const char *name) {
  std::string result;
  if (desc_index == 0) {
    return result;
  }

  unsigned char str[256];
  int res = libusb_get_string_descriptor_ascii(handle, desc_index, str, sizeof(str));
  if (res > 0) {
    result = (const char *)str;
  } else {
    trace(__FILE__, __LINE__, "Error libusb_get_string_descriptor_ascii() for %s: %s\n", name, libusb_error_name(res));
  }
  return result;
}
/*----------------------------------------------------------------------------*/
static bool device_info(libusb_device *dev, UsbDeviceInfo *dst) {
  struct libusb_device_descriptor desc;
  libusb_device_handle *handle = nullptr;
  int r;

  r = libusb_get_device_descriptor(dev, &desc);
  if (r < 0) {
    trace(__FILE__, __LINE__, "Failed libusb_get_device_descriptor(): %d:%s\n", r, libusb_error_name(r));
    return false;
  }

  dst->idVendor = desc.idVendor;
  dst->idProduct = desc.idProduct;
  dst->busNumber = libusb_get_bus_number(dev);
  dst->deviceAddress = libusb_get_device_address(dev);

  r = libusb_open(dev, &handle);
  if (r < 0) {
    trace(__FILE__, __LINE__, "Failed libusb_open(): %d:%s\n", r, libusb_error_name(r));
  }


  if (handle) {
    dst->vendor = device_string_descriptor(handle, desc.iManufacturer, "Vendor");
    dst->product = device_string_descriptor(handle, desc.iProduct, "Product");
    dst->serial = device_string_descriptor(handle, desc.iSerialNumber, "Serial");

    libusb_close(handle);
  } else {
    return false;
  }

  //Get vendor and product name from hardcoded DB
  if(dst->vendor.empty() && dst->product.empty()) {
    const char *str = usb_get_vendor_name(desc.idVendor);
    if(str) {
      dst->vendor = str;
    }
    str = usb_get_product_name(desc.idVendor, desc.idProduct);
    if(str) {
      dst->product = str;
    }
  }

  return true;
}
/*----------------------------------------------------------------------------*/
std::vector<UsbDeviceInfo> usbDeviceList()
{
  libusb_device **devs = nullptr;
  libusb_context *ctx = nullptr;
  int r;
  ssize_t cnt;
  std::vector<UsbDeviceInfo> result;

  do {
    r = libusb_init(&ctx);
    if (r < 0) {
      trace(__FILE__, __LINE__, "Failed libusb_init(): %d:%s\n", r, libusb_error_name(r));
      break;
    }

    cnt = libusb_get_device_list(ctx, &devs);
    if (cnt < 0) {
      trace(__FILE__, __LINE__, "Failed libusb_init(): %d:%s\n", cnt, libusb_error_name(cnt));
      break;
    }

    for (ssize_t i = 0; i < cnt; i++) {
      UsbDeviceInfo info;
      if(device_info(devs[i], &info)) {
        result.push_back(info);
      }

    }
  } while(0);
  if(devs) {
    libusb_free_device_list(devs, 1);
  }
  if(ctx) {
    libusb_exit(ctx);
  }

  return result;
}
/*----------------------------------------------------------------------------*/
