# rotvi editor ( or !^ editor )
A vi like terminal-based text editor on Linux

warning: inorder to compile, your `ncurses6-config --version` have to newer than 6.0
         and you `cmake --version` have to be more recent than 3.10

Make a quick start: 
```bash
$ mkdir build && cd build 
$ cmake ..
$ make rotvi
$ ./rotvi yourfile.txt
```

# overview

├── 📁 bin/                             
│   └── 📄 rotvi                        
├── 📁 include/                     
│   ├── 📄 api.hpp                     
│   ├── 📄 color.hpp                  
│   ├── 📄 cursor.hpp                  
│   ├── 📁 syntax/
│   │   ├── 📁 CXX/
│   │   │   ├── 📄 keywords.hpp         
│   │   │   └── 📄 match.hpp           
│   │   └── 📄 syntax.hpp              
│   └── 📄 tools.hpp                    
├── 📁 lib/
│   ├── 📄 apiInit.cpp                 
│   ├── 📄 cmdProcessor.cpp             
│   ├── 📄 cursor.cpp                   
│   ├── 📄 file.cpp                    
│   ├── 📄 keyPressHandler.cpp          
│   ├── 📄 main.cpp                     
│   └── 📄 syntax.cpp                                     

# source files

`rotvi`: lasted version compiled by author
`api.hpp`: wrappers of interfaces provided by ncurses
`color.hpp`: provide highlight color compile-time register, avoid run-time searching or checking
`cursor.hpp`: store cursor's position and config infos, buffering content of the opened file
`keywords.hpp`: keywords of CXX file, regular expressions on certain pattern
`match.hpp`: recieve tokens, mapping them to their highlight colors
`syntax.hpp`: very simple tokenlize
`tools.hpp`: generic utils
`apiInit.cpp`: ncurses configs and compile-time registerd colors will init here before actually open the target file 
`cmdProcessor.cpp`: execute the commands recieved in `COMMAND_LINE` mode
`cursor.cpp`: write buffer back to the file, clamp the cursor, fresh the screen
`file.cpp`: load or create a file
`keyPressHandler.cpp`: recieve keyborad actions once a time, and execute it
`main.cpp`: entry of this software
`syntax.cpp`: the implementation of the template class `ColorFmt::operator()`
