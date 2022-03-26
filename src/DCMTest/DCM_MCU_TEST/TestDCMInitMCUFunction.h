#pragma once
/**
 * @file TestDCMInitMCUFunction.h
 * @brief Check the parameter validity of InitMCU
 * @author Guangyun Wang
 * @date 2021/04/16
 * @copyrigh AccoTEST Business Unit of Beijing Huafeng Test & Controller Technology Co., Ltd.
*/
#include "..\DCMTestMain.h"
XT_TEST(FunctionFunctionTest, TestDCMInitMCUFunction)
{
	string strFuncName;
	GetFunctionName(this->GetName(), strFuncName, FUNCTION);
	CFuncReport funcReport(strFuncName.c_str(), "FunctionFunctionTest");

	map<BYTE, USHORT> mapSlot;
	int nRetVal = GetBoardInfo(mapSlot, g_lpszVectorFilePath);

	if (0 == mapSlot.size())
	{
		//No board is inserted.
		XT_EXPECT_TRUE(FALSE);
		funcReport.SetNoBoardValid();
		funcReport.Print(this, g_lpszReportFilePath);
		return;
	}

	SaveBoardSN(funcReport, mapSlot);
	auto iterSlot = mapSlot.begin();

	//Load vector.
	XT_EXPECT_EQ(nRetVal, 0);
	if (0 != nRetVal)
	{
		funcReport.AddTestItem("Load vector");
		funcReport.SaveAddtionMsg("Load vector(%s) fail.", g_lpszVectorFilePath);
		iterSlot = mapSlot.begin();
		while (mapSlot.end() != iterSlot)
		{
			for (USHORT usChannel = 0; usChannel < DCM_MAX_CHANNELS_PER_BOARD; ++usChannel)
			{
				funcReport.SaveFailChannel(iterSlot->first, usChannel);
			}
			++iterSlot;
		}
		mapSlot.clear();
		funcReport.Print(this, g_lpszReportFilePath);
		return;
	}

	dcm.SetPinGroup("G_ALLPIN", "CH0,CH1,CH2,CH3,CH4,CH5,CH6,CH7,CH8,CH9,CH10,CH11,CH12,CH13,CH14,CH15");

	int nLineOffset = 1;
	const char* lpszLabel = "TEST_INS_ST";

	funcReport.AddTestItem("Check Function");
	dcm.SetChannelStatus("G_ALLPIN", DCM_ALLSITE, DCM_LOW);
	dcm.InitMCU("G_ALLPIN");
	for (auto& Slot : mapSlot)
	{
		for (BYTE byControllerIndex = 0; byControllerIndex < DCM_MAX_CONTROLLERS_PRE_BOARD;++byControllerIndex)
		{
		}
	}


	int nTargetOperand = 10;
	char lpszOperand[8] = { 0 };
	_itoa_s(nTargetOperand, lpszOperand, sizeof(lpszOperand), 10);
	dcm.SetInstruction("G_ALLPIN", lpszLabel, nLineOffset, "REPEAT", "1");
	dcm.SetOperand("G_ALLPIN", lpszLabel, nLineOffset, lpszOperand);

	int nLineNumber = dcm_GetLabelLineNo(lpszLabel, TRUE) + nLineOffset;

	BOOL bFirstFail = TRUE;

	for (auto& Board : mapSlot)
	{
		for (BYTE byController = 0; byController < DCM_MAX_CONTROLLERS_PRE_BOARD; ++byController)
		{
			BOOL bTestFail = FALSE;

			nRetVal = dcm_GetOperand(Board.first, byController, nLineNumber);
			if (nRetVal != nTargetOperand)
			{
				if (bFirstFail)
				{
					funcReport.SaveAddtionMsg("The actual operand(%d) is not equal to set(%d)", nTargetOperand, nRetVal);
					bFirstFail = FALSE;
				}
				bTestFail = TRUE;
			}
			if (bTestFail)
			{
				USHORT usChannelOffset = byController * DCM_CHANNELS_PER_CONTROL;
				for (USHORT usChannel = 0; usChannel < DCM_CHANNELS_PER_CONTROL; ++usChannel)
				{
					funcReport.SaveFailChannel(Board.first, usChannel + usChannelOffset);
				}
			}
		}
	}


	funcReport.AddTestItem("Check label operand");

	const char* lpszTargetLabel = "TEST_INS_SP";
	dcm.SetInstruction("G_ALLPIN", lpszLabel, nLineOffset, "JUMP", lpszLabel);
	dcm.SetOperand("G_ALLPIN", lpszLabel, nLineOffset, lpszTargetLabel);
	nTargetOperand = dcm_GetLabelLineNo(lpszTargetLabel, TRUE);

	bFirstFail = TRUE;
	for (auto& Board : mapSlot)
	{
		for (BYTE byController = 0; byController < DCM_MAX_CONTROLLERS_PRE_BOARD; ++byController)
		{
			BOOL bTestFail = FALSE;

			nRetVal = dcm_GetOperand(Board.first, byController, nLineNumber);
			if (nRetVal != nTargetOperand)
			{
				if (bFirstFail)
				{
					funcReport.SaveAddtionMsg("The actual operand(%d) is not equal to set(%d)", nTargetOperand, nRetVal);
					bFirstFail = FALSE;
				}
				bTestFail = TRUE;
			}
			if (bTestFail)
			{
				USHORT usChannelOffset = byController * DCM_CHANNELS_PER_CONTROL;
				for (USHORT usChannel = 0; usChannel < DCM_CHANNELS_PER_CONTROL; ++usChannel)
				{
					funcReport.SaveFailChannel(Board.first, usChannel + usChannelOffset);
				}
			}
		}
	}

	funcReport.Print(this, g_lpszReportFilePath);

	dcm.Disconnect("G_ALLPIN");
	dcm_CloseFile();
}