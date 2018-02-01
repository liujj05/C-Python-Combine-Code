# Python 与 C 的混合编程

## 参考

1. python的[官方文档](https://docs.python.org/2/extending/extending.html#id5)
2. CSDN上面的[简明教程](http://www.voidcn.com/article/p-vrcojger-uo.html)

## C代码在Python中的使用

简而言之，最终需要的效果是，我们自己开发一个包，叫作spam

'''
import spam
status = spam.system("dir")
'''

上例是官方教程中的例子。这个spam包中，只有一个函数叫system，作用是把输入的字符串作为CMD命令执行，将返回字符串写入status