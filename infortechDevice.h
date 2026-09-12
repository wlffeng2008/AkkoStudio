#ifndef INFORTECHDEVICE_H
#define INFORTECHDEVICE_H

#include "inforDataDef.h"

class InfortechDevice
{
public:
	static InfortechDevice* getInstance();
	~InfortechDevice();


	// 打开控制台日志
	void openLog(bool isOpen = true);
	// 注册回调函数
	void regDevChangeCall(InfortechDef::DevMsgCall fun);


	// 设置鼠标信息
	void addMouseInf(uint16_t vendorId, uint16_t productId);
	// 设置接收器信息
	void addDongleInf(uint16_t vendorId, uint16_t productId);
	// 设置鼠标Boot模式信息
	void addMouseBootInf(uint16_t vendorId, uint16_t productId);
	// 设置鼠接收器Boot模式信息
	void addDongleBootInf(uint16_t vendorId, uint16_t productId);
	// 一次性设置所有设备
	void setSupportDeviceSet(std::vector<InfortechDef::DeviceItem> allDevices);


	// 获取当前接入的设备集合
	std::vector<InfortechDef::HidDevice> getDevices();
	// 获取当前接入的接收器集合
	std::vector<InfortechDef::HidDevice> getDongles();


	// 连接设备
	bool connectDevice(const InfortechDef::HidDevice& device);
	// 断开连接
	void disconnectDevice();
	// 获取当前连接的设备
	std::unique_ptr<InfortechDef::HidDevice> getOpenedDevice();


	// 查询当前设备类型
	InfortechDef::DeviceType getCurDeviceType();
	// 进入配对模式
	bool entryPairMode();
	// 查询当前设备与鼠标的连接状态，连接:true，未连接:false
	bool getMouseConnectState();
	// 初始化设备
	bool initDevice();

	// 查询接收器固件版本号，返回值：成功:[次版本号，主版本号]，失败:[0,0]
	std::string getDongleVersion();
	// 获取设备完整信息，包含 MAC 地址、固件版本、电池状态等核心数据
	std::unique_ptr<InfortechDef::DevInf> getDeviceInf();
	// 获取鼠标配置信息（DPI、轮询率、休眠时间等）
	std::unique_ptr<InfortechDef::MouseCfg> getMouseCfg();
	// 查询鼠标额外配置
	std::unique_ptr<InfortechDef::ExtCfgInfo> getMouseExtraInfo();
	// 获取鼠标所有按键的配置信息
	std::unique_ptr<InfortechDef::AllBtnCfg> getButtonCfg();
	// 获取鼠标传感器配置信息
	std::unique_ptr<InfortechDef::SensorCfg> getSensorCfg();
	// 获取鼠标电感轴配置
	std::unique_ptr<InfortechDef::AxisCfg> getAxisCfgInfo(uint32_t keyId);
	// 获取当前可用灯珠数
	uint32_t getCurLedNum();
	// 获取接收器氛围灯配置
	std::unique_ptr<InfortechDef::DGAmbientCfg> getDGAmbientCfg();


	// 设置基础按键功能（如将按键映射为计算器、音量调节等）
	std::unique_ptr<InfortechDef::AllBtnCfg> setBaseKeyInf(uint32_t keyId, uint32_t baseFunId);
	// 设置火力键
	std::unique_ptr<InfortechDef::AllBtnCfg> setFireKeyInf(uint32_t keyId, InfortechDef::FireKeyInf fireKeyData);
	// 设置快捷键
	std::unique_ptr<InfortechDef::AllBtnCfg> setShortcutKeyInf(uint32_t keyId, InfortechDef::ComboKeyInf shortcutKeyData);
	// 设置宏按键
	std::unique_ptr<InfortechDef::AllBtnCfg> setMacroKeyInf(uint32_t keyId, InfortechDef::MacroKeyInf macroKeyData);


