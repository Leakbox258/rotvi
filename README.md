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
```
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
```
# source files

`rotvi`: lasted version compiled by author<br>
`api.hpp`: wrappers of interfaces provided by ncurses<br>
`color.hpp`: provide highlight color compile-time register, avoid run-time searching or checking<br>
`cursor.hpp`: store cursor's position and config infos, buffering content of the opened file<br>
`keywords.hpp`: keywords of CXX file, regular expressions on certain pattern<br>
`match.hpp`: recieve tokens, mapping them to their highlight colors<br>
`syntax.hpp`: very simple tokenlize<br>
`tools.hpp`: generic utils<br>
`apiInit.cpp`: ncurses configs and compile-time registerd colors will init here before actually open the target file <br>
`cmdProcessor.cpp`: execute the commands recieved in `COMMAND_LINE` mode<br>
`cursor.cpp`: write buffer back to the file, clamp the cursor, fresh the screen<br>
`file.cpp`: load or create a file<br>
`keyPressHandler.cpp`: recieve keyborad actions once a time, and execute it<br>
`main.cpp`: entry of this software<br>
`syntax.cpp`: the implementation of the template class `ColorFmt::operator()`<br>

# introductions
- ### three modes --- NORMAL, INSERT, COMMANDLINE
    - NORMAL: read mode, the mode entry into the ncurses virtual screen
        - h / left: ←
        - l / right: →
        - k / up: ↑
        - j / down: ↓
        - 0 / home: move cursor to the beginning of this line
        - $ / end: move cursor to the end of this line
        - i: chmod into INSERT mode, and do inserting in front of the char under the cursor  
        - a: just like 'i', but insert after the char
        - o: open a new line under current line, and chmod into INSERT
        - O: open a new line above current line, and chmod into INSERT
        - x: delete character under cursor
        - d: delete the line
        - ':': chmod into COMMAND_LINE
        - G: goto the end of this file
    - INSERT: write mode, move cursor with ← → ↑ ↓ 
    - COMMAND_LINE: execute commands, just simple cmds now
        - q: quit, when never modify the file
        - q!: quit, and give up new modify
        - w: buffer write in file
        - wq: write in then quit
        - up: roll the page upon to the next alignment of your terminal size
        - down: roll the page down to the next alignment of your terminal size
    - back to NORMAL mode: press Esc (escape), but after make breakpoint in gdb, I've found that it will take a while (about 0.8s, much longer than other keys) for our program to recieve the signal from the keyborad, don't know why.   
    
- ### namespace `api`: in this namespace, there are many wrappers of interfaces provided by ncurses
    - `Key_...`: numeric act as identifiers of certain keys on the board
    - `api_init()`: init ncurses config and enable interfaces
    - `enum color`: the colors defined by ncurses, or will be inited at beginning of the program
    - `struct ColorAttr`: just declare a pure virtual function `operator()`
        - `struct ColorAttrOn`: derived from `struct ColorAttr`, wrapper of `attron` provided by ncurses, requiring two `enum color` template args
        - `struct ColorAttrOff`: just like `struct ColorAttrOn`
    - `namespace api::detail`: the use of color pairs is totally determined by source code, so it'll be not neccessary to search or check color pairs in run-time. This sub namespace acts as a registar, collecting the color infos and init them in a very early period time of the process.  
        - `getColorPacks()`: get a static `std::set<u_int16_t>`, which store all color pairs(packed into a `u_int16_t`) explicit used in the source file.
        - `getColors()`: get a static `std::set<color>`, which store the colors not provided by ncurses
        - `getColorPairIdx()`: get a static `std::map<u_in16_t, u_int8_t>`, map packed color to an unique id, because `init_pair` provided by ncurses have a fairly strict rule on the range of the color pair id. 
        - `RegisterHelper(color, color)`: pack the two colors, and distribute an id
        - `struct CTRegister`: requiring two `enum color` template args, and have a inline static bool member `inUse`.
            - To initialze `inUse`, `RegisterHelper(color, color)` will be called, at the same time collecting infos.
            - `struct CTRegister` will be declared as static ones in corresponding `struct ColorAttr`'s `operator()`, and then we explicit use the member `inUse`, forcing it to be initialized.
            - `inUse` is an inline static bool value, which means it's initialization has to be done in `__cxx_global_var_init`, which is a sub period of `__libc_start_main`, far before `main()` and `api_init()`
            - after doing all this, the color infos are all collected in these static `std::set<T>`. Then in `api_init()`, the program will do some initializations required by ncurses. Other use of `struct ColorAttr` will be very easy, because all the instances of this kind of struct are static, and encoded in .bss (maybe)
