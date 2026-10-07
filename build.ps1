# Build script for STM32F103C8T6 Washing Machine FSM
$ErrorActionPreference = "Stop"

$toolDir = "C:\ST\STM32CubeIDE_1.7.0\STM32CubeIDE\plugins\com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.9-2020-q2-update.win32_2.0.0.202105311346\tools\bin"
$cc = Join-Path $toolDir "arm-none-eabi-gcc.exe"
$objcopy = Join-Path $toolDir "arm-none-eabi-objcopy.exe"
$size = Join-Path $toolDir "arm-none-eabi-size.exe"

$cflags = @(
    "-mcpu=cortex-m3",
    "-mthumb",
    "-O2",
    "-Wall",
    "-fdata-sections",
    "-ffunction-sections",
    "-ICore/Inc",
    "-c"
)

if (!(Test-Path "build")) {
    New-Item -ItemType Directory -Path "build" | Out-Null
}

$srcs = @(
    "Core/Src/main.c",
    "Core/Src/fsm.c",
    "Core/Src/button.c",
    "Core/Src/led.c",
    "Core/Src/stm32f1xx_hal.c",
    "Core/Src/startup_stm32f103c8tx.c"
)

$objs = @()
foreach ($src in $srcs) {
    $baseName = [System.IO.Path]::GetFileNameWithoutExtension($src)
    $obj = "build/$baseName.o"
    $objs += $obj
    Write-Host "Compiling $src ..."
    & $cc $cflags $src -o $obj
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Compilation failed on $src"
        exit 1
    }
}

Write-Host "Linking build/WashingMachine.elf ..."
$ldflags = @(
    "-mcpu=cortex-m3",
    "-mthumb",
    "-TSTM32F103C8TX_FLASH.ld",
    "-Wl,--gc-sections",
    "--specs=nano.specs",
    "--specs=nosys.specs",
    "-Wl,-Map=build/WashingMachine.map"
)

& $cc $ldflags $objs -o "build/WashingMachine.elf"
if ($LASTEXITCODE -ne 0) {
    Write-Error "Linking failed"
    exit 1
}

Write-Host "Generating build/WashingMachine.hex ..."
& $objcopy -O ihex "build/WashingMachine.elf" "build/WashingMachine.hex"

Write-Host "Generating build/WashingMachine.bin ..."
& $objcopy -O binary "build/WashingMachine.elf" "build/WashingMachine.bin"

Write-Host "`n=== FIRMWARE MEMORY USAGE ==="
& $size "build/WashingMachine.elf"

Write-Host "`n>>> BUILD SUCCESSFUL! <<<"
Write-Host "HEX FILE: $((Get-Item 'build/WashingMachine.hex').FullName)"
