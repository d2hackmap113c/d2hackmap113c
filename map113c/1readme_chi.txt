Download URL: https://github.com/d2hackmap113c/d2hackmap113c/
使用说明
  如果下载到d:\d2hackmap下,则使用下面的命令行启动
  连接服务器启动参数
    D:\game\diablo2\D2Loader.exe -locale chi -lq -pdir D:\d2hackmap\map113c -direct -skiptobnet -w
  单机或TCP启动参数
    D:\game\diablo2\D2Loader.exe -locale CHI -lq -pdir D:\d2hackmap\map113c -direct -w
	D2Loader会扫描加载D:\d2hackmap\map113c下的所有dll,这里只有一个loadhackmap.dll,游戏开始循环后loadhackmap.dll会复制加载D:\d2hackmap\map113c\runtime\d2hackmap.dll

编译说明
  本程序使用Visual Studio 2008编译,系统环境Windows XP(SP3),在网上可找到VS2008 90天试用版,链接指到microsoft.com的比较安全。只需要安装VC，打开src\d2hackmap.sln按F7即可编译。不需要退出游戏，编译完成后在游戏中按Ctrl+R即可加载新的d2hackmap.dll
  所有源文件均使用gvim9.0编辑，TAB设置为2
    set tabstop=2
    set shiftwidth=2
  src\loadhackmap.sln编译生成的loadhackmap.dll是用来加载d2hackmap.dll的，如果修改了这个项目则需要重新启动游戏。

配置说明
  默认加载config.txt,也可以通过命令行参数-d2hackmapcfg=指定配置文件。在config.txt中可以加载别的配置文件。
  config_def.txt     大部分的配置都在这个文件中
  itemconf_def.txt   物品配置都在这个文件中，依然是三个物品模式，但是为了适应从非资料片到资料片的流水线操作，进入资料片会用第一个模式，进入非资料片会用第二个模式，用退格键可以切换。
  config_my.txt      我自己的配置文件
  itemconf_left.txt  我在老战网的物品配置
  itemconf_right.txt 我在新战网的物品配置

游戏按键说明
    本d2hackmap使用左手控制常用按键，将map113c\default.key复制到游戏目录下覆盖原来的default.key，并将save目录下的所有.key文件删除即可使用下面的游戏键盘设置
	I 人物画面
	U 任务记录
	9 技能树
	V 开关小地图
	技能1-8: Q,E,A,S,D,Z,X,C
	技能9-16: Y,H,TAB,鼠标侧面前按键,鼠标滚轮按下
d2hackmap常用按键说明,所有按键均可在map113c/config.txt中修改，每次启动游戏都会生成map113c\runtime\keymap.log，里面列举了所有设置的按键
	Ctrl+R 重新加载配置文件,如果d2hackmap.dll已修改则重新加载dll
	Alt+ENTER 切换全屏窗口模式
	Control+L 锁定鼠标在当前窗口(仅出城后有效)
	5	一键回城,如果在传送站上则通过传送站回城
	6 退出游戏
	7 看属性下一页
	8 在原特效，透视，灯光三个模式中切换。默认是原特效，打开透视灯光就没什么可玩的了。
	= 下一场游戏
	\ 镶嵌保护
	" 血保开关
	Alt+C 人物画面
	Alt+Q 任务记录
	Alt+T 技能树
	Alt+F 开当前区域地图，默认的配置只自动开重要的区域块，自动全开就没什么可玩的了。
	按住R,T,Ctrl+F,G: 屏幕偏移，将人物移到一个角上，可以看到双倍远的距离
	方向键: 移动小地图，如果没显示小地图则移动屏幕，再按一下R,T,G可以复位
	Alt+S 一直显示地上的物品
	Alt+G 显示所有地上的物品
	J 城外开关
	K 看别的玩家装备
	Shift+F: 多开时让其余人物跟随,数量由config.txt中的MultiClientGroupSize1决定
	Shift+G: 多开时让其余人物跟随,数量由config.txt中的MultiClientGroupSize2决定
	Shift+T: 多开时让其余人物停止跟随
	Shift+E: 多开时让其余人物进入离自己最近的门
	按住Alt键或鼠标侧面前按键: 多开时将鼠标左右键点击操作发送给其余窗口执行
	鼠标滚轮,切换导航目标
	H 切换窗口在前台时是否自动使用技能
	Ctrl+Alt+K 切换窗口在后台时是否自动使用技能
	F键: 按住时自动使用技能,如果右键是传送的话则自动传送到目标地点,50FCR亚马逊轻松跑图。如果打开cube的话自动合并背包中的宝石和7号符文

