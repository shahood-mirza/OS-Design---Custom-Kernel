//****************************************************************************
//**
//**    kb.c
//**		
//**
//****************************************************************************
//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================
#include <kb.h>
#include <mode7.h>
#include <stdint.h>
#include <irq.h>
#include <registers.h>
#include <port.h>

//============================================================================
//    IMPLEMENTATION PRIVATE DEFINITIONS / ENUMERATIONS / SIMPLE TYPEDEFS
//============================================================================

/*Ports*/
#define KEYBOARD_CONTROL_PORT		0x64 
#define KEYBOARD_DATA_PORT		0x60
#define KEYBOARD_ENCODER_PORT		0x60

/*Control words*/
#define KEYBOARD_OUTPUT_BUFFER_FULL	0X1 // 0000 0001
#define KEYBOARD_INPUT_BUFFER_FULL	0X2 // 0000 0010
#define KEYBOARD_ENCODER_SET_LEDS	0xED
#define KEYBOARD_ENCODER_ACKNOWLEDGE	0xFA

/*Special keys scancodes*/
#define KEYBOARD_KEY_RELEASE		0x80
#define KEYBOARD_LEFT_SHIFT		0x2A
#define KEYBOARD_RIGHT_SHIFT		0x36
#define KEYBOARD_ALT 			0x38
#define KEYBOARD_CONTROL		0x1D
#define KEYBOARD_CAPS_LOCK		0x3A
#define KEYBOARD_NUM_LOCK		0x45
#define KEYBOARD_SCROLL_LOCK		0x46
#define KEYBOARD_ESCAPED		0xE0


//============================================================================
//    IMPLEMENTATION PRIVATE CLASS PROTOTYPES / EXTERNAL CLASS REFERENCES
//============================================================================
//============================================================================
//    IMPLEMENTATION PRIVATE STRUCTURES / UTILITY CLASSES
//============================================================================
//============================================================================
//    IMPLEMENTATION REQUIRED EXTERNAL REFERENCES (AVOID)
//============================================================================
//============================================================================
//    IMPLEMENTATION PRIVATE DATA
//============================================================================

/*An array that matches each scancode to a character */
uint8_t kbScanCodeList[128] =
{
    0,  /* 00: Unused (Internal buffer overrun error code) */
   27,  /* 01: Escape */
  '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 
 '\b',	/* 0E: Backspace */
 '\t',	/* 0F: Tab */
  'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 
 '\n',  /* 1C: Enter key */
    0,  /* 1D: Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 
    0,  /* 2A: Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   
    0,  /* 36: Right shift */
  '*',  
    0,	/* 38: Alt */
  ' ',	/* 39: Space bar */
    0,	/* 3A: Caps lock */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* F1 to F10 */
    0,	/* 45: Num lock*/
    0,	/* 46: Scroll Lock */
  'H',	/* 47: Keypad Home Key */
  '^',	/* 48: Keypad Up Arrow */
  'U',	/* 49: Keypad Page Up */
  '-',
  '<',	/* 4B: Keypad Left Arrow */
    0,  /* 4C: Keypad Center */
  '>',	/* 4D: Keypad Right Arrow */
  '+',
  'E',	/* 4F: Keypad End key*/
  'V',	/* 50: Keypad Down Arrow */
  'D',	/* 51: Keypad Page Down */
  'I',	/* 52: Keypad Insert Key */
  'D',	/* 53: Keypad Delete Key */
    0,  /* Unused */
    0,  /* 55: FN on IBM Thinkpad keyboards */
    0,  /* 56: Unlabelled key on non-US keyboards */
    0,	/* 57: F11 Key */
    0,	/* 58: F12 Key */
    0,	/* 59-7F: Unused */
};

/*An array for the escaped scancodes */
uint8_t kbEscapedList[128] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*00 - 0F*/
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*10 - 1B*/
 '\n',  /* 1C: Keypad enter */
    0,  /* 1D: Right control */
    0, 0, /*1E - 1F*/
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*20 - 29*/
    0,  /* 2A: Fake left shift */
    0, 0, 0, 0, 0, /*2B - 2F*/
    0, 0, 0, 0, 0, /*30 - 34*/
  '/',  /*35: Keypad / */
    0,  /* 36: Fake right shift */
  'P',  /* 37: Print screen */
    0,	/* 38: Right Alt */
    0, 0, 0, 0, 0, 0, 0, /*39 - 3F*/
    0, 0, 0, 0, 0, 0, /* 40 - 45*/
    0,	/* 46: Ctrl + Break */
  'H',	/* 47: Grey Home Key*/
  '^',	/* 48: Grey Up Arrow */
  'P',	/* 49: Grey Page Up */
    0,
  '<',	/* 4B: Grey Left Arrow */
    0,  
  '>',	/* 4D: Grey Right Arrow */
    0,
  'E',	/* 4F: Grey End key*/
  'V',	/* 50: Grey Down Arrow */
  'D',	/* 51: Grey Page Down */
  'I',	/* 52: Grey Insert Key */
  'D',	/* 53: Grey Delete Key */
    0, 0, 0, 0, 0, 0, 0, 
    0,  /* 5B: Left Windows Key*/
    0,  /* 5C: Right Windows Key*/
    0,  /* 5D: Menu Key*/
    0,  /* 5E-7F: Unused */
};

