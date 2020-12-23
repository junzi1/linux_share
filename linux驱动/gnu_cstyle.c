#include <stdio.h>

//1. 0长数组
struct lista {
	char *a;
	char b[0];
};

//2.switch
void func_a()
{
	int n;
	switch(n) {
	case 1:
		printf("good 1\n");
		break;
	case 2:
		printf("good 1\n");
		break;
	default:
		break;
	}
}

//3.语句表达式
#define mymin(type,a,b) \
({type __a = a,__b = b;__a < __b?a:b;})

//4.typeof
int tmp;
typeof(tmp) tmp1 = 9;

//5.可变参数宏
#define log_print(info,...) printf(info,##__VA_ARGS__);

//6.标号元素
char tmpchar;
struct lista hah = {
	.a = &tmpchar,
};

//7.当前函数名
//__func__  c99支持
//__FILE__,__FUNCTION__,__LINE__

/*
(1)、内核编译（make）之后会生成两个文件，一个Image，一个zImage，其中Image为内核映像文件，而zImage为内核的一种映像压缩文件，Image大约为4M，而zImage不到2M。

那么uImage又是什么的？它是uboot专用的映像文件，它是在zImage之前加上一个长度为64字节的“头”，说明这个内核的版本、加载位置、生成时间、大小等信息；其0x40之后与
zImage没区别。

*/
int main()
{
	log_print("%d\n",mymin(int,1,6));
	return 0;
}
