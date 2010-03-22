@echo off
:: Launches Epiar in console mode

IF EXIST Epiar_debug.exe (
	start /W Epiar_debug.exe %*
) ELSE (
	start /W Epiar.exe %*
)