/*Special key status*/
uint32_t _alt;
uint32_t _ctrl;
uint32_t _shift;
uint32_t _capslock;
uint32_t _scrolllock;
uint32_t _numlock;
uint32_t _escaped;


//============================================================================
//    INTERFACE DATA
//============================================================================
//============================================================================
//    IMPLEMENTATION PRIVATE FUNCTION PROTOTYPES
//============================================================================


/* Send a command to the keyboard control register */
void keyboard_send_command (uint8_t port, uint8_t cmd);

/* Wait for an acknowledge signal from the keyboard encoder */
void keyboard_encoder_wait ();

/* Wait for keyboard controller output buffer to be full */
void keyboard_controller_read_wait();

/* Wait for keyboard controller input buffer to be clear */
void keyboard_controller_write_wait();

/* Set the status of the leds */
void keyboard_set_leds ();

/* Functions to change certain scancode mappings depending on special keys */
void keyboard_keyswap_caps();
void keyboard_keyswap_shift();
void keyboard_keyswap_numlock();

/* Keyboard handler function */
void keyboard_handler(struct isrregs *r);

//============================================================================
//    IMPLEMENTATION PRIVATE FUNCTIONS
//============================================================================


void keyboard_send_command (uint8_t port, uint8_t cmd) {
// implement
}

void keyboard_encoder_wait ()
{
// implement
}

void keyboard_controller_read_wait()
{
// implement

}

void keyboard_controller_write_wait()
{
// implement
}

void keyboard_set_leds () 
{
// implement
}

void keyboard_keyswap_caps()
{

	/*check any letter to know which more we are in*/
	if (kbScanCodeList[0x10] == 'q')
	{
		/*lowercase to uppercase*/
		kbScanCodeList[0x10] = 'Q';
		kbScanCodeList[0x11] = 'W';
		kbScanCodeList[0x12] = 'E';
		kbScanCodeList[0x13] = 'R';
		kbScanCodeList[0x14] = 'T';
		kbScanCodeList[0x15] = 'Y';
		kbScanCodeList[0x16] = 'U';
		kbScanCodeList[0x17] = 'I';
		kbScanCodeList[0x18] = 'O';
		kbScanCodeList[0x19] = 'P';
		kbScanCodeList[0x1E] = 'A';
		kbScanCodeList[0x1F] = 'S';
		kbScanCodeList[0x20] = 'D';
		kbScanCodeList[0x21] = 'F';
		kbScanCodeList[0x22] = 'G';
		kbScanCodeList[0x23] = 'H';
		kbScanCodeList[0x24] = 'J';
		kbScanCodeList[0x25] = 'K';
		kbScanCodeList[0x26] = 'L';
		kbScanCodeList[0x2C] = 'Z';
		kbScanCodeList[0x2D] = 'X';
		kbScanCodeList[0x2E] = 'C';
		kbScanCodeList[0x2F] = 'V';
		kbScanCodeList[0x30] = 'B';
		kbScanCodeList[0x31] = 'N';
		kbScanCodeList[0x32] = 'M';
	}
	else
	{
		/*uppercase to lowercase*/
		kbScanCodeList[0x10] = 'q';
		kbScanCodeList[0x11] = 'w';
		kbScanCodeList[0x12] = 'e';
		kbScanCodeList[0x13] = 'r';
		kbScanCodeList[0x14] = 't';
		kbScanCodeList[0x15] = 'y';
		kbScanCodeList[0x16] = 'u';
		kbScanCodeList[0x17] = 'i';
		kbScanCodeList[0x18] = 'o';
		kbScanCodeList[0x19] = 'p';
		kbScanCodeList[0x1E] = 'a';
		kbScanCodeList[0x1F] = 's';
		kbScanCodeList[0x20] = 'd';
		kbScanCodeList[0x21] = 'f';
		kbScanCodeList[0x22] = 'g';
		kbScanCodeList[0x23] = 'h';
		kbScanCodeList[0x24] = 'j';
		kbScanCodeList[0x25] = 'k';
		kbScanCodeList[0x26] = 'l';
		kbScanCodeList[0x2C] = 'z';
		kbScanCodeList[0x2D] = 'x';
		kbScanCodeList[0x2E] = 'c';
		kbScanCodeList[0x2F] = 'v';
		kbScanCodeList[0x30] = 'b';
		kbScanCodeList[0x31] = 'n';
		kbScanCodeList[0x32] = 'm';
	}

}

