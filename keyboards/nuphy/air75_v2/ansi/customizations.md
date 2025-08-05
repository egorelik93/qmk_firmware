# JinCao1 Non-Stock Customizations & Fixes

The following customizations/changes/fixes were applied on top of NuPhy's stock firmware. It includes various personal enhancements and bug fixes.

## Customizations

- `Fn + B` when held temporarily displays the current battery percentage on the F and number row.
The F row represents the 10s percentage and number row the ones. Example, 35% will light `F3` and `5`.
- `Fn + \` turns on percent battery display as well as the stock side LED indicator. Side LED battery gauge steps are enhanced, turning off 1 LED every 20%.
- Battery indicator colours: green for 100% to 81%, yellow for 80% to 41%, orange/amber for 40% to 21%, and red for 20% and below.
- `Fn + M + Z` to toggle the RF disconnect sleep timer between `5s` and `120s` (NuPhy default). Default is set to `5s`. This persist through restarts.
This sets how long the board tries to connect (left light blinking) before giving up.
- `Fn + M + S` toggles idle sleep timer between `30s` and default `360s`. This is temporary.
- `Fn + M + D` toggles QMK debugging. Don't turn this on when not connected to QMK toolbox.
The letter `D` will light up red when enabled.
- Side indicators will flash red for 0.5s when board enters sleep mode, as an indicator.
This is a deep sleep state. There are no indicators in other sleep modes.
- Bluetooth connection indicators will be lit blue when establishing connection. This lights the corresponding
BT mode key. No indicator for RF as the sidelight is a different colour.
- Default startup LED brightness set to zero and side led set to lowest brightness. This is because I don't use LEDs so I don't need to toggle them off when resetting the board or flashing new firmware.
- 3ms debounce instead of 2ms (potential stability)
- 3 sleep modes (inspired by @adi4086) - Toggle sleep mode button moved to `Fn + M + ]`.
  - Deep Sleep (NRF off, MCU off, LED off) - lowest power consumption. This is the default. Right indicator blinks green.
  - Light Sleep (NRF off, LED off) - no real reason to use this, but might wake up quicker. Right indicator blinks yellow.
  - No Sleep - for those that want their board to always be on... Right indicator blinks red.
- Keyboard will never go to deep sleep in USB mode. This seems to cause issues on wake and I don't have a solution. I'm expecting that the device is powered and if it's not the keyboard would reset anyway.
- Keyboard won't go to deep sleep if charging on wireless mode as charging interrupts the MCU causing it to sleep and wake repeatedly. To restore the proper sleep mode you must wake the board while it's off the charger.
- Moved RGB brightness down/up to layer 4 as `Fn + M + F1` and `Fn + M + F2` respectively to reduce chance of mispress in NuFolio case. This matches the brightness down/up keys on F1 and F2.

## Fixes

- Fix keyboard randomly crashing/freezing.
- Fix keyboard not sleeping properly and draining battery. This version sleeps the processor and uses almost no battery on sleep.
- Fix LED lights not powering down when not used. This increases battery life around 50-70% when LEDs aren't used.
- Fix keystrokes being lost on wake. Wake keystrokes will appear after a very short delay while board re-establishes connection. BT wake keys may not be as reliable as the 2.4ghz dongle.  
  This is achieved through a buffer of 64 key actions (key down and key up are 2 actions). The buffer is cleared if connection is not established within 1s after the last action.
  Key events after the buffer is full will also be dropped.
- Enhance keyboard reports transmission logic to greatly reduce stuck/lost key strokes. It may still occasionally drop/repeat keys but it's rare.
- Slightly enhanced sidelight refresh intervals for smoother animations.
- Reduced unused side LED tables to save a chunk of memory. This may be essential to the RF queue as the board only has 16kb memory available - the queue alone uses over 1.2kb.

## VIA

This firmware is still compatible with VIA. Grab the latest/most relevant [VIA JSON](/keyboards/nuphy/air75_v2/ansi/keymaps/via/air75_v2_via_v3.json) and load it into VIA using the *Design* tab. This is used the same way as stock NuPhy. I'm not a Mac user so this is default behaviour placed here for reference.

The following keys are unnamed as they are QMK key combinations. They will show as follows in VIA as there is no way to customize these names in VIA.

| Key Code     | Function               |
| ------------ | ---------------------- |
| `0xc1`       | Launch Mission Control |
| `0xc2`       | Launchpad              |
| `G(KC_SPC)`  | Mac Search             |
| `G(S(KC_4))` | Mac Print Screen Area  |
| `G(S(KC_3))` | Mac Print Screen Whole |

## RF Firmware

This firmware is built and tested against NuPhy's RF firmware `v1.0.3`. Refer to the [RF Firmware folder](rf_firmware) if you want to get the update.
The experience you get with the firmware may vary from mine and is dependent on various external factors.

## Author

[@jincao1](https://github.com/jincao1)

# adi4086 Customizations & Fixes

The following customizations were applied on top of the stock firmware. 

-  Fn + M + O Changes the side light to display on either left side or right side or alternating sides every 1 minute.

-  Numlock indicator using side leds while color

-  FN + M + N shorcut will change the led indicator from side leds to KC_NUM key. If KC_NUM does not exist it will look for KC_INS and NUMLOCK_INS

-  New custom key: NUMLOCK_INS. When held for more than 300ms it will trigger NUM_LOCK when pressed it will act as KC_INS

-  New algorithm to prevent unnecessary EEPROM writes. When you change RGB modes or side lights it will only save the setting to EEPROM after 30 seconds. <br />
   If for instance your need to press mode button for 5 times to reach your favourite RGB effect, normally you will get 1 EEPROM save for each press (5 EEPROM writes). <br />
   With the new algorithm you will get 1 write to EEPROM after 40 seconds from last press. As a side efect if you change something and you shutdown the keyboard immediately you will lose the change. <br />

-  Bluetooth and RF will signal on corresponding number keys when trying to connect (Blue color and keys 1 to 3 for BT_1 to BT_3 or Green color and key 4 when it is trying to connect using 2.4G)

-  Side light brightness levels are now 7 (instead of 6) from 0 (disabled) to 6 (full). A new lower step has been introduced. Default level after reset is 1 (the lowest ilumination possible for side leds)

-  The system indicators now have 3 levels of brightness (before there was only 1 level) that are dependend on the side light brightness. Thus for example capslock will no longer light at full brightness if side leds have a lower brightness selected.

-  The custom MAC keys now have double functionality: <br />
<pre>    MAC_VOICE if Mac mode selected and F5             if Win mode is selected 
    TASK      if Mac mode selected and KC_CALC        if Win mode is selected 
    SEARCH    if Mac mode selected and KC_LCTL + KC_F if Win mode is selected 
    PRT_SCR   if Mac mode selected and PrintScreen    if Win mode is selected 
    PRT_AREA  if Mac mode selected and PrintScreen    if Win mode is selected </pre>

-  Fn + B will show current battery levels using numbers from 1 to 0 (0 means 100%). The color will be red, yellow, light green, dark green) based on battery level. <br />
   Last led will have a different color based on battery percentage as well. <br />
   Example: <br />
   if 1 to 4 lights up and the last led that lights up 5 is dark green color you will have between 47 and 50% battery <br />
   if 1 to 4 lights up and the last led that lights up 5 is light green color you will have between 44 and 46 % battery <br />
   if 1 to 4 lights up and the last led that lights up 5 is red color you will have between 41 and 43 % battery <br />

-  Default startup LED brightness is set to half of max. Default RGB effect is RGB_MATRIX_CYCLE_LEFT_RIGHT

-  Default tapping has been reduced to 2ms (from 8ms default)

-  LTO enabled to reduce firmware size

-  Side light algorithms are modified in order to reduce firmware size and to make animations more smooth.

-  Enhance keyboard reports transmission logic to greatly reduce stuck/lost key strokes. It may still occasionally drop/repeat keys but it's rare. (algorithm from jincao1 with small modifications)

-  Fix keyboard randomly crashing/freezing (algorithm from jincao1 with small modifications)

-  MCU Deep / Light sleep algorithm is applied using code from nuphy / jincao1

-  Fix LED lights not powering down when not used.

-  Fix keystrokes being lost on wake. Wake keystrokes will appear after a very short delay while board re-establishes connection. BT may not be as reliable as the dongle. <br />
   This is achieved through a buffer of 64 key actions (key down and key up are 2 actions). The buffer is cleared if connection is not established within 1s after the last action. <br />
   Key events after the buffer is full will also be dropped. (Buffer algorithm taken from jincao1) <br />

-  FN + M + F1 => enables Debug mode. This mode will show messages in console including matrix scan rate and various settings changes

-  FN + M + ESC => will reboot keyboard. If you do not unpress the ESC key, it will enter bootmode so you can load new firmware.

-  FN + M + F2 => Long press will enter DFU mode so you can upload new RF firmware. The key will remain lit up as red until you reboot the keyboard. This mode is activated only when keyboard is connected to laptop/computer via USB. Short press will change the delay after keyboard wakeup (from 80 ms - 880 ms in 200 ms increments). 80ms recommended for RF 1.x, 480ms recommended for RF 2.x.

-  All QMK animations are now included in this firmware

-  FN + M + ] => will trigger MCU deep sleep imediately (only if keyboard is not charging or connected via USB)

-  FN + M + G => enabled GAME MODE. This Mode disables some rgb light functions to ensure maximum scan rate is achived. The G letter will light up green when mode is active and red when game mode is disabled. Long pressing FN + M + G will reset game mode settings to default. <br />
   RGB animations and Side light animations can be modified by user. The new settings will be saved after 30 seconds. But the adjustment is limited.

-  CAPS_WORD is enabled by default. Double press Left Shift key to activate it for 5 seconds. CAPS_LOCK will light up light blue. This function can be disabled (CAPS_WORD) when holding FN + M + CAPS_LOCK. Also by default this is disabled when GAME MODE is active.

-  FM + M + WIN => Will Enable or Disable the WIN key (useful in some games) (also called WIN_LOCK function)

-  FN + M + K => SOCD_TOG This will allow you to choose the SOCD / Rapid Trigger / Key Cancelation mode (custom algorithm): <br />

(0) Disabled: <br />
<pre>Keys   | .. | A. | AD | A.
Report | .. | A. | AD | A. </pre>
 
(1) Cancellation: <br />
<pre>Keys   | .. | A. | AD | A.
Report | .. | A. | .D | .. ----- (D cancels A, no restore on D keyup) </pre>
 
(2) Exclusion: <br />
<pre>Keys   | .. | A. | AD | A. 
Report | .. | A. | .D | A. ----- (D excludes A, restores A on D keyup) </pre>
 
(3) Nullification: <br />
<pre>Keys   | .. | A. | AD | A. 
Report | .. | A. | .. | A. ----- (D nullifies A, neither registered, A restored on D keyup) </pre>


-  Variable matrix scan rate in order to help with battery usage. After 10 second of idle time the Light MCU sleep is enabled and scan rate goes to ~ 700. After 30 seconds of idle time scan rate further decreases to ~ 300. <br />
Matrix scan rate default: ~ 1700 - 1900 <br />
Matrix scan rate keyboard is idle : ~ 300 - 700 <br />
Matrix scan rate in GAME_MODE: ~ 3200 - 3900 <br />

-  Debounce is set by default to 5ms and debounce type is asym_eager_defer_pk 

-  Debounce type can be modified by key DEBOUNCE_T (FN + M + D). <br />
Debounce algorithms that can be selected are: <br />
  asym_eager_defer_pk (this is the default) => GREEN light <br />
  sym_eager_pr => YELLOW light <br />
  sym_defer_pk (actually a variation of it) => RED light <br />

- Debounce time is also selectable: <br />
To select press: <br />
  DEBOUNCE_D (FN + M + F11) to decrease <br />
  DEBOUNCE_I (FN + M + F12) to increase <br />
 <br />
the steps will be: <br />
  1 from 1 - 10 => will light up F1-F10 in green <br />
  2 from 12 - 30 => will light up F1-F10 in yellow <br />
  5 from 35 - 75 => will light up F1-F10 in red <br />
  and 100 ms (max) => will light up in purple <br />
 <br />
Example: <br />
  Debounce 6 => F1-F6 light up in green color <br />
  Debounce 18 => F1-F4 light up in yellow color <br />
  Debounce 65 => F1-F7 light up in red color <br />
  Debounce 100 => F1-F12 in purple color <br />
 <br />
  Setting for debounce (type and value in ms) are saved differently for game mode and normal mode so you will have 2 separate settings <br />
  The settings are also show when transitioning from game mode to normal mode in QMK Console if debug is activated <br />

-  New RGB animation available (including in GAME mode). When selected it will light up: ESC, A , S ,D, W and arrow keys

-  New RGB commands are available but not set to any keys: RGB_RMOD, RGB_HUD, RGB_M_P, RGB_SAI, RGB_SAD, RGB_TOG

-  Light Sleep can be adjusted from 1 minute to 100 minutes using keys SLEEP_D and SLEEP_I (FN + M + '-' and FN + M + '+'). <br />
   Adjustment are different depending on mode: <br />
   Example: by default 2 minutes until light sleep when deep sleep is enabled (sleep mode green), 6 minutes when deep sleep disabled (sleep mode yellow), no adjustment for no sleep (sleep mode red). <br />
   So you can setup up different settings based on sleep mode. The adjustment is similar to the example for debounce explained above. <br />

-  Version will be shown when Debug is enabled like (example): <br />
    Keyboard: nuphy/air75_v2/ansi @ QMK 0.25.10-62-g2a4e8d | BUILD: 2024-07-09-09:31:29 (1e4798ae3e) <br />

- Keyboard had 3 sleep settings selectable via FN + ]. <br />
  1. Side led green. Behaviour: <br />
 <br />
   - After 10 seconds idle MCU goes to light sleep and matrix scan rate goes to ~ 700 <br />
   - After 2 minutes (adjustable), side lights are turned off <br />
   - After 6 minutes (4 minutes after side lights are turned off), RF goes to sleep (if you are connected via 2.4G), MCU goes do deep sleep. If you are connected via BT the RF will go to sleep after 30 minutes but the MCU will go to deep sleep after 6 minutes. <br />
 <br />
  2. Side led yellow. Behaviour: <br />
   - After 10 seconds idle MCU goes to light sleep and matrix scan rate goes to ~ 700 <br />
   - After 6 minutes (adjustable), side lights are turned off <br />
   - After 10 minutes (4 minutes after side lights are turned off), RF goes to sleep (if you are connected via 2.4G), if you are connected via BT the RF will go to sleep after 30 minutes. MCU does not go to deep sleep. <br />
 <br />
  3. Side led red. Behaviour: <br />
   - After 10 seconds idle MCU goes to light sleep and matrix scan rate goes to ~ 700 <br />
   - Side lights do not go to sleep. RF goes to sleep after 30 minutes. MCU does not go to deep sleep <br />
 <br />
  Side indicators will flash blue for 0.5s when board enters MCU deep sleep mode when on BT and will falsh green for 0.5s when on 2.4G, as an indicator. <br />
  Keyboard will not enter sleep regardless of settings if it is charging or if it is connected via USB. <br />
  Keyboard will not enter sleep if GAME MODE is active <br />
 <br />
## Author

[@adi4086](https://github.com/adi4086)

# DP19 Customizations & Fixes

This code is forked from [Nuphy's repo](https://github.com/nuphy-src/qmk_firmware) and based from their `nuphy-air75` branch they currently have an open PR for.

A lot of these customizations and fixes were implemented by @jincao1 & @adi4086 and this is mostly an adopted version of their code combined and tweaked for my liking. Please pay them a visit if you like what you see here

## Customizations

- `Fn + B` when held temporarily displays the current battery percentage on the F and number row.
The F row represents the 10s percentage and number row the ones. Example, 35% will light `F1` -> `F3` and `1` -> `5`.
- Side indicators will flash red for 0.3s when board enters deep sleep.
- Bluetooth connection indicators will be lit blue when establishing connection. This lights the corresponding
BT mode key. Green indicator for RF connection
- RGB will be disabled when brightness is 0 for both side lights and keyboard lights. Default is on for both
- 3ms debounce instead of 2ms (potential stability)

### Sleep Mode (inspired by @adi4086 3 mode sleep)

Sleep mode in this version will turn off all leds after 2 minutes then deep sleep after 6 minutes by turning the MCU to low power mode. This saves significant battery over the light sleep currently implemented in Nuphy's branch.

This does mean the first few key presses after the board is in deep sleep will be lost.

During the light sleep phase RF will stay connected and key press events will be captured.

If you use the 2.4 RF receiver, when it's disconnected from your computer the board will go into immediate deep sleep mode. 
* Since I generally use the USB ports on my monitor when I get up and take my laptop this disconnects and puts the board to sleep.

## Fixes

- Removed unused code and eeprom variables to reduce size
- Make calls to default eeprom init functions in place of relying on a flag to be set in the eeconfig
- Fix keyboard randomly crashing/freezing. (@jincao1)
- Fix keyboard not sleeping properly and draining battery. This version sleeps the processor and uses almost no battery on sleep. (@jincao1)
- Fix LED lights not powering down when not used. This increases battery life around 50-70% when LEDs aren't used. Leds are powered off when brightness is 0. (@jincao1)
- Enhance keyboard reports transmission logic to greatly reduce stuck/lost key strokes. It may still occasionally drop/repeat keys but it's rare. (@jincao1)
- Slightly enhanced sidelight refresh intervals for smoother animations. (@jincao1)
- Reduced unused side LED tables to save a chunk of memory. This may be essential to the RF queue as the board only has 16kb memory available - the queue alone uses over 1.2kb. (@jincao1)

## Keymaps

The standard `via` keymap enables all rgb animations and keyboard features. I use the builds of my `DP19` keymap which has the following customizations. You can build on top of this firmware by copying the folder and making your own customizations to those files. See QMK docs for more

- Default RGB set to `Reactive Simple` and Gradient mode is turned on for all `Reactive` effects. This changes the rgb color in a gradient scale with each keypress. 
  - All but two RGB affects are in this keymap, Reactive simple and solid which I use for testing.
- Side lights are set to SIDE_OFF and brightness 0 to disable them by default
- Disabled Keylock, Mousekey, and N key rollover (NKRO). Since I use Mac which doeesn't support NKRO I've added options in the code to not include extra functions since they won't ever be called. This reduces the firmware size
- Added RF_DFU key to `Fn + M + {` binding to easily get RF module into DFU mode for flashing. Must be in USB mode and hold key unti it appears in RF Connect app.