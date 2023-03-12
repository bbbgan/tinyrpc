## *Introduce*
*Tinyrpc*是一个轻量级的RPC框架,该框架采用第三方库[rapidjson](https://github.com/Tencent/rapidjson)实现JSON的序列化和反序列化，利用[Tinynet](https://github.com/bbbgan/tinynet)实现TCP级别的消息传输，根据JSON文件自动生成 stub 文件头文件，用户只需要include头文件即可发起或者接收RPC。

## *Install*

```
$ git clone git@github.com:bbbgan/tinynet.git
$ cd tinynet && ./build.sh
```

## *TODO*
+ benchmark
+ more example