void keyboard_keyswap_shift()
{
	keyboard_keyswap_caps();
	if (_shift == 1)
	{
  		kbScanCodeList[0x02] = '!';
		kbScanCodeList[0x03] = '@';
		kbScanCodeList[0x04] = '#';
		kbScanCodeList[0x05] = '$';
		kbScanCodeList[0x06] = '%';
		kbScanCodeList[0x07] = '^';
		kbScanCodeList[0x08] = '&';
		kbScanCodeList[0x09] = '*';
		kbScanCodeList[0x0A] = '(';
		kbScanCodeList[0x0B] = ')';
		kbScanCodeList[0x0C] = '_';
		kbScanCodeList[0x0D] = '+';
		kbScanCodeList[0x1A] = '{';
		kbScanCodeList[0x1B] = '}';
		kbScanCodeList[0x27] = ':';
		kbScanCodeList[0x28] = '"';
		kbScanCodeList[0x29] = '~';
		kbScanCodeList[0x2B] = '¦';
		kbScanCodeList[0x33] = '<';
		kbScanCodeList[0x34] = '>';
		kbScanCodeList[0x35] = '?';
	}
	else
	{
  		kbScanCodeList[0x02] = '1';
		kbScanCodeList[0x03] = '2';
		kbScanCodeList[0x04] = '3';
		kbScanCodeList[0x05] = '4';
		kbScanCodeList[0x06] = '5';
		kbScanCodeList[0x07] = '6';
		kbScanCodeList[0x08] = '7';
		kbScanCodeList[0x09] = '8';
		kbScanCodeList[0x0A] = '9';
		kbScanCodeList[0x0B] = '0';
		kbScanCodeList[0x0C] = '-';
		kbScanCodeList[0x0D] = '=';
		kbScanCodeList[0x1A] = '[';
		kbScanCodeList[0x1B] = ']';
		kbScanCodeList[0x27] = ';';
		kbScanCodeList[0x28] = '\'';
		kbScanCodeList[0x29] = '`';
		kbScanCodeList[0x2B] = '\\';
		kbScanCodeList[0x33] = ',';
		kbScanCodeList[0x34] = '.';
		kbScanCodeList[0x35] = '/';
	}
}

void keyboard_keyswap_numlock()
{
	if (_numlock == 1)
	{
  		kbScanCodeList[0x47] = '7';
		kbScanCodeList[0x48] = '8';
		kbScanCodeList[0x49] = '9';
		kbScanCodeList[0x4B] = '4';
		kbScanCodeList[0x4C] = '5';
		kbScanCodeList[0x4D] = '6';
		kbScanCodeList[0x4F] = '1';
		kbScanCodeList[0x50] = '2';
		kbScanCodeList[0x51] = '3';
		kbScanCodeList[0x52] = '0';
		kbScanCodeList[0x53] = '.';
	}
	else
	{
  		kbScanCodeList[0x47] = 'H';
		kbScanCodeList[0x48] = '^';
		kbScanCodeList[0x49] = 'U';
		kbScanCodeList[0x4B] = '<';
		kbScanCodeList[0x4C] = 0;
		kbScanCodeList[0x4D] = '>';
		kbScanCodeList[0x4F] = 'E';
		kbScanCodeList[0x50] = 'V';
		kbScanCodeList[0x51] = 'D';
		kbScanCodeList[0x52] = 'I';
		kbScanCodeList[0x53] = 'D';
	}
	
}

/* Handles the keyboard interrupt */
void keyboard_handler(struct isrregs *r) 
{
    // implement
}

//============================================================================
//    INTERFACE FUNCTIONS
//============================================================================

/* Installs the keyboard handler into IRQ1 */
void install_keyboard()
{
    // implement
	irq_install_handler(1, keyboardHandler);
}

//============================================================================
//    INTERFACE CLASS BODIES
//============================================================================
//****************************************************************************
//**
//**    END[kb.c]
//**
//****************************************************************************
