SRC += mcu_pwr.c side.c side_table.c rf.c sleep.c rf_driver.c kb_util.c side_driver.c ws2812_bitbang.c rf_queue.c redefine.c

RIGHT_SIDE_ENABLE ?= yes

ifeq ($(RIGHT_SIDE_ENABLE), yes)
    SRC += side_right.c
    OPT_DEFS += -DSIDE_SEPARATE
endif

UART_DRIVER_REQUIRED = yes

# can be separated with needed fields implemented in ansi.h
SRC += debounce.c

UART_DRIVER_REQUIRED = yes
OS_DETECTION_ENABLE = yes

# GCC_EXTRA_OPTIONS =
# OPT = s $(GCC_EXTRA_OPTIONS)

OPT ?= 2
CUSTOM_MATRIX = lite
SRC += matrix.c

# socd cleaner source code
SRC += common/features/socd_cleaner.c

SLEEP_LED_ENABLE = no