	// 设置轮询率，rate 范围：[125、250、500、1000、2000、4000、8000]
	std::unique_ptr<InfortechDef::MouseCfg> setReportRate(uint32_t rate);
	// 设置休眠时间，time 范围：[10、30、60、120、180、900、1800]，单位：s
	std::unique_ptr<InfortechDef::MouseCfg> setSleepTime(uint32_t time);
	// 设置防抖延迟, time 范围：[1、2、4、8、15、20]，单位：ms
	std::unique_ptr<InfortechDef::MouseCfg> setShakeDelayTime(uint8_t time);
	// 设置静默高度，height范围：[700、1000、2000]，单位：um
	std::unique_ptr<InfortechDef::MouseCfg> setSilenceHeight(uint32_t height);


	// 开启/关闭竞技模式
	std::unique_ptr<InfortechDef::MouseCfg> setCompetitiveMode(bool enable);
	// 开启/关闭移动同步
	std::unique_ptr<InfortechDef::MouseCfg> setMotionSync(bool enable);
	// 开启/关闭直线修正
	std::unique_ptr<InfortechDef::MouseCfg> setStraight(bool enable);
	// 开启/关闭波纹修正
	std::unique_ptr<InfortechDef::MouseCfg> setRippleControl(bool enable);
	// 开启/关闭移动关灯
	std::unique_ptr<InfortechDef::MouseCfg> setMoveOffLed(bool enable);


	// 开启/关闭 BHOP 模式
	std::unique_ptr<InfortechDef::MouseCfg> setBHOPMode(bool enable);
	// 设置 BHOP 灵敏度值，value范围：[100、200、300 ... 900、1000]
	std::unique_ptr<InfortechDef::MouseCfg> setBHOPValue(uint32_t value);


	// 设置 DPI 总挡位数量，size 范围：[1-8]
	std::unique_ptr<InfortechDef::MouseCfg> setDpiGroupSize(uint8_t size);
	// 设置当前 DPI 挡位
	std::unique_ptr<InfortechDef::MouseCfg> setDpiIndex(uint8_t index);
	// 设置指定DPI挡位的指示灯颜色
	std::unique_ptr<InfortechDef::MouseCfg> setDpiColor(uint8_t index, std::string color);
	// 设置指定 DPI 挡位的X/Y轴灵敏度
	std::unique_ptr<InfortechDef::MouseCfg> setDpiX(uint8_t index, uint32_t value);
	std::unique_ptr<InfortechDef::MouseCfg> setDpiY(uint8_t index, uint32_t value);
	std::unique_ptr<InfortechDef::MouseCfg> setDpiXY(uint8_t index, uint32_t value);


	// 设置正面 RGB 灯的显示效果，支持关灯、常亮、呼吸三种模式，0（关灯）、1（常亮）、2（呼吸）
	std::unique_ptr<InfortechDef::MouseCfg> setRgbEffect(uint8_t effectType);
	// 设置正面 RGB 灯的亮度，brightness 范围：[1 - 254]
	std::unique_ptr<InfortechDef::MouseCfg> setRgbBrightness(uint8_t brightness);
	// 设置正面 RGB 灯的动态效果的速度，speedLevel 范围：[1、2、3、4、5]
	std::unique_ptr<InfortechDef::MouseCfg> setRgbSpeed(uint8_t speedLevel);


	// 设置鼠标氛围灯的灯效：【 1关灯 2长亮 3闪烁 4呼吸 5光谱 6波浪 】
	std::unique_ptr<InfortechDef::MouseCfg> setAmbientEffect(uint8_t ambientEffect);
	// 设置鼠标氛围灯亮度 【01 - FF】
	std::unique_ptr<InfortechDef::MouseCfg> setAmbientBrightness(uint8_t ambientBrightness);
	// 设置鼠标氛围灯效速度 【01 - 05】
	std::unique_ptr<InfortechDef::MouseCfg> setAmbientSpeed(uint8_t ambientSpeed);
	// 设置鼠标氛围灯颜色
	std::unique_ptr<InfortechDef::MouseCfg> setAmbientColor(std::string ambientColor);


