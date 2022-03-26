#pragma once
/**
 * @file TestDCMSetPrereadVectorParamValidity.h
 * @brief Check the parameter validity of SetPrereadFunction
 * @author Guangyun Wang
 * @date 2021/06/02
 * @copyright AccoTEST Business Unit of Beijing Huafeng Test & Controller Technology Co., Ltd.
*/

#include "..\DCMTestMain.h"
XT_TEST(ParamValidityTest, TestDCMSetPrereadVectorParamValidity)
{
	string strFuncName;
	GetFunctionName(this->GetName(), strFuncName, PARAM_VADILITY);
	CErrorMSG errMsg(strFuncName.c_str(), "ParamValidityTest");///<Error message
	int nRetVal = 0;
	const int nTestCount = 5;
	const char* lpszLabel[nTestCount] = { nullptr, "NO_LABEL", "", "FIRST_ST", "FIRST_SP" };


	int nValidBoardCount = dcm_GetValidBoard();
	if (0 == nValidBoardCount)
	{
		///<No board is inserted
		XT_EXPECT_TRUE(FALSE);
		errMsg.AddNewError(STRING_ERROR_MSG);
		errMsg.SaveErrorMsg("No board inserted.");
		errMsg.Print(this, g_lpszReportFilePath);
	}

	nRetVal = dcm.SetPrereadVector("FRIST_ST", "FRIST_SP");
	XT_EXPECT_EQ(VECTOR_FILE_NOT_LOADED, nRetVal);
	if (VECTOR_FILE_NOT_LOADED != nRetVal)
	{
		///<Not warning when vector is not loaded
		errMsg.AddNewError(STRING_ERROR_MSG);
		errMsg.SaveErrorMsg("No Warning when vector is not loaded!");
	}
	///<Load vector
	dcm.LoadVectorFile(g_lpszVectorFilePath, FALSE);
	///<Defined pin group G_ALLPIN
	dcm.SetPinGroup("G_ALLPIN", "CH0,CH1,CH2,CH3,CH4,CH5,CH6,CH7,CH8,CH9,CH10,CH11,CH12,CH13,CH14,CH15");

	int nStringType = 0;
	BOOL bStartLabelExist = FALSE;
	BOOL bStopLabelExist = FALSE;
	int nStartLine = 0;
	int nStopLine = 0;
	for (int nStartLabelIndex = 0; nStartLabelIndex < nTestCount; ++nStartLabelIndex)
	{
		for (int nStopLabelIndex = 0; nStopLabelIndex < nTestCount; ++nStopLabelIndex)
		{
			nRetVal = dcm.SetPrereadVector(lpszLabel[nStartLabelIndex], lpszLabel[nStopLabelIndex]);
			nStringType = dcm_GetStringType(lpszLabel[nStartLabelIndex]);
			bStartLabelExist = 2 == dcm_GetStringType(lpszLabel[nStartLabelIndex]) ? TRUE : FALSE;
			bStopLabelExist = 2 == dcm_GetStringType(lpszLabel[nStartLabelIndex]) ? TRUE : FALSE;
			if (nullptr == lpszLabel[nStartLabelIndex])
			{

			}
			else if (0 == strlen(lpszLabel[nStartLabelIndex]))
			{
				nStartLine = 0;
			}
			else
			{
				nStartLine = dcm_GetLabelLineNo(lpszLabel[nStartLabelIndex]);
			}
			if (nullptr == lpszLabel[nStopLabelIndex])
			{

			}
			else if (0 == strlen(lpszLabel[nStopLabelIndex]))
			{
				nStopLine = 0xFFFF;
			}
			else
			{
				nStopLine = dcm_GetLabelLineNo(lpszLabel[nStopLabelIndex]);
			}

			if (!bStartLabelExist || !bStopLabelExist)
			{
				if (START_LABEL_ERROR == nRetVal)
				{
					XT_EXPECT_FALSE(bStartLabelExist);
					if (bStartLabelExist)
					{
						errMsg.AddNewError(STRING_ERROR_MSG);
						errMsg.SetErrorItem("StartLabel", lpszLabel[nStartLabelIndex]);
						errMsg.SaveErrorMsg("Warning when start label is right, the return value is %d!", nRetVal);
					}
				}
				else if (STOP_LABEL_ERROR == nRetVal)
				{
					XT_EXPECT_FALSE(bStopLabelExist);
					if (bStopLabelExist)
					{
						XT_EXPECT_TRUE(FALSE);
						errMsg.AddNewError(STRING_ERROR_MSG);
						errMsg.SetErrorItem("StopLabel", lpszLabel[nStopLabelIndex]);
						errMsg.SaveErrorMsg("Warning when end label is right, the return value is %d!", nRetVal);
					}
				}
				else if (nStartLine > nStopLine)
				{
					errMsg.AddNewError(STRING_ERROR_MSG);
					errMsg.SaveErrorMsg("The label is not existed but return is not right, the return value is %d!", nRetVal);
				}
			}
			else
			{
				if (START_LABLE_AFTER_END == nRetVal)
				{
					XT_EXPECT_GREATEREQ(nStartLine, nStopLine);
					if (nStartLine < nStopLine)
					{
						errMsg.AddNewError(STRING_ERROR_MSG);
						errMsg.SaveErrorMsg("Warning when the label is all right, the return value is %d!", nRetVal);
					}
				}
				else if (EXECUTE_SUCCESS == nRetVal)
				{
					XT_EXPECT_LESS(nStartLine, nStopLine);
					if (nStartLine >= nStopLine)
					{
						errMsg.AddNewError(STRING_ERROR_MSG);
						errMsg.SaveErrorMsg("The label is wrong, but the return value is 0!", nRetVal);
					}
				}
			}
		}
	}

	errMsg.Print(this, g_lpszReportFilePath);

	dcm_CloseFile();///<Unload the vector file
}