
# 配置 Flash的大小

![](arduino-esp8266-22.png)


# 使用内存的API

```
#include <FS.h>


FSInfo info

SPIFFS.begin()     //启动ESP8266闪存文件系统
SPIFFS.info(info)  //取闪存文件系统信息
info.totalBytes    //可用空间
info.usedBytes     //已用空间
info.maxPathLength //最长文件名
info.maxOpenFiles  //最多可打开文件
info.blockSize     //存储块大小
info.pageSize      //存储页大小

SPIFFS.format()    //格式化SPIFFS,清除内容
SPIFFS.exists(文件名)         //文件是否存在
SPIFFS.remove(文件名)         //移除文件
File F=SPIFFS.open(文件,"w")  //"r","w"=重写内容,"a"=追加内容
```