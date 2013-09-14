#pragma once

typedef enum
{
	//
	// No error - success
	//
	EErrorCodes_Success = 0,

	//
	// Terminal communication :: Message parsing errors
	//
	EErrorCodes_NoCommand,
	EErrorCodes_MissingArgument,
	EErrorCodes_InvalidArgGetSet,
	EErrorCodes_InvalidArgInt,
	EErrorCodes_InvalidArgString,
	EErrorCodes_InvalidOpcodeMissing,
	EErrorCodes_UnknownMessage,

	//
	// Terminal communication :: General errors
	//
	EErrorCodes_CommandTypeNotSupported,

	//
	// General exceptions
	//
	EErrorCodes_UnhandledException

}EErrorCodes;
