# patcher
[![CMake (Windows ARM32/64)](https://github.com/horoni/patcher/actions/workflows/cmake_win-arm.yml/badge.svg)](https://github.com/horoni/patcher/actions/workflows/cmake_win-arm.yml)
[![CMake (Linux, macOS, Windows)](https://github.com/horoni/patcher/actions/workflows/cmake.yml/badge.svg)](https://github.com/horoni/patcher/actions/workflows/cmake.yml)

example config:
```json
{
    "path": "Minecraft.Windows.exe",
    "switches": [
        {
            "title": "Enable/Disable the EPPL Mod",
            "patches": [
                "0x87A886 >> 0C >> FF",
                "0x87ABA8 >> 0C >> FF"
            ]
        },
        {
            "title": "Patch Nether height limit (256)",
            "patches": [
                "0x8B91E6 >> 80 >> FF"
            ]
        }
    ]
}
```
