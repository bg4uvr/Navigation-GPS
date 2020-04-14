### AVR M8 + 240128液晶屏全中文显示航海用GPS解码装置（实验性）

---

**文章首发地址**

   https://www.hellocq.net/forum/read.php?tid=160030



---

**工作状态照片**

<img src="doc/1.jpg" alt="工作状态" title="工作状态" style="zoom:80%;" />



---

**功能简介**

1. 全中文显示
2. 显示内容包括：航向、航速、经纬度、里程表(掉电记忆)、日期、时间、高度、定位状态、收星数
3. 自动适应4800和9600bps的gps模块。（默认为4800的，9600时可以自动切换，不过启动时要多等上2秒钟。注意：更换gps后需要重新开机启动）。

代码已经过初步测试，如发现存在bug，欢迎指出，我会尽量加以改进。m8内部空间基本用完了，再加其他功能基本非常困难了。



---

**ATmega8(L)的熔丝配置说明 (熟悉avr的朋友可以跳过这段)：**

新的芯片，熔丝是出厂默认的状态，在此状态，读入熔丝配置，在以下关于熔丝的说明中：

- **"1" 指不编程，在大多数的编程器中，是把勾去掉。**
- **"0" 指编程，在大多数的编程器中，是打上勾。**



1. **必须更改的熔丝**

   **sksel3:0=1111 sut1:0=11**

   说明：这2组熔丝是设置mcu的时钟源和上电启动时间的。这不是唯一可以工作的的组合，不过为了方便，请选用此组合。
   
2. **建议更改的熔丝**
   

   **eesave=0**

   说明：这个熔丝是设置在擦除芯片时，是否擦除内部eeprom。由于eeprom内存储了里程数据，为了再次编程时，保留原来的里程数据，可以编程此熔丝。
   
   **bodlevel=0 boden=0**
   
   说明：这2个熔丝设置掉电检测的电压值为4伏，和使能掉电复位功能。因为程序中对eeprom区进行写入和读出操作，为了避免掉电时出现意外改写，请编程此熔丝。



3. **千万不能更改的熔丝**

   **rstdisbl和ispen**
   
   如果改了这2个熔丝，你将无法再使用isp编程器来给芯片编程，必须使用并行编程器才能恢复。
   
   **wdton**
   
   如果改了这个熔丝，芯片将反复进入重新启动，无法正常工作。因为这个熔丝会使能看门狗，但本程序中未使用它。不过，这个熔丝即使设置错了，仍然可以用isp编程器来改正。



只更改以上的熔丝，并写入芯片，烧入代码后，应该就可以工作了。



---

**代码更新历史记录：**

* 2007.9.19 
  * 修正速度、航向输出屏蔽无效0位时的对位错误bug
  * 修正未定位时仍然增加里程的bug

* 2007.9.19 
  
  * 完善高度数据处理部分的代码，现可对5位、6位格式的高度数据，带小数点与不带小数点的高度数据，带负号与不带负号的高度数据，都能正确对齐显示。
  
* 2007.9.20 
  
  * 修正上次更新造成的里程表不计数bug
  
* 2007.9.20
  
  * 修正里程表计数值比实际值放大10倍的错误。
  
* 2007.9.22
  
  * 实在无聊，加了一只led指示gps的定位状态。没连接灭，没定位闪，定位亮。
  
* 2007.9.26
  
  * 更改原lcd的上电自复位为mcu软件复位，以降低原上电复位对电源上升速率的要求，彻底解决有时上电时lcd显示乱的问题。
  
* 2007.9.28 
  * 新增编译变量，支持公英制界面编译。可以分别编译出公英制显示界面的代码。
  * 改led的状态为：未连接亮，未定位闪，定位灭

* 2007.9.29
  
  * 根据朋友要求，更改日期和时间信息从gprmc语句取得，以适合绝大多数的gps模块。造成的问题是，即使gps模块接有备份电池，启动后未定位前，日期显示仍是不正确的。不过定位后自动更新，并在重新上电启动前，一直维持正确。
  
* 2007.10.03
  
  * 修正上次更新造成的日期显示错误
  
* 2009.10.19

  公开源代码，原因是希望我的代码能对大家学习单片机或gps解码，能有所帮助。

  * 本源代码任何人可免费使用，而不必通知我，但不得用于任何商业用途。

  * 请遵守网络道德，禁止加入个人信息并声称为自己的作品，转帖请注明本帖原始地址：

     http://www.hellocq.net/forum/showthread.php?t=160030

  * 本代码不保证不存在缺陷。本人精力有限，不保证可以及时回复关于本程序的相关问题。

* 2020.2.9
  
  * 非常抱歉，本代码中发现存在一个错误，会造成星期显示为错误结果，因当年未仔细测试加之水平有限，此错误直到今天才被意外发现。
  
* 2020.4.14

  * 把AVR Studio完整的工程文件，以及部分说明文件，保存到 [Github](https://github.com/bg4uvr/Navigation-GPS) 和 [Gitee](https://gitee.com/bg4uvr/Navigation-GPS) 

    

---

**字库生成软件的设置**

<img src="doc/2.JPG" alt="字库生成软件的设置" title="字库生成软件的设置"  />



---



多年前学习AVR的过程中的"作品"，水平有限错误不足难免，希望能对正在玩这种东西的朋友有所帮助~

