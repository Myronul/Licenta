Codul sursa dezvoltat se afla in: Core/Inc si Core/Src (modularizat in bibliotecile de functii)

Universitatea “Politehnica” din Bucureşti
Facultatea de Electronică, Telecomunicaţii şi Tehnologia Informaţiei
Program de studiu RST	Anexa 1
 
TEMA PROIECTULUI DE DIPLOMĂ
a studentului MIRON N.F. Andrei-Auraș, 444D
 
1. Titlul temei: Biblioteca de functii in limbaj C pentru transformarea imaginilor si efecte audio cu aplicatii pe microcontrolere

2. Descrierea temei:
Se va realiza o bibliotecă de funcții în limbaj de programare C pentru transformarea imaginilor (rotație, scalare, translatare) și realizarea de efecte audio. Funcțiile implementate vor fi testate în timp real prin realizarea practică a unui sistem compus din două subsisteme fiecare cu microcontroler ARM propriu: un subsistem pentru procesare grafică și audio și un subsistem de comandă a transformărilor grafice și a tipurilor de efecte audio. Sistemul realizat și biblioteca de funcții asociată se pot utiliza în aplicații multimedia cu microcontroler. Se vor utiliza cunoștințe matematice (geometrie, operații cu matrice), de prelucrare de semnal (generarea unui semnal eșantionat, filtrarea semnalelor) și de programare în limbaj de nivel înalt. Realizarea programelor se va face utilizând programul MATLAB (pentru validarea algoritmilor) și medii de dezvoltare a programelor pentru microcontrolere (STM32Cube IDE, STM32 CubeMx ) pentru generarea codului.

3. Contribuția originală:
1. Scrierea următoarelor funcții în limbaj C:
- generarea unor imagini geometrice simple
- rotația unei imagini în jurul unei axe
- translatarea unei imagini
- scalarea unei imagini
- generarea unor semnale audio sintetizate
- generarea unor efecte audio (ecou, reverberație, vibrato)

2. Proiectarea următoarelor module software:
- modul pentru conectarea și comanda ecranului LCD
- planificatorul de procese pentru procesele de prelucrare grafică, prelucrare audio, sincronizare și comandă
- protocolul de comunicație între subsistemele de procesare și de comandă


3. Evaluarea performanțelor sistemului
- timp de calcul
- rezoluție grafică
- calitatea semnalelor audio generate


4. Resurse și bibliografie:
1. Richard Szeliski, Computer Vision: Algorithms and Applications 2nd Edition, Springer 2022
2. Using the ADSP-2100 Family Volume 1 (Rev. 1.0); Chapter 9; https://www.analog.com/en/lp/001/adsp-manuals.html
3. https://users.cs.cf.ac.uk/Dave.Marshall/CM0268/PDF/10_CM0268_Audio_FX.pdf
4. https://www.researchgate.net/publication/236629475_Implementing_Professional_Audio_Effects_with_DSPs
5. STM32 Microcontrollers, https://www.st.com/en/microcontrollers-microprocessors/stm32-32-bit-arm-cortex-mcus.html
6. a-Si TFT LCD Single Chip Driver 320(RGB) x 480 Resolution, 16.7M-color With Internal GRAM Specification
7. Note de curs (Sorin Zoican): Arhitectura Microprocesoarelor 2. Microcontrolere
8. Note de curs (Sorin Zoican): Sisteme de operare





# 🎓 C Function Library for Image Transformation and Audio Effects on Microcontrollers

> **Bachelor's Thesis** — University POLITEHNICA of Bucharest, Faculty of Electronics, Telecommunications and Information Technology  
> **Specialization:** Telecommunications Networks and Software (RST), Series 444D  
> **Author:** Miron Andrei-Auraș  
> **Supervisor:** Prof. dr. ing. Sorin Zoican  
> **Year:** 2025

---

## 📋 Table of Contents

