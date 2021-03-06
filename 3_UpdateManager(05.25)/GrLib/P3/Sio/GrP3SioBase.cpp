/*
 DVR serial I/O device base

 */

//====================================================================
// uses
#include <GrLnxApp.h>
#include <GrTypeBase.h>
#include <GrDebug.h>
#include <P3/Sio/GrP3SioBase.h>
#include <P3/Sio/GrP3SioMng.h>

//====================================================================
//local const


//====================================================================
//local type


//====================================================================
//local function

//====================================================================
//var


//====================================================================

//--------------------------------------------------------------------
Cls_GrP3SioPiBase::Cls_GrP3SioPiBase( void* A_SioMng, __u8 A_Uart, __u8 A_Id )
{
		m_SioMng	=	A_SioMng;
		m_UartCh	=	A_Uart;
		m_Id			=	A_Id;
}
//--------------------------------------------------------------------
Cls_GrP3SioPiBase::~Cls_GrP3SioPiBase()
{

}
//--------------------------------------------------------------------
BOOL8	Cls_GrP3SioPiBase::RtlBkgProc( void )
{
		return	FALSE;
}
//--------------------------------------------------------------------
void	Cls_GrP3SioPiBase::RtlTimer( void )
{

}
//--------------------------------------------------------------------
void	Cls_GrP3SioPiBase::RtlPiCmd( __u32 A_Cmd, __u32 A_Para0, __u32 A_Para1 )
{

}
//--------------------------------------------------------------------
