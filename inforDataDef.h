
#ifndef INFORDATADEF_H
#define INFORDATADEF_H

#include <unordered_set>
#include <functional> 
#include <Windows.h>
#include <iostream>
#include <memory>
#include <array>

#pragma pack(push, 1)
namespace InfortechDef {
	// 设备结构
	struct DeviceItem
	{
		uint16_t vid;
		uint16_t mousePid;
		uint16_t mouseBootPid;
		uint16_t donglePid;
		uint16_t dongleBootPid;
	};
	enum class DeviceType : uint8_t
	{
		Undefined = 0,	// 未定义
		Dongle = 1,		// 接收器
		Mouse = 2,		// 鼠标
		DongleBoot = 3,	// 接收器 boot
		MouseBoot = 4	// 鼠标 boot
	};

	// 设备信息结构体
	struct HidDevice {
		uint16_t vendorId;
		uint16_t productId;
		DeviceType iniDevType = DeviceType::Undefined;
		uint16_t usage = 0x02;
		uint16_t usagePage = 0xFFA8;
		uint8_t reportId = 0x0b;
		std::string manufacturer = "";		// 厂商描述
		std::string description = "";		// 设备描述
		std::string devicePath = "";		// 设备路径
		bool isOpen = false;				// 是否打开

		// 重载相等运算符用于集合比较
		bool operator==(const HidDevice& other) const {
			return vendorId == other.vendorId &&
				productId == other.productId &&
				usage == other.usage &&
				usagePage == other.usagePage &&
				devicePath == other.devicePath;
		}

		std::string getDeviceTypeStr();
	};

	// 回调消息类型
	enum class MsgType : uint8_t
	{
		DeviceChange,			// 设备状态变动
		DeviceDisconnect,		// 设备连接未建立
		PairSucceed,			// 配对成功
		PairFailed,				// 配对失败
		PairTimeout,			// 配对超时
		fwUpgradeFailed,		// 固件升级失败
		fwUpgradeProgress,		// 固件升级进度
		fwUpgradeSucceed,		// 固件升级完成
		DpiChange,				// dpi 挡位切换
		RateChange,				// 轮询率切换
		ConfChange,				// 配置文件切换
		BatteryChange,			// 电池状态改变
		ConnectStateChange,		// 接收器与鼠标连接状态改变
		TriggerPosChange,		// 触觉行程上报
		CalibrateChange,		// 触觉行程校准上报
		SetReceiverWallpaper	// 接收器壁纸进度
	};

	class MouseRptInf {
	public:
		// DPI上报
		struct DpiRpt
		{
			uint8_t dpiId;
		};
		// 轮询率上报
		struct RateRpt
		{
			uint8_t reportRate;
		};
		// 配置文件切换上报
		struct ConfRpt
		{
			uint8_t cfgId;
		};
		// 电池状态上报
		struct BatteryStateRpt
		{
			uint8_t level;
			bool filled;
			bool charging;
		};
		// 连接状态上报
		struct ConnectStateRpt
		{
			uint8_t connectState;
		};
		// 触觉行程上报
		struct TriggerRpt
		{
			uint8_t mouseKeyValue;
			uint8_t triggerPos;
			int16_t triggerValue;
			uint8_t triggerPercent;
		};
		// 触觉校准上报
		struct CalibrateRpt
		{
			uint8_t mouseKeyValue;
		};
		// 升级进度
		struct FwUpProgress
		{
			uint8_t progress;
		};
		// 壁纸设置进度
		struct WpProgress
		{
			uint8_t progress;
		};

		virtual ~MouseRptInf() = default;
		virtual std::unique_ptr<DpiRpt> getDpiRptInf();							// dpi 挡位切换
		virtual std::unique_ptr<RateRpt> getRateRptInf();						// 轮询率切换
		virtual std::unique_ptr<ConfRpt> getConfRptInf();						// 配置文件切换
		virtual std::unique_ptr<BatteryStateRpt> getBatteryStateRptInf();		// 电池状态改变
		virtual std::unique_ptr<ConnectStateRpt> getConnectStateRptInf();		// 接收器与鼠标连接状态改变
		virtual std::unique_ptr<TriggerRpt> getTriggerRptInf();					// 触觉行程上报
		virtual std::unique_ptr<CalibrateRpt> getCalibrateRptInf();				// 触觉行程校准上报
		virtual std::unique_ptr<FwUpProgress> getFwUpProgress();				// 升级进度
		virtual std::unique_ptr<WpProgress> getWpProgress();					// 壁纸设置进度
	};
	struct DevMsg {
		MsgType type;
		std::unique_ptr<MouseRptInf> inf = nullptr;
	};
	// 回调函数定义
	using DevMsgCall = std::function<void(const DevMsg& msg)>;

	// 设备信息
	struct DevInf {
		std::array<uint8_t, 6> macAddress;				// 设备 MAC 地址
		std::array<uint8_t, 2> receiverFwVer;			// 接收器固件版本，[次版本号，主版本号]
		std::array<uint8_t, 2> mouseFwVer;				// 鼠标固件版本，[次版本号，主版本号]
		bool mouseBatteryCharging;						// 是否正在充电
		uint8_t mouseBatterylevel;						// 电池电量（0-100）
		bool mouseBatteryFilled;						// 电池是否充满
		uint8_t confId;									// 当前配置文件 ID

		std::string getMacAddressStr();
		std::string getReceiverFwVerStr();
		std::string getMouseFwVerStr();
	};

	// 额外配置信息
	struct ExtCfgInfo {
		uint16_t mouseVid;
		uint16_t mousePid;
		uint8_t sensorType;
		uint16_t maxDpi;
		uint16_t maxDpiUi;
		uint16_t maxRpt;
		std::unordered_set<uint32_t> indAxisBtns;
		std::unordered_set<uint32_t> silenceHeightRange;
	};

	// 基础按键
	struct BaseKeyInf {
		uint32_t funKeyId;						// 功能键 ID
	};
	// 火力键
	struct FireKeyInf {
		uint8_t interval;						// 点击间隔
		uint8_t clickNum;						// 点击次数
		bool keepClick;	 						// 保持点击
	};
	// 组合键
	struct ComboKeyInf {
		std::vector<uint32_t> sysKey;			// 系统键 ID，最大支持 2 个系统键
		uint32_t customKey;						// 自选键 ID
	};
	// 宏键
	enum class Action : uint8_t {
		Press = 0x01,
		Release = 0x02
	};
	enum class MacroType : uint8_t {
		FixedCountLoop,							// 固定次数循环
		NormalPlayback,							// 正常播放
		StopOnRelease,							// 释放停止
		ImmediateStopOnRelease,					// 释放立即停止
		StopOnTrigger,							// 触发停止
		Unknown									// 未知
	};
	struct MacroItem {
		Action action;							// 动作
		uint16_t keyId;							// 按键 ID
		uint16_t delayTime;						// 延迟时间
	};
	struct MacroKeyInf {
		uint8_t macroId;						// 宏 ID
		MacroType macroType;					// 宏循环类型
		uint8_t cycleNumber;					// 宏循环次数
		std::vector<MacroItem> data;			// 宏数据
	};

	// 鼠标键配置
	class MouseKeyInf {
	public:
		virtual ~MouseKeyInf() = default;
		virtual std::unique_ptr<BaseKeyInf> getBaseKeyInf();	// 获取基础键
		virtual std::unique_ptr<ComboKeyInf> getComboKeyInf();	// 获取组合键
		virtual std::unique_ptr<FireKeyInf> getFireKeyInf();	// 获取火力键
		virtual std::unique_ptr<MacroKeyInf> getMacroKeyInf();	// 获取宏键
	};
	// 配置键类型
	enum class KeyType : uint8_t {
		Undefined,												// 未定义
		BaseKey,												// 基础键
		FireKey,												// 火力键
		ComboKey,												// 组合键
		MacroKey												// 宏键
	};
	// 鼠标按键配置键信息
	struct MouseBtnInf {
		uint32_t keyId;											// 按键 ID
		KeyType keyType;										// 按键类型
		std::unique_ptr<MouseKeyInf> cfg;						// 按键配置
	};
	// 所有鼠标按键配置
	typedef std::vector<MouseBtnInf> AllBtnCfg;					// 配置数据

	// DPI 配置结构
	struct DpiItem {
		uint32_t xDpi;							// 水平方向 DPI
		uint32_t yDpi;							// 竖直方向 DPI
		std::string color;						// DPI 挡位颜色：R:[0],G:[1],B:[2]
	};
	struct DpiStruct
	{
		uint8_t curDpiId;						// 当前 DPI 挡位
		std::vector<DpiItem> data;				// DPI 数据
	};
	
	// 鼠标配置信息
	struct MouseCfg {
		uint32_t reportRate;					// 轮询率（Hz）
		uint32_t sleepTime;						// 休眠时间（s）
		uint8_t shakeDelayTime;					// 防抖延迟（ms）
		uint32_t silenceHeight;					// 静默高度（um）

		bool competitiveMode;					// 竞技模式开关
		bool motinSync;							// 移动同步开关
		bool straight;							// 直线修正开关
		bool rippleControl;						// 波纹修正开关

		bool isSupportMoveOffLed;				// 是否支持移动关灯
		bool moveOffLed;						// 移动关灯开关
		bool isSupportDpiLight;					// 是否支持 DPI 灯光控制

		uint8_t rgbBrightness;					// 正面 RGB 灯亮度 [1 - 254]
		uint8_t rgbEffect;						// 正面 RGB 灯效 [0:关灯，1:常亮，2:呼吸]
		uint8_t rgbSpeed;						// 正面 RGB 灯效速度 [1 - 5]

		uint8_t ambientEffect;					// 氛围灯的灯效：【 0不支持 1关灯 2长亮 3闪烁 4呼吸 5光谱 6波浪 】
		uint8_t ambientBrightness;				// 氛围灯亮度 【01 - FF】
		uint8_t ambientSpeed;					// 氛围灯效速度 【01 - 05】
		std::string ambientColor;				// 氛围灯颜色

		DpiStruct dpiData;						// DPI 配置
	};

	// 接收器灯光
	struct DGAmbientCfg {
		uint8_t ambientEffect;				// 氛围灯的灯效：【 0不支持 1关灯 2长亮 3闪烁 4呼吸 5光谱 6波浪 】
		uint8_t ambientBrightness;			// 氛围灯亮度 【01 - FF】
		uint8_t ambientSpeed;				// 氛围灯效速度 【01 - 05】
		std::string ambientColor;			// 氛围灯颜色
	};

	// 传感器配置信息
	struct SensorCfg {
		int8_t angle;							// 传感器角度（-30~30）
		bool angleSwitch;						// 传感器开关（true=开启，false=关闭）
	};

	// 固件信息
	enum class FwDeviceType : uint8_t
	{
		Undefined = 0,	// 未定义
		Mouse = 1,		// 鼠标
		Dongle = 2,		// 接收器
	};
	struct FwInfo {
		uint16_t vendorId;
		uint16_t productId;
		uint16_t bootVendorId;
		uint16_t bootProductId;
		std::string version;					// 固件版本号（如 "1.6"）
		std::string fileSize;					// 固件包大小（如 "233.3 kb"）
		FwDeviceType fwType;
	};

	// RGB 色值
	struct RgbColor {
		uint8_t r;
		uint8_t g;
		uint8_t b;
	};
	// 神光同步：配置所有灯珠
	typedef std::vector<RgbColor> LedColorGroup;
	// 神光同步：配置部分灯珠
	struct LedColorIndeces {
		RgbColor color;
		std::vector<uint8_t> indices;
	};

	// 电感轴配置信息
	struct AxisCfg {
		uint32_t keyId;

		uint8_t maxTrigger;
		uint8_t triggerValue;

		bool rapidSwitch;
		uint8_t maxRapid;
		uint8_t rapidValue;

		uint8_t maxFeedback;
		uint8_t feedbackValue;
		bool feedbackSwitch;
	};
};
#pragma pack(pop)

#endif // INFORDATADEF_H