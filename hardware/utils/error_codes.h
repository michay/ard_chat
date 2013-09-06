#pragma once

typedef enum
{
	//
	// No error - success
	//
	EErrorCodes_Success = 0,

	//
	// Terminal communication :: pc -> ard Communication errors
	//
	EErrorCodes_InvalidChecksum,

	//
	// Terminal communication :: ard -> pc Communication errors
	//
	EErrorCodes_BufferOverflow,

	//
	// Terminal communication :: Message parsing errors
	//
	EErrorCodes_MissingArgument,
	EErrorCodes_InvalidArgGetSet,
	EErrorCodes_InvalidArgInt,
	EErrorCodes_InvalidOpcodeMissing,
	EErrorCodes_UnknownMessage,

	//
	// Terminal communication :: General errors
	//
	EErrorCodes_CommandTypeotSupported,

	//
	// General exceptions
	//
	EErrorCodes_UnhandledException

}EErrorCodes;
