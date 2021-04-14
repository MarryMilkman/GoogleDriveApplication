  Server build and run:
1) Download chilkat-9.5.0 from https://www.chilkatsoft.com + unpack the archive
2) cmake build:
flags:
-Dboost_custom - if need point to folder with boost
-Dchilkat (required) - point to folder with chilkat-9.5.0

cmake ..  -G "Ninja"  -Dboost_custom=D:/plugins/boost_1_74_0 -Dchilkat=D:/downloads/chilkat-9.5.0
cmake --build .
3) Run google_drive_server
google_drive_server --help


  Client build and run:
Just run from QtCreator