- ### token highlight: currently the highlight only support CXX files.
    - `struct token`: contain the position of the token, length and extra attribute
    - `class ColorFmt`: a template class
        - when recognize the file as certain known source file, the `class cursor` will hold a static const reference of `class ColorFmt` with specific template arg (currently only CXX files)
        - `operator()`: apply `tokenlize` to the buffer, and input tokens to a template function `colorMatch`, to get a pair of static functors `struct ColorAttr`, and then use them
    - `inline std::vector<token> tokenlize(T)`: recieve a set of memory-contiguous chars, use very simple linear scan to split into a vector of token.
    - `colorMatchImpl(const char *, int, token::attr)`: use regular expressions and the attribute to judge the type of current token, return a pair of `struct ColorAttr`

# todo
- enhanced syntax analysis support: current linear scan algorithm is designed from CXX, don't work well on other languages. Maybe need to add more complex compiler-frontend syntax analysis algorithm
- support wide character: only visible chars can be recognize correctlly, wide chars display in a strong way, and will disturb the position calculating of cursor.
- roll screen horizontally: though the editor can roll screen vertically, it can't roll horizontally, because the tab and wide chars are very difficult to handle, especially when they are truncated by the screens.

~~以下为gpt翻译~~

# 源文件

`rotvi`: 作者编译的最新版本  
`api.hpp`: 对 ncurses 提供的接口进行封装  
`color.hpp`: 在编译期注册高亮颜色，避免运行时搜索或检查  
`cursor.hpp`: 存储光标位置和配置信息，缓冲已打开文件的内容  
`keywords.hpp`: CXX 文件的关键字，基于某些模式的正则表达式  
`match.hpp`: 接收token，将它们映射到对应的高亮颜色  
`syntax.hpp`: 非常简单的token化实现  
`tools.hpp`: 通用工具函数  
`apiInit.cpp`: 在实际打开目标文件前初始化 ncurses 配置和编译期注册的颜色  
`cmdProcessor.cpp`: 在 `COMMAND_LINE` 模式下执行接收到的命令  
`cursor.cpp`: 将缓冲区写回文件，限制光标位置，刷新屏幕  
`file.cpp`: 加载或创建文件  
`keyPressHandler.cpp`: 每次接收一次键盘操作并执行相应操作  
`main.cpp`: 本软件的入口  
`syntax.cpp`: 模板类 `ColorFmt::operator()` 的实现  

# 介绍说明
- ### 三种模式 --- NORMAL、INSERT、COMMANDLINE
    - NORMAL：读取模式，进入 ncurses 虚拟屏幕时的默认模式  
        - h / left: ←  
        - l / right: →  
        - k / up: ↑  
        - j / down: ↓  
        - 0 / home: 将光标移动到当前行首  
        - $ / end: 将光标移动到当前行尾  
        - i: 进入 INSERT 模式，在光标所在字符前插入  
        - a: 类似于 'i'，但在光标所在字符后插入  
        - o: 在当前行下方新开一行，并进入 INSERT 模式  
        - O: 在当前行上方新开一行，并进入 INSERT 模式  
        - x: 删除光标所在字符  
        - d: 删除整行  
        - ':': 进入 COMMAND_LINE 模式  
        - G: 跳转到文件末尾  
    - INSERT：写入模式，可通过方向键移动光标  
    - COMMAND_LINE：执行命令，目前支持简单命令  
        - q: 退出，文件未被修改时使用  
        - q!: 强制退出，放弃所有修改  
        - w: 将缓冲区内容写入文件  
        - wq: 写入后退出  
        - up: 上滚一页（按终端对齐）  
        - down: 下滚一页（按终端对齐）  
    - 返回 NORMAL 模式：按下 Esc（Escape），但使用 gdb 断点调试时发现从键盘接收该按键信号比其他键慢很多（约 0.8 秒），原因未知。  

