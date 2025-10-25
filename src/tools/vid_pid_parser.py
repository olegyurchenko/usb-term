#!/usr/bin/env python3

def parse_usb_ids(filename):
    vendors = {}
    current_vendor = None
    
    with open(filename, 'r', encoding='utf-8', errors='ignore') as f:
        for line in f:
            if line.startswith('#') or not line.strip():
                continue
            
            # Vendor line (no leading tab)
            if not line.startswith('\t'):
                parts = line.strip().split(None, 1)
                if len(parts) == 2 and len(parts[0]) == 4:
                    vid = parts[0]
                    name = parts[1].replace('"', '\\"')
                    vendors[vid] = {'name': name, 'products': {}}
                    current_vendor = vid
            
            # Product line (one leading tab)
            elif current_vendor and line.startswith('\t') and not line.startswith('\t\t'):
                parts = line.strip().split(None, 1)
                if len(parts) == 2 and len(parts[0]) == 4:
                    pid = parts[0]
                    name = parts[1].replace('"', '\\"')
                    vendors[current_vendor]['products'][pid] = name
    
    return vendors

def generate_c_code(vendors, output_file):
    with open(output_file, 'w') as f:
        f.write('#include <stdint.h>\n')
        f.write('#include <stddef.h>\n\n')
        f.write('typedef struct {\n')
        f.write('    uint16_t id;\n')
        f.write('    const char *name;\n')
        f.write('} usb_id_t;\n\n')
        
        f.write('typedef struct {\n')
        f.write('    uint16_t vid;\n')
        f.write('    const char *vendor_name;\n')
        f.write('    const usb_id_t *products;\n')
        f.write('    int product_count;\n')
        f.write('} usb_vendor_t;\n\n')
        
        # Generate product arrays
        for vid, vendor_data in vendors.items():
            if vendor_data['products']:
                f.write(f'static const usb_id_t products_{vid}[] = {{\n')
                for pid, pname in vendor_data['products'].items():
                    f.write(f'    {{0x{pid}, "{pname}"}},\n')
                f.write('};\n\n')
        
        # Generate vendor array
        f.write('static const usb_vendor_t usb_vendors[] = {\n')
        for vid, vendor_data in vendors.items():
            vname = vendor_data['name']
            if vendor_data['products']:
                prod_count = len(vendor_data['products'])
                f.write(f'    {{0x{vid}, "{vname}", products_{vid}, {prod_count}}},\n')
            else:
                f.write(f'    {{0x{vid}, "{vname}", NULL, 0}},\n')
        f.write('};\n\n')
        
        f.write(f'#define USB_VENDOR_COUNT {len(vendors)}\n\n')
        
        # Generate lookup functions
        f.write('''
const char* usb_get_vendor_name(uint16_t vid) {
    for (int i = 0; i < USB_VENDOR_COUNT; i++) {
        if (usb_vendors[i].vid == vid) {
            return usb_vendors[i].vendor_name;
        }
    }
    return NULL;
}

const char* usb_get_product_name(uint16_t vid, uint16_t pid) {
    for (int i = 0; i < USB_VENDOR_COUNT; i++) {
        if (usb_vendors[i].vid == vid) {
            for (int j = 0; j < usb_vendors[i].product_count; j++) {
                if (usb_vendors[i].products[j].id == pid) {
                    return usb_vendors[i].products[j].name;
                }
            }
            return NULL;
        }
    }
    return NULL;
}
''')

if __name__ == '__main__':
    vendors = parse_usb_ids('usb.ids')
    generate_c_code(vendors, 'usb_ids.c')
    print(f'Generated usb_ids.c with {len(vendors)} vendors')
