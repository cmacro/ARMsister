SET PATH=e:\Keil_v5\ARM\ARMCC\Bin;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;E:\Program Files\aria2;C:\Program Files (x86)\NVIDIA Corporation\PhysX\Common;C:\Program Files\Git\cmd;E:\Go\bin;C:\Program Files\TortoiseGit\bin;C:\Program Files\dotnet\;C:\Program Files\Microsoft SQL Server\130\Tools\Binn\;E:\Python36;C:\Program Files\Microsoft VS Code\bin;E:\Python36\Scripts;e:\Program Files (x86)\Embarcadero\RAD Studio\10.0\bin
SET CPU_TYPE=STM32F103C8
SET CPU_VENDOR=STMicroelectronics
SET UV2_TARGET=showtimeXR
SET CPU_CLOCK=0x007A1200
"e:\Keil_v5\ARM\ARMCC\Bin\ArmAsm" --Via "..\out\startup_stm32f103xb._ia"
"e:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\out\main.__i"
"e:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\out\armsis.__i"
"e:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\out\rtc.__i"
"e:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\out\led.__i"
"e:\Keil_v5\ARM\ARMCC\Bin\ArmLink" --Via "..\out\showtimeXR.lnp"
"e:\Keil_v5\ARM\ARMCC\Bin\fromelf.exe" "..\out\showtimeXR.axf" --i32combined --output "..\out\showtimeXR.hex"
