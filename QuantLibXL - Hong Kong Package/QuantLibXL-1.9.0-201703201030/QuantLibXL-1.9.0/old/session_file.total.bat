
@REM Where the Launcher.xla is to be found
SET LAUNCHER_DIR=.

@REM This environment variable tells the launcher which session file to use.
@REM If this variable is not set then Launcher.xla looks for a file called session_file.xml in the same directory.
@REM If that file is not found then the launcher defaults to hard coded parameters.
SET XL_LAUNCHER_PATH=%LAUNCHER_DIR%\session_file.EUR.xml

START "EXCEL" "EXCEL.EXE" /e /r "%LAUNCHER_DIR%\Launcher.xla"

SET XL_LAUNCHER_PATH=%LAUNCHER_DIR%\session_file.EUR_OLD.xml

START "EXCEL" "EXCEL.EXE" /e /r "%LAUNCHER_DIR%\Launcher.xla"

SET XL_LAUNCHER_PATH=%LAUNCHER_DIR%\session_file.AUD.xml

START "EXCEL" "EXCEL.EXE" /e /r "%LAUNCHER_DIR%\Launcher.xla"

SET XL_LAUNCHER_PATH=%LAUNCHER_DIR%\session_file.GBP.xml

START "EXCEL" "EXCEL.EXE" /e /r "%LAUNCHER_DIR%\Launcher.xla"

SET XL_LAUNCHER_PATH=%LAUNCHER_DIR%\session_file.HKD.xml

START "EXCEL" "EXCEL.EXE" /e /r "%LAUNCHER_DIR%\Launcher.xla"

SET XL_LAUNCHER_PATH=%LAUNCHER_DIR%\session_file.JPY.xml

START "EXCEL" "EXCEL.EXE" /e /r "%LAUNCHER_DIR%\Launcher.xla"

SET XL_LAUNCHER_PATH=%LAUNCHER_DIR%\session_file.SEK.xml

START "EXCEL" "EXCEL.EXE" /e /r "%LAUNCHER_DIR%\Launcher.xla"

SET XL_LAUNCHER_PATH=%LAUNCHER_DIR%\session_file.USD.xml

START "EXCEL" "EXCEL.EXE" /e /r "%LAUNCHER_DIR%\Launcher.xla"