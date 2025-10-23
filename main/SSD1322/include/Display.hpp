#include <stdint.h>
#include <stddef.h>
#include <Font.hpp>
#include <Icon.hpp>

class Display
{
    constexpr const static size_t DISPLAY_HEIGHT = 64;
    constexpr const static size_t DISPLAY_WIDTH = 256;
    constexpr const static size_t BUFFER_SIZE = DISPLAY_HEIGHT * DISPLAY_WIDTH / 2;
    static uint8_t* buffer;
    static spi_device_handle_t display_handle;
    enum class Commands
    {
        ENABLE_GREY_SCALE_TABLE = 0x00,
        SET_COLUMN_ADDRESS = 0x15,
        WRITE_RAM = 0x5C,
        READ_RAM = 0x5D,
        SET_ROW_ADDRESS = 0x75,
        SET_REMAP_DUAL_COM_LINE_MODE = 0xA0,
        SET_DISPLAY_START_LINE = 0xA1,
        SET_DISPLAY_OFFSET = 0xA2,
        SET_MODE_NORMAL = 0xA4,
        SET_MODE_ALL_WITE = 0xA5,
        SET_MODE_ALL_BLACK = 0xA6,
        SET_MODE_INVERSE = 0xA7,
        ENABLE_PARTIAL_DISPLAY = 0xA8,
        EXIT_PARTIAL_DISPLAY = 0xA9,
        SET_FUNCTION_SELECTION = 0xAB,
        DISPLAY_ON = 0xAF,
        DISPLAY_OFF = 0xAE,
        SET_PHASE_LENGTH = 0xB1,
        SET_FRONT_CLOCK_DIVIDER = 0xB3,
        DISPLAY_ENCHANCEMENT_A = 0xB4,
        SET_GPIO = 0xB5,
        SET_PRE_CHARGE_PERIOD = 0xB6,
        SET_GRAY_SCALE_TABLE = 0xB8,
        RESET_GRAY_SCALE_TABLE = 0xB9,
        SET_PRE_CHARGE_VOLTAGE = 0xBB,
        SET_Vcomh_VOLTAGE = 0xBE,
        SET_CONTRAST_CURRENT = 0xC1,
        MASTER_CURRENT_CONTROL = 0xC7,
        SET_MULTIPLEX_RATIO = 0xCA,
        DISPLAY_ENCHANCEMENT_B = 0xD1,
        COMMAND_LOCK = 0xFD,
    };  
    static void sendCommand(Commands command);
    static void sendData(uint8_t data);
public:
    Display() = delete;
    static void initialize();
    static void update();
    static void clear();
    static void drawLine(int16_t from_x, int16_t from_y, int16_t to_x, int16_t to_y, uint8_t color);
    // static void drawRect();
    // static void drawTriangle();
    // static void drawCircle();
    static void drawText(int16_t x, int16_t y, const char* text, Font* font, uint8_t size, uint8_t color);
    static void drawPixel(int16_t x, int16_t y, uint8_t color);
    static void drawBitmap(int16_t x, int16_t y, uint8_t* bitmap, size_t width, size_t height);
    static void drawIcon(int16_t x, int16_t y, Icon* icon, uint8_t size, uint8_t color);
};