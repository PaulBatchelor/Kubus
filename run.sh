./kubus config.ini &
#chuck sine.ck &
sleep 1
PID=$(jack_lsp | grep -i mplayer | grep out_0 | cut -d ' ' -f 2 | cut -d ':' -f 1)
SRC="MPlayer $PID:out_0"
#SRC="ChucK:outport 0" 
jack_connect "$SRC" "RtApiJack:inport 0" 


