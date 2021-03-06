# 评分规则（非最终版本）

## 分数会基于以下几个部分

- 展示 & Review (40%)
- 最终提交 (60%)

其中，各部分的分数组成如下

### 展示 & Review

- 能显示非单调的图像 (10%)
- 说明自己对架构的理解以及回答问题(CPU,PPU,NES均可) (60%)
- 对本次作业的反馈和建议 (30%)

### 最终提交

Deadline: 周六中午？

最终提交需要在各仓库提交时的Commit Message里唯一注明，否则为DDL前的最近一次提交。

除非单独注明，以下测试中，用Linux平台下的Vivado编译源码，在Basys3开发板和普通VGA显示器上进行测试

- 最终提交中的PPU内模块(ri,bg,spr)没有逐字或明显地抄袭标程  (10%，否则-90%)
- 工程能完成编译并写入FPGA  (30%)
- PPU接口部分(ri)正确性   (20%)
- 随机选择5个ROM测试运行以及手柄输入能显示非单调图像 (40%)

## 细节说明

### 非单调图像

非单调图像的分数定义：

- 非初始颜色色块    (40%)
- 随机静态图案  (30%)
- 随机可变图案  (10%)
- ROM内可能出现的图案   (10%)
- 基本完整正确的ROM对应位置的画面   (5%)

对于评测运行时产生的图像，取能取得的最高分数。

**注：在最终提交中上传合法的运行截图或照片有助于评分**

### 接口部分

接口部分正确性评测方法：
保持接口部分(ppu_ri)不变，将PPU的背景和精灵渲染模块(ppu_bg, ppu_spr)替换为标程的模块，没有实现的接口使用默认值进行测试。在原工程通过编译的前提下保证修改后能通过编译。

- 能正常发送NMI(vertical blanking) (20%)
- 背景部分能显示非单调图像 (60%)
- 精灵部分能显示非单调图像 (20%)

**注：在最终提交中上传并注明一个可用于接口正确性评测的仓库分支有助于评分**

## 反馈

对评分规则的意见，请在Issue里讨论。
