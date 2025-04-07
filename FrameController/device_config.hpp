extern "C"
{
  float tempmonGetTemp(void);

  struct usb_string_descriptor_struct
  {
      uint8_t bLength;
      uint8_t bDescriptorType;
      uint16_t wString[10];
  };

  usb_string_descriptor_struct usb_string_serial_number =
  {
       22,  // 2 + 2*length of the sn string
       3,
       {'M','Y', 'S','N', '0', '0', '0','0', '1', 0},
  };
}