- [About the Project](#-about-the-project)
- [System Architecture](#-system-architecture)
- [Hardware](#-hardware)
- [Software Structure](#-software-structure)
- [Developed Libraries](#-developed-libraries)
  - [lcd.h / lcd.c — ILI9488 LCD Driver](#lcdh--lcdc--ili9488-lcd-driver)
  - [graphics.h / graphics.c — Graphics Engine](#graphicsh--graphicsc--graphics-engine)
  - [audio.h / audio.c — Audio Processing](#audioh--audioc--audio-processing)
  - [sdsys.h / sdsys.c — SD File System](#sdsysh--sdsysc--sd-file-system)
  - [kernel.h / kernel.c — Microkernel OS](#kernelh--kernelc--microkernel-os)
  - [controller.h / controller.c — Control Module](#controllerh--controllerc--control-module)
  - [app.h / app.c — Multimedia FSM Application](#apph--appc--multimedia-fsm-application)
  - [font.h / font.c — Text Rendering](#fonth--fontc--text-rendering)
- [Implemented Algorithms](#-implemented-algorithms)
  - [Image Scaling](#image-scaling--nearest-neighbor)
  - [Image Rotation](#image-rotation--matrix-rotation)
  - [Image Translation](#image-translation)
  - [Echo Effect (FIR)](#echo-effect-fir)
  - [Vibrato Effect (FIR)](#vibrato-effect-fir)
  - [Reverb Effect (IIR)](#reverb-effect-iir)
- [Microkernel OS — Details](#-microkernel-os--details)
- [Measured Performance](#-measured-performance)
- [SD Card Structure](#-sd-card-structure)
- [Custom File Formats](#-custom-file-formats)
- [MATLAB Scripts](#-matlab-scripts)
- [Repository Structure](#-repository-structure)
- [Setup & Configuration](#-setup--configuration)
- [Dependencies & Tools](#-dependencies--tools)

---

## 🎯 About the Project

This project is a **complete C function library** for image and audio processing, running on STM32F407VET6 microcontrollers. The core goal is to demonstrate that an embedded system can execute real-time signal processing algorithms without dedicated hardware acceleration (no GPU or external DSP).

The system is built as an **interactive multimedia device**, capable of:
- Displaying and transforming images (scaling, rotation, translation) on a 480×320 color LCD
- Playing back and processing audio with real-time effects (echo, vibrato, reverb)
- Running multiple tasks concurrently via a **Round-Robin microkernel OS** implemented from scratch in C and Assembly
- Receiving commands from a dedicated control module over SPI

---

## 🏗️ System Architecture

The system uses a **two-subsystem master-slave architecture**, each running on its own STM32F407VET6:

```
┌──────────────────────────────────┐        SPI         ┌──────────────────────────────────┐
│         COMMAND MODULE           │ ─────────────────► │       PROCESSING MODULE          │
│           (Master)               │   Transmit-Only    │           (Slave)                │
│                                  │                    │                                  │
│  • 5 buttons (PB5–PB9)           │                    │  • LCD ILI9488 (480×320)         │
│  • TIM2 @1ms (debounce)          │                    │  • DAC + DMA (audio output)      │
│  • SPI Transmit Only Master      │                    │  • SD Card (SDIO + FATFS)        │
│  • STM32F407VET6                 │                    │  • SPI Receive Only Slave        │
│                                  │                    │  • Microkernel OS                │
└──────────────────────────────────┘                    └──────────────────────────────────┘
```

The **command module** captures button presses, applies hardware debounce via a timer, and transmits serialized command codes over SPI. The **processing module** receives commands and executes graphics rendering, audio playback, and SD card file management in parallel, orchestrated by the custom OS kernel.

---

## 🔧 Hardware

### Microcontroller — STM32F407VET6

| Parameter         | Value                          |
|-------------------|-------------------------------|
| Architecture      | ARM Cortex-M4 with FPU        |
| Max Clock         | 168 MHz                       |
| Flash             | 512 KB                        |
| RAM (SRAM)        | 192 KB                        |
| DMA Channels      | 16                            |
| Peripherals       | SPI, I2C, USART, DAC, SDIO, TIM |

### External Peripherals

| Peripheral    | Model / Specs                                         |
|---------------|-------------------------------------------------------|
| LCD Display   | ILI9488, 480×320 px, RGB 18-bit, SPI interface        |
| Audio Output  | Internal DAC + DMA + TIM2 trigger, amplifier + speaker |
| Storage       | SD Card via SDIO, FATFS middleware                    |
| Debug         | ST-Link, SWD interface                                |
| Input         | 5 buttons on PB5–PB9 (TIM2 debounce)                 |

---

## 📁 Software Structure

```
Licenta/
├── Core/
│   ├── Inc/                    # Header files
│   │   ├── lcd.h               # ILI9488 LCD driver
│   │   ├── graphics.h          # Graphics engine + image transforms
│   │   ├── audio.h             # Audio playback + effects
│   │   ├── sdsys.h             # SD file system interface
│   │   ├── kernel.h            # Round-Robin microkernel OS
│   │   ├── controller.h        # Button control module
│   │   ├── app.h               # Multimedia FSM application
│   │   └── font.h              # 11×18 bitmap font
│   └── Src/
│       ├── lcd.c
│       ├── graphics.c
│       ├── audio.c
│       ├── sdsys.c
│       ├── kernel.c
│       ├── controller.c
│       ├── app.c
│       └── font.c
├── Drivers/                    # STM32 HAL + CMSIS
├── Middlewares/                # FatFS
└── ...
```

---

## 📚 Developed Libraries

### `lcd.h` / `lcd.c` — ILI9488 LCD Driver

Full driver for the ILI9488 color display (480×320, RGB 18-bit) over SPI.

| Function | Description |
|----------|-------------|
| `ILI9488_driver_init()` | Full initialization: gamma, power control, interface config |
| `LCD_send_command(uint8_t cmd)` | Send command byte over SPI |
| `LCD_send_data(uint8_t data)` | Send single data byte |
| `LCD_send_data_multi(uint8_t *data, unsigned int size)` | Bulk transfer via **DMA** (full frame) |
| `set_adress_window(x0,y0,x1,y1,char x)` | Set addressing window for read/write |
| `read_pixel_frame(...)` | Read pixel from display memory |
| `read_pixel_format()` | Query active pixel format |

> **Performance note:** A full 480×320 frame in binary format with DMA takes **1825ms** vs. 3372ms without DMA in text format — roughly a **2× speedup**.

---

### `graphics.h` / `graphics.c` — Graphics Engine

The core display subsystem. Defines the `ENTITY` structure and provides all drawing primitives and image transformation operations.

#### The `ENTITY` Structure

```c
typedef struct {
    union {
        char path[64];         // File path on SD card
        uint8_t *localBuffer;  // Local RAM buffer
        uint16_t color;        // Primitive color (solid rectangle)
    };
    int16_t x0, y0;           // Pivot (top-left corner)
    int16_t x1, y1;           // Offset / dimensions
    uint8_t id;               // Entity type identifier
} ENTITY;
```

Using a `union` allows handling all three entity modes (SD card, local RAM, primitive) with minimal memory overhead.

| Function | Description |
|----------|-------------|
| `init_cursor()` | Initialize graphics cursor |
| `draw_pixel(x, y, color)` | Draw single pixel |
| `draw_rectangle(x,y,w,h,color)` | DMA-accelerated rectangle |
| `draw_rectangle_slow(...)` | Pixel-by-pixel rectangle |
| `fill_screen1() / fill_screen2()` | Full screen color fill |
| `print_character(c, x, y, color)` | Render bitmap character |
| `print_string(str, x, y, color)` | Render string |
| `draw_entity(ENTITY*)` | Smart dispatch: SD / RAM / primitive |
| `init_entity_sd() / free_entity_sd()` | SD card entity lifecycle |
| `translation_entity(ENTITY*, dx, dy)` | Optimized translation with delta redraw |
| `translation_test(ENTITY*, step, delay)` | Automatic screen-border traversal |
| `scaling_entity(ENTITY*, float factor)` | Nearest Neighbor scaling, frame-by-frame |
| `rotate_entity(ENTITY*, int theta)` | 90°/180°/270° rotation |
| `erase_entity(ENTITY)` | Overwrite entity area with background color |
| `convert_color_16_to_18(uint16_t, uint8_t*)` | R5G6B5 → RGB 18-bit conversion |

**Translation optimization:** When displacement on an axis is smaller than the entity's size **and** the redraw area is ≤ 1024 pixels, only the **delta rectangle** (the newly exposed or covered strip) is redrawn instead of the entire entity, significantly reducing SPI traffic.

---

### `audio.h` / `audio.c` — Audio Processing

Real-time audio playback and processing, based on **double-buffering** with DMA and the internal DAC.

#### Double-Buffer Architecture

```
  SD Card ──► [read 2048 samples] ──► Buffer A ──► DAC ──► Speaker
                                      Buffer B
                                         ▲
                         (process in the free half-window)
```

- Circular buffer of **2048 samples**
- DMA streams continuously to the DAC
- **Half-transfer callback** triggers processing in the idle half
- Supported sample rates: **8 kHz** and **44.1 kHz** (configurable via TIM2)

| Function | Description |
|----------|-------------|
| `play_audio_file(char *path)` | Raw playback from SD card |
| `play_audio_file_echo(char *path, int8_t delay, float alpha)` | Playback with echo effect (FIR) |
| `play_audio_file_vibrato(char *path)` | Playback with vibrato effect (FIR) |
| `play_audio_file_reverb(char *path)` | Playback with reverb effect (IIR) |

---

### `sdsys.h` / `sdsys.c` — SD File System

Complete SD card I/O (SDIO + FATFS), optimized for frame-by-frame processing within the MCU's 192KB RAM constraint.

| Function | Description |
|----------|-------------|
| `init_cardSD()` | Mount FATFS |
| `read_image_file(ENTITY*, uint16_t *index, bool *flagImgDone)` | Read binary image frame (3072 bytes = 32×32×3) |
| `read_image_file_scaling(...)` | Specialized read for scaling; computes optimal line count |
| `write_image_file(...)` | Write scaled image frames back to SD card |
| `read_audio_file(char*, uint32_t *buffer, bool *flagAudioDone)` | Read audio frame (5120 bytes = 1024 samples × 5 chars), ASCII→int conversion |
| `assign_filePath()` | Assign file path |
| `return_file_name_current_path()` | Query current path |
| `assign_file_path_entity()` | Assign path to entity |

Static variables inside file-reading functions maintain state between calls, enabling seamless multi-frame streaming.

---

### `kernel.h` / `kernel.c` — Microkernel OS

**The most complex module in the project.** A fully custom Round-Robin OS kernel written from scratch in C and inline Assembly — no external RTOS used.

#### Task Control Block (TCB)

```c
typedef struct TCB_t {
    int32_t        *pstack;            // Pointer to current stack top
    struct TCB_t   *pnext;             // Next TCB (circular linked list)
    void           (*pfunction)(void); // Task entry function
    int32_t        stack[2048];        // Dedicated stack: 2048 × 4 = 8 KB
    uint8_t        pID;                // Unique process ID
    STATE          state;              // awake / sleep
    unsigned int   n;                  // Countdown for kernel_delay()
} TCB;
```

| Parameter           | Value        |
|---------------------|-------------|
| Max tasks           | 10 (MAXPR)   |
| Stack per task      | 8 KB         |
| Scheduler type      | Round-Robin  |
| Preemption interval | Every 1ms (TIM4 ISR → PendSV) |

| Function | Description |
|----------|-------------|
| `kernel_add_process(void (*fn)(void))` | Static TCB init, insert into circular list |
| `kernel_start()` | **Naked** Assembly: sets MSP to first task stack, emulates interrupt return to launch first task |
| `kernel_scheduler()` | Round-Robin: returns next TCB with state=awake |
| `kernel_delay(unsigned int ms)` | Sets state=sleep, triggers PendSV |
| `kernel_count_sleep()` | Called from TIM4 ISR every 1ms; decrements `n` for sleeping tasks |

#### Context Switch — `PendSV_Handler()`

```c
// Pseudocode — fully implemented in naked inline Assembly
void PendSV_Handler(void) {
    CPSID I;                         // Disable interrupts
    PUSH {R4-R11};                   // Save regs (R0-R3 auto-saved by HW)
    currentTCB->pstack = SP;         // Save SP into current TCB
    currentTCB = kernel_scheduler(); // Select next task
    SP = currentTCB->pstack;         // Restore new task SP
    POP {R4-R11};                    // Restore registers
    CPSIE I;                         // Re-enable interrupts
    BX LR;                           // Return (EXEC_RETURN)
}
```

Hardware automatically stacks R0–R3, R12, LR, PC, xPSR on exception entry.

#### Synchronization & Critical Sections

- Global **`mutex`** variable protects shared resources (SD card, LCD)
- TIM4 ISR checks `startOS && !mutex && !flagAllSleep` before setting PENDSVSET
- PendSV runs at **minimum priority (0xFF)**, ensuring critical DMA/SPI transfers complete before any context switch

**Experimental verification:** Two tasks toggling a GPIO produced a **0.49 kHz square wave** at 50% duty cycle, confirmed on oscilloscope.

---

### `controller.h` / `controller.c` — Control Module

Defines the SPI command protocol for the command module.

| Button  | Hex Code |
|---------|----------|
| Right   | `0x11`   |
| Left    | `0x33`   |
| Up      | `0x55`   |
| Down    | `0x77`   |
| Start   | `0x99`   |
| Select  | `0xBB`   |

`dataController` and `currentDx` are declared `extern` and accessible system-wide.

---

### `app.h` / `app.c` — Multimedia FSM Application

The top-level application, implemented as a **Finite State Machine (FSM)** with a navigable main menu.

```
                     ┌─────────────────────────────────────┐
                     │             MAIN MENU               │
                     │      [navigate with Up / Down]      │
                     └───┬─────────────┬──────────┬────────┘
                         │             │          │
                    ┌────▼────┐  ┌─────▼────┐  ┌─▼──────────┐
                    │   OS    │  │ Graphics │  │   Audio    │
                    │  Demo   │  │          │  │            │
                    └────┬────┘  └──┬──┬──┬─┘  └──┬──┬──┬───┘
                         │         │  │  │        │  │  │
                    4 concurrent  Scale Tran Rot Player Echo Vibrato
                      tasks             slat ion         Reverb
```

**OS Demo** (`demo_os_1()`) launches **4 concurrent tasks** on the kernel:
- `Task0` — `translation_test()` bouncing entity 1 across screen borders
- `Task1` — Continuous audio playback from SD card
- `Task2` — `translation_test()` bouncing entity 2 across screen borders
- `Task3` — Entity translation controlled interactively via buttons

---

### `font.h` / `font.c` — Text Rendering

**11×18 pixel bitmap font**, full printable ASCII set. Each character is stored as a bit matrix and rendered pixel-by-pixel onto the display via `print_character()` and `print_string()`.

---

## 🧮 Implemented Algorithms

### Image Scaling — Nearest Neighbor

Each destination pixel maps to the nearest source pixel:

$$I_{orig} = \left\lfloor \frac{I_{new}}{F} \right\rfloor, \quad J_{orig} = \left\lfloor \frac{J_{new}}{F} \right\rfloor$$

where `F` is the scaling factor.

**Frame-based optimization for limited RAM:**  
Since the MCU has only 192KB of RAM, a full image cannot be held in memory. The algorithm processes images in 32×32 pixel frames. The optimal number of source lines `x` needed to produce one output frame is:

$$dim_{M1} \cdot F^2 \cdot x \leq 1024 \text{ pixels}$$

Scaled results are written back to the SD card under `graphic/scalare/`.

---

### Image Rotation — Matrix Rotation

Rotating point `(x1, y1)` around the image center by angle `β`:

$$\begin{bmatrix} x_2 \\ y_2 \end{bmatrix} = \begin{bmatrix} \cos\beta & -\sin\beta \\ \sin\beta & \cos\beta \end{bmatrix} \cdot \begin{bmatrix} x_1 \\ y_1 \end{bmatrix}$$

**Implementation steps:**
1. Subtract image center (normalize to pivot)
2. Apply rotation matrix
3. Add offset back
4. Supported angles: **90°, 180°, 270°**
5. Pixel-by-pixel processing directly from SD card (no full image loaded into RAM)

---

### Image Translation

Efficient update of the entity pivot `(x0, y0)`.

**Delta redraw optimization:** If displacement `dx` or `dy` is smaller than the entity's size **and** the redraw area is ≤ 1024 pixels, only the **delta rectangle** (the newly exposed or covered strip) is redrawn — not the full entity. This dramatically reduces SPI transfer count.

---

### Echo Effect (FIR)

$$y[n] = x[n] + \alpha \cdot x[n-i]$$

where:
- `α` ∈ [0, 1] — echo amplitude coefficient
- `i` — delay index: $i = \frac{delay\_ms}{T_s \cdot 1000}$

Both `delay` and `alpha` are configurable at call time.

---

### Vibrato Effect (FIR)

Variable sinusoidal delay:

$$y[n] = x[n] + x[n - int[n]]$$

where `int[n]` varies sinusoidally. Implemented using a 3-value array `{10, 12, 14}` ms that cycles every **8 frames** (~5 Hz), simulating the frequency modulation characteristic of vibrato.

---

### Reverb Effect (IIR)

Multi-tap IIR filter:

$$y[n] = \beta \cdot x[n] + \sum_{k} \alpha_k \cdot y[n - k]$$

| Parameter | Values |
|-----------|--------|
| Delays `k[]` | {220, 440, 660, 880} samples |
| Gains `αk[]` | {0.3, 0.25, 0.2, 0.15} |
| Stability condition | $\sum \alpha_k < 1$ (✓ sum = 0.9) |
| Complexity | O(N²) |

Reverb is the most computationally intensive effect and benefits most from the double-buffering architecture.

---

## 🖥️ Microkernel OS — Details

### How the OS Launches (`kernel_start()`)

```
1. TCBs are statically allocated for each added task
2. kernel_start() (naked function):
   a. Sets MSP (Main Stack Pointer) to TCB[0]'s stack
   b. Loads EXEC_RETURN (0xFFFFFFF9) into R14
   c. BX LR → CPU believes it is returning from an interrupt
   d. Hardware auto-pops {R0-R3, R12, LR, PC, xPSR}
   e. PC ← first task function address
   f. First task begins execution!
3. TIM4 sets PENDSVSET every 1ms
4. PendSV_Handler performs Round-Robin context switch
```

### Task Lifecycle

```
    [CREATED]
        │
        ▼
    [AWAKE] ◄──────────────────────────────────────────────┐
        │                                                   │
        │ kernel_delay(n)                                   │
        ▼                                                   │
    [SLEEP] ──► (TIM4 ISR decrements n every 1ms) ─────────┘
                        (when n == 0 → state = AWAKE)
```

---

## 📊 Measured Performance

### LCD Frame Transfer (480×320 pixels)

| Method | Time |
|--------|------|
| Text format, no DMA | 3372 ms |
| Text format, with DMA | 3274 ms |
| **Binary format, with DMA** | **1825 ms** |

→ **~2× speedup from binary over text format**  
→ **12.16 ms per 32×32 frame** in binary + DMA mode

### Maximum Resolution at 30 FPS

Condition: `k² × 12.16ms < 33.33ms`  
→ `k ≈ 1.5` → **max resolution ~48×48 pixels** at 30 FPS

### Image Transformations

| Operation | Time | Throughput |
|-----------|------|------------|
| Scaling 2×2 (factor 50) | 183 ms | 0.0183 ms/pixel |
| Rotation 100×100 pixels | 156 ms | 0.0156 ms/pixel |

### OS Task Switching

| Measurement | Value |
|-------------|-------|
| Switch frequency (2 GPIO tasks) | 0.49 kHz |
| Duty cycle | 50% |

---

## 💾 SD Card Structure

```
SD_CARD/
├── graphic/
│   ├── image1.bin        # Binary image files (custom format)
│   ├── image2.bin
│   ├── ...
│   └── scalare/          # Output of scaling_entity()
│       ├── scaled_1.bin
│       └── ...
└── Audio/
    ├── audio1.txt        # ASCII audio files (4 digits + newline per sample)
    ├── audio2.txt
    └── ...
```

---

## 📦 Custom File Formats

### Binary Image Format (`.bin`)

```
Offset   Size   Description
------   ----   -----------
0x0000   2B     Image width  (little-endian, uint16_t)
0x0002   2B     Image height (little-endian, uint16_t)
0x0004   3072B  Frame 0: 32×32 pixels × 3 bytes/pixel (RGB 18-bit)
0x0C04   3072B  Frame 1
...      ...    ...
```

### ASCII Audio Format (`.txt`)

```
1234\n
5678\n
0987\n
...
```

5 bytes per sample (4 digits + `\n`). ASCII-to-int conversion is handled inline by `string_to_int()`.

---

## 🔬 MATLAB Scripts

The project includes MATLAB scripts for:

1. **Image file generator** — Converts standard images into the custom binary format (4-byte header + RGB18 frames)
2. **Audio file generator** — Exports audio samples to the ASCII `.txt` format (4-digit values + newline)
3. **Audio filter validation** — Simulates and verifies echo, vibrato, and reverb filters on `.wav` files before MCU deployment

These scripts are provided in the thesis appendices and serve as pre-processing tools for all multimedia content.

---

## 🗂️ Repository Structure

```
Licenta/
├── Core/
│   ├── Inc/                # All library headers
│   └── Src/                # C implementations
├── Drivers/
│   ├── CMSIS/              # ARM CMSIS headers
│   └── STM32F4xx_HAL_Driver/
├── Middlewares/
│   └── Third_Party/
│       └── FatFs/          # FATFS SD card middleware
├── .ioc                    # STM32CubeMX configuration file
└── ...                     # STM32CubeIDE project files
```

---

## ⚙️ Setup & Configuration

### 1. Clone the repository

```bash
git clone https://github.com/Myronul/Licenta.git
cd Licenta
```

### 2. Open in STM32CubeIDE

1. Open **STM32CubeIDE**
2. `File → Import → Existing Projects into Workspace`
3. Select the cloned directory
4. Build: `Project → Build Project` (or `Ctrl+B`)

### 3. Reconfigure pinout (optional)

The `.ioc` file can be opened in **STM32CubeMX** to modify peripheral configurations.

### 4. Flash to the microcontroller

Connect the board via **ST-Link** (SWD) and run `Run → Debug` or `Run → Run` from STM32CubeIDE.

### 5. Prepare the SD card

- Format the SD card as **FAT32**
- Create the directory structure described in [SD Card Structure](#-sd-card-structure)
- Generate image and audio files using the MATLAB scripts
- Copy generated files into the appropriate directories

---

## 🛠️ Dependencies & Tools

| Tool / Library | Version / Notes |
|----------------|-----------------|
| STM32CubeIDE | ≥ 1.12 |
| STM32CubeMX | ≥ 6.x |
| GCC `arm-none-eabi-gcc` | Bundled with STM32CubeIDE |
| STM32F4 HAL | Included in project (`Drivers/`) |
| FatFS | Included in project (`Middlewares/`) |
| MATLAB | R2020b+ (for generation scripts) |
| ST-Link driver | ST-Link V2/V3 |

---

## 📖 References

- **ILI9488 Datasheet** — LCD driver, SPI commands, gamma configuration
- **STM32F407 Reference Manual** (RM0090) — DMA, DAC, TIM, SPI, SDIO
- **ARM Cortex-M4 Technical Reference Manual** — PendSV, context switch, register set
- **FatFS Documentation** — [elm-chan.org/fsw/ff](http://elm-chan.org/fsw/ff/00index_e.html)
- Miron Andrei-Auraș, *"C Function Library for Image Transformation and Audio Effects on Microcontrollers"*, Bachelor's Thesis, UPB ETTI, 2025

---

<div align="center">

**University POLITEHNICA of Bucharest**  
Faculty of Electronics, Telecommunications and Information Technology  
Telecommunications Networks and Software

*Bachelor's Thesis 2025 — Miron Andrei-Auraș*

</div>

