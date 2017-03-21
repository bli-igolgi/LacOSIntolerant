/* The following array is taken from 
 *  http://www.osdever.net/bkerndev/Docs/keyboard.htm
 *  All credits where due
 */
unsigned char reg_key_map[128] = {
  0, 27,  /* Esc        */
  '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
  '-', '=', '\b', '\t', 'q', 'w', 'e', 'r', 't', 'y',
  'u', 'i', 'o', 'p', '[', ']', '\n', 0,            /* 29 - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', /* 39 */
 '\'', '`', 0,                                      /* Left shift   */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',                /* 49 */
  'm', ',', '.', '/', 0,                            /* Right shift - 54 */
  '*',
    0,  /* Alt            */
  ' ',  /* Space bar      */
    0,  /* Caps lock - 58 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* F1 - F10 Keys */
    0,  /* Num lock - 69  */
    0,  /* Scroll Lock    */
    0,  /* Home key       */
    0,  /* Up Arrow       */
    0,  /* Page Up        */
  '-',  
    0,  /* Left Arrow     */
    0,  
    0,  /* Right Arrow    */
  '=',  
    0,  /* 79 - End key   */
    0,  /* Down Arrow     */
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
  0, 27,  /* Esc        */
  '!', '@', '#', '$', '%', '^', '&', '*', '(', ')',
  '_', '+', '\b', '\t', 'Q', 'W', 'E', 'R', 'T', 'Y',
  'U', 'I', 'O', 'P', '{', '}', '\n', 0,            /* 29 - Control */
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', /* 39 */
 '\"', '~', 0,                                      /* Left shift   */
 '\\', 'Z', 'X', 'C', 'V', 'B', 'N',                /* 49 */
  'M', '<', '>', '?', 0,                            /* Right shift - 54 */
  '*',
    0,  /* Alt            */
  ' ',  /* Space bar      */
    0,  /* Caps lock - 58 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* F1 - F10 Keys */
    0,  /* Num lock - 69  */
    0,  /* Scroll Lock    */
    0,  /* Home key       */
    0,  /* Up Arrow       */
    0,  /* Page Up        */
  '_',  
    0,  /* Left Arrow     */
    0,  
    0,  /* Right Arrow    */
  '+',  
    0,  /* 79 - End key   */
    0,  /* Down Arrow     */
    0,  /* Page Down      */
    0,  /* Insert Key     */
    0,  /* Delete Key     */
    0, 0, 0,
    0,  /* F11 Key        */
    0,  /* F12 Key        */
    0,  /* All other keys are undefined */
};
