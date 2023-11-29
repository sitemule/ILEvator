#!/QOpenSys/usr/bin/qsh

## run this:  compile.sh -f JSONPARS0A.JSON-Basic-features.rpgle -l NOXDB -c 277 -o ''

# --- Command runner ----------------------------------
run() {
  cmd="$1"
  #!DEBUG# echo $cmd
  system  -q -kpieb $cmd
  ret_code=$?
  if [ ${ret_code} != 0 ]; then
    printf "Error: [%d] when executing command: '$cmd'" $ret_code
  fi
}


# --- Interpreat the EVENT file   ---------------------
#ROR      0 001 1 000032 000032 025 000032 033 RNF1324 E 20 054 Keywords DFTACTGRP, ACTGRP, or USRPRF are not allowed.
print_event() {
  file_name="./$1"
  obj_lib="$2"
  object_name="$3"

  run "CPYTOSTMF  FROMMBR('/qsys.lib/$obj_lib.lib/EVFEVENT.file/$object_name.mbr') TOSTMF('/tmp/eventfile.txt') STMFOPT(*REPLACE) STMFccsid(1252)"

  while IFS= read -r line
  do
    text_type=${line:0:5}
    if [ ${text_type} =  "ERROR" ];then
      err_id=${line:48:7}
      err_sev=${line:58:2}
      if [ ${err_sev} -ne  "00" ];then
        line_no=${line:37:6}
        col_no=${line:44:3}
        err_text=${line:65:100}
        echo "$file_name:$line_no,$col_no,$err_text"
      fi
    fi
  done < /tmp/eventfile.txt
}


# --- Main line --------------------------

while getopts ":f:o:b:l:c:" opt; do
  case $opt in
    f) file_name="$OPTARG"
    ;;
    o) options="$OPTARG"
    ;;
    b) obj_lib="$OPTARG"
    ;;
    l) libl="$OPTARG"
    ;;
    c) ccsid="$OPTARG"
    ;;
    \?) echo "Invalid option -$OPTARG" >&2
    exit 1
    ;;
  esac

  case $OPTARG in
    -*) echo "Option $opt needs a valid argument"
    exit 1
    ;;
  esac
done

wordlist=$(echo ${file_name} | tr ' ' '+' )
wordlist=$(echo ${wordlist} | tr '.' ' ' )
wordlist=$(echo ${wordlist} | tr '-' ' ' )
words=$(echo ${wordlist} | wc -w)
object_name=$(echo ${wordlist} | cut -d' ' -f1 | tr '[a-z]' '[A-Z]')
extension=$(echo ${wordlist} | cut -d' ' -f"$words")
text=''
midext=''

if [ $words -eq 3 ];then
  text=$(echo ${wordlist} | cut -d' ' -f2)
elif [ $words -eq 4 ];then
  text=$(echo ${wordlist} | cut -d' ' -f2)
  midext=$(echo ${wordlist} | cut -d' ' -f3)
fi

text=$(echo ${text} | tr '+' ' ' )

# For examples using icebreak / noxDb
for l in ${libl}
do 
  liblist -a ${l}
done  
touch error.txt
setccsid 1208 error.txt
setccsid 1208 "${file_name}"
run "CRTSRCPF $obj_lib/srctemp RCDLEN(240) ccsid($ccsid)"
run "CPYFRMSTMF FROMSTMF('$file_name') TOMBR('/QSYS.LIB/$obj_lib.LIB/srctemp.file/srctemp.mbr')  MBROPT(*REPLACE)"

if [ "$midext" = "srvpgm" ]
then
  if [ "$extension" = "sqlrpgle" ]
  then
  	run "CRTSQLRPGI obj($obj_lib/$object_name) OBJTYPE(*SRVPGM) SRCFILE($obj_lib/srctemp) SRCMBR(SRCTEMP) RPGPPOPT(*LVL2) TEXT('$text') COMPILEOPT('$options') DBGVIEW(*NONE) ')"
  else
    run "CRTRPGMOD  MODULE($obj_lib/$object_name) SRCFILE($obj_lib/srctemp) SRCMBR(SRCTEMP) TEXT('$text')  $options"
    run "CRTSRVPGM  SRVPGM($obj_lib/$object_name) DETAIL(*BASIC) EXPORT(*ALL)"
  fi
else
  if [ "$extension" = "sqlrpgle" ]
  then
  	run "CRTSQLRPGI obj($obj_lib/$object_name) OBJTYPE(*PGM) SRCFILE($obj_lib/srctemp) SRCMBR(SRCTEMP) TEXT('$text') RPGPPOPT(*LVL2) COMPILEOPT('$options') DBGVIEW(*NONE) ')"
  else
    run "CRTBNDRPG  PGM($obj_lib/$object_name) SRCFILE($obj_lib/srctemp) SRCMBR(SRCTEMP) TEXT('$text') $options"
  fi
fi
print_event "${file_name}" ${obj_lib} ${object_name}
run "DLTF $obj_lib/srctemp"
exit
