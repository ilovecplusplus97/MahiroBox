# MahiroBox
一款使用C++编写的仿制[MahiroPatter](https://github.com/daiisuukee/mahiro_patter)的绪山真寻键盘监控软件<br><br>
![软件界面](https://c2.im5i.com/2023/01/08/Yg1y8.png)<br><br>
# 自定义风格
MahiroBox v1.0.2 添加了自定义风格功能，但是软件中没有自定义风格的界面（因为我懒），所以只能通过在mahiro_style文件夹下添加文件夹来自定义风格
## 创建风格
进入mahiro_style文件夹<br>
![mahiro文件夹](https://c2.im5i.com/2023/01/10/YKaeL.png)<br>
会发现有个default文件夹，那就是风格模板，复制一份，改为你的风格的名字，就成功了。
## 修改风格
进入你的风格文件夹，你会发现有这样一些文件
![风格文件夹里的文件](https://c2.im5i.com/2023/01/10/YKp6t.png)<br>
这些文件是资源文件，它们有这样的作用
- *.png：显示在屏幕上的图片
- info.xml: 配置文件，引领程序加载你的图片

如果你要更改显示在屏幕上的真寻的样子，那么你就只用修改图片即可
## 修改info.xml文件
如果你要给真寻添加一个动作，只是在文件夹中添加8.png是不行的，这时就需要修改info.xml文件了
打开info.xml文件，有如下内容
```xml
<?xml version="1.0" encoding="utf-8"?>

<root>
	<left>
		<image>left.png</image>
		<image>null</image>
	</left>
	<right>
		<image>right.png</image>
		<image>right_pressed.png</image>
	</right>
	<mahiro>
		<image>1.png</image>
		<image>2.png</image>
		<image>3.png</image>
		<image>4.png</image>
		<image>5.png</image>
		<image>6.png</image>
		<image>7.png</image>
	</mahiro>
</root>
```
可以看到，在root节点下有left, right和mahiro三个节点，这三个节点保存这样的内容
- left: 左手的图片路径
- right: 右手的图片路径
- mahiro: 真寻的图片路径

要修改路径就要修改节点下的image节点，image节点包含的是图片的路径（注：如果不显示就可以将image节点的内容改为null）
## 使用风格
修改完成后，重启MahiroBox，双击打开设置，在选项一栏中找到风格，修改为你的风格即可<br><br>
# 尾页
![点个star吧](https://c2.im5i.com/2023/01/10/YK7bq.png)