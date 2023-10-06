
## File 类的读取与写入到文件。

```
   File F=SD.open(文件[,FILE_READ|FILE_WRITE])
 * F.available()      //返回可读取字节数
 * F.close()          //关闭文件，并确保写入其中的所有数据都已物理保存到SD卡中。
 * F.flush()          //确保写入文件的所有字节都已物理保存到SD卡。关闭文件后，此操作会自动完成
 * F.peek()           //byte或-1.从文件中读取一个字节，而无需前进到下一个字节
 * F.position()       //获取文件中的当前位置
 * F.print(数据[,格式])//将数据打印到文件中，该文件必须已打开才能进行写入,（char，byte，int，long或string）,BIN（二进制）（基数2），DEC（十进制）（基数10），OCT（八进制）（基数8），HEX（十六进制）
 * F.println(数据[,格式]) //将数字打印为数字序列，每个数字为一个ASCII字符（例如，数字123作为三个字符“ 1”，“ 2”，“ 3”发送）。
 * F.seek(新位置)      //true|false 在文件中寻找新位置，该位置必须介于0和文件的大小（含）之间
 * F.size()           //unsigned long 获取文件的大小
 * F.read()           //byte (or character), or -1 从文件中读取一个字节
 * F.write(数据[,长度])//将数据写入文件。
 * F.isDirectory()          //boolean 目录（或文件夹）是特殊类型的文件，此功能报告当前文件是否为目录
 * F.openNextFile(句柄)      //char 报告目录中的下一个文件或文件夹
 * F.rewindDirectory(句柄)   //将带您回到目录中的第一个文件，与openNextFile（）结合使用
```



## 文件函数

```
    F.size()                     //<size_t>获取文件的大小
    F.available()                //<int>返回可读取字节数
    F.seek(新位置[,模式]);         //<bool>0=文件首,1=现位置,2=文件尾
    F.position()                 //<size_t>获取文件中的当前位置

    F.peek()                     //<int>拷贝1字节,指针不变
    F.read()                     //<int>读取1字节,-1=失败
    F.read(uint8_t* 数组,长度)    //<size_t>读取数据
    F.readBytes(char *数组,长度)  //<size_t>读取字节集(uint8_t*)

    F.write(数据[,长度])          //<size_t>写入数据,返回位置
    F.flush()                   //确保写入文件的所有字节都已物理保存到SD卡。关闭文件后，此操作会自动完成



    F.bool() const;       //<operator>返回对象
    F.name()              //<char*>获取对象名称
    F.getLastWrite();     //<time_t>获取最后写入时间

    F.isDirectory(void)   //<bool>是否为目录
    F.openNextFile(模式)  //<File>打开下个文件[FILE_READ]
    F.rewindDirectory(void);  //将带您回到目录中的第一个文件，与openNextFile（）结合使用
    F.close();

```

## String 文本对象  t.String(数值字符与串,[格式])

```
String S = "你好文本" ;                             //使用常量String
String S =  String('a');                           //将常量char转换为String
String S =  String("This is a string");            //将常量文本转换为String对象
String S =  String(stringTwo+"with more");         //连接两个文本
String S =  String(13);                            //使用常量整数
String S =  String(analogRead(0),DEC);             //转为10进制整数文本
String S =  String(45,HEX);                        //转为16进制整数文本
String S =  String(255,BIN);                       //转为 2进制整数文本
String S =  String(millis(),DEC);                  //转为10进制长整数文本
char[] 字符串数组,通常,字符串以空字符(ASCII代码0)结尾,数组应该多1字节
  char Str1[9];                                              //声明一个char数组，而不像在Str1中那样初始化它
  char Str2[8] = {'a', 'r', 'd', 'u', 'i', 'n', 'o'};        //声明一个字符数组（带有一个额外的字符），编译器将添加所需的空字符，如Str2中所示
  char Str3[8] = {'a', 'r', 'd', 'u', 'i', 'n', 'o', '\0'};  //明确添加空字符Str3
  char Str4[ ] = "arduino";                                  //用引号引起来的字符串常量进行初始化；编译器将调整数组大小以适合字符串常量和终止的空字符Str4
  char Str5[8] = "arduino";                                  //使用显式大小和字符串常量Str5初始化数组
  char Str6[9] = "arduino";                                  //初始化数组，为更大的字符串Str6留出额外的空间,空终止

char myString [] ="这是第一行"
                  "这是第二行";
char * myStrings [] = {"字符串1","字符串2","字符串3","字符串4"};  //*为数组指针String S =  String(5.698,3);                       //转为3位精度小数文本
String S =  String(char[]);                        //字符串到文本null结尾.

```

## 对 String 的操作

```

    t.charAt(位置)            <char>取字符
    t.compareTo(子串)         <int>字符串排序比较,-值子串在后,0=相同,+值子串在前
    t.concat(子串)            <String>字符串组合
    t.c_str()                <int>取字符串指针,null终止.不要用指针修改字符串,字符串变动时指针会失效.

    t.equals(子串)            <bool>比较字符串,区分大小写
    t.equalsIgnoreCase(子串)  <bool>比较字符串,不区分大小写
    t.startsWith(字符串)       <bool>比较字符串前缀
    t.endsWith(字符串)         <bool>比较字符串后缀

    t.getBytes(byte[],长)     转为字节数组保存
    t.toCharArray(char[],长)  转为字符数组保存
    t.indexOf(字符或串,位置)     <long>查找字符串,返回字符数位置,非内存长度位置,失败-1
    t.lastIndexOf(字符或串,位置) <long>倒找字符串,返回字符数位置,非内存长度位置,失败-1
    t.length()                 <long>字符串字符数,非内存占用长度
    t.remove(起始,[长度])      删除子串
    t.replace([子串],新串)     <String>替换字符串
    t.reserve(内存长度)        申请内存
    t.setCharAt(位置,char)    修改字符

    t.substring(起始,[截止])   <String>取子字符串
    t.toInt()                <long> 转为整数.失败返回零
    t.toFloat()              <float>转为小数.浮点数的精度只有6-7个十进制数字
    t.toLowerCase()          转为小写
    t.toUpperCase()          转为大写
    t.trim()                 删首尾空格与换行

```

## 对 Char[] 数组的操作

```
char[] 字符串数组,通常,字符串以空字符(ASCII代码0)结尾,数组应该多1字节
  char Str1[9];                                              //声明一个char数组，而不像在Str1中那样初始化它
  char Str2[8] = {'a', 'r', 'd', 'u', 'i', 'n', 'o'};        //声明一个字符数组（带有一个额外的字符），编译器将添加所需的空字符，如Str2中所示
  char Str3[8] = {'a', 'r', 'd', 'u', 'i', 'n', 'o', '\0'};  //明确添加空字符Str3
  char Str4[ ] = "arduino";                                  //用引号引起来的字符串常量进行初始化；编译器将调整数组大小以适合字符串常量和终止的空字符Str4
  char Str5[8] = "arduino";                                  //使用显式大小和字符串常量Str5初始化数组
  char Str6[9] = "arduino";                                  //初始化数组，为更大的字符串Str6留出额外的空间,空终止

char myString [] ="这是第一行"
                  "这是第二行";
char * myStrings [] = {"字符串1","字符串2","字符串3","字符串4"};  //*为数组指针
```
