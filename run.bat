chdir project/release

set url=http://localhost:8080/messages
set iters=10000

curl_race.exe %url% %iters%

pause
