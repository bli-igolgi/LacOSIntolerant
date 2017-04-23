/*
 * The different key maps based on shift/capslock status
 */

/* The following array is taken from 
 *  http://www.osdever.net/bkerndev/Docs/keyboard.htm
 *  All credits where due
 */
unsigned char reg_key_map[128] = {
  0, 0,  /* Esc */
  '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
  '-', '=', '\b', '\t', 'q', 'w', 'e', 'r', 't', 'y',
  'u', 'i', 'o', 'p', '[', ']', '\n', 0,            /* 29 - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', /* 39 */
 '\'', '`', 0,                                      /* Left shift   */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',                /* 49 */
  'm', ',', '.', '/', 0,                            /* Right shift - 54 */
  '*',
    0,  /* Alt - 56       */
  ' ',  /* Space bar      */
    0,  /* Caps lock - 58 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* F1 - F10 Keys */
    0,  /* Num lock - 69  */
    0,  /* Scroll Lock    */
    0,  /* Home key       */
    0,  /* Up Arrow - 72  */
    0,  /* Page Up        */
  '-',  
    0,  /* Left Arrow     */
    0,  
    0,  /* Right Arrow    */
  '+',  
    0,  /* End key        */
    0,  /* Down Arrow - 80*/
    0,  /* Page Down      */
    0,  /* Insert Key     */
    0,  /* Delete Key     */
    0, 0, 0,
    0,  /* F11 Key        */
    0,  /* F12 Key        */
    0,  /* All other keys are undefined */
};

// Key map for shift
unsigned char shift_key_map[128] = {
  0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')',
  '_', '+', '\b', '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U',
  'I', 'O', 'P', '{', '}', '\n', 0, 'A', 'S', 'D', 'F', 'G',
  'H', 'J', 'K', 'L', ':', '\"', '~', 0, '|', 'Z', 'X', 'C', 'V',
  'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' ', 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '_', 0, 0, 0, '+', 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

// Key map for caps lock
unsigned char caps_key_map[128] = {
  0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
  '-', '=', '\b', '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U',
  'I', 'O', 'P', '[', ']', '\n', 0, 'A', 'S', 'D', 'F', 'G',
  'H', 'J', 'K', 'L', ';', '\'', '`', 0, '\\', 'Z', 'X', 'C', 'V',
  'B', 'N', 'M', ',', '.', '/', 0, '*', 0, ' ', 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '_', 0, 0, 0, '+', 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

// Key map for caps lock and shift
unsigned char caps_shift_key_map[128] = {
  0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')',
  '_', '+', '\b', '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u',
  'i', 'o', 'p', '{', '}', '\n', 0, 'a', 's', 'd', 'f', 'g',
  'h', 'j', 'k', 'l', ':', '\"', '~', 0, '|', 'z', 'x', 'c', 'v',
  'b', 'n', 'm', '<', '>', '?', 0, '*', 0, ' ', 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '_', 0, 0, 0, '+', 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};
