## *Introduce*

*Tinyrpc*是一个轻量级的RPC框架,该框架采用第三方库[rapidjson](https://github.com/Tencent/rapidjson)实现JSON的序列化和反序列化，依靠[Tinynet](https://github.com/bbbgan/tinynet)传输消息。

## *Install*

```
$ git clone git@github.com:bbbgan/tinynet.git
$ cd tinynet && ./build.sh
```

## **How to Use**

定义Json文件Arithmetic.json： *name*是方法的名称，*params*是方法的参数，*return*是返回值。

```json
{
  "name": "Arithmetic",
  "rpc": [
    {
      "name": "Add",
      "params": {"lhs": 0.0, "rhs": 0.0},
      "returns": 0.0
    },
    {
      "name": "Sub",
      "params": {"lhs": 0.0, "rhs": 0.0},
      "returns": 0.0
    },
    {
      "name": "Mul",
      "params": {"lhs": 0.0, "rhs": 0.0},
      "returns": 0.0
    },
  ]
}
```

生成*Stub*头文件

```
$ tinyrpcstub -c -s -i Arithmetic.json
```

*include*头文件，重写基类的方法，即可发起RPC服务，具体的可以参考*example*

## *TODO*
+ DOCTESTS
+ more example