	// 设置接收器氛围灯的灯效：【 1关灯 2长亮 3闪烁 4呼吸 5光谱 6波浪 】
	std::unique_ptr<InfortechDef::DGAmbientCfg> setDGAmbientEffect(uint8_t ambientEffect);
	// 设置接收器氛围灯亮度 【01 - FF】
	std::unique_ptr<InfortechDef::DGAmbientCfg> setDGAmbientBrightness(uint8_t ambientBrightness);
	// 设置接收器氛围灯效速度 【01 - 05】
	std::unique_ptr<InfortechDef::DGAmbientCfg> setDGAmbientSpeed(uint8_t ambientSpeed);
	// 设置接收器氛围灯颜色
	std::unique_ptr<InfortechDef::DGAmbientCfg> setDGAmbientColor(std::string ambientColor);


	// 设置全部灯珠灯效
	std::unique_ptr<InfortechDef::LedColorGroup> setAllLed(const InfortechDef::LedColorGroup& ledData);
	// 设置部分灯珠灯效
	std::unique_ptr<InfortechDef::LedColorIndeces> setPartLed(const InfortechDef::LedColorIndeces& ledData);
	// 关闭所有灯珠灯效
	bool clearAllLed();


	// 开启/关闭鼠标传感器
	std::unique_ptr<InfortechDef::SensorCfg> setSensorSwitch(bool enable);
	// 设置鼠标传感器角度
	std::unique_ptr<InfortechDef::SensorCfg> setSensorAngle(uint8_t angle);


	// 设置当前触发点
	std::unique_ptr<InfortechDef::AxisCfg> setAxisTriggerValue(uint32_t keyId, uint8_t value);
	// 设置快速触发开关
	std::unique_ptr<InfortechDef::AxisCfg> setAxisRapidSwitch(uint32_t keyId, bool enable);
	// 设置快速触发设定值
	std::unique_ptr<InfortechDef::AxisCfg> setAxisRapidValue(uint32_t keyId, uint8_t value);
	// 设置触发反馈开关
	std::unique_ptr<InfortechDef::AxisCfg> setAxisFeedbackSwitch(uint32_t keyId, bool enable);
	// 设置触发反馈强度
	std::unique_ptr<InfortechDef::AxisCfg> setAxisFeedbackValue(uint32_t keyId, uint8_t value);
	// 打开触觉行程上报
	bool openAxisReport();
	// 关闭触觉行程上报
	bool closeAxisReport();
	// 打开触觉行程校准
	bool openAxisCalibrate(uint32_t keyId);
	// 关闭触觉行程校准
	bool closeAxisCalibrate();
	// 恢复工厂校准值
	bool recoverAxisCalibrate(uint32_t keyId);


	// 加载固件
	std::vector<uint8_t> loadFwFile(const std::string& filePath);
	// 读取固件信息
	std::unique_ptr<InfortechDef::FwInfo> readFwInfo(const std::vector<uint8_t>& fwBuffer);
	// 固件升级
	void deviceFwUpgrade(const std::vector<uint8_t>& fwBuffer);


	// 恢复出厂设置
	bool restoreDefaultConfig();
	// 切换鼠标配置文件，id范围：[0-4]，返回值：成功:配置Id，失败:-1
	int switchProfile(uint8_t id);
	// 导出配置文件
	bool exportConfigFile(std::string fileName);
	// 导入配置文件
	bool importConfigFile(std::string fileName);


	// 加载壁纸
	std::vector<uint8_t> loadWallpaperFile(const std::string& filePath);
	// 设置接收器壁纸
	bool setReceiverWallpaper(const std::vector<uint8_t>& wpBuffer);

private:
	// 单例
    InfortechDevice();
};

#endif // INFORTECHDEVICE_H
