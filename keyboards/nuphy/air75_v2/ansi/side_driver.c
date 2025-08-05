// Copyright 2023 Persama (@Persama)
// SPDX-License-Identifier: GPL-2.0-or-later
/*
Copyright 2023 @ Nuphy <https://nuphy.com/>

rgb_t    side_leds[SIDE_LED_NUM] = {0};
bool     flush_side_leds         = 0;

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <ch.h>
#include <hal.h>

#include "quantum.h"
#include "ws2812.h"
#include "ws2812_driver.h"

void side_sendByte(uint8_t byte) {
    // WS2812 protocol wants most significant bits first
    for (unsigned char bit = 0; bit < 8; bit++) {
        bool is_one = byte & (1 << (7 - bit));
        // using something like wait_ns(is_one ? T1L : T0L) here throws off timings
        if (is_one) {
            // 1
            gpio_write_pin_high(DRIVER_SIDE_PIN);
            wait_ns(WS2812_T1H);
            gpio_write_pin_low(DRIVER_SIDE_PIN);
            wait_ns(WS2812_T1L);
        } else {
            // 0
            gpio_write_pin_high(DRIVER_SIDE_PIN);
            wait_ns(WS2812_T0H);
            gpio_write_pin_low(DRIVER_SIDE_PIN);
            wait_ns(WS2812_T0L);
        }
    }
}

// Setleds for standard RGB
void side_ws2812_setleds(rgb_led_t *ledarray, uint16_t leds) {
    // this code is very time dependent, so we need to disable interrupts
    chSysLock();

    for (uint8_t i = 0; i < leds; i++) {
        // WS2812 protocol dictates grb order
        side_sendByte(ledarray[i].g);
        side_sendByte(ledarray[i].r);
        side_sendByte(ledarray[i].b);
    }

 //   wait_ns(WS2812_RES);

    chSysUnlock();
}
