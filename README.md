# patcher
[![CMake (Windows ARM32/64)](https://github.com/horoni/patcher/actions/workflows/cmake_woa.yml/badge.svg)](https://github.com/horoni/patcher/actions/workflows/cmake_woa.yml)
[![CMake (Linux)](https://github.com/horoni/patcher/actions/workflows/cmake_linux.yml/badge.svg)](https://github.com/horoni/patcher/actions/workflows/cmake_linux.yml)
[![CMake (Windows)](https://github.com/horoni/patcher/actions/workflows/cmake_windows.yml/badge.svg)](https://github.com/horoni/patcher/actions/workflows/cmake_windows.yml)

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
## Credits
[Nuklear](https://github.com/Immediate-Mode-UI/Nuklear) <br>
[json](https://github.com/nlohmann/json) by [nlohmann](https://github.com/nlohmann)

## License
```
Copyright (C) 2025  horoni

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
```
