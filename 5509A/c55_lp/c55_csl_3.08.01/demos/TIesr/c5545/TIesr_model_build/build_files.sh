#!/bin/sh 

Dist/LinuxDebugGnu/bin/testtiesrflex \
      "start( WakeGram ).
	WakeGram ---> ( [_Fill] Phrase [_Fill] ) | _Fill.
	Phrase ---> t i voice trigger." \
      Data/GramDir \
      Data/filler_model \
      English \
      2 0 1 0 0 0 0


