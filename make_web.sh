
cd ~/emsdk
source ./emsdk_env.sh
cd ~/Documents/code/Gravity_Sim
emcc *.c -c -Os -Wall ./web/libraylib.a -I. -I/usr/include -I/usr/include/x86_64-linux-gnu -DPLATFORM_WEB
cp gravity.* web
emcc -o gravity.html *.o -Os -Wall ./web/libraylib.a -I. -L. -I/usr/include -I/usr/include/x86_64-linux-gnu -s USE_GLFW=3 --shell-file ./web/minshell.html -DPLATFORM_WEB
cp gravity.* web
rm *.o
rm gravity.*
cd web
emrun gravity.html