- ### 命名空间 `api`：该命名空间封装了大量 ncurses 接口
    - `Key_...`: 数字形式的键值标识符  
    - `api_init()`: 初始化 ncurses 配置并启用接口  
    - `enum color`: ncurses 定义的颜色，或程序启动时初始化的颜色  
    - `struct ColorAttr`: 仅声明一个纯虚函数 `operator()`  
        - `struct ColorAttrOn`: 继承自 `ColorAttr`，是对 ncurses 的 `attron` 的封装，需要两个 `enum color` 模板参数  
        - `struct ColorAttrOff`: 功能类似于 `ColorAttrOn`  
    - `namespace api::detail`: 颜色对的使用在源码中完全确定，因此运行时不需搜索或检查，此子命名空间是一个注册器，在程序早期阶段收集颜色信息并初始化。  
        - `getColorPacks()`: 获取一个静态 `std::set<u_int16_t>`，存储源码中显式使用的颜色对（打包为 `u_int16_t`）  
        - `getColors()`: 获取一个静态 `std::set<color>`，存储 ncurses 未提供的颜色  
        - `getColorPairIdx()`: 获取一个静态 `std::map<u_in16_t, u_int8_t>`，将打包的颜色对映射到唯一 ID，因为 ncurses 提供的`init_pair` 对颜色对 ID 范围有严格限制  
        - `RegisterHelper(color, color)`: 打包两个颜色，并分配一个 ID  
        - `struct CTRegister`: 需要两个 `enum color` 模板参数，包含一个 inline static bool 成员 `inUse`  
            - 初始化 `inUse` 时将调用 `RegisterHelper(color, color)`，并收集信息  
            - `CTRegister` 将在相应的 `ColorAttr` 的 `operator()` 中以 static 方式声明，然后显式使用成员 `inUse`，强制其初始化  
            - `inUse` 是一个 inline static bool 值，其初始化需在 `__cxx_global_var_init` 中完成，该阶段属于 `__libc_start_main` 的子阶段，早于 `main()` 和 `api_init()`  
            - 经过上述过程，颜色信息将全部收集到这些静态 `std::set<T>` 中，然后在 `api_init()` 中进行 ncurses 所需的初始化。之后对 `ColorAttr` 的使用将变得非常简单，因为这些结构体的实例是静态的，编码在 .bss 中(大概)  

- ### token高亮：目前仅支持 CXX 文件的高亮  
    - `struct token`: 包含token的位置、长度和额外属性  
    - `class ColorFmt`: 一个模板类  
        - 当识别为已知源文件类型时，`class cursor` 将持有一个 `class ColorFmt` 的特定模板参数的静态常量引用（当前仅支持 CXX 文件）  
        - `operator()`: 对缓冲区内容调用 `tokenlize`，并将token输入到模板函数 `colorMatch`，从而获取一对静态仿函数 `struct ColorAttr`，并应用它们  
    - `inline std::vector<token> tokenlize(T)`: 接收一段内存连续的字符，使用非常简单的线性扫描方式拆分为token向量  
    - `colorMatchImpl(const char *, int, token::attr)`: 使用正则表达式和token属性判断当前token类型，返回一对 `struct ColorAttr`  

# todo

- 增强语法分析支持：目前的线性扫描算法是为 CXX 设计的，对其他语言支持不佳。可能需要引入更复杂的编译器前端语法分析算法  
- 支持宽字符：目前只能正确识别可见字符，宽字符的显示存在问题，并会干扰光标位置的计算  
- 实现横向滚屏：尽管编辑器已经支持纵向滚屏，但尚不支持横向滚屏，这是因为 tab 和宽字符很难处理，尤其在被屏幕裁剪时更为复杂  