#pragma once

//通信协议值
enum EM_PROTOCOL
{
	INVALID_PROTOCOL,			// 无效值
									   
	//登陆							 
	SIGNIN,                     // 登陆
	SIGNIN_SECCUSS,             // 登陆成功
	SIGNIN_FAILED,              // 登陆失败
	SIGNIN_ALREADY,             // 登陆失败
	SIGNUP,						// 注册
	SIGNUP_SECCUSS,             // 注册成功
	SIGNUP_FAILED,              // 注册失败